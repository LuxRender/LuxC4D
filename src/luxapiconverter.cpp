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

#include <customgui_datetime.h>
#include <olight.h>

#include "luxapiconverter.h"
#include "luxc4dcameratag.h"
#include "luxc4dlighttag.h"
#include "luxc4dsettings.h"
#include "luxmaterialdata.h"
#include "tluxc4dcameratag.h"
#include "tluxc4dlighttag.h"
#include "tluxc4dportaltag.h"
#include "utilities.h"



/*****************************************************************************
 * Helper functions and classes.
 *****************************************************************************/

/// Returns TRUE if two normal vectors are the same (within some error margin).
static inline Bool equalNormals(const Vector& v1, const Vector& v2)
{
  Vector diff(v2-v1);
  return diff.x*diff.x + diff.y*diff.y + diff.z*diff.z < 0.001*0.001 ;
}


/// Returns TRUE if two normal vectors are the same (within some error margin).
static inline Bool equalUVs(const LuxVector2D& uv1, const LuxVector2D& uv2)
{
  LuxVector2D diff(uv2-uv1);
  return diff.x*diff.x + diff.y*diff.y < 0.0001*0.0001 ;
}


/// Converts a C4D dispersion into Lux roughness.
static inline LuxFloat c4dDispersionToLuxRoughness(LuxFloat dispersion)
{
  return dispersion * dispersion * 0.2;
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


/// Converts a scene into a set of Lux API commands and sends them to a LuxAPI
/// implementation, which can consume the data.
///
/// @param[in]  document
///   The C4D document that should be converted and exported.
/// @param[in]  receiver
///   The LuxAPI implementation which will consume the converted scene.
/// @return
///   TRUE if the scene could be exported, otherwise FALSE
Bool LuxAPIConverter::convertScene(BaseDocument& document, LuxAPI& receiver)
{
  Bool returnValue = FALSE;

  // init internal data
  mDocument = &document;
  mReceiver = &receiver;
  clearTemporaryData();

  // get global scene data like camera, environment, render settings...
  if (!obtainGlobalSceneData()) { return FALSE; }

  // create file head (only important for file export)
  tagDateTime time;
  DateTimeNow(time);
  CHAR buffer[256];
  sprintf(buffer, "# LuxRender scene file\n# Exported by LuxC4D on %d/%d/%d",
                  (int)time.lDay, (int)time.lDay, (int)time.lYear);

  // start the scene
  if (!mReceiver->startScene(buffer)) { return FALSE; }

  // export global data
  if (!exportFilm() ||
      !exportCamera() ||
      !exportPixelFilter() ||
      !exportSampler() ||
      !exportSurfaceIntegrator() ||
      !exportAccelerator())
  {
    goto CLEANUP_AND_RETURN;
  }

  // export scene description
  if (!mReceiver->worldBegin() ||
      !exportLights() ||
      !exportAutoLight() ||
      !exportStandardMaterial() ||
      !exportGeometry() ||
      !mReceiver->worldEnd())
  {
    goto CLEANUP_AND_RETURN;
  }

  // close scene
  if (!mReceiver->endScene())  goto CLEANUP_AND_RETURN;

  // everything was fine ...
  returnValue = TRUE;

CLEANUP_AND_RETURN:

  clearTemporaryData();
  return returnValue;
}


/// Allocates a new instance of HierarchyData which will be used to keep track
/// of implicit visibility. It will be called during the object tree traversal
/// in Hierarchy::Run().
///
/// @return
///    Pointer to the allocated HierarchyData. C4D owns the memory.
void* LuxAPIConverter::Alloc(void)
{
  return gNewNC HierarchyData();
}


/// Deallocates an instance of HierarchyData. It will be called during the
/// object tree traversal in Hierarchy::Run().
///
/// If for the corresponding node of the hierarchy a new scope was opened,
/// it will be closed here.
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
    ((HierarchyData*)dst)->mVisible = ((HierarchyData*)src)->mVisible;
    ((HierarchyData*)dst)->mMaterialName = ((HierarchyData*)src)->mMaterialName;
    ((HierarchyData*)dst)->mMaterialIsEmissive = ((HierarchyData*)src)->mMaterialIsEmissive;
  }
}


/// Does the actual object export. It will be called during the object tree
/// traversal in Hierarchy::Run() for every object and will call this->mDo()
/// which exports that object.
///
/// @param[in/out]  data
///   Private helper data that was passed from the parent object (as a copy).
/// @param[in]  object
///   The current object of this iteration.
/// @param[in]  globalMatrix
///   The global matrix of the current object.
/// @param[in]  controlObject
///   Is TRUE, if the current object is an input object for a generator or
///   deformer, i.e. the current object is invisible, but there is somewhere
///   in the hierarchy a generated/deformed copy of it.
/// @return
///   TRUE if it was executed successfully and we should continue to traverse
///   through the hierarchy
Bool LuxAPIConverter::Do(void*         data,
                         BaseObject*   object,
                         const Matrix& globalMatrix,
                         Bool          controlObject)
{
  // only if there is an object:
  if (!object)  return TRUE;

  // get render visibility and store it in HierarchyData, if it's explicit
  // (otherwise the implicit visibility from the parent object will be used)
  LONG mode = object->GetRenderMode();
  if (data && (mode != MODE_UNDEF)) {
    ((HierarchyData*)data)->mVisible = (mode == MODE_ON);
  }

  return (this->*mDo)(*((HierarchyData*)data), *object, globalMatrix, controlObject);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxAPIConverter.
 *****************************************************************************/

/// Clears all data that is stored during the conversion process.
void LuxAPIConverter::clearTemporaryData(void)
{
  mTempParamSet.clear();
  mCamera       = 0;
  mLightCount   = 0;
  mAreaLightObjects.erase();
  mReusableMaterials.erase();
  mMaterialUsage.erase();
  mCachedObject = 0;
  mPolygonCache.erase();
  mPointCache.erase();
  mNormalCache.erase();
  mUVCache.erase();
  mDo           = 0;
}


/// Determines all objects and settings of the current scene, which are global
/// and not dependant on any hierarchy. These include the render settings,
/// the LuxC4D Scene Settings (if available), the render camera, the environment
/// to use and so on...
///
/// @return
///   TRUE if all necessary data could be gathered and FALSE if not.
Bool LuxAPIConverter::obtainGlobalSceneData(void)
{
  // obtain active render settings
  RenderData* renderData = mDocument->GetActiveRenderData();
  if (!renderData)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertScene(): could not obtain render data");
  mC4DRenderSettings = renderData->GetDataInstance();
  if (!mC4DRenderSettings)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertScene(): could not obtain render settings");

  // get base container of LuxC4DSettings video post effect node - if available
  mLuxC4DSettings = 0;
  PluginVideoPost* videoPost = renderData->GetFirstVideoPost();
  for (; videoPost; videoPost = videoPost->GetNext()) {
    if (videoPost->GetType() == PID_LUXC4D_SETTINGS) {
      mLuxC4DSettings = (LuxC4DSettings*)videoPost->GetNodeData();
      break;
    }
  }
  if (!mLuxC4DSettings)  ERRLOG("LuxAPIConverter::convertScene(): could not obtain LuxC4DSettings -> defaults will be exported");

  // obtain global scene scale which is applied to everything
  if (mLuxC4DSettings) {
    mC4D2LuxScale = mLuxC4DSettings->getC4D2LuxScale();
    mBumpSampleDistance = mLuxC4DSettings->getBumpSampleDistance() *
                          mC4D2LuxScale;
    mTextureGamma = mLuxC4DSettings->getTextureGamma();
  } else {
    mC4D2LuxScale = 0.01;
    mBumpSampleDistance = 0.001 * mC4D2LuxScale;
    mTextureGamma = mC4DRenderSettings->GetReal(RDATA_RENDERGAMMA);
  }

  // obtain stage object if there is one
  BaseObject *stageObject = mDocument->GetHighest(Ostage, FALSE);
  if (stageObject && (stageObject->GetType() == Ostage) &&
      stageObject->GetDeformMode())
  {
#if _C4D_VERSION>=100
    // ignore the stage object if it belongs to a layer that should not be
    // rendered (only for C4D >R10)
    const LayerData* layerData = stageObject->GetLayerData(mDocument);
    if (!layerData || layerData->render) {
#endif
      mCamera = (CameraObject*)getParameterLink(*stageObject, STAGEOBJECT_CLINK, Ocamera);
      if (mCamera && (mCamera->GetType() != Ocamera)) { mCamera = 0; }
#if _C4D_VERSION>=100
    }
#endif
  }

  // if no camera was specified in a stage object obtain the camera object from
  // the render BaseDraw
  if (!mCamera) {
    BaseDraw* view = mDocument->GetRenderBaseDraw();
    if (!view)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::exportCamera(): could not obtain render base draw");
    BaseObject* cameraObj = view->GetSceneCamera(mDocument);
    if (!cameraObj)  cameraObj = view->GetEditorCamera();
    if (!cameraObj)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::exportCamera(): could not obtain camera");
    if (cameraObj->GetType() != Ocamera)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::exportCamera(): obtained camera object is no camera");
    mCamera = (CameraObject*)cameraObj;
  }

  return TRUE;
}


/// Determines the film resolution and converts it into a Lux "Film" statement.
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportFilm(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);
  GeAssert(mC4DRenderSettings);

  // obtain film resolution from C4D render settings
  LuxInteger xResolution = (LuxInteger)mC4DRenderSettings->GetLong(RDATA_XRES);
  LuxInteger yResolution = (LuxInteger)mC4DRenderSettings->GetLong(RDATA_YRES);

  // obtain output filename
  String filenameC4D = mC4DRenderSettings->GetString(RDATA_PATH);
  if (!filenameC4D.Content()) {
    Filename docFilename = mDocument->GetDocumentName();
    docFilename.ClearSuffix();
    filenameC4D = docFilename.GetString();
  }
  CHAR filenameCString[128];
  filenameC4D.GetCString(filenameCString,
                         sizeof(filenameCString)/sizeof(filenameCString[0]),
                         St7bit);
  LuxString filename(filenameCString);

  // fill parameter set with parameters not coming from the settings object
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_INTEGER, "xresolution", &xResolution);
  mTempParamSet.addParam(LUX_INTEGER, "yresolution", &yResolution);
  mTempParamSet.addParam(LUX_STRING,  "filename",    &filename);

  // if no settings object found, use defaults
  if (!mLuxC4DSettings) {
    LuxBool writeTonemappedTGA = TRUE;
    mTempParamSet.addParam(LUX_BOOL, "write_tonemapped_tga ", &writeTonemappedTGA);
    return mReceiver->film("fleximage", mTempParamSet);
  }
  
  // otherwise, obtain settings from object
  const char *name;
  mLuxC4DSettings->getFilm(name, mTempParamSet);
  return mReceiver->film(name, mTempParamSet);
}


/// Exports the currently active camera. At the moment only perspective and
/// orthogonal cameras are supported.
///
/// TODO: Add environment and realistic camera.
///       Check for camera in stage object.
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportCamera(void)
{
  static LuxAPI::IdentifierName cCameraTypeNames[LuxC4DCameraTag::CAMERA_TYPE_NUMBER] = {
      "perspective",
      "orthographic"
    };

  static const char* cLensSamplingTypes[IDD_LENS_SAMPLING_NUMBER] = {
      "uniform",
      "exponential",
      "inverse exponential",
      "gaussian",
      "inverse gaussian"
    };

  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);
  GeAssert(mC4DRenderSettings);
  GeAssert(mCamera);

  // calculate world to camera transformation matrix and swap Z and Y axis, as
  // in Lux the camera looks along the Y axis
  Matrix c4dCamMat = mCamera->GetMgn();
  Matrix world2CamMatrix;
  world2CamMatrix.v1 = Vector(c4dCamMat.v1.x, c4dCamMat.v3.x, c4dCamMat.v2.x);
  world2CamMatrix.v2 = Vector(c4dCamMat.v1.y, c4dCamMat.v3.y, c4dCamMat.v2.y);
  world2CamMatrix.v3 = Vector(c4dCamMat.v1.z, c4dCamMat.v3.z, c4dCamMat.v2.z);
  world2CamMatrix.off.x = -c4dCamMat.off * c4dCamMat.v1;
  world2CamMatrix.off.y = -c4dCamMat.off * c4dCamMat.v3;
  world2CamMatrix.off.z = -c4dCamMat.off * c4dCamMat.v2;
  if (!mReceiver->transform(LuxMatrix(world2CamMatrix, mC4D2LuxScale))) {
    return FALSE;
  }

  // get camera Lux camera parameters
  LuxC4DCameraTag::CameraParameters parameters;
  if (!LuxC4DCameraTag::getCameraParameters(*mCamera,
                                            mC4D2LuxScale,
                                            *mC4DRenderSettings,
                                            parameters))
  {
    return FALSE;
  }

  // setup parameter set
  LuxString lensSamplingType;
  mTempParamSet.clear();

  // store field of view only for perspective camera
  if (parameters.mType == LuxC4DCameraTag::CAMERA_TYPE_PERSPECTIVE) {
    mTempParamSet.addParam(LUX_FLOAT, "fov", &parameters.mFov);
  }

  // grab screen window, if set
  if (parameters.mScreenWindowSet) {
    mTempParamSet.addParam(LUX_FLOAT, "screenwindow", &parameters.mScreenWindow, 4);
  }

  // grab near clipping distance, if set
  if (parameters.mNearClippingSet) {
    mTempParamSet.addParam(LUX_FLOAT, "hither", &parameters.mNearClipping);
  }

  // grab DOF parameters, if set
  if (parameters.mDofEnabled) {
    mTempParamSet.addParam(LUX_FLOAT, "lensradius", &parameters.mLensRadius);
    mTempParamSet.addParam(LUX_BOOL,  "autofocus",  &parameters.mAutofocus);
    if (!parameters.mAutofocus) {
      mTempParamSet.addParam(LUX_FLOAT, "focaldistance", &parameters.mFocalDistance);
    }
    mTempParamSet.addParam(LUX_INTEGER, "blades",        &parameters.mBladeNumber);
    lensSamplingType = cLensSamplingTypes[parameters.mLensSamplingType];
    mTempParamSet.addParam(LUX_STRING,  "distribution", &lensSamplingType);
    if ((parameters.mLensSamplingType == IDD_LENS_SAMPLING_EXPONENTIAL) ||
        (parameters.mLensSamplingType == IDD_LENS_SAMPLING_INVERSE_EXPONENTIAL))
    {
      mTempParamSet.addParam(LUX_INTEGER, "power", &parameters.mExponentialPower);
    }
  }

  // send "Camera" instruction to Lux API
  GeAssert(parameters.mType >= 0);
  GeAssert(parameters.mType < LuxC4DCameraTag::CAMERA_TYPE_NUMBER);
  return mReceiver->camera(cCameraTypeNames[parameters.mType], mTempParamSet);
}


/// Exports the pixel filter of the LuxC4D settings, if available. If not, we
/// export the default pixel filter (gaussian).
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPixelFilter(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // if no settings object found, use defaults
  mTempParamSet.clear();
  if (!mLuxC4DSettings) {
    LuxFloat xWidth = 1.3;
    LuxFloat yWidth = 1.3;
    mTempParamSet.addParam(LUX_FLOAT, "xwidth", &xWidth);
    mTempParamSet.addParam(LUX_FLOAT, "ywidth", &yWidth);
    return mReceiver->pixelFilter("gaussian", mTempParamSet);
  }

  // otherwise, obtain settings from object
  const char *name;
  mLuxC4DSettings->getPixelFilter(name, mTempParamSet);
  return mReceiver->pixelFilter(name, mTempParamSet);
}


/// Exports the sampler of the LuxC4D settings, if available. If not, we
/// export the default sampler (lowdiscrepancy).
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSampler(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // if no settings object found, use defaults
  mTempParamSet.clear();
  if (!mLuxC4DSettings) {
    return mReceiver->sampler("lowdiscrepancy", mTempParamSet);
  }

  // otherwise, obtain settings from object
  const char *name;
  mLuxC4DSettings->getSampler(name, mTempParamSet);
  return mReceiver->sampler(name, mTempParamSet);
}


/// Exports the surface integrator of the LuxC4D settings, if vailable. If not,
/// we use the default integrator (path, maxdepth=10).
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSurfaceIntegrator(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // if no settings object found, use defaults
  mTempParamSet.clear();
  if (!mLuxC4DSettings) {
    LuxInteger maxDepth = 10;
    mTempParamSet.addParam(LUX_INTEGER, "maxdepth", &maxDepth);
    return mReceiver->surfaceIntegrator("path", mTempParamSet);
  }

  // otherwise, obtain settings from object
  const char *name;
  mLuxC4DSettings->getSurfaceIntegrator(name, mTempParamSet);
  return mReceiver->surfaceIntegrator(name, mTempParamSet);
}


/// TODO ...
/// Will export the geometry accelerator used by Lux. (at the moment it's
/// exporting only "kdtree").
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportAccelerator(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  mTempParamSet.clear();
  return mReceiver->accelerator("kdtree", mTempParamSet);
}


/// Exports all enabled lights of the scene. Objects used as area lights will
/// be stored in a set and later the geometry export ignores these.
///
/// @return
///   TRUE, if successful, FALSE otherwise.
Bool LuxAPIConverter::exportLights(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // traverse complete scene hierarchy and export all needed objects
  mDo = &LuxAPIConverter::doLightExport;
  HierarchyData data;
  return Run(mDocument, FALSE, 1.0, VFLAG_EXTERNALRENDERER, &data, 0);
}


/// Does the actual export of a light object and will be called by Do().
///
/// @param[in]  data
///   The hierarchy data for this object.
/// @param[in]  object
///   The object to export.
/// @param[in]  globalMatrix
///   The global matrix of the object to export.
/// @param[in]  controlObject
///   TRUE if this object is a control object for generated geometry further
///   down the hierarchy.
/// @return
///   TRUE, if successful, FALSE otherwise.
Bool LuxAPIConverter::doLightExport(HierarchyData& hierarchyData,
                                    BaseObject&    object,
                                    const Matrix&  globalMatrix,
                                    Bool           controlObject)
{
  // skip generator objects, invisible objects, objects that are no lights
  // or lights that are not enabled
  if (controlObject || !hierarchyData.mVisible ||
      (object.GetType() != Olight) || !object.GetDeformMode())
  {
    return TRUE;
  }

#if _C4D_VERSION>=100
  // skip objects that belong to a layer that should not be rendered
  const LayerData* layerData = object.GetLayerData(mDocument);
  if (layerData && !layerData->render) {
    return TRUE;
  }
#endif

  // export light object
  return exportLight(object, globalMatrix);
}


/// Exports a light object and sends it to an implementation of LuxAPI.
///
/// @param[in]  lightObject
///   The light object to export.
/// @param[in]  globalMatrix
///   The global matrix of the object (will be obtained during scene hierarchy
///   traversal).
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportLight(BaseObject&   lightObject,
                                  const Matrix& globalMatrix)
{
  // get parameters from light object and/or from its associated light tag
  LuxC4DLightTag::LightParameters parameters;
  if (!LuxC4DLightTag::getLightParameters(lightObject, mC4D2LuxScale, parameters)) {
    return FALSE;
  }

  // get some general light parameters
  Vector scaledPosition = globalMatrix.off * mC4D2LuxScale;
  Vector direction      = globalMatrix.v3;
  normalize(direction);

  // open attribute scope and write light group, if it's not empty
  if (!mReceiver->setComment("light '" + lightObject.GetName() + "'"))  return FALSE;
  if (!mReceiver->attributeBegin())  return FALSE;
  if (parameters.mGroup.Content()) {
    LuxString lightGroup;
    convert2LuxString(parameters.mGroup, lightGroup);
    if (!mReceiver->lightGroup(lightGroup.c_str()))  return FALSE;
  }

  // now determine the data depending on the light type and export the data
  // needed for this type
  debugLog("exporting light object '" + lightObject.GetName() + "' ...");
  switch (parameters.mType) {

    case IDD_LIGHT_TYPE_POINT:
      {
        PointLightData data;
        data.mColor = parameters.mColor * 0.1;
        data.mGain  = parameters.mBrightness;
        data.mFrom  = scaledPosition;
        ++mLightCount;
        if (!exportPointLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_SPOT:
      {
        SpotLightData data;
        data.mColor          = parameters.mColor;
        data.mGain           = parameters.mBrightness * 0.1;
        data.mFrom           = scaledPosition;
        data.mTo             = scaledPosition + direction;
        data.mConeAngle      = Deg(parameters.mOuterAngle) * 0.5;
        data.mConeDeltaAngle = data.mConeAngle - Deg(parameters.mInnerAngle) * 0.5;
        ++mLightCount;
        if (!exportSpotLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_DISTANT:
      {
        DistantLightData data;
        data.mColor = parameters.mColor;
        data.mGain  = parameters.mBrightness * 0.1;
        data.mFrom  = scaledPosition;
        data.mTo    = scaledPosition + direction;
        ++mLightCount;
        if (!exportDistantLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_AREA:
      {
        AreaLightData data;
        data.mColor          = parameters.mColor;
        data.mGain           = parameters.mBrightness * 50.0;
        data.mFlippedNormals = (parameters.mFlippedNormals != 0);
        data.mSamples        = parameters.mSamples;
        data.mLightMatrix    = lightObject.GetMg();
        data.mShape          = parameters.mShapeType;
        data.mSize           = parameters.mShapeSize;
        if (data.mShape == IDD_AREA_LIGHT_SHAPE_OBJECT) {
          if (parameters.mShapeObject) {
            mAreaLightObjects.add(parameters.mShapeObject);
            data.mShapeObject = parameters.mShapeObject;
            data.mLightMatrix = data.mShapeObject->GetMg();
          } else {
            break;
          }
        }
        ++mLightCount;
        if (!exportAreaLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_SUN:
      {
        SunLightData data;
        data.mGain      = parameters.mBrightness * 0.1;
        data.mSamples   = parameters.mSamples;
        data.mSunDir    = -direction;
        data.mTurbidity = parameters.mTurbidity;
        data.mRelSize   = parameters.mRelSize;
        ++mLightCount;
        if (!exportSunLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_SKY:
      {
        SkyLightData data;
        data.mGain      = parameters.mBrightness * 0.1;
        data.mSamples   = parameters.mSamples;
        data.mSunDir    = -direction;
        data.mTurbidity = parameters.mTurbidity;
        data.mAdvanced  = parameters.mAdvanced;
        data.mAConst    = parameters.mA;
        data.mBConst    = parameters.mB;
        data.mCConst    = parameters.mC;
        data.mDConst    = parameters.mD;
        data.mEConst    = parameters.mE;
        ++mLightCount;
        if (!exportSkyLight(data))  return FALSE;
        break;
      }

    case IDD_LIGHT_TYPE_SUNSKY:
      {
        SunSkyLightData data;
        data.mGain      = parameters.mBrightness * 0.1;
        data.mSamples   = parameters.mSamples;
        data.mSunDir    = -direction;
        data.mTurbidity = parameters.mTurbidity;
        data.mRelSize   = parameters.mRelSize;
        data.mAdvanced  = parameters.mAdvanced;
        data.mAConst    = parameters.mA;
        data.mBConst    = parameters.mB;
        data.mCConst    = parameters.mC;
        data.mDConst    = parameters.mD;
        data.mEConst    = parameters.mE;
        ++mLightCount;
        if (!exportSunSkyLight(data))  return FALSE;
        break;
      }

    default:
      ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::exportLight(): invalid light type passed");
  }

  // close attribute scope
  if (!mReceiver->attributeEnd())  return FALSE;

  return TRUE;
}


/// Sends a point light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed point light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPointLight(PointLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_COLOR, "L",    &data.mColor);
  mTempParamSet.addParam(LUX_FLOAT, "gain", &data.mGain);
  mTempParamSet.addParam(LUX_POINT, "from", &data.mFrom);
  return mReceiver->lightSource("point", mTempParamSet);
}


/// Sends a spot light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed spot light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSpotLight(SpotLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_COLOR, "L",              &data.mColor);
  mTempParamSet.addParam(LUX_FLOAT, "gain",           &data.mGain);
  mTempParamSet.addParam(LUX_POINT, "from",           &data.mFrom);
  mTempParamSet.addParam(LUX_POINT, "to",             &data.mTo);
  mTempParamSet.addParam(LUX_FLOAT, "coneangle",      &data.mConeAngle);
  mTempParamSet.addParam(LUX_FLOAT, "conedeltaangle", &data.mConeDeltaAngle);
  return mReceiver->lightSource("spot", mTempParamSet);
}


/// Sends a distant light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed distant light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportDistantLight(DistantLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_COLOR, "L",    &data.mColor);
  mTempParamSet.addParam(LUX_FLOAT, "gain", &data.mGain);
  mTempParamSet.addParam(LUX_POINT, "from", &data.mFrom);
  mTempParamSet.addParam(LUX_POINT, "to",   &data.mTo);
  return mReceiver->lightSource("distant", mTempParamSet);
}


/// Sends a complete area light section to the LuxAPI receiver, which consists of:
///   AttributeBegin
///   Transform [ ... ]
///   AreaLightSource "area" ...
///   Shape ...
///   AttributeEnd
///
/// @param[in]  data
///   Structure that contains all needed point light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportAreaLight(AreaLightData& data)
{
  LuxParamSet shapeParams(8);
  const char* shapeName;
  LuxFloat    radius, width, height, zMin, zMax;
  Real        xRad, yRad, zRad;
  Bool        flipYZ;
  PointsT     points;
  TrianglesT  triangles;
  QuadsT      quads;

  // prepare area light export:
  // - update light parameters
  // - setup shape parameters
  switch (data.mShape) {
    // disc area light
    case IDD_AREA_LIGHT_SHAPE_DISC:
      radius = data.mSize.x * 0.5 * mC4D2LuxScale;
      flipYZ = TRUE;
      shapeParams.addParam(LUX_FLOAT, "radius", &radius);
      shapeName = "disk";
      break;
    // rectangle area light
    case IDD_AREA_LIGHT_SHAPE_RECTANGLE:
      width  = data.mSize.x * mC4D2LuxScale;
      height = data.mSize.y * mC4D2LuxScale;
      flipYZ = TRUE;
      shapeParams.addParam(LUX_FLOAT, "width",  &width);
      shapeParams.addParam(LUX_FLOAT, "height", &height);
      shapeName = "quad";
      break;
    // sphere area light
    case IDD_AREA_LIGHT_SHAPE_SPHERE:
      radius = data.mSize.x * 0.5 * mC4D2LuxScale;
      flipYZ = TRUE;
      shapeParams.addParam(LUX_FLOAT, "radius", &radius);
      shapeName = "sphere";
      break;
    // cylinder area light
    case IDD_AREA_LIGHT_SHAPE_CYLINDER:
      radius =  data.mSize.x * 0.5 * mC4D2LuxScale;
      zMin   = -data.mSize.z * 0.5 * mC4D2LuxScale;
      zMax   =  data.mSize.z * 0.5 * mC4D2LuxScale;
      flipYZ = TRUE;
      shapeParams.addParam(LUX_FLOAT, "radius", &radius);
      shapeParams.addParam(LUX_FLOAT, "zmin",   &zMin);
      shapeParams.addParam(LUX_FLOAT, "zmax",   &zMax);
      shapeName = "cylinder";
      break;
    // cube area light
    case IDD_AREA_LIGHT_SHAPE_CUBE:
      xRad = data.mSize.x * 0.5 * mC4D2LuxScale;
      yRad = data.mSize.y * 0.5 * mC4D2LuxScale;
      zRad = data.mSize.z * 0.5 * mC4D2LuxScale;
      points.init(8);
      points[0] = Vector(-xRad, -yRad, -zRad);
      points[1] = Vector( xRad, -yRad, -zRad);
      points[2] = Vector( xRad,  yRad, -zRad);
      points[3] = Vector(-xRad,  yRad, -zRad);
      points[4] = Vector(-xRad, -yRad,  zRad);
      points[5] = Vector( xRad, -yRad,  zRad);
      points[6] = Vector( xRad,  yRad,  zRad);
      points[7] = Vector(-xRad,  yRad,  zRad);
      triangles.init(6*2*3);
      triangles[ 0] = 0;  triangles[ 1] = 1;  triangles[ 2] = 2;  
      triangles[ 3] = 0;  triangles[ 4] = 2;  triangles[ 5] = 3;  
      triangles[ 6] = 7;  triangles[ 7] = 6;  triangles[ 8] = 5;  
      triangles[ 9] = 7;  triangles[10] = 5;  triangles[11] = 4;  
      triangles[12] = 1;  triangles[13] = 0;  triangles[14] = 4;  
      triangles[15] = 1;  triangles[16] = 4;  triangles[17] = 5;  
      triangles[18] = 2;  triangles[19] = 1;  triangles[20] = 5;  
      triangles[21] = 2;  triangles[22] = 5;  triangles[23] = 6;  
      triangles[24] = 3;  triangles[25] = 2;  triangles[26] = 6;  
      triangles[27] = 3;  triangles[28] = 6;  triangles[29] = 7;  
      triangles[30] = 0;  triangles[31] = 3;  triangles[32] = 7;  
      triangles[33] = 0;  triangles[34] = 7;  triangles[35] = 4;  
      flipYZ = FALSE;
      shapeParams.addParam(LUX_POINT, "P",
                           &points.front(), points.size());
      shapeParams.addParam(LUX_TRIANGLE, "triindices",
                           &triangles.front(), triangles.size());
      shapeName = "mesh";
      break;
    // hemisphere area light
    case IDD_AREA_LIGHT_SHAPE_HEMISPHERE:
      radius = data.mSize.x * 0.5 * mC4D2LuxScale;
      zMin   = 0;
      flipYZ = TRUE;
      shapeParams.addParam(LUX_FLOAT, "radius", &radius);
      shapeParams.addParam(LUX_FLOAT, "zmin",   &zMin);
      shapeName = "sphere";
      break;
    // object area light
    case IDD_AREA_LIGHT_SHAPE_OBJECT:
      if (!convertGeometry(*data.mShapeObject, triangles, points))  return FALSE;
      if (!triangles.size() || !points.size())  return TRUE;
      flipYZ = FALSE;
      shapeParams.addParam(LUX_POINT, "P",
                           &points.front(), points.size());
      shapeParams.addParam(LUX_TRIANGLE, "triindices",
                           &triangles.front(), triangles.size());
      shapeName = "mesh";
      break;
    default:
      return TRUE;
  }

  // export transformation matrix of  area light
  if (flipYZ) {
    Vector temp = data.mLightMatrix.v2;
    data.mLightMatrix.v2 = data.mLightMatrix.v3;
    data.mLightMatrix.v3 = -temp;
  }
  LuxMatrix  transformMatrix(data.mLightMatrix, mC4D2LuxScale);
  if (!mReceiver->transform(transformMatrix))  return FALSE;

  // export area light
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_COLOR,   "L",        &data.mColor);
  mTempParamSet.addParam(LUX_FLOAT,   "gain",     &data.mGain);
  mTempParamSet.addParam(LUX_INTEGER, "nsamples", &data.mSamples);
  if (!mReceiver->areaLightSource("area", mTempParamSet))  return FALSE;

  // if the normals should be flipped, export "reverseorientation"
  if (data.mFlippedNormals && !mReceiver->reverseOrientation())  return FALSE;

  // export area light shape
  if (!mReceiver->shape(shapeName, shapeParams))  return FALSE;

  return TRUE;
}


/// Sends a sun light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed sun light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSunLight(SunLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_FLOAT,   "gain",      &data.mGain);
  mTempParamSet.addParam(LUX_INTEGER, "nsamples",  &data.mSamples);
  mTempParamSet.addParam(LUX_VECTOR,  "sundir",    &data.mSunDir);
  mTempParamSet.addParam(LUX_FLOAT,   "turbidity", &data.mTurbidity);
  mTempParamSet.addParam(LUX_FLOAT,   "relsize",   &data.mRelSize);
  return mReceiver->lightSource("sun", mTempParamSet);
}


/// Sends a sky light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed sky light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSkyLight(SkyLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_FLOAT,   "gain",      &data.mGain);
  mTempParamSet.addParam(LUX_INTEGER, "nsamples",  &data.mSamples);
  mTempParamSet.addParam(LUX_VECTOR,  "sundir",    &data.mSunDir);
  mTempParamSet.addParam(LUX_FLOAT,   "turbidity", &data.mTurbidity);
  if (data.mAdvanced) {
    mTempParamSet.addParam(LUX_FLOAT,   "aconst",    &data.mAConst);
    mTempParamSet.addParam(LUX_FLOAT,   "bconst",    &data.mBConst);
    mTempParamSet.addParam(LUX_FLOAT,   "cconst",    &data.mCConst);
    mTempParamSet.addParam(LUX_FLOAT,   "dconst",    &data.mDConst);
    mTempParamSet.addParam(LUX_FLOAT,   "econst",    &data.mEConst);
  }
  return mReceiver->lightSource("sky", mTempParamSet);
}


/// Sends a sun-sky light with its parameters to the LuxAPI receiver.
///
/// @param[in]  data
///   Structure that contains all needed sun-sky light parameters.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportSunSkyLight(SunSkyLightData& data)
{
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_FLOAT,   "gain",      &data.mGain);
  mTempParamSet.addParam(LUX_INTEGER, "nsamples",  &data.mSamples);
  mTempParamSet.addParam(LUX_VECTOR,  "sundir",    &data.mSunDir);
  mTempParamSet.addParam(LUX_FLOAT,   "turbidity", &data.mTurbidity);
  mTempParamSet.addParam(LUX_FLOAT,   "relsize",   &data.mRelSize);
  if (data.mAdvanced) {
    mTempParamSet.addParam(LUX_FLOAT,   "aconst",    &data.mAConst);
    mTempParamSet.addParam(LUX_FLOAT,   "bconst",    &data.mBConst);
    mTempParamSet.addParam(LUX_FLOAT,   "cconst",    &data.mCConst);
    mTempParamSet.addParam(LUX_FLOAT,   "dconst",    &data.mDConst);
    mTempParamSet.addParam(LUX_FLOAT,   "econst",    &data.mEConst);
  }
  return mReceiver->lightSource("sunsky", mTempParamSet);
}


/// Exports an automatic light if no light has been exported. This is always
/// done, even if the C4D auto light is disabled, as Lux crashes if the scene
/// doesn't contain any lights.
///
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::exportAutoLight(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // we export the auto light only, if no other light has been exported
  if (mLightCount) {
    return TRUE;
  }

  debugLog("exporting auto light ...");

  // calculate slightly rotated matrix based on camera matrix
  Matrix lightMatrix    = mCamera->GetMgn() * MatrixRotY(Rad(-20.0));
  Vector scaledPosition = lightMatrix.off * mC4D2LuxScale;
  Vector direction      = lightMatrix.v3;
  normalize(direction);

  // setup distant (parallel) light and export it
  DistantLightData data;
  data.mColor = Vector(1.0);
  data.mGain  = 0.1;
  data.mFrom  = scaledPosition;
  data.mTo    = scaledPosition + direction;
  return exportDistantLight(data);
}


/// Exports all used materials of the current scene + a default material
/// (grey matte).
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportStandardMaterial(void)
{
  LuxMaterialData materialData(gLuxMatteInfo);
  materialData.setChannel(LUX_MATTE_DIFFUSE,
                          gNewNC LuxConstantTextureData(LuxColor(0.8, 0.8, 0.8)));
  return materialData.sendToAPI(*mReceiver, "_default", 0);
}


/// Exports all geometry objects of the scene that are not used by area lights.
///
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportGeometry(void)
{
  // safety checks
  GeAssert(mDocument);
  GeAssert(mReceiver);

  // open global attribute scope where the default material is defined
  if (!mReceiver->setComment("start of world scope") ||
      !mReceiver->attributeBegin())
  {
    return FALSE;
  }

  // traverse complete scene hierarchy and export all needed objects
  mDo = &LuxAPIConverter::doGeometryExport;
  HierarchyData data;
  data.mMaterialName = "_default";
  if (!Run(mDocument, FALSE, 1.0, VFLAG_EXTERNALRENDERER | VFLAG_POLYGONAL, &data, 0)) {
    return FALSE;
  }

  // close global attribute scope
  if (!mReceiver->setComment("end of world scope") || !mReceiver->attributeEnd()) {
    return FALSE;
  }

  return TRUE;
}


/// Does the actual export of a geometry object and will be called by Do().
///
/// @param[in]  data
///   The hierarchy data for this object.
/// @param[in]  object
///   The object to export.
/// @param[in]  globalMatrix
///   The global matrix of the object to export.
/// @param[in]  controlObject
///   TRUE if this object is a control object for generated geometry further
///   down the hierarchy.
/// @return
///   TRUE, if successful, FALSE otherwise.
Bool LuxAPIConverter::doGeometryExport(HierarchyData& hierarchyData,
                                       BaseObject&    object,
                                       const Matrix&  globalMatrix,
                                       Bool           controlObject)
{
  // find texture tag with valid link that is not restricted to a selection
  TextureTag*   textureTag = 0;
  BaseMaterial* material = 0;
  for (BaseTag* tag=object.GetFirstTag(); tag; tag=tag->GetNext()) {
    if (tag->GetType() == Ttexture) {
      material = (BaseMaterial*)getParameterLink(*tag, TEXTURETAG_MATERIAL, Mbase);
      if (!material) {
        continue;
      }
      String restriction = getParameterString(*tag, TEXTURETAG_RESTRICTION);
      if (restriction.Content()) {
        continue;
      }
      textureTag = (TextureTag*)tag;
      break;
    }
  }

  // if we have found a valid texture tag, export material
  if (textureTag && material) {
    if (!exportMaterial(*textureTag,
                        *material,
                        hierarchyData.mMaterialName,
                        hierarchyData.mMaterialIsEmissive))
    {
      return FALSE;
    }
  }

  // skip generator objects, invisible objects, objects that are no polygon
  // objects or objects that have already been exported as area light
  if (controlObject || !hierarchyData.mVisible ||
      (object.GetType() != Opolygon) || mAreaLightObjects.get(&object))
  {
    return TRUE;
  }

#if _C4D_VERSION>=100
  // skip objects that belong to a layer that should not be rendered
  const LayerData* layerData = object.GetLayerData(mDocument);
  if (layerData && !layerData->render) {
    return TRUE;
  }
#endif

  // start new attribute scope
  hierarchyData.mObjectName = object.GetName();
  if (!mReceiver->setComment("start of object '" + hierarchyData.mObjectName + "'"))  return FALSE;
  if (!mReceiver->attributeBegin())  return FALSE;

  // check if object has portal tag and if it does, export it as portal shape
  BaseTag* portalTag = findTagForParamObject(&object, PID_LUXC4D_PORTAL_TAG);
  Bool doObjectExport = TRUE;
  if (portalTag) {
    if (!exportPortalObject((PolygonObject&)object,
                            globalMatrix,
                            *portalTag,
                            doObjectExport))
    {
      return FALSE;
    }
  }

  // if we still want the object exported:
  if (doObjectExport) {
    // export material reference
    if (!mReceiver->namedMaterial(hierarchyData.mMaterialName.c_str()))  return FALSE;
    if (hierarchyData.mMaterialIsEmissive) {
      LuxParamSet areaParamSet(3);
      LuxString   textureName = hierarchyData.mMaterialName + ".L";
      LuxFloat    gain = 50.0;
      areaParamSet.addParam(LUX_TEXTURE, "L",    &textureName);
      areaParamSet.addParam(LUX_FLOAT,   "gain", &gain);
      if (!mReceiver->areaLightSource("area", areaParamSet))  return FALSE;
    }
    // export polygon object
    if (!exportPolygonObject((PolygonObject&)object, globalMatrix))  return FALSE;
  }

  // close attribute scope
  if (!mReceiver->setComment("end of object '" + hierarchyData.mObjectName + "'"))  return FALSE;
  if (!mReceiver->attributeEnd())  return FALSE;

  return TRUE;
}


/// Reads the texture settings from a texture tag and obtains the parameters
/// that are relevant for Lux.
///
/// @param[in]  textureTag
///   Reference to the C4D texture tag.
/// @param[out]  luxTexMapping
///   The structure where the Lux texture mapping parameters will be stored.
/// @return
///   TRUE if executed successful, FALSE otherwise.
Bool LuxAPIConverter::convertTextureMapping(TextureTag&     textureTag,
                                            TextureMapping& luxTexMapping)
{
  // get base container of tag
  BaseContainer* data = textureTag.GetDataInstance();
  if (!data) {
    ERRLOG_RETURN_VALUE(FALSE, "could not obtain container for texture tag");
  }

  // get UV shift and scale from texture tag
  luxTexMapping.mUShift = getParameterReal(textureTag, TEXTURETAG_OFFSETX);
  luxTexMapping.mVShift = getParameterReal(textureTag, TEXTURETAG_OFFSETY);
  luxTexMapping.mUScale = getParameterReal(textureTag, TEXTURETAG_TILESX);
  luxTexMapping.mVScale = getParameterReal(textureTag, TEXTURETAG_TILESY);

  // convert to Lux system where everything is inverse
  luxTexMapping.mUShift *= -luxTexMapping.mUScale;
  luxTexMapping.mVShift *= -luxTexMapping.mVScale;

  // if there is almost no shift or scale, don't export it
  if ((abs(luxTexMapping.mUShift) < 0.001) &&
      (abs(luxTexMapping.mVShift) < 0.001) &&
      (abs(luxTexMapping.mUScale - 1.0) < 0.001) &&
      (abs(luxTexMapping.mVScale - 1.0) < 0.001))
  {
    luxTexMapping.mHasDefaultParams = TRUE;
  } else {
    luxTexMapping.mHasDefaultParams = FALSE;
  }

  // now we have to invert the V scale as we will do the same with the UV coordinates
  // (this is to avoid a problem in bump shading - don't ask me why ...)
  luxTexMapping.mVScale = -luxTexMapping.mVScale;

  return TRUE;
}


/// Exports a material including its textures.
///
/// @param[in]  textureTag
///   The texture the material assigned is to. It's needed for obtaining the
///   texture mapping.
/// @param[in]  material
///   The material to export.
/// @param[out]  materialName
///   Here we return the name under which the material will be exported. Use
///   that one to reference it in the object.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE, if successful, FALSE otherwise.
Bool LuxAPIConverter::exportMaterial(TextureTag&   textureTag,
                                     BaseMaterial& material,
                                     LuxString&    materialName,
                                     Bool&         hasEmissionChannel)
{
  TextureMapping mapping;

  // get texture mapping
  if (!convertTextureMapping(textureTag, mapping)) {
    return FALSE;
  }

  // if the texture has default mapping parameters, we look if this material
  // was already used before (also with default texture mapping)
  // TODO: the reusable material map should be
  //       (material + projection) -> (material name)
  //       this will become important when we export more texture mapping types
  //       than only UV
  if (mapping.mHasDefaultParams) {
    LuxString* reusableMaterialName = mReusableMaterials.get(&material);
    if (reusableMaterialName) {
      materialName = *reusableMaterialName;
      return TRUE;
    }
  }

  // determine material name:
  String c4dMaterialName;

  // if the texture mapping doesn't have default parameters, we export the
  // material only for this object and give it a unique name dependent of the
  // object
  if (!mapping.mHasDefaultParams) {
    BaseObject* object = textureTag.GetObject();
    if (object) {
      c4dMaterialName = object->GetName();
    } else {
      c4dMaterialName = "???";
    }
  }

  // attach material name
  c4dMaterialName += "::";
  c4dMaterialName += material.GetName();

  // TODO: attach projection type
  // ...

  // if material name was already used, create a new unique one by adding a counter
  LONG* usageCount = mMaterialUsage.get(c4dMaterialName);
  if (!usageCount) {
    mMaterialUsage.add(c4dMaterialName, 1);
  } else {
    ++(*usageCount);
    c4dMaterialName = c4dMaterialName + " " + LongToString(*usageCount);
  }

  // convert C4D string to LuxString
  convert2LuxString(c4dMaterialName, materialName);

  // get texture mapping
  if (!convertTextureMapping(textureTag, mapping)) {
    return FALSE;
  }

  // convert standard C4D material to different Lux materials - depending on
  // the used material channels
  Bool success = FALSE;
  if (material.IsInstanceOf(Mmaterial)) {
    Bool hasDiffuse = getParameterLong(material, MATERIAL_USE_COLOR);
    Bool hasTransparency = getParameterLong(material, MATERIAL_USE_TRANSPARENCY);
    Bool hasReflection = getParameterLong(material, MATERIAL_USE_REFLECTION);

    // D    -> diffuse
    if (hasDiffuse && !hasTransparency && !hasReflection) {
      success = exportDiffuseMaterial(mapping,
                                      (Material&)material,
                                      materialName,
                                      hasEmissionChannel);
    // T    -> transparent
    // TR   -> transparent
    // DTR  -> transparent
    } else if ((!hasDiffuse && hasTransparency) ||
               (hasDiffuse && hasTransparency && hasReflection))
    {
      success = exportTransparentMaterial(mapping,
                                          (Material&)material,
                                          materialName,
                                          hasEmissionChannel);
    // DT   -> translucent
    } else if (hasDiffuse && hasTransparency && !hasReflection) {
      success = exportTranslucentMaterial(mapping,
                                          (Material&)material,
                                          materialName,
                                          hasEmissionChannel);
    // R    -> reflective
    } else if (!hasDiffuse && !hasTransparency && hasReflection) {
      success = exportReflectiveMaterial(mapping,
                                         (Material&)material,
                                         materialName,
                                         hasEmissionChannel);
    // DR   -> glossy
    } else if (hasDiffuse && !hasTransparency && hasReflection) {
      success = exportGlossyMaterial(mapping,
                                     (Material&)material,
                                     materialName,
                                     hasEmissionChannel);
    // -    -> dummy
    } else {
      success = exportDummyMaterial(material, materialName, hasEmissionChannel);
    }

  // if it's not a standard material, just export a placeholder material (matte)
  // with the colour set to the average colour of the C4D material
  } else {
    success = exportDummyMaterial(material, materialName, hasEmissionChannel);
  }

  // if material export was successful, we add the material and its name to the
  // list of reusable materials, if the texture mapping had defaults only
  if (success && mapping.mHasDefaultParams) {
    mReusableMaterials.add(&material, materialName);
  }

  return success;
}


/// Exports the material as matte "dummy" material that has the average color
/// of the material.
///
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportDummyMaterial(BaseMaterial& material,
                                          LuxString&    materialName,
                                          Bool&         hasEmissionChannel)
{
  // export dummy matte material with average colour of input material
  LuxMaterialData materialData(gLuxMatteInfo);
  materialData.setChannel(LUX_MATTE_DIFFUSE,
                          gNewNC LuxConstantTextureData(material.GetAverageColor()));
  return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);
}


/// Exports the material as Lux matte material using the colour and bump
/// channels and the Oren-Nayar sigma value.
///
/// @param[in]  mapping
///   The mapping parameters of texture.
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportDiffuseMaterial(const TextureMapping& mapping,
                                            Material&             material,
                                            LuxString&            materialName,
                                            Bool&                 hasEmissionChannel)
{
  LuxMaterialData materialData(gLuxMatteInfo);

  // obtain diffuse channel
  if (getParameterLong(material, MATERIAL_USE_COLOR)) {
    materialData.setChannel(LUX_MATTE_DIFFUSE,
                            convertColorChannel(mapping,
                                                material,
                                                MATERIAL_COLOR_SHADER,
                                                MATERIAL_COLOR_COLOR,
                                                MATERIAL_COLOR_BRIGHTNESS,
                                                MATERIAL_COLOR_TEXTURESTRENGTH));
  }

  // obtain sigma (if illumination model is Oren-Nayar)
  if (getParameterLong(material, MATERIAL_ILLUMINATION_MODEL) ==
        MATERIAL_ILLUMINATION_MODEL_ORENNAYAR)
  {
    LuxFloat roughness = getParameterReal(material, MATERIAL_ILLUMINATION_ROUGHNESS);
    if (roughness > 0.001) {
      materialData.setChannel(LUX_MATTE_SIGMA,
                              gNewNC LuxConstantTextureData(roughness * 180.0));
    }
  }

  // obtain bump and emission channels
  addBumpChannel(mapping, material, materialData, LUX_MATTE_BUMP);
  addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

  return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);
}


/// Exports the material as Lux glossy material using the colour, reflection
/// and bump channels.
///
/// @param[in]  mapping
///   The mapping parameters of texture.
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportGlossyMaterial(const TextureMapping& mapping,
                                           Material&             material,
                                           LuxString&            materialName,
                                           Bool&                 hasEmissionChannel)
{
  LuxMaterialData materialData(gLuxGlossyInfo);

  // obtain diffuse channel
  if (getParameterLong(material, MATERIAL_USE_COLOR)) {
    materialData.setChannel(LUX_GLOSSY_DIFFUSE,
                            convertColorChannel(mapping,
                                                material,
                                                MATERIAL_COLOR_SHADER,
                                                MATERIAL_COLOR_COLOR,
                                                MATERIAL_COLOR_BRIGHTNESS,
                                                MATERIAL_COLOR_TEXTURESTRENGTH));
  }

  // obtain specular reflection channel + dispersion
  if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
    materialData.setChannel(LUX_GLOSSY_SPECULAR,
                            convertColorChannel(mapping,
                                                material,
                                                MATERIAL_REFLECTION_SHADER,
                                                MATERIAL_REFLECTION_COLOR,
                                                MATERIAL_REFLECTION_BRIGHTNESS,
                                                MATERIAL_REFLECTION_TEXTURESTRENGTH));
    LuxFloat roughness = getParameterReal(material,
                                          MATERIAL_REFLECTION_DISPERSION,
                                          0.0);
    roughness = c4dDispersionToLuxRoughness(roughness);
    materialData.setChannel(LUX_GLOSSY_UROUGHNESS,
                            gNewNC LuxConstantTextureData(roughness));
    materialData.setChannel(LUX_GLOSSY_VROUGHNESS,
                            gNewNC LuxConstantTextureData(roughness));
  }

  // obtain bump and emission channels
  addBumpChannel(mapping, material, materialData, LUX_GLOSSY_BUMP);
  addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

  return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);
}


/// Exports the material as Lux mirror or shiny metal material (depending on
/// the dispersion of the reflection) using the colour, reflection and bump
/// channels.
///
/// @param[in]  mapping
///   The mapping parameters of texture.
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportReflectiveMaterial(const TextureMapping& mapping,
                                               Material&             material,
                                               LuxString&            materialName,
                                               Bool&                 hasEmissionChannel)
{
  // get dispersion
  LuxFloat roughness = 0.0;
  if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
    roughness = getParameterReal(material,
                                 MATERIAL_REFLECTION_DISPERSION,
                                 0.0);
  }

  // if roughness nearly or equal 0, we convert to a mirror material
  if (roughness < 0.001) {

    LuxMaterialData materialData(gLuxMirrorInfo);

    // obtain specular reflection channel + dispersion
    if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
      materialData.setChannel(LUX_MIRROR_REFLECTION,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_REFLECTION_SHADER,
                                                  MATERIAL_REFLECTION_COLOR,
                                                  MATERIAL_REFLECTION_BRIGHTNESS,
                                                  MATERIAL_REFLECTION_TEXTURESTRENGTH));
    }

    // obtain bump and emission channels
    addBumpChannel(mapping, material, materialData, LUX_MIRROR_BUMP);
    addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

    return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);

  // if roughness is not nearly or equal 0, we convert to a shiny metal material
  } else {

    LuxMaterialData materialData(gLuxShinyMetalInfo);

    // set reflection channel to 0
    if (!materialData.setChannel(LUX_SHINY_METAL_REFLECTION,
                                 gNewNC LuxConstantTextureData(LuxColor(0.0))))
    {
      ERRLOG_RETURN_VALUE(FALSE, ("LuxAPIConverter::exportShinyMetalMaterial(): could not set reflection channel for material "
                                  + materialName).c_str());
    }

    // obtain specular reflection channel + dispersion
    if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
      materialData.setChannel(LUX_SHINY_METAL_SPECULAR,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_REFLECTION_SHADER,
                                                  MATERIAL_REFLECTION_COLOR,
                                                  MATERIAL_REFLECTION_BRIGHTNESS,
                                                  MATERIAL_REFLECTION_TEXTURESTRENGTH));
      roughness = c4dDispersionToLuxRoughness(roughness);
      materialData.setChannel(LUX_SHINY_METAL_UROUGHNESS,
                              gNewNC LuxConstantTextureData(roughness));
      materialData.setChannel(LUX_SHINY_METAL_VROUGHNESS,
                              gNewNC LuxConstantTextureData(roughness));
    }

    // obtain bump and emission channels
    addBumpChannel(mapping, material, materialData, LUX_SHINY_METAL_BUMP);
    addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

    return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);
  }
}


/// Exports the material as Lux glass or rough glass material (depending on the
/// disperison of the transparency channel) using the colour, reflection
/// and bump channels.
///
/// @param[in]  mapping
///   The mapping parameters of texture.
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportTransparentMaterial(const TextureMapping& mapping,
                                                Material&             material,
                                                LuxString&            materialName,
                                                Bool&                 hasEmissionChannel)
{
  // get dispersion
  LuxFloat roughness = 0.0;
  if (getParameterLong(material, MATERIAL_USE_TRANSPARENCY)) {
    roughness = getParameterReal(material,
                                 MATERIAL_TRANSPARENCY_DISPERSION,
                                 0.0);
  }

  // setup set for additional parameters (e.g. "architectural")
  LuxParamSet addParams(1);

  // get index of refraction, snap it to 1.0 and check if we should use
  // architectural glass
  Real ior = getParameterReal(material,
                              MATERIAL_TRANSPARENCY_REFRACTION,
                              1.0);
  LuxBool architectural = false;
  if (abs(ior - 1.0) < 0.0001) {
    ior = 1.0;
    architectural = true;
    addParams.addParam(LUX_BOOL, "architectural", &architectural);
  }


  // if roughness nearly or equal 0, we convert to a glass material
  if (roughness < 0.001) {

    LuxMaterialData materialData(gLuxGlassInfo);

    // obtain reflection channel
    if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
      materialData.setChannel(LUX_GLASS_REFLECTION,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_REFLECTION_SHADER,
                                                  MATERIAL_REFLECTION_COLOR,
                                                  MATERIAL_REFLECTION_BRIGHTNESS,
                                                  MATERIAL_REFLECTION_TEXTURESTRENGTH));
    } else {
      materialData.setChannel(LUX_GLASS_REFLECTION,
                              gNewNC LuxConstantTextureData(LuxColor(0.0)));
    }

    // obtain transparency channel + IOR
    if (getParameterLong(material, MATERIAL_USE_TRANSPARENCY)) {
      materialData.setChannel(LUX_GLASS_TRANSMISSION,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_TRANSPARENCY_SHADER,
                                                  MATERIAL_TRANSPARENCY_COLOR,
                                                  MATERIAL_TRANSPARENCY_BRIGHTNESS,
                                                  MATERIAL_TRANSPARENCY_TEXTURESTRENGTH));
      if (!architectural) {
        materialData.setChannel(LUX_GLASS_IOR,
                                gNewNC LuxConstantTextureData(getParameterReal(material,
                                                                               MATERIAL_TRANSPARENCY_REFRACTION,
                                                                               1.0)));
      }
    }

    // obtain bump and emission channels
    addBumpChannel(mapping, material, materialData, LUX_GLASS_BUMP);
    addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

    return materialData.sendToAPI(*mReceiver, materialName.c_str(), &addParams);

  // if roughness is not nearly or equal 0, we convert to a rough glass material
  } else {

    LuxMaterialData materialData(gLuxRoughGlassInfo);

    // obtain reflection channel
    if (getParameterLong(material, MATERIAL_USE_REFLECTION)) {
      materialData.setChannel(LUX_ROUGH_GLASS_REFLECTION,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_REFLECTION_SHADER,
                                                  MATERIAL_REFLECTION_COLOR,
                                                  MATERIAL_REFLECTION_BRIGHTNESS,
                                                  MATERIAL_REFLECTION_TEXTURESTRENGTH));
    } else {
      materialData.setChannel(LUX_GLASS_REFLECTION,
                              gNewNC LuxConstantTextureData(LuxColor(0.0f)));
    }

    // obtain transparency channel + IOR + roughness
    if (getParameterLong(material, MATERIAL_USE_TRANSPARENCY)) {
      materialData.setChannel(LUX_ROUGH_GLASS_TRANSMISSION,
                              convertColorChannel(mapping,
                                                  material,
                                                  MATERIAL_TRANSPARENCY_SHADER,
                                                  MATERIAL_TRANSPARENCY_COLOR,
                                                  MATERIAL_TRANSPARENCY_BRIGHTNESS,
                                                  MATERIAL_TRANSPARENCY_TEXTURESTRENGTH));
      if (!architectural) {
        materialData.setChannel(LUX_ROUGH_GLASS_IOR,
                                gNewNC LuxConstantTextureData(getParameterReal(material,
                                                                               MATERIAL_TRANSPARENCY_REFRACTION,
                                                                               1.0)));
      }
      roughness = c4dDispersionToLuxRoughness(roughness);
      materialData.setChannel(LUX_ROUGH_GLASS_UROUGHNESS,
                              gNewNC LuxConstantTextureData(roughness));
      materialData.setChannel(LUX_ROUGH_GLASS_VROUGHNESS,
                              gNewNC LuxConstantTextureData(roughness));
    }

    // obtain bump and emission channel
    addBumpChannel(mapping, material, materialData, LUX_GLASS_BUMP);
    addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

    return materialData.sendToAPI(*mReceiver, materialName.c_str(), &addParams);
  }
}


/// Exports the material as Lux matte translucent material using the colour,
/// transparency and bump channels and the Oren/Nayar sigma.
///
/// @param[in]  mapping
///   The mapping parameters of texture.
/// @param[in]  material
///   The material to export.
/// @param[in]  materialName
///   The name under which the material will be stored.
/// @param[out]  hasEmissionChannel
///   Will be set to TRUE if the material emits light and to FALSE if it doesn't.
/// @return
///   TRUE if exported successfully, FALSE otherwise.
Bool LuxAPIConverter::exportTranslucentMaterial(const TextureMapping& mapping,
                                                Material&             material,
                                                LuxString&            materialName,
                                                Bool&                 hasEmissionChannel)
{
  LuxMaterialData materialData(gLuxMatteTranslucentInfo);

  // obtain diffuse channel
  if (getParameterLong(material, MATERIAL_USE_COLOR)) {
    materialData.setChannel(LUX_MATTE_DIFFUSE,
                            convertColorChannel(mapping,
                                                material,
                                                MATERIAL_COLOR_SHADER,
                                                MATERIAL_COLOR_COLOR,
                                                MATERIAL_COLOR_BRIGHTNESS,
                                                MATERIAL_COLOR_TEXTURESTRENGTH));
  }

  // obtain transmission channel
  if (getParameterLong(material, MATERIAL_USE_TRANSPARENCY)) {
    materialData.setChannel(LUX_MATTE_TRANSLUCENT_TRANSMISSION,
                            convertColorChannel(mapping,
                                                material,
                                                MATERIAL_TRANSPARENCY_SHADER,
                                                MATERIAL_TRANSPARENCY_COLOR,
                                                MATERIAL_TRANSPARENCY_BRIGHTNESS,
                                                MATERIAL_TRANSPARENCY_TEXTURESTRENGTH));
  }

  // obtain sigma (if illumination model is Oren-Nayar)
  if (getParameterLong(material, MATERIAL_ILLUMINATION_MODEL) ==
        MATERIAL_ILLUMINATION_MODEL_ORENNAYAR)
  {
    LuxFloat roughness = getParameterReal(material, MATERIAL_ILLUMINATION_ROUGHNESS);
    if (roughness > 0.001) {
      materialData.setChannel(LUX_MATTE_SIGMA,
                              gNewNC LuxConstantTextureData(roughness * 180.0));
    }
  }

  // obtain bump and emission channels
  addBumpChannel(mapping, material, materialData, LUX_MATTE_BUMP);
  addEmissionChannel(mapping, material, materialData, hasEmissionChannel);

  return materialData.sendToAPI(*mReceiver, materialName.c_str(), 0);
}


/// Reads a standard C4D material and adds its bump channel to the corresponding
/// Lux material (if available).
///
/// @param[in]  mapping
///   The converted Lux texture mapping parameters.
/// @param[in]  material
///   The C4D standard material.
/// @param[out]  materialData
///   The Lux material settings to which the bump channel will be added.
/// @param[in]  channelId
///   The ID of the bump channel of the Lux material data.
/// @return
///   TRUE if executed successfully, FALSE otherwise.
Bool LuxAPIConverter::addBumpChannel(const TextureMapping& mapping,
                                     Material&             material,
                                     LuxMaterialData&      materialData,
                                     ULONG                 channelId)
{
  if (getParameterLong(material, MATERIAL_USE_BUMP)) {
    materialData.setBumpSampleDistance(mBumpSampleDistance);
    return materialData.setChannel(channelId,
                                   convertFloatChannel(mapping,
                                                       material,
                                                       MATERIAL_BUMP_SHADER,
                                                       MATERIAL_BUMP_STRENGTH,
                                                       mC4D2LuxScale));
  }

  return TRUE;
}


/// Reads a standard C4D material and adds its luminance channel as emission
/// channel to a Lux material (if available).
///
/// @param[in]  mapping
///   The converted Lux texture mapping parameters.
/// @param[in]  material
///   The C4D standard material.
/// @param[out]  materialData
///   The Lux material settings to which the bump channel will be added.
/// @return
///   TRUE if executed successfully, FALSE otherwise.
Bool LuxAPIConverter::addEmissionChannel(const TextureMapping& mapping,
                                         Material&             material,
                                         LuxMaterialData&      materialData,
                                         Bool&                 hasEmissionChannel)
{
  if (getParameterLong(material, MATERIAL_USE_LUMINANCE)) {
    hasEmissionChannel = TRUE;
    return materialData.setEmissionChannel(convertColorChannel(mapping,
                                                               material,
                                                               MATERIAL_LUMINANCE_SHADER,
                                                               MATERIAL_LUMINANCE_COLOR,
                                                               MATERIAL_LUMINANCE_BRIGHTNESS,
                                                               MATERIAL_LUMINANCE_TEXTURESTRENGTH));
  } else {
    hasEmissionChannel = FALSE;
  }

  return TRUE;
}


/// Reads out a greyscale shader of a channel of a C4D standard material. We
/// analyse the channel mix options, too and try to emulate the C4D
/// behaviour in the exported Lux material.
///
/// @param[in]  mapping
///   The texture mapping of the corresponding Lux material.
/// @param[in]  material
///   The C4D standard material of which we read out the channel/shader.
/// @param[in]  shaderId
///   The ID of the C4D channel/shader.
/// @param[in]  strengthId
///   The ID of the texture mixing description parameter.
/// @param[in]  strengthScale  (default == 1.0)
///   An optional scaling that can be applied to scale the texture values.
///   We need this for example for the bump channel which needs to be scaled
///   according to the geometric export scale.
/// @return
///   An AutoRef of the converted Lux texture.
LuxTextureDataH LuxAPIConverter::convertFloatChannel(const TextureMapping& mapping,
                                                     Material&             material,
                                                     LONG                  shaderId,
                                                     LONG                  strengthId,
                                                     Real                  strengthScale)
{
  // fetch bitmap shader, if available
  BaseList2D* bitmapLink = getParameterLink(material, shaderId, Xbitmap);

  // get texture strength
  LuxFloat strength = getParameterReal(material, strengthId, 1.0);

  // if the material channel doesn't have a bitmap shader or the texture
  // strength is too small, just create a constant texture of the value 0.0
  if ((abs(strength) < 0.001) || !bitmapLink) {
    return gNewNC LuxConstantTextureData(0.0);
  }

  // if we are here, we've got a bitmap shader -> let's create an imagemap texture
  Filename bitmapPath = getParameterFilename(*bitmapLink, BITMAPSHADER_FILENAME);
  Filename fullPath;
  GenerateTexturePath(mDocument->GetDocumentPath(), bitmapPath, Filename(), &fullPath);
  LuxTextureDataH texture = gNewNC LuxImageMapData(LUX_FLOAT_TEXTURE,
                                                   mapping,
                                                   fullPath,
                                                   mTextureGamma);

  // if strength is not ~1.0, scale texture
  strength *= strengthScale;
  if (abs(strength - 1.0) > 0.001) {
    LuxScaleTextureDataH scaledTexture = gNewNC LuxScaleTextureData(LUX_FLOAT_TEXTURE);
    scaledTexture->mTexture1 = gNewNC LuxConstantTextureData(strength);
    scaledTexture->mTexture2 = texture;
    texture = scaledTexture;
  }

  return texture;
}


/// Reads out a colour shader of a channel of a C4D standard material. We
/// analyse the channel mix options, too and try to emulate the C4D
/// behaviour in the exported Lux material.
///
/// @param[in]  mapping
///   The texture mapping of the corresponding Lux material.
/// @param[in]  material
///   The C4D standard material of which we read out the channel/shader.
/// @param[in]  shaderId
///   The ID of the C4D channel/shader.
/// @param[in]  colorId
///   The ID of the colour description parameter of the channel.
/// @param[in]  brightnessId
///   The ID of the brightness description parameter of the channel.
/// @param[in]  mixerId
///   The ID of the texture mixing description parameter.
/// @return
///   An AutoRef of the converted Lux texture.
LuxTextureDataH LuxAPIConverter::convertColorChannel(const TextureMapping& mapping,
                                                     Material&             material,
                                                     LONG                  shaderId,
                                                     LONG                  colorId,
                                                     LONG                  brightnessId,
                                                     LONG                  mixerId)
{
  // fetch bitmap shader, if available
  BaseList2D* bitmapLink = getParameterLink(material, shaderId, Xbitmap);

  // get texture strength and base colour
  LuxFloat strength = getParameterReal(material, mixerId, 1.0);
  LuxColor color = getParameterVector(material, colorId) *
                   getParameterReal(material, brightnessId, 1.0);

  // if the material channel doesn't have a bitmap shader or the texture
  // strength is too small, just create a constant texture of the colour which
  // is also specified in the channel
  if ((strength < 0.001) || !bitmapLink) {
    return gNewNC LuxConstantTextureData(color);
  }

  // if we are here, we've got a bitmap shader -> let's create an imagemap texture
  Filename bitmapPath = getParameterFilename(*bitmapLink, BITMAPSHADER_FILENAME);
  Filename fullPath;
  GenerateTexturePath(mDocument->GetDocumentPath(), bitmapPath, Filename(), &fullPath);
  LuxTextureDataH texture = gNewNC LuxImageMapData(LUX_COLOR_TEXTURE,
                                                   mapping,
                                                   fullPath,
                                                   mTextureGamma);

  // if the texture strength is < 100%, we mix the colour with the texture
  if (strength < 0.999) {
    LuxMixTextureDataH mixTexture = gNewNC LuxMixTextureData(LUX_COLOR_TEXTURE);
    mixTexture->mTexture1 = gNewNC LuxConstantTextureData(color);
    mixTexture->mTexture2 = texture;
    mixTexture->mAmount = strength;
    texture = mixTexture;
  }

  return texture;
}


/// Exports a polygon object and sends it to a LuxAPI implementation.
///
/// @param[in]  object
///   The polygon object to export.
/// @param[in]  globalMatrix
///   The global matrix of the object (will be obtained during scene hierarchy
///   traversal).
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPolygonObject(PolygonObject& object,
                                          const Matrix&  globalMatrix)
{
  // only export get polygon object with geometry/polygons
  if (!object.GetPolygonCount()) {
    return TRUE;
  }

  debugLog("exporting polygon object '" + object.GetName() + "' ...");

  // convert and cache geometry
  TrianglesT     triangles;
  PointsT        points;
  NormalsT       normals;
  UVsSerialisedT uvs;
  if (!convertGeometry(object, triangles, points, &normals, &uvs)) {
    return FALSE;
  }

  // skip empty objects
  if (!triangles.size() || !points.size())  return TRUE;

  // write transformation matrix
  LuxMatrix  transformMatrix(globalMatrix, mC4D2LuxScale);
  if (!mReceiver->transform(transformMatrix))  return FALSE;

  // export geometry/shape + normals + UVs (if given)
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_TRIANGLE, "triindices",
                         triangles.arrayAddress(), triangles.size());
  mTempParamSet.addParam(LUX_POINT, "P",
                         points.arrayAddress(), points.size());
  if (normals.size()) {
    mTempParamSet.addParam(LUX_NORMAL, "N",
                           normals.arrayAddress(), normals.size());
  }
  if (uvs.size()) {
    mTempParamSet.addParam(LUX_UV, "uv",
                           uvs.arrayAddress(), uvs.size());
  }
  if (!mReceiver->shape("mesh", mTempParamSet))  return FALSE;

  return TRUE;
}


/// Exports a portal shape and sends it to a LuxAPI implementation.
///
/// @param[in]  object
///   The polygon object wich defines the portal shape.
/// @param[in]  globalMatrix
///   The global matrix of the object (will be obtained during scene hierarchy
///   traversal).
/// @param[in]  tag
///   The portal tag with additional settings (must be of type
///   PID_LUXC4D_PORTAL_TAG!).
/// @param[out]  exportObject
///   This will be set by this function and if it is set to TRUE, the portal
///   object should be exported also as a standard polygon object. If it's set
///   to FALSE it should be exported only as portal shap.
/// @return
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::exportPortalObject(PolygonObject& object,
                                         const Matrix&  globalMatrix,
                                         BaseTag&       tag,
                                         Bool&          exportObject)
{
  static const Real cThicknessExtension = 1.1;

  // in the beginning we set exportObject to TRUE and only set it to FALSE,
  // when we really have exported the portal and the user doesn't want us to
  // export the object twice
  exportObject = TRUE;

  // only export get polygon object with geometry/polygons
  if (!object.GetPolygonCount()) { return TRUE; }

  // obtain settings from tag
  BaseContainer* tagData = tag.GetDataInstance();
  if (!tagData) { return FALSE; }
  if (!tagData->GetBool(IDD_PORTAL_ENABLED)) { return TRUE; }
  Bool simplify = tagData->GetBool(IDD_PORTAL_SIMPLIFY);
  exportObject = tagData->GetBool(IDD_PORTAL_EXPORT_OBJECT);
  Bool flipNormals = tagData->GetBool(IDD_PORTAL_FLIP_NORMALS);

  // log
  debugLog("exporting portal polygon object '" + object.GetName() + "' ...");

  // the container for the geometry
  TrianglesT triangles;
  PointsT    points;

  // if we should create a simplified geometry:
  if (simplify) {
    // determine bounding box centre and radius
    Vector bboxCentre = object.GetMp() * mC4D2LuxScale;
    Vector bboxRad = object.GetRad() * mC4D2LuxScale;
    // setup points depending on the face direction
    points.init(4);
    switch (tagData->GetLong(IDD_PORTAL_FACE_DIRECTION)) {
      case IDD_PORTAL_FACE_DIR_X_PLUS:
        points[0] = bboxCentre + Vector(-bboxRad.x*cThicknessExtension, -bboxRad.y, -bboxRad.z);
        points[1] = bboxCentre + Vector(-bboxRad.x*cThicknessExtension, -bboxRad.y,  bboxRad.z);
        points[2] = bboxCentre + Vector(-bboxRad.x*cThicknessExtension,  bboxRad.y,  bboxRad.z);
        points[3] = bboxCentre + Vector(-bboxRad.x*cThicknessExtension,  bboxRad.y, -bboxRad.z);
        break;
      case IDD_PORTAL_FACE_DIR_X_MINUS:
        points[0] = bboxCentre + Vector( bboxRad.x*cThicknessExtension, -bboxRad.y,  bboxRad.z);
        points[1] = bboxCentre + Vector( bboxRad.x*cThicknessExtension, -bboxRad.y, -bboxRad.z);
        points[2] = bboxCentre + Vector( bboxRad.x*cThicknessExtension,  bboxRad.y, -bboxRad.z);
        points[3] = bboxCentre + Vector( bboxRad.x*cThicknessExtension,  bboxRad.y,  bboxRad.z);
        break;
      case IDD_PORTAL_FACE_DIR_Y_PLUS:
        points[0] = bboxCentre + Vector(-bboxRad.x, -bboxRad.y*cThicknessExtension, -bboxRad.z);
        points[1] = bboxCentre + Vector( bboxRad.x, -bboxRad.y*cThicknessExtension, -bboxRad.z);
        points[2] = bboxCentre + Vector( bboxRad.x, -bboxRad.y*cThicknessExtension,  bboxRad.z);
        points[3] = bboxCentre + Vector(-bboxRad.x, -bboxRad.y*cThicknessExtension,  bboxRad.z);
        break;
      case IDD_PORTAL_FACE_DIR_Y_MINUS:
        points[0] = bboxCentre + Vector( bboxRad.x,  bboxRad.y*cThicknessExtension, -bboxRad.z);
        points[1] = bboxCentre + Vector(-bboxRad.x,  bboxRad.y*cThicknessExtension, -bboxRad.z);
        points[2] = bboxCentre + Vector(-bboxRad.x,  bboxRad.y*cThicknessExtension,  bboxRad.z);
        points[3] = bboxCentre + Vector( bboxRad.x,  bboxRad.y*cThicknessExtension,  bboxRad.z);
        break;
      case IDD_PORTAL_FACE_DIR_Z_PLUS:
        points[0] = bboxCentre + Vector( bboxRad.x, -bboxRad.y, -bboxRad.z*cThicknessExtension);
        points[1] = bboxCentre + Vector(-bboxRad.x, -bboxRad.y, -bboxRad.z*cThicknessExtension);
        points[2] = bboxCentre + Vector(-bboxRad.x,  bboxRad.y, -bboxRad.z*cThicknessExtension);
        points[3] = bboxCentre + Vector( bboxRad.x,  bboxRad.y, -bboxRad.z*cThicknessExtension);
        break;
      case IDD_PORTAL_FACE_DIR_Z_MINUS:
        points[0] = bboxCentre + Vector(-bboxRad.x, -bboxRad.y,  bboxRad.z*cThicknessExtension);
        points[1] = bboxCentre + Vector( bboxRad.x, -bboxRad.y,  bboxRad.z*cThicknessExtension);
        points[2] = bboxCentre + Vector( bboxRad.x,  bboxRad.y,  bboxRad.z*cThicknessExtension);
        points[3] = bboxCentre + Vector(-bboxRad.x,  bboxRad.y,  bboxRad.z*cThicknessExtension);
        break;
      default:
        ERRLOG_RETURN_VALUE(FALSE, "invalid face direction in portal tag '" + tag.GetName() + "'");
    }
    // setup triangles
    triangles.init(2*3);
    triangles[ 0] = 0;  triangles[ 1] = 1;  triangles[ 2] = 2;  
    triangles[ 3] = 0;  triangles[ 4] = 2;  triangles[ 5] = 3;  
  } else {
    // convert and cache geometry
    if (!convertGeometry(object, triangles, points)) { return FALSE; }
  }

  // skip empty objects
  if (!triangles.size() || !points.size()) {
    debugLog("  which is empty -> nothing exported");
    return TRUE;
  }

  // write transformation matrix
  LuxMatrix  transformMatrix(globalMatrix, mC4D2LuxScale);
  if (!mReceiver->transform(transformMatrix))  return FALSE;

  // setup shape parameters
  mTempParamSet.clear();
  mTempParamSet.addParam(LUX_POINT, "P",
                         points.arrayAddress(), points.size());
  mTempParamSet.addParam(LUX_TRIANGLE, "triindices",
                         triangles.arrayAddress(), triangles.size());
  

  // if the normals should be flipped, export "reverseorientation"
  if (flipNormals && !mReceiver->reverseOrientation())  return FALSE;

  // export shape
  if (!mReceiver->portalShape("mesh", mTempParamSet)) { return FALSE; }

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
///   The point normals (if available) will be stored here (if pointer is 0
///   no normals are obtained).
/// @param[out]  uvs
///   The UV coordinates (if available) will be stored here (if pointer is 0
///   no UVs are obtained).
/// @return 
///   TRUE, if successful, FALSE otherwise
Bool LuxAPIConverter::convertGeometry(PolygonObject&  object,
                                      TrianglesT&     triangles,
                                      PointsT&        points,
                                      NormalsT*       normals,
                                      UVsSerialisedT* uvs)
{
  // clear output arrays
  triangles.erase();
  points.erase();
  if (normals)  normals->erase();
  if (uvs)      uvs->erase();

  // this must be a new (not cached) object
  GeAssert(&object != mCachedObject);

  // convert and cache the geometry of the object
  if (!convertAndCacheObject(object, (normals == 0), (uvs == 0))) {
    return FALSE;
  }

  // skip empty objects
  if (!mPolygonCache.size() || !mPointCache.size()) {
    return TRUE;
  }

  // initialise triangle array
  if (!triangles.init((mPolygonCache.size() + mQuadCount) * 3)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertGeometry(): not enough memory to allocate triangle array");
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

  // adopt point cache and normal cache (for now - as long as we don't split objects)
  points.adopt(mPointCache);
  if (normals)  normals->adopt(mNormalCache);

  // store UVs if destination array is available
  if (uvs) {
    // initialise UV array
    if (!uvs->init(mUVCache.size() << 1)) {
      ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertGeometry(): not enough memory to allocate UV array");
    }
    // copy UVs from 2D vectors to serialised floats
    LuxVector2D* uv;
    for (ULONG uvIx=0; uvIx<mUVCache.size(); ++uvIx) {
      uv = &(mUVCache[uvIx]);
      (*uvs)[ uvIx << 1   ] = uv->x;
      (*uvs)[(uvIx << 1)+1] = 1.0 - uv->y;
    }
  }

  return TRUE;
}


/// Extracts the visible geometry of an object (TODO), its vertex normals and UV
/// coordinates. Vertices with several different normals or UV coordinates (for
/// different adjacent polygons) are then duplicated as Lux only supports one
/// normal and one UV coordinate per vertex. The resulting geometry is then
/// cached for further splits for different materials later in the conversion
/// process.
/// 
/// @param[in]  object
///   The object to convert and cache.
/// @param[in]  noNormals
///   Set this to TRUE if no normals should be converted and cached.
/// @param[in]  noUVs
///   Set this to TRUE if no UVs should be converted and cached.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::convertAndCacheObject(PolygonObject& object,
                                            Bool           noNormals,
                                            Bool           noUVs)
{
  // clear cache
  mPolygonCache.erase();
  mPointCache.erase();
  mNormalCache.erase();
  mUVCache.erase();
  mQuadCount = 0;

  // get polygons + points and return if there are none
  SizeT           polygonCount = object.GetPolygonCount();
  const CPolygon* polygons     = getPolygons(object);
  SizeT           pointCount   = object.GetPointCount();
  if (!polygonCount || !polygons || !pointCount) {
    return TRUE;
  }

  // copy polygons into cache
  if (!mPolygonCache.init(polygonCount)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheObject(): not enough memory to allocate polygon cache");
  }
  memcpy(mPolygonCache.arrayAddress(), polygons, sizeof(CPolygon)*polygonCount);

  // get normals and store them in FixArray1D as we own the memory
  C4DNormalsT normals;
  Vector*     c4dNormals(0);
  if (!noNormals) {
    c4dNormals = object.CreatePhongNormals();
    if (c4dNormals) {
      normals.setArrayAddress(c4dNormals, polygonCount*4);
      // check if normals are actually just plain face normals
      SizeT polygonIx = 0;
      for (polygonIx=0; polygonIx<polygonCount; ++polygonIx) {
        if (!equalNormals(normals[polygonIx*4], normals[polygonIx*4+1]) ||
            !equalNormals(normals[polygonIx*4], normals[polygonIx*4+2]) ||
            !equalNormals(normals[polygonIx*4], normals[polygonIx*4+3]))
        {
          break;
        }
      }
      // if all normals on each face are the same, we don't have to care about
      // vertex normals at all
      if (polygonIx >= polygonCount) {
        normals.erase();
      }
    }
  }

  // get first UVW tag and read out the UV coordinates (i.e. at the moment we
  // don't support several UV maps, as Lux can store only one Uv per vertex per
  // polygon
  UVsT uvs;
  if (!noUVs) {
    UVWTag* uvwTag = (UVWTag*)object.GetTag(Tuvw);
    if (uvwTag && uvs.init(polygonCount*4)) {
      UVWStruct polygonUVWs;
#if _C4D_VERSION>=115
      const void* tagData = uvwTag->GetDataAddressR();
      for (ULONG polygonIx=0; polygonIx<polygonCount; ++polygonIx) {
        uvwTag->Get(tagData, (LONG)polygonIx, polygonUVWs);
#else
      for (ULONG polygonIx=0; polygonIx<polygonCount; ++polygonIx) {
        polygonUVWs = uvwTag->Get((LONG)polygonIx);
#endif
        uvs[polygonIx*4]   = polygonUVWs.a;
        uvs[polygonIx*4+1] = polygonUVWs.b;
        uvs[polygonIx*4+2] = polygonUVWs.c;
        uvs[polygonIx*4+3] = polygonUVWs.d;
      }
    }
  }

  // log which vertex attributes are available
  if (uvs.size()) {
    debugLog("  it has UV coordinates");
  } else {
    debugLog("  it has no UV coordinates");
  }
  if (normals.size()) {
    debugLog("  it has vertex normals");
  } else if (c4dNormals) {
    debugLog("  it has vertex normals == face normals");
  } else {
    debugLog("  it has no vertex normals");
  }

  // if we have only points and polygons:
  if (!normals.size() && !uvs.size())  return convertAndCacheGeometry(object);
  // if we have only points, polygons and normals:
  if ( normals.size() && !uvs.size())  return convertAndCacheGeometry(object, normals);
  // if we have only points, polygons and UVs:
  if (!normals.size() &&  uvs.size())  return convertAndCacheGeometry(object, uvs);
  // if we have points, polygons, UVs and normals:
  return convertAndCacheGeometry(object, uvs, normals);
}


/// Converts an object without vertex normals and caches the geometry. Basically
/// it just filters out all vertices that are not used.
/// It gets called by convertAndCacheObject(). 
///
/// @param[in]  object
///   The object to convert.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxAPIConverter::convertAndCacheGeometry(PolygonObject& object)
{
  // get points from C4D and polygon count from cache (polygon cache was
  // already created by convertAndCacheObject())
  ULONG         pointCount = object.GetPointCount();
  const Vector* points     = getPoints(object);
  ULONG         polyCount  = mPolygonCache.size();
  if (!pointCount || !points || !polyCount) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheGeometry(): geometry expected, but not found");
  }

  // initialise point map
  FixArray1D<ULONG> pointMap;
  if (!pointMap.init(pointCount)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheGeometry(): not enough memory to allocate point map");
  }
  pointMap.fillWithZero();

  // count how many polygons use each point
  CPolygon* poly;
  ULONG     newPointCount = 0;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    // store pointer to polygon for convenience
    poly = &(mPolygonCache[polyIx]);
    // 1st point of polygon
    if (pointMap[poly->a] == 0) {
      pointMap[poly->a] = MAXULONG;
      ++newPointCount;
    }
    // 2nd point of polygon
    if (pointMap[poly->b] == 0) {
      pointMap[poly->b] = MAXULONG;
      ++newPointCount;
    }
    // 3rd point of polygon
    if (pointMap[poly->c] == 0) {
      pointMap[poly->c] = MAXULONG;
      ++newPointCount;
    }
    // 4th point of polygon (only if it's a quad)
    if (poly->c != poly->d) {
      if (pointMap[poly->d] == 0) {
        pointMap[poly->d] = MAXULONG;
        ++newPointCount;
      }
      ++mQuadCount;
    }
  }

  // log the info we have at the moment
  debugLog("  poly count:         %lu", (unsigned long)polyCount);
  debugLog("  point count:        %lu", (unsigned long)pointCount);
  debugLog("  new point count:    %lu", (unsigned long)newPointCount);

  // initialise point cache array (which will hold only used points)
  if (!mPointCache.init(newPointCount)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheGeometry(): not enough memory to allocate point map");
  }

  // now fill point cache with used points and update polygons
  ULONG oldPointIx, newPointIx = 0;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    // store pointer to polygon for convenience
    poly = &(mPolygonCache[polyIx]);
    // 1st point of polygon
    if (pointMap[oldPointIx = poly->a] == MAXULONG) {
      mPointCache[newPointIx] = points[oldPointIx] * mC4D2LuxScale;
      pointMap[oldPointIx] = newPointIx;
      ++newPointIx;
    }
    poly->a = pointMap[oldPointIx];
    // 2nd point of polygon
    if (pointMap[oldPointIx = poly->b] == MAXULONG) {
      mPointCache[newPointIx] = points[oldPointIx] * mC4D2LuxScale;
      pointMap[oldPointIx] = newPointIx;
      ++newPointIx;
    }
    poly->b = pointMap[oldPointIx];
    // 3rd point of polygon
    if (pointMap[oldPointIx = poly->c] == MAXULONG) {
      mPointCache[newPointIx] = points[oldPointIx] * mC4D2LuxScale;
      pointMap[oldPointIx] = newPointIx;
      ++newPointIx;
    }
    poly->c = pointMap[oldPointIx];
    // 4th point of polygon
    if (pointMap[oldPointIx = poly->d] == MAXULONG) {
      mPointCache[newPointIx] = points[oldPointIx] * mC4D2LuxScale;
      pointMap[oldPointIx] = newPointIx;
      ++newPointIx;
    }
    poly->d = pointMap[oldPointIx];
  }

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIx == newPointCount);

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
Bool LuxAPIConverter::convertAndCacheGeometry(PolygonObject&     object,
                                              const C4DNormalsT& normals)
{
  // The container type for storing the point-to-polygon data.
  typedef FixArray1D<Point2PolyN>  Point2PolyMapT;


  // initialise point map
  ULONG         pointCount;
  const Vector* points;
  PointMapT     pointMap;
  if (!setupPointMap(object, pointCount, points, pointMap)) {
    return FALSE;
  }

  // initialise point2poly map
  Point2PolyMapT point2PolyMap;
  if (!point2PolyMap.init(pointMap[pointCount])) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithNormals(): not enough memory to allocate poin2PolyMap");
  }
  point2PolyMap.fillWithZero();

  // Collect point2poly information and create new entries only for points
  // with distinct normals. It basically works like that:
  // for each polygon
  //   for each point of the polygon
  //     find entry in point2poly array with same normals
  //     if found: store array offset of entry as temporary point index
  //     else:     store array offset of first free entry as temporary point index
  //               store new entry in point2poly map
  ULONG         polyCount = mPolygonCache.size();
  CPolygon*     poly;
  ULONG         newPointCount = 0;
  Point2PolyN*  point2Poly;
  const Vector* normal = &(normals[0]);
  Bool          isQuad;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly   = &mPolygonCache[polyIx];
    isQuad = (poly->c != poly->d);
    // 1st point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->a]]); point2Poly->normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->normalRef), *normal))  break;
    }
    poly->a = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->normalRef) {
      point2Poly->normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 2nd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->b]]); point2Poly->normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->normalRef), *normal))  break;
    }
    poly->b = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->normalRef) {
      point2Poly->normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 3rd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->c]]); point2Poly->normalRef; ++point2Poly) {
      if (equalNormals(*(point2Poly->normalRef), *normal))  break;
    }
    poly->c = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->normalRef) {
      point2Poly->normalRef = normal;
      ++newPointCount;
    }
    ++normal;
    // 4th point of polygon
    if (isQuad) {
      for (point2Poly=&(point2PolyMap[pointMap[poly->d]]); point2Poly->normalRef; ++point2Poly) {
        if (equalNormals(*(point2Poly->normalRef), *normal))  break;
      }
      poly->d = (LONG)(point2Poly - point2PolyMap.arrayAddress());
      if (!point2Poly->normalRef) {
        point2Poly->normalRef = normal;
        ++newPointCount;
      }
    } else {
      poly->d = poly->c;
    }
    ++normal;
  }
  debugLog("  new point count:     %lu", (unsigned long)newPointCount);

  // initialise point cache and normal cache
  if (!mPointCache.init(newPointCount) || !mNormalCache.init(newPointCount)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithNormals(): not enough memory to allocate point and/or normal cache");
  }

  // now determine new node IDs and fill normal and point caches
  ULONG  newPointIx = 0;
  Vector normalised;
  for (ULONG pointIx=0; pointIx<pointCount; ++pointIx) {
    for (ULONG point2PolyIx=pointMap[pointIx]; point2PolyIx<pointMap[pointIx+1]; ++point2PolyIx) {
      point2Poly = &(point2PolyMap[point2PolyIx]);
      if (!point2Poly->normalRef)  break;
      mPointCache[newPointIx]  = points[pointIx] * mC4D2LuxScale;
      normalised               = *(point2Poly->normalRef);
      mNormalCache[newPointIx] = normalize(normalised);
      point2Poly->newPoint     = newPointIx;
      ++newPointIx;
    }
  }

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIx == newPointCount);

  // set new points in polygon
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly = &mPolygonCache[polyIx];
    poly->a = point2PolyMap[poly->a].newPoint;
    poly->b = point2PolyMap[poly->b].newPoint;
    poly->c = point2PolyMap[poly->c].newPoint;
    poly->d = point2PolyMap[poly->d].newPoint;
  }

  // set cached object (== activate cache) and return
  mCachedObject = &object;
  return TRUE;
}


///
Bool LuxAPIConverter::convertAndCacheGeometry(PolygonObject& object,
                                              const UVsT&    uvs)
{
  // The container type for storing the point-to-polygon data.
  typedef FixArray1D<Point2PolyU>  Point2PolyMapT;


  // initialise point map
  ULONG         pointCount;
  const Vector* points;
  PointMapT     pointMap;
  if (!setupPointMap(object, pointCount, points, pointMap)) {
    return FALSE;
  }

  // initialise point2poly map
  Point2PolyMapT point2PolyMap;
  if (!point2PolyMap.init(pointMap[pointCount])) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithUVs(): not enough memory to allocate poin2PolyMap");
  }
  point2PolyMap.fillWithZero();

  // Collect point2poly information and create new entries only for points
  // with distinct UVs. It basically works like that:
  // for each polygon
  //   for each point of the polygon
  //     find entry in point2poly array with same UVs
  //     if found: store array offset of entry as temporary point index
  //     else:     store array offset of first free entry as temporary point index
  //               store new entry in point2poly map
  ULONG              polyCount = mPolygonCache.size();
  CPolygon*          poly;
  ULONG              newPointCount = 0;
  Point2PolyU*       point2Poly;
  const LuxVector2D* uv = &(uvs[0]);
  Bool               isQuad;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly   = &mPolygonCache[polyIx];
    isQuad = (poly->c != poly->d);
    // 1st point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->a]]); point2Poly->uvRef; ++point2Poly) {
      if (equalUVs(*(point2Poly->uvRef), *uv))  break;
    }
    poly->a = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->uvRef) {
      point2Poly->uvRef = uv;
      ++newPointCount;
    }
    ++uv;
    // 2nd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->b]]); point2Poly->uvRef; ++point2Poly) {
      if (equalUVs(*(point2Poly->uvRef), *uv))  break;
    }
    poly->b = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->uvRef) {
      point2Poly->uvRef = uv;
      ++newPointCount;
    }
    ++uv;
    // 3rd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->c]]); point2Poly->uvRef; ++point2Poly) {
      if (equalUVs(*(point2Poly->uvRef), *uv))  break;
    }
    poly->c = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->uvRef) {
      point2Poly->uvRef = uv;
      ++newPointCount;
    }
    ++uv;
    // 4th point of polygon
    if (isQuad) {
      for (point2Poly=&(point2PolyMap[pointMap[poly->d]]); point2Poly->uvRef; ++point2Poly) {
        if (equalUVs(*(point2Poly->uvRef), *uv))  break;
      }
      poly->d = (LONG)(point2Poly - point2PolyMap.arrayAddress());
      if (!point2Poly->uvRef) {
        point2Poly->uvRef = uv;
        ++newPointCount;
      }
    } else {
      poly->d = poly->c;
    }
    ++uv;
  }
  debugLog("  new point count:     %lu", (unsigned long)newPointCount);

  // initialise point cache and normal cache
  if (!mPointCache.init(newPointCount) || !mUVCache.init(newPointCount)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithUVs(): not enough memory to allocate point and/or UV cache");
  }

  // now determine new node IDs and fill normal and point caches
  ULONG newPointIx = 0;
  for (ULONG pointIx=0; pointIx<pointCount; ++pointIx) {
    for (ULONG point2PolyIx=pointMap[pointIx]; point2PolyIx<pointMap[pointIx+1]; ++point2PolyIx) {
      point2Poly = &(point2PolyMap[point2PolyIx]);
      if (!point2Poly->uvRef)  break;
      mPointCache[newPointIx] = points[pointIx] * mC4D2LuxScale;
      mUVCache[newPointIx] = *(point2Poly->uvRef);
      point2Poly->newPoint = newPointIx;
      ++newPointIx;
    }
  }

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIx == newPointCount);

  // set new points in polygon
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly = &mPolygonCache[polyIx];
    poly->a = point2PolyMap[poly->a].newPoint;
    poly->b = point2PolyMap[poly->b].newPoint;
    poly->c = point2PolyMap[poly->c].newPoint;
    poly->d = point2PolyMap[poly->d].newPoint;
  }

  // set cached object (== activate cache) and return
  mCachedObject = &object;
  return TRUE;
}


/// This function takes the point and UV coordinates and normals of all
/// polygons and creates for each UV/normal combination of a point one
/// instance in the point cache. ( 
/// combinations for different polygons. We do that because Lux supports only
/// one UV coordinate and one normal per point.
///
Bool LuxAPIConverter::convertAndCacheGeometry(PolygonObject&     object,
                                              const UVsT&        uvs,
                                              const C4DNormalsT& normals)
{
  // The container type for storing the point-to-polygon data.
  typedef FixArray1D<Point2PolyUN>  Point2PolyMapT;


  // initialise point map
  ULONG         pointCount;
  const Vector* points;
  PointMapT     pointMap;
  if (!setupPointMap(object, pointCount, points, pointMap)) {
    return FALSE;
  }

  // initialise point2poly map
  Point2PolyMapT point2PolyMap;
  if (!point2PolyMap.init(pointMap[pointCount])) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithUVsAndNormals(): not enough memory to allocate poin2PolyMap");
  }
  point2PolyMap.fillWithZero();

  // Collect point2poly information and create new entries only for points
  // with distinct UVs. It basically works like that:
  // for each polygon
  //   for each point of the polygon
  //     find entry in point2poly array with same UVs and normals
  //     if found: store array offset of entry as temporary point index
  //     else:     store array offset of first free entry as temporary point index
  //               store new entry in point2poly map
  ULONG              polyCount = mPolygonCache.size();
  CPolygon*          poly;
  ULONG              newPointCount = 0;
  Point2PolyUN*      point2Poly;
  const LuxVector2D* uv = &(uvs[0]);
  const Vector*      normal = &(normals[0]);
  Bool               isQuad;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly   = &mPolygonCache[polyIx];
    isQuad = (poly->c != poly->d);
    // 1st point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->a]]); point2Poly->ref.uv; ++point2Poly) {
      if (equalUVs(*(point2Poly->ref.uv), *uv) &&
          equalNormals(*(point2Poly->ref.normal), *normal))
      {
        break;
      }
    }
    poly->a = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->ref.uv) {
      point2Poly->ref.uv     = uv;
      point2Poly->ref.normal = normal;
      ++newPointCount;
    }
    ++uv;
    ++normal;
    // 2nd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->b]]); point2Poly->ref.uv; ++point2Poly) {
      if (equalUVs(*(point2Poly->ref.uv), *uv) &&
          equalNormals(*(point2Poly->ref.normal), *normal))
      {
        break;
      }
    }
    poly->b = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->ref.uv) {
      point2Poly->ref.uv     = uv;
      point2Poly->ref.normal = normal;
      ++newPointCount;
    }
    ++uv;
    ++normal;
    // 3rd point of polygon
    for (point2Poly=&(point2PolyMap[pointMap[poly->c]]); point2Poly->ref.uv; ++point2Poly) {
      if (equalUVs(*(point2Poly->ref.uv), *uv) &&
          equalNormals(*(point2Poly->ref.normal), *normal))
      {
        break;
      }
    }
    poly->c = (LONG)(point2Poly - point2PolyMap.arrayAddress());
    if (!point2Poly->ref.uv) {
      point2Poly->ref.uv     = uv;
      point2Poly->ref.normal = normal;
      ++newPointCount;
    }
    ++uv;
    ++normal;
    // 4th point of polygon
    if (isQuad) {
      for (point2Poly=&(point2PolyMap[pointMap[poly->d]]); point2Poly->ref.uv; ++point2Poly) {
        if (equalUVs(*(point2Poly->ref.uv), *uv) &&
            equalNormals(*(point2Poly->ref.normal), *normal))
        {
          break;
        }
      }
      poly->d = (LONG)(point2Poly - point2PolyMap.arrayAddress());
      if (!point2Poly->ref.uv) {
        point2Poly->ref.uv     = uv;
        point2Poly->ref.normal = normal;
        ++newPointCount;
      }
    } else {
      poly->d = poly->c;
    }
    ++uv;
    ++normal;
  }
  debugLog("  new point count:     %lu", (unsigned long)newPointCount);

  // initialise point cache and normal cache
  if (!mPointCache.init(newPointCount) ||
      !mUVCache.init(newPointCount) ||
      !mNormalCache.init(newPointCount))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::convertAndCacheWithUVsAndNormals(): not enough memory to allocate point, UV and/or normal cache");
  }

  // now determine new node IDs and fill normal and point caches
  ULONG  newPointIx = 0;
  Vector normalised;
  for (ULONG pointIx=0; pointIx<pointCount; ++pointIx) {
    for (ULONG point2PolyIx=pointMap[pointIx]; point2PolyIx<pointMap[pointIx+1]; ++point2PolyIx) {
      point2Poly = &(point2PolyMap[point2PolyIx]);
      if (!point2Poly->ref.uv)  break;
      mPointCache[newPointIx]  = points[pointIx] * mC4D2LuxScale;
      mUVCache[newPointIx]     = *(point2Poly->ref.uv);
      normalised               = *(point2Poly->ref.normal);
      mNormalCache[newPointIx] = normalize(normalised);
      point2Poly->newPoint = newPointIx;
      ++newPointIx;
    }
  }

  // if that is not true, there is a hole in the logic
  GeAssert(newPointIx == newPointCount);

  // set new points in polygon
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly = &mPolygonCache[polyIx];
    poly->a = point2PolyMap[poly->a].newPoint;
    poly->b = point2PolyMap[poly->b].newPoint;
    poly->c = point2PolyMap[poly->c].newPoint;
    poly->d = point2PolyMap[poly->d].newPoint;
  }

  // set cached object (== activate cache) and return
  mCachedObject = &object;
  return TRUE;
}


/// This functions creates an array with n+1 entries, where n is the point
/// count of the object. This array will be a helper index into the
/// point2PolyMap, which will be created later:
///
/// The entry range for vertex i in the point2PolyMap is
///   pointMap[i] ... pointMap[i+1]-1
///
/// The last entry contains the size of the point2PolyMap.
///
/// @param[in]  object
///   The polygon object that is currently converted and cached.
/// @param[out]  pointCount
///   The number of points of the polygon object (is provided for convenience).
/// @param[out]  points
///   The pointer to the point coordinate array (is provided for convenience).
/// @param[out]  pointMap
///   The point map which is created.
/// @return
///   TRUE if executed successfully, FALSE otherwise.
Bool LuxAPIConverter::setupPointMap(PolygonObject& object,
                                    ULONG&         pointCount,
                                    const Vector*& points,
                                    PointMapT&     pointMap)
{
  // get points from C4D and polygon count from cache (polygons were already
  // created by convertAndCacheGeometry())
  ULONG polyCount = mPolygonCache.size();
  pointCount      = object.GetPointCount();
  points          = getPoints(object);
  if (!polyCount || !pointCount || !points) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::setupPointMap(): geometry expected, but not found");
  }

  // initialise point map
  if (!pointMap.init(pointCount+1)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::setupPointMap(): not enough memory to allocate point map");
  }
  pointMap.fillWithZero();

  // count number of polygons per point (+ number of quads)
  CPolygon* poly;
  for (ULONG polyIx=0; polyIx<polyCount; ++polyIx) {
    poly = &(mPolygonCache[polyIx]);
    ++pointMap[poly->a];
    ++pointMap[poly->b];
    ++pointMap[poly->c];
    if (poly->c != poly->d) {
      ++pointMap[poly->d];
      ++mQuadCount;
    }
  }
  debugLog("  poly count:          %lu", (unsigned long)polyCount);
  debugLog("  point count:         %lu", (unsigned long)pointCount);

  // convert polygon counts of point map into start positions in point2Poly map
  ULONG point2PolyCount, point2PolyMapSize = 0;
  for (ULONG point=0; point<=pointCount; ++point) {
    point2PolyCount   =  pointMap[point];
    pointMap[point]   =  point2PolyMapSize;
    point2PolyMapSize += point2PolyCount;
  }
  debugLog("  point2poly map size: %lu", (unsigned long)point2PolyMapSize);

  // make sure that there are not too many polygons, which might cause integer
  // overflows somewhere else
  if (point2PolyMapSize > MAXLONG) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::setupPointMap(): point2PolyMap will be too big for object '" + object.GetName() + "'");
  }

  return TRUE;
}
