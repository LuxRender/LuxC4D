/************************************************************************
 * LuxC4D - CINEMA 4D plug-in for export to LuxRender                   *
 * (http://www.luxrender.net)                                           *
 *                                                                      *
 * Author:                                                              *
 * Marcus Spranger (abstrax)                                            *
 *                                                                      *
 ************************************************************************
 *                                                                      *
 * This file is part of LuxC4D.                                         *
 *                                                                      *
 * LuxC4D is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * LuxC4D is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with LuxC4D.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#include "customgui_datetime.h"

#include "luxapiconverter.h"



#define ERRLOG_RETURN(msg)   { GePrint((msg)); return FALSE; }



/*****************************************************************************
 * Helper functions.
 *****************************************************************************/

/// Returns TRUE if two normal vectors are the same (within some error margin).
static inline Bool equalNormals(const Vector& v1, const Vector& v2)
{
  Vector diff(v2-v1);
  return diff.x*diff.x + diff.y*diff.y + diff.z*diff.z < 0.001*0.001 ;
}



/*****************************************************************************
 * Implementation of public member functions of class LuxAPIConverter.
 *****************************************************************************/

/// Constructs and initialises a new LuxAPIConverter instance.
LuxAPIConverter::LuxAPIConverter(void)
: mReceiver(0),
  mTempParamSet(64)
{}


/// Destroys the LuxAPIConverter instance and frees its resources.
LuxAPIConverter::~LuxAPIConverter(void)
{}


/// Converts a scene into a the Lux format and sends it to a LuxAPI
/// implementation, which can then consume the data.
///
/// @param[in]  document
///   The C4D document that should be converted and exported.
/// @param[in]  receiver
///   The LuxAPI implementation which will consume the converted scene.
/// @return
///   TRUE if the scene could be exported, otherwise FALSE
Bool LuxAPIConverter::convertScene(BaseDocument& document, LuxAPI& receiver)
{
  // init internal data
  mDocument = &document;
  mReceiver = &receiver;
  mXResolution = 0;
  mYResolution = 0;
  mCachedObject = 0;
  mPolygonCache.erase();
  mPointCache.erase();
  mNormalCache.erase();

  // create file head (only important for file export)
  tagDateTime time;
  DateTimeNow(time);
  CHAR buffer[256];
  sprintf(buffer, "# LuxRender scene file\n# Exported by LuxC4D on %d/%d/%d",
                  (int)time.lDay, (int)time.lDay, (int)time.lYear);

  // start the scene
  if (!receiver.startScene(buffer))  return FALSE;

  // export global data
  if (!exportFilm(document, receiver) ||
      !exportCamera(document, receiver) ||
      !exportPixelFilter(document, receiver) ||
      !exportSampler(document, receiver) ||
      !exportSurfaceIntegrator(document, receiver) ||
      !exportAccelerator(document, receiver))
  {
    return FALSE;
  }

  // export scene description
  if (!receiver.worldBegin() ||
      !exportLights(document, receiver) ||
      !exportTextures(document, receiver) ||
      !exportGeometry(document, receiver) ||
      !receiver.worldEnd())
  {
    return FALSE;
  }

  // close scene
  if (!receiver.endScene())  return FALSE;

  return TRUE;
}


/// Allocates a new instance of HierarchyData which will be used to keep track
/// of implicite visibility. It will be called during the object tree traversal
/// in Hierarchy::Run().
///
/// @return
///    Pointer to the allocated HierarchyData. C4D owns the memory.
void* LuxAPIConverter::Alloc(void)
{
  return gNewNC HierarchyData(TRUE);
}


/// Deallocates an instance of HierarchyData. It will be called during the
/// object tree traversal in Hierarchy::Run().
///
/// @param[in]  data
///   The pointer to the HierarchyData instance to be deallocated.
void LuxAPIConverter::Free(void* data)
{
  HierarchyData* hierarchyData = (HierarchyData*)data;
  gDelete(hierarchyData);
}


/// Copies the content from instance of HierarchyData to another. It will be
/// called during the object tree traversal in Hierarchy::Run().
///
/// @param[in]  src
///   The HierarchyData instance to copy from.
/// @param[in]  dst
///   The HierarchyData instance to copy to.
void LuxAPIConverter::CopyTo(void* src, void* dst)
{
  if (src && dst) {
    *(HierarchyData*)dst = *(HierarchyData*)src;
  }
}


/// Does the actual object export. It will be called during the object tree
/// traversal in Hierarchy::Run() for every object.
///
/// @param[in]  src
///   The HierarchyData instance to copy from.
/// @param[in]  dst
///   The HierarchyData instance to copy to.
/// @return
///   TRUE if it was executed successfully and we should continue to traverse
///   through the hierarchy
Bool LuxAPIConverter::Do(void*          data,
                         BaseObject*    object,
                         const Matrix&  globalMatrix,
                         Bool           controlObject)
{
  // if there is an object:
  if (object) {
    // get render visibilty and store it in HierarchyData, if it's explicit
    // (otherwise the implicit visibility from the parent object will be used)
    LONG mode = object->GetRenderMode();
    if (data && (mode != MODE_UNDEF)) {
      ((HierarchyData*)data)->mVisible = (mode == MODE_ON);
    }
    // if it's not an object that is used by a generator or deformer as input
    if (!controlObject && 
        // and if it's a polygon object
        (object->GetType() == Opolygon) &&
        // and if it's visible:
        ((HierarchyData*)data)->mVisible)
    {
      GeAssert(mDocument);
      GeAssert(mReceiver);
      // check if the assigned layer should be rendered:
      const LayerData* layerData = object->GetLayerData(mDocument);
      if (!layerData || layerData->render) {
        // export the object
        return exportPolygonObject(*((PolygonObject*)object), *mReceiver);
      }
    }
  }
  return TRUE;
}



/*****************************************************************************
 * Implementation of private member functions of class LuxAPIConverter.
 *****************************************************************************/

/// NOT FINISHED ...
/// Determines the film resolution and converts it into a Lux "Film" statement.
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportFilm(BaseDocument& document, LuxAPI& receiver)
{
  // obtain render data container
  RenderData* renderData = document.GetActiveRenderData();
  if (!renderData)  ERRLOG_RETURN("LuxAPIConverter::exportFilm(): could not obtain render data");
  BaseContainer* renderSettings = renderData->GetDataInstance();
  if (!renderSettings)  ERRLOG_RETURN("LuxAPIConverter::exportFilm(): could not obtain render settings");
  mXResolution = renderSettings->GetLong(RDATA_XRES);
  mYResolution = renderSettings->GetLong(RDATA_YRES);

  // obtain film resolution and output filename
  LuxIntegerT xResolution = (LuxIntegerT)mXResolution;
  LuxIntegerT yResolution = (LuxIntegerT)mYResolution;
  LuxBoolT    writeTonemappedTGA = TRUE;
  String  filenameC4D = renderSettings->GetString(RDATA_PATH);
  GePrint(filenameC4D);
  if (!filenameC4D.Content()) {
    filenameC4D = document.GetDocumentName().GetString();
  }
  CHAR filenameCString[64];
  filenameC4D.GetCString(filenameCString, (sizeof(filenameCString)-1)/sizeof(filenameCString[0]), St7bit);
  LuxStringT  filename(filenameCString);

  // fill parameter set
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_INTEGER, "xresolution",           &xResolution);
  mTempParamSet.addParam(LuxParamSet::LUX_INTEGER, "yresolution",           &yResolution);
  mTempParamSet.addParam(LuxParamSet::LUX_BOOL,    "write_tonemapped_tga ", &writeTonemappedTGA);
  mTempParamSet.addParam(LuxParamSet::LUX_STRING,  "filename",              &filename);

  // send instruction to Lux API
  return receiver.film("fleximage", mTempParamSet);
}


/// NOT FINISHED ...
/// Determines the current camera position and orientation converts it into a
/// Lux "LookAt" statement. Then the camera settings are determined and
/// converted into a "Camera" statement.
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportCamera(BaseDocument& document, LuxAPI& receiver)
{
  // check if needed data is available
  if (!mXResolution && !mYResolution) {
    ERRLOG_RETURN("LuxAPIConverter::exportCamera(): frame resolution wasn't calculated yet");
  }

  // obtain camera object and its container
  BaseDraw* view = document.GetRenderBaseDraw();
  if (!view)  ERRLOG_RETURN("LuxAPIConverter::exportCamera(): could not obtain render base draw");
  BaseObject* cameraObj = view->GetSceneCamera(&document);
  if (!cameraObj)  cameraObj = view->GetEditorCamera();
  if (!cameraObj)  ERRLOG_RETURN("LuxAPIConverter::exportCamera(): could not obtain camera");
  if (cameraObj->GetType() != Ocamera)  ERRLOG_RETURN("LuxAPIConverter::exportCamera(): obtained camera object is no camera");
  CameraObject* camera = (CameraObject*)cameraObj;
  BaseContainer* cameraData = camera->GetDataInstance();
  if (!cameraData)  ERRLOG_RETURN("LuxAPIConverter::exportCamera(): couldn't obtain container from camera object");

  // obtain camera position and orientation in Lux-style
  Matrix camMat = camera->GetMgn();
  Vector camPos = camMat.off;
  Real largestDim = camPos.x;
  if (camPos.y > largestDim)  largestDim = camPos.y;
  if (camPos.z > largestDim)  largestDim = camPos.z;
  Vector trgPos = camPos + camMat.v3*largestDim;
  Vector upVec = camMat.v2;

  // send "LookAt" instruction to Lux API
  if (!receiver.lookAt(LuxVectorT(camPos), LuxVectorT(trgPos), LuxVectorT(upVec))) {
    return FALSE;
  }

  // obtain camera parameters
  LONG projection = cameraData->GetLong(CAMERA_PROJECTION);
  LReal filmWidth = (LReal)cameraData->GetReal(CAMERAOBJECT_APERTURE);
  LReal focalLength = (LReal)cameraData->GetReal(CAMERA_FOCUS);
  LReal zoom = (LReal)cameraData->GetReal(CAMERA_ZOOM);

  // calculate field of view and determine camera type
  LuxAPI::IdentifierNameT cameraType;
  LReal fovCorrection = (mXResolution < mYResolution) ? 1.0L : (LReal)mYResolution / (LReal)mXResolution;
  LuxFloatT fov;
  if (projection == Pperspective) {
    fov = (LuxFloatT)(2.0L * atan(0.5L*fovCorrection*filmWidth / focalLength) / pi * 180.0L);
    cameraType = "perspective";
  } else {
    fov = (LuxFloatT)(1024.0L * zoom * fovCorrection);
    cameraType = "orthographic";
  }

  // fill parameter set
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_FLOAT, "fov", &fov);

  // send "Camera" instruction to Lux API
  return receiver.camera(cameraType, mTempParamSet);
}


/// TODO ...
/// Will export the pixel filter used by Lux. (at the moment it's exporting
/// only "mitchell").
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPixelFilter(BaseDocument& document, LuxAPI& receiver)
{
  mTempParamSet.clear();
  return receiver.pixelFilter("mitchell", mTempParamSet);
}


/// TODO ...
/// Will export the sampler used by Lux. (at the moment it's exporting
/// only "lowdiscrepancy").
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSampler(BaseDocument& document, LuxAPI& receiver)
{
  mTempParamSet.clear();
  return receiver.sampler("lowdiscrepancy", mTempParamSet);
}


/// TODO ...
/// Will export the surface integrator used by Lux. (at the moment it's
/// exporting only "path" with "maxdepth = 2").
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSurfaceIntegrator(BaseDocument& document, LuxAPI& receiver)
{
  LuxIntegerT maxDepth = 2;
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_INTEGER, "maxdepth", &maxDepth);
  return receiver.surfaceIntegrator("path", mTempParamSet);
}


/// TODO ...
/// Will export the geometry accelerator used by Lux. (at the moment it's
/// exporting only "kdtree").
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportAccelerator(BaseDocument& document, LuxAPI& receiver)
{
  mTempParamSet.clear();
  return receiver.accelerator("kdtree", mTempParamSet);
}


/// TODO ...
/// Will export the lights. It will very likely be renamed to "exportLight"
/// and be called by Do() - similar to exportGeometry. (at the moment it's
/// exporting only a standard "sunsky").
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportLights(BaseDocument& document, LuxAPI& receiver)
{
  LuxIntegerT nSamples = 1;
  LuxVectorT  sundir(0.5, 0.5, 1.0);
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_INTEGER, "nsamples", &nSamples);
  mTempParamSet.addParam(LuxParamSet::LUX_VECTOR,  "sundir",   &sundir);

  return ( receiver.attributeBegin() &&
           receiver.lightSource("sunsky", mTempParamSet) &&
           receiver.attributeEnd() );
}


/// TODO ...
/// Will export the textures. (at the moment it's exporting only a white 
/// "color texture)
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportTextures(BaseDocument& document, LuxAPI& receiver)
{
  LuxColorT color(0.98, 0.98, 0.98);
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_COLOR, "value", &color);

  return receiver.texture("white", "color", "constant", mTempParamSet);
}


/// Exports the complete geometry of the scene.
///
/// @param[in]  document
///   The C4D scene to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportGeometry(BaseDocument& document, LuxAPI& receiver)
{
  HierarchyData data(TRUE);
  return Run(&document, FALSE, 1.0, VFLAG_EXTERNALRENDERER | VFLAG_POLYGONAL, &data, 0);
}


/// Exports a polygon object and sends it to an implementation of LuxAPI.
///
/// @param[in]  object
///   The object to export.
/// @param[in]  receiver
///   The implementation of LuxAPI that will consume the statement.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPolygonObject(PolygonObject& object, LuxAPI& receiver)
{
  // get polygon object
  PolygonObject* polyObject = (PolygonObject*)&object;
  if (!polyObject->GetPolygonCount()) {
    return TRUE;
  }

  GeDebugOut("exporting object '" + object.GetName() + "' ...");

  // extract geometry (TODO)
  TriangleIDsT  selectedTriangles;
  TrianglesT    triangles;
  PointsT       points;
  NormalsT      normals;
  if (!convertGeometry(*polyObject, triangles, points, normals)) {
    return FALSE;
  }

  // skip empty objects
  if (!triangles.size() || !points.size())  return TRUE;

  // AttributeBegin
  if (!receiver.attributeBegin())  return FALSE;

  // write transformation matrix
  LuxMatrixT  globalMatrix(object.GetMg());
  if (!receiver.transform(globalMatrix))  return FALSE;

  // write material (TODO - just a placeholder)
  LuxStringT  kd = "white";
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_TEXTURE, "Kd", &kd);
  if (!receiver.material("matte", mTempParamSet))  return FALSE;

  // export geometry/shape (TODO - placeholder)
  mTempParamSet.clear();
  mTempParamSet.addParam(LuxParamSet::LUX_POINT, "P",
                         points.getArrayAddress(), points.size());
  if (normals.size()) {
    mTempParamSet.addParam(LuxParamSet::LUX_NORMAL, "N",
                           normals.getArrayAddress(), normals.size());
  }
  mTempParamSet.addParam(LuxParamSet::LUX_TRIANGLE, "indices",
                         triangles.getArrayAddress(), triangles.size());
  if (!receiver.shape("trianglemesh", mTempParamSet))  return FALSE;

  // AttributeEnd
  if (!receiver.attributeEnd())  return FALSE;

  return TRUE;
}


/// Converts an object or part of it (not done yet) into a Lux triangle mesh.
///
/// @param[in]  object
///   The C4D scene to convert.
/// @param[out]  triangles
///   The point indices for the Lux triangles will be stored here.
/// @param[out]  points
///   The point positions will be stored here.
/// @param[out]  normals
///   The point normals (if available) will be stored here.
/// @return 
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::convertGeometry(PolygonObject& object,
                                      TrianglesT&    triangles,
                                      PointsT&       points,
                                      NormalsT&      normals)
{
  // clear output arrays
  triangles.erase();
  points.erase();
  normals.erase();

  // this must be a new (not cached) object
  GeAssert(&object != mCachedObject);

  // convert and cache the geometry of the object
  if (!convertAndCacheGeometry(object)) {
    return FALSE;
  }

  // skip empty objects
  if (!mPolygonCache.size() || !mPointCache.size()) {
    return TRUE;
  }

  // initialise triangle and point arrays
  if (!triangles.init((mPolygonCache.size() + mQuadCount) * 3)) {
    ERRLOG_RETURN("LuxAPIConverter::convertGeometry(): not enough memory to allocate triangle array");
  }

  // store polygons as triangles in array using the correct order for right-handed coords
  CPolygon* polygon;
  ULONG     triangleIndex = 0;
  for (ULONG poly=0; poly<mPolygonCache.size(); ++poly) {
    polygon = &(mPolygonCache[poly]);
    triangles[triangleIndex]   = polygon->a;
    triangles[++triangleIndex] = polygon->c;
    triangles[++triangleIndex] = polygon->b;
    if (polygon->c != polygon->d) {
      triangles[++triangleIndex] = polygon->a;
      triangles[++triangleIndex] = polygon->d;
      triangles[++triangleIndex] = polygon->c;
    }
    ++triangleIndex;
  }

  // delete polygon cache as we don't need it anymore
  mPolygonCache.erase();

  // adopt point cache and normal cache
  points.adopt(mPointCache);
  normals.adopt(mNormalCache);

  return TRUE;
}


/// Extracts the used geometry of an object, duplicates points with different
/// normals and caches them for later splitting into areas with different
/// materials.
/// 
/// @param[in]  object
///   The object to convert and cache.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::convertAndCacheGeometry(PolygonObject& object)
{
  // clear cache
  mPolygonCache.erase();
  mPointCache.erase();
  mNormalCache.erase();
  mQuadCount = 0;

  // get polygons + points and return if there are none
  ULONG polygonCount = object.GetPolygonCount();
  const CPolygon* polygons = object.GetPolygonR();
  ULONG pointCount = object.GetPointCount();
  if (!polygonCount || !polygons || !pointCount) {
    return TRUE;
  }

  // copy polygons into cache
  if (!mPolygonCache.init(polygonCount)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheGeometry(): not enough memory to allocate polygon cache");
  }
  memcpy(mPolygonCache.getArrayAddress(), polygons, sizeof(CPolygon)*polygonCount);

  // get normals and store them in FixArray1D as we own the memory
  Vector* c4dNormals = object.CreatePhongNormals();
  C4DNormalsT normals;
  if (c4dNormals) {
    normals.setArrayAddress(c4dNormals, (VULONG)polygonCount*4);
    // check if normals are actually just plain face normals
    VULONG polygonIndex = 0;
    for (polygonIndex=0; polygonIndex<polygonCount; ++polygonIndex) {
      if (!equalNormals(normals[polygonIndex*4], normals[polygonIndex*4+1]) ||
          !equalNormals(normals[polygonIndex*4], normals[polygonIndex*4+2]) ||
          !equalNormals(normals[polygonIndex*4], normals[polygonIndex*4+3]))
      {
        break;
      }
    }
    // if all normals on each face are the same, we don't have to care about
    // vertex normals at all
    if (polygonIndex >= polygonCount) {
      normals.erase();
    }
  }

  // TODO: get UV coordinates and store them in FixArray1D
  // ...

  // log the info we have
  GeDebugOut(object.GetName());
  if (normals.size()) {
    GeDebugOut("  has vertex normals");
  } else if (c4dNormals) {
    GeDebugOut("  has vertex normals == face normals");
  } else {
    GeDebugOut("  has no vertex normals");
  }

  // if we have only points and polygons, but no normals:
  if (normals.size() == 0)  return convertAndCacheWithoutNormals(object);

  // if we have points, polygons and normals:
  return convertAndCacheWithNormals(object, normals);
}


/// Converts an object without vertex normals and caches the geometry. Called
/// by convertAndCacheGeometry().
///
/// @param[in]  object
///   The object to convert.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::convertAndCacheWithoutNormals(PolygonObject& object)
{
  // get points from C4D and polygon count from cache (polygons were already
  // created by (convertAndCacheGeometry())
  ULONG pointCount = object.GetPointCount();
  const Vector* points = object.GetPointR();
  ULONG polygonCount = mPolygonCache.size();
  if (!pointCount || !points || !polygonCount) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithoutNormals(): geometry expected, but not found");
  }

  // initialise point map
  FixArray1D<ULONG> pointMap;
  if (!pointMap.init(pointCount)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithoutNormals(): not enough memory to allocate point map");
  }
  pointMap.fillWithZero();

  // determine and count which points are actually used by polygons
  CPolygon* polygon;
  ULONG     newPointCount = 0;
  for (ULONG poly=0; poly<polygonCount; ++poly) {
    // store pointer to polygon for convenience
    polygon = &(mPolygonCache[poly]);
    // 1st point of polygon
    if (pointMap[polygon->a] == 0) {
      pointMap[polygon->a] = MAXULONG;
      ++newPointCount;
    }
    // 2nd point of polygon
    if (pointMap[polygon->b] == 0) {
      pointMap[polygon->b] = MAXULONG;
      ++newPointCount;
    }
    // 3rd point of polygon
    if (pointMap[polygon->c] == 0) {
      pointMap[polygon->c] = MAXULONG;
      ++newPointCount;
    }
    // 4th point of polygon (only if it's a quad)
    if (polygon->c != polygon->d) {
      if (pointMap[polygon->d] == 0) {
        pointMap[polygon->d] = MAXULONG;
        ++newPointCount;
      }
      ++mQuadCount;
    }
  }

  // log the info we have at the moment
  GeDebugOut("  poly count:      " + LongToString(polygonCount));
  GeDebugOut("  point count:     " + LongToString(pointCount));
  GeDebugOut("  new point count: " + LongToString(newPointCount));

  // initialise point cache array
  if (!mPointCache.init(newPointCount)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithoutNormals(): not enough memory to allocate point map");
  }

  // now fill point cache only with used points and update polygons
  ULONG oldPointIndex, newPointIndex = 0;
  for (ULONG polygonIndex=0; polygonIndex<polygonCount; ++polygonIndex) {
    // store pointer to polygon for convenience
    polygon = &(mPolygonCache[polygonIndex]);
    // 1st point of polygon
    if (pointMap[oldPointIndex = polygon->a] == MAXULONG) {
      mPointCache[newPointIndex] = points[oldPointIndex];
      pointMap[oldPointIndex] = newPointIndex;
      ++newPointIndex;
    }
    polygon->a = pointMap[oldPointIndex];
    // 2nd point of polygon
    if (pointMap[oldPointIndex = polygon->b] == MAXULONG) {
      mPointCache[newPointIndex] = points[oldPointIndex];
      pointMap[oldPointIndex] = newPointIndex;
      ++newPointIndex;
    }
    polygon->b = pointMap[oldPointIndex];
    // 3rd point of polygon
    if (pointMap[oldPointIndex = polygon->c] == MAXULONG) {
      mPointCache[newPointIndex] = points[oldPointIndex];
      pointMap[oldPointIndex] = newPointIndex;
      ++newPointIndex;
    }
    polygon->c = pointMap[oldPointIndex];
    // 4th point of polygon
    if (pointMap[oldPointIndex = polygon->d] == MAXULONG) {
      mPointCache[newPointIndex] = points[oldPointIndex];
      pointMap[oldPointIndex] = newPointIndex;
      ++newPointIndex;
    }
    polygon->d = pointMap[oldPointIndex];
  }
  GeDebugOut("  new point index: " + LongToString(newPointIndex));

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIndex == newPointCount);

  return TRUE;
}


/// Converts an object with vertex normals and/or UV coordinates (not done yet)
/// and caches the geometry. Called by convertAndCacheGeometry().
///
/// @param[in]  object
///   The object to convert.
/// @param[in]  normals
///   The vertex normals obtained from CINEMA 4D.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::convertAndCacheWithNormals(PolygonObject& object,
                                                 C4DNormalsT&   normals)
{
  // get points from C4D and polygon count from cache (polygons were already
  // created by (convertAndCacheGeometry())
  ULONG pointCount = object.GetPointCount();
  const Vector* points = object.GetPointR();
  ULONG polygonCount = mPolygonCache.size();
  if (!pointCount || !points || !polygonCount) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithNormals(): geometry expected, but not found");
  }

  // initialise point map
  FixArray1D<ULONG> pointMap;
  if (!pointMap.init(pointCount+1)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithNormals(): not enough memory to allocate point map");
  }
  pointMap.fillWithZero();

  // count number of polygons per point (+ number of quads)
  CPolygon* polygon;
  for (ULONG polygonIndex=0; polygonIndex<polygonCount; ++polygonIndex) {
    polygon = &(mPolygonCache[polygonIndex]);
    ++pointMap[polygon->a];
    ++pointMap[polygon->b];
    ++pointMap[polygon->c];
    if (polygon->c != polygon->d) {
      ++pointMap[polygon->d];
      ++mQuadCount;
    }
  }
  GeDebugOut(object.GetName());
  GeDebugOut("  poly count:       " + LongToString(polygonCount));
  GeDebugOut("  point count:      " + LongToString(pointCount));

  // convert polygon counts of point map into start positions in point2Poly map
  ULONG point2PolyCount = 0, point2PolyHelper;
  for (ULONG point=0; point<=pointCount; ++point) {
    point2PolyHelper = pointMap[point];
    pointMap[point]  = point2PolyCount;
    point2PolyCount  += point2PolyHelper;
  }
  GeDebugOut("  point2poly count: " + LongToString(point2PolyCount));
  if (point2PolyCount > MAXLONG) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithNormals(): point2Poly map will be too big for object '" + object.GetName() + "'");
  }

  // initialise point2poly array
  Point2PolysT point2Polys;
  if (!point2Polys.init(point2PolyCount)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithNormals(): not enough memory to allocate poin2poly map");
  }
  point2Polys.fillWithZero();

  // collect point2poly information and create new entries only for points
  // with distinct normals
  ULONG       newPointCount = 0;
  Point2Poly* point2Poly;
  Vector*     normal = &(normals[0]);
  Bool        isQuad;
  for (ULONG polyIndex=0; polyIndex<polygonCount; ++polyIndex) {
    polygon = &mPolygonCache[polyIndex];
    isQuad = (polygon->c != polygon->d);
    // 1st point of polygon
    for (point2Poly=&(point2Polys[pointMap[polygon->a]]); point2Poly->data.normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->data.normalRef), *normal))  break;
    }
    polygon->a = (LONG)(point2Poly - point2Polys.getArrayAddress());
    if (!point2Poly->data.normalRef) {
      point2Poly->data.normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 2nd point of polygon
    for (point2Poly=&(point2Polys[pointMap[polygon->b]]); point2Poly->data.normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->data.normalRef), *normal))  break;
    }
    polygon->b = (LONG)(point2Poly - point2Polys.getArrayAddress());
    if (!point2Poly->data.normalRef) {
      point2Poly->data.normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 3rd point of polygon
    for (point2Poly=&(point2Polys[pointMap[polygon->c]]); point2Poly->data.normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->data.normalRef), *normal))  break;
    }
    polygon->c = (LONG)(point2Poly - point2Polys.getArrayAddress());
    if (!point2Poly->data.normalRef) {
      point2Poly->data.normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 4th point of polygon
    if (isQuad) {
      for (point2Poly=&(point2Polys[pointMap[polygon->d]]); point2Poly->data.normalRef; ++point2Poly) {
        if (equalNormals(*(point2Poly->data.normalRef), *normal))  break;
      }
      polygon->d = (LONG)(point2Poly - point2Polys.getArrayAddress());
      if (!point2Poly->data.normalRef) {
        point2Poly->data.normalRef = normal;
        ++newPointCount;
      }
    } else {
      polygon->d = polygon->c;
    }
    ++normal;
  }
  GeDebugOut("  new point count: " + LongToString(newPointCount));

  // initialise point cache and normal cache
  if (!mPointCache.init(newPointCount) || !mNormalCache.init(newPointCount)) {
    ERRLOG_RETURN("LuxAPIConverter::convertAndCacheWithNormals(): not enough memory to allocate point and/or normal cache");
  }

  // now determine new node IDs and fill normal and point caches
  ULONG       newPointIndex = 0;
  for (ULONG pointIndex=0; pointIndex<pointCount; ++pointIndex) {
    for (ULONG point2PolyIndex=pointMap[pointIndex]; point2PolyIndex<pointMap[pointIndex+1]; ++point2PolyIndex) {
      point2Poly = &(point2Polys[point2PolyIndex]);
      if (!point2Poly->data.normalRef)  break;
      mPointCache[newPointIndex] = points[pointIndex];
      point2Poly->data.normalRef->Normalize();
      mNormalCache[newPointIndex] = *(point2Poly->data.normalRef);
      point2Poly->newPoint = newPointIndex;
      ++newPointIndex;
    }
  }
  GeDebugOut("  new point index: " + LongToString(newPointIndex));

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIndex == newPointCount);

  // set new points in polygon
  for (ULONG polygonIndex=0; polygonIndex<polygonCount; ++polygonIndex) {
    polygon = &mPolygonCache[polygonIndex];
    polygon->a = point2Polys[polygon->a].newPoint;
    polygon->b = point2Polys[polygon->b].newPoint;
    polygon->c = point2Polys[polygon->c].newPoint;
    polygon->d = point2Polys[polygon->d].newPoint;
  }

  // set cached object (== activate cache) and return
  mCachedObject = &object;
  return TRUE;
}
