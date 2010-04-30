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

#include "dynarray1d.h"
#include "fixarray1d.h"
#include "luxapi.h"
#include "luxc4dportaltag.h"
#include "luxc4dsettings.h"
#include "luxmaterialdata.h"
#include "luxtexturedata.h"
#include "rbtreeset.h"
#include "rbtreemap.h"



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

  /// Helper structure to keep track of visibility and textures during
  /// hierarchy traversal.
  struct HierarchyData {

    Bool      mVisible;
    String    mObjectName;
    LuxString mMaterialName;
    Bool      mHasEmissionChannel;
    LuxString mLightGroup;

    HierarchyData(Bool visible=TRUE)
    : mVisible(visible),
      mHasEmissionChannel(FALSE)
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
    LuxColor       mColor;
    LuxFloat       mGain;
    LuxBool        mFlippedNormals;
    LuxInteger     mSamples; 
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
    Bool       mAdvanced;
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
    Bool       mAdvanced;
    LuxFloat   mAConst;
    LuxFloat   mBConst;
    LuxFloat   mCConst;
    LuxFloat   mDConst;
    LuxFloat   mEConst;
  };

  /// Structure that stores all parameters of a infinite light
  struct InfiniteLightData {
    LONG     mType;
    LuxFloat mGain;
    LuxColor mColor;
    Filename mFilename;
  };


  // Stores the name and additional information of a material, that can be
  // reused.
  struct ReusableMaterial {
    LuxString mName;
    Bool      mHasEmissionChannel;
    LuxString mLightGroup;

    ReusableMaterial(const LuxString& name,
                     Bool             hasEmissionChannel,
                     const LuxString& lightGroup)
    : mName(name), mHasEmissionChannel(hasEmissionChannel), mLightGroup(lightGroup)
    {}

    ReusableMaterial(const ReusableMaterial& other)
    {
      *this = other;
    }

    ReusableMaterial& operator=(const ReusableMaterial& other)
    {
      mName               = other.mName;
      mHasEmissionChannel = other.mHasEmissionChannel;
      mLightGroup         = other.mLightGroup;
      return *this;
    }
  };


  // Union which is used during conversion of vertices with normals.
  union Point2PolyN {
    const Vector* normalRef;
    ULONG         newPoint;
  };

  // Union which is used during conversion of vertices with UVs.
  union Point2PolyU {
    const LuxVector2D* uvRef;
    ULONG              newPoint;
  };

  // Union which is used during conversion of vertices with UVs and normals.
  union Point2PolyUN {
    struct {
      const LuxVector2D* uv;
      const Vector*      normal;
    }                  ref;
    ULONG              newPoint;
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

  /// The container type for storing a set of objects.
  typedef RBTreeSet<BaseList2D*>                           ObjectsT;
  /// The map from material name to number of materials that use this name.
  typedef RBTreeMap<String, LONG>                          MaterialUsageMapT;
  /// The container type for storing the texture tags of an object.
  typedef DynArray1D<TextureTag*>                          TextureTagsT;
  /// The container type for storing C4D polygons.
  typedef FixArray1D<CPolygon>                             C4DPolygonsT;
  /// The container type for storing C4D normal vectors.
  typedef FixArray1D<Vector>                               C4DNormalsT;
  /// Helper array, which is used during the geometry conversion.
  typedef FixArray1D<ULONG>                                PointMapT;


  // static costants
  static SizeT cMaxTextureTags;

  // references used by the whole conversion process and stored for convenience
  BaseDocument*   mDocument;
  LuxAPI*         mReceiver;
  BaseContainer*  mC4DRenderSettings;
  LuxC4DSettings* mLuxC4DSettings;
  LReal           mC4D2LuxScale;
  Real            mBumpSampleDistance;
  Real            mColorGamma;
  Real            mTextureGamma;

  // temporary data stored during the conversion and shared between
  // several functions
  LuxParamSet        mTempParamSet;
  Bool               mIsBidirectional;
  CameraObject*      mCamera;
  BaseObject*        mSkyObject;
  ULONG              mPortalCount;
  ULONG              mLightCount;
  ObjectsT           mAreaLightObjects;
  MaterialUsageMapT  mMaterialUsage;

  // the currently cached object
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

  // the current conversion function that will be called by Do()
  Bool            (LuxAPIConverter::*mDo)(HierarchyData& hierarchyData,
                                          BaseObject&    object,
                                          const Matrix&  globalMatrix,
                                          Bool           controlObject);


  void clearTemporaryData(void);
  Bool obtainGlobalSceneData(void);
  
  Bool exportFilm(void);
  Bool exportCamera(void);
  Bool exportPixelFilter(void);
  Bool exportSampler(void);
  Bool exportSurfaceIntegrator(void);
  Bool exportAccelerator(void);

  Bool exportLights(void);
  Bool doLightExport(HierarchyData& data,
                     BaseObject&    object,
                     const Matrix&  globalMatrix,
                     Bool           controlObject);

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

  Bool exportInfiniteLight(void);

  Bool exportStandardMaterial(void);

  Bool exportGeometry(void);
  Bool doGeometryExport(HierarchyData& data,
                        BaseObject&    object,
                        const Matrix&  globalMatrix,
                        Bool           controlObject);

  Bool exportMaterial(BaseObject&   object,
                      TextureTagsT& textureTags,
                      LuxString&    materialName,
                      Bool&         hasEmissionChannel,
                      LuxString&    lightGroup);

  LuxMaterialDataH convertDummyMaterial(BaseMaterial& material);
  LuxMaterialDataH convertDiffuseMaterial(LuxTextureMappingH mapping,
                                          Material&          material);
  LuxMaterialDataH convertGlossyMaterial(LuxTextureMappingH mapping,
                                         Material&          material);
  LuxMaterialDataH convertReflectiveMaterial(LuxTextureMappingH mapping,
                                             Material&          material);
  LuxMaterialDataH convertTransparentMaterial(LuxTextureMappingH mapping,
                                              Material&          material);
  LuxMaterialDataH convertTranslucentMaterial(LuxTextureMappingH mapping,
                                              Material&          material);

  Bool addBumpChannel(LuxTextureMappingH mapping,
                      Material&          material,
                      LuxMaterialData&   materialData);
  Bool addEmissionChannel(LuxTextureMappingH mapping,
                          Material&          material,
                          LuxMaterialData&   materialData);
  Bool addAlphaChannel(LuxTextureMappingH mapping,
                       Material&          material,
                       LuxMaterialData&   materialData);

  LuxTextureDataH convertFloatChannel(LuxTextureMappingH        mapping,
                                      Material&                 material,
                                      LONG                      shaderId,
                                      LONG                      strengthId,
                                      Real                      strengthScale = 1.0f,
                                      LuxImageMapData::Channel  channel = LuxImageMapData::IMAGE_CHANNEL_NONE);
  LuxTextureDataH convertColorChannel(LuxTextureMappingH mapping,
                                      Material&          material,
                                      LONG               shaderId,
                                      LONG               colorId,
                                      LONG               brightnessId,
                                      LONG               mixerId);

  Bool exportPolygonObject(PolygonObject& object,
                           const Matrix&  globalMatrix);
  Bool exportPortalObject(PolygonObject& object,
                          const Matrix&  globalMatrix,
                          BaseTag&       tag,
                          Bool&          exportObject);
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
                     PointMapT&     pointMap);
};



#endif  // #ifndef __LUXAPICONVERTER_H__
