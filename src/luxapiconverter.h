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

#ifndef __LUXAPICONVERTER_H__
#define __LUXAPICONVERTER_H__ 1



#include <c4d.h>

#include "fixarray1d.h"
#include "luxapi.h"
#include "luxc4dsettings.h"
#include "rbtreeset.h"



/***************************************************************************//*!
 The class does the actual conversion from a CINEMA 4D scene to a Lux scene.
 It creates Lux API compatible data, which is then streamed to a LuxAPI
 implementation, which consumes the data (e.g. writes it into a file or sends
 it to a Lux renderer)

 It's derived from the class Hierarchy which allows it to traverse the whole
 object tree, while deformers and generators are evaluated. This is done all
 internally in CINEMA 4D and should just always work :)
*//****************************************************************************/
class LuxAPIConverter : public Hierarchy
{
public:

  LuxAPIConverter(void);
  ~LuxAPIConverter(void);

  Bool convertScene(BaseDocument& document,
                    LuxAPI&       receiver);

  // Callback functions called while Hierarchy traverses the hierarchy.
  virtual void* Alloc(void);
  virtual void Free(void* data);
  virtual void CopyTo(void* src,
                      void* dst);
  virtual Bool Do(void*         data,
                  BaseObject*   object,
                  const Matrix& globalMatrix,
                  Bool          controlObject);


private:

  /// Enum to differentiate between different hierarchy traverses.
  enum ObjectType {
    UNSPECIFIED_OBJECTS=0,
    POLYGON_OBJECTS,
    LIGHT_OBJECTS
  };


  /// Helper structure to keep track of visibility during hierarchy traversal.
  struct HierarchyData {

    Bool       mVisible;

    HierarchyData(Bool visible = TRUE)
    : mVisible(visible)
    {}
  };
 

  /// Structure that stores all parameters of a point light.
  struct PointLightData {
    LuxColor mColor;
    LuxFloat mGain;
    LuxPoint mFrom;
  };

  /// Structure that stores all parameters of a spot light.
  struct SpotLightData {
    LuxColor mColor;
    LuxFloat mGain;
    LuxPoint mFrom;
    LuxPoint mTo;
    LuxFloat mConeAngle;
    LuxFloat mConeDeltaAngle;
  };

  /// Structure that stores all parameters of a distant light.
  struct DistantLightData {
    LuxColor mColor;
    LuxFloat mGain;
    LuxPoint mFrom;
    LuxPoint mTo;
  };

  /// Structure that stores all parameters of an area light.
  struct AreaLightData {
    LuxColor      mColor;
    LuxFloat      mGain;
    LuxBool       mFlippedNormals;
    LuxInteger    mSamples; 
    Matrix         mLightMatrix;
    LONG           mShape;
    Vector         mSize;
    PolygonObject* mShapeObject;
  };

  /// Structure that stores all parameters of a sun light.
  struct SunLightData {
    LuxFloat   mGain;
    LuxInteger mSamples; 
    LuxVector  mSunDir;
    LuxFloat   mTurbidity;
    LuxFloat   mRelSize;
  };

  /// Structure that stores all parameters of a sky light.
  struct SkyLightData {
    LuxFloat   mGain;
    LuxInteger mSamples; 
    LuxVector  mSunDir;
    LuxFloat   mTurbidity;
    Bool        mAdvanced;
    LuxFloat   mAConst;
    LuxFloat   mBConst;
    LuxFloat   mCConst;
    LuxFloat   mDConst;
    LuxFloat   mEConst;
  };

  /// Structure that stores all parameters of a sun+sky light.
  struct SunSkyLightData {
    LuxFloat   mGain;
    LuxInteger mSamples; 
    LuxVector  mSunDir;
    LuxFloat   mTurbidity;
    LuxFloat   mRelSize;
    Bool        mAdvanced;
    LuxFloat   mAConst;
    LuxFloat   mBConst;
    LuxFloat   mCConst;
    LuxFloat   mDConst;
    LuxFloat   mEConst;
  };

  /// The container type for storing a selection of triangle IDs.
  typedef FixArray1D<ULONG>       TriangleIDsT;
  /// The container type for storing the point IDs of triangles.
  typedef FixArray1D<LuxInteger>  TrianglesT;
  /// The container type for storing the point IDs of triangles.
  typedef FixArray1D<LuxInteger>  QuadsT;
  /// The container type for storing point positions.
  typedef FixArray1D<LuxPoint>    PointsT;
  /// The contianer type for storing normal vectors.
  typedef FixArray1D<LuxNormal>   NormalsT;
  /// The container type for storing UV coordinates as 2D vectors.
  typedef FixArray1D<LuxVector2D> UVsT;
  /// The container type for storing UV coordinates as float array.
  typedef FixArray1D<LuxFloat>    UVsSerialisedT;

  /// The container type for storing C4D polygons.
  typedef FixArray1D<CPolygon>    C4DPolygonsT;
  /// The container type for storing C4D normal vectors.
  typedef FixArray1D<Vector>      C4DNormalsT;
  /// The container type for storing a set of objects.
  typedef RBTreeSet<BaseList2D*>  ObjectsT;
  ///
  typedef FixArray1D<ULONG>       PointMapT;


  // references used by the whole conversion process and stored for convenience
  BaseDocument*   mDocument;
  LuxAPI*         mReceiver;
  BaseContainer*  mC4DRenderSettings;
  LuxC4DSettings* mLuxC4DSettings;
  LReal           mC4D2LuxScale;

  // temporary data stored during the conversion and shared between
  // several functions
  LuxParamSet     mTempParamSet;
  CameraObject*   mCamera;
  ObjectType      mObjectType;
  ULONG           mLightCount;
  ObjectsT        mAreaLightObjects;
  BaseObject*     mCachedObject;
  // stores all C4D polygons of the current object, matching mPointCache
  C4DPolygonsT    mPolygonCache;
  // stores all points of the current object, in Lux coordinates
  PointsT         mPointCache;
  // stores all normals of the current object, in Lux coordinates
  NormalsT        mNormalCache;
  // stores all UVs of the current object
  UVsT            mUVCache;
  // number of quads in polygon cache (used for calculating triangle count)
  ULONG           mQuadCount;


  void clearTemporaryData(void);
  
  Bool exportFilm(void);
  Bool exportCamera(void);
  Bool exportPixelFilter(void);
  Bool exportSampler(void);
  Bool exportSurfaceIntegrator(void);
  Bool exportAccelerator(void);

  Bool exportLights(void);

  Bool exportLight(BaseObject&   object,
                   const Matrix& globalMatrix);
  Bool exportPointLight(PointLightData& data);
  Bool exportSpotLight(SpotLightData& data);
  Bool exportDistantLight(DistantLightData& data);
  Bool exportAreaLight(AreaLightData& data);
  Bool exportSunLight(SunLightData& data);
  Bool exportSkyLight(SkyLightData& data);
  Bool exportSunSkyLight(SunSkyLightData& data);
  
  Bool exportAutoLight(void);

  Bool exportStandardMaterial(void);

  Bool exportGeometry(void);

  Bool exportPolygonObject(PolygonObject& object,
                           const Matrix&  globalMatrix);
  Bool convertGeometry(PolygonObject&  object,
                       TrianglesT&     triangles,
                       PointsT&        points,
                       NormalsT*       normals = 0,
                       UVsSerialisedT* uvs = 0);
  Bool convertAndCacheObject(PolygonObject& object,
                             Bool           noNormals,
                             Bool           noUVs);
  Bool convertAndCacheGeometry(PolygonObject& object);
  Bool convertAndCacheGeometry(PolygonObject&     object,
                               const C4DNormalsT& normals);
  Bool convertAndCacheGeometry(PolygonObject& object,
                               const UVsT&    uvs);
  Bool convertAndCacheGeometry(PolygonObject&     object,
                               const UVsT&        uvs,
                               const C4DNormalsT& normals);

  Bool setupPointMap(PolygonObject& object,
                     ULONG&         pointCount,
                     const Vector*& points,
                     PointMapT&     pointMap,
                     ULONG&         point2PolyMapSize);
};



#endif  // #ifndef __LUXAPICONVERTER_H__
