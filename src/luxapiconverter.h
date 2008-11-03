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



#include "c4d.h"

#include "fixarray1d.h"
#include "luxapi.h"
#include "luxc4dsettings.h"



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

  /// Helper structure to keep track of the visibility in the hierarchy.
  struct HierarchyData {

    enum ObjectType {
      POLYGON_OBJECTS,
      LIGHT_OBJECTS
    };

    Bool       mVisible;
    ObjectType mObjectType;


    HierarchyData(void)
    : mVisible(TRUE), mObjectType(POLYGON_OBJECTS)
    {}

    HierarchyData(Bool visible, ObjectType objectType)
    : mVisible(visible), mObjectType(objectType)
    {}
  };
  

  /// Helper structure for storing normal and UV references for each polygon a
  /// point is connected to. (For every combination point/poly combination there
  /// is one instance stored)
  /// To safe memory, this structure will be overwritten with the new point ID
  /// in the cached geometry. (Hence the union)
  union Point2Poly {
    struct {
      Vector* normalRef;
      // TODO: uvRef
    } data;
    ULONG newPoint;
  } Point2PolyT;

  /// The container type for storing the point-to-polygon data.
  typedef FixArray1D<Point2Poly>  Point2PolysT;

  /// The container type for storing a selection of triangle IDs.
  typedef FixArray1D<ULONG>       TriangleIDsT;
  /// The container type for storing the point IDs of triangles.
  typedef FixArray1D<LuxIntegerT> TrianglesT;
  /// The container type for storing point positions.
  typedef FixArray1D<LuxPointT>   PointsT;
  /// The contianer type for storing normal vectors.
  typedef FixArray1D<LuxNormalT>  NormalsT;

  /// The container type for storing C4D polygons.
  typedef FixArray1D<CPolygon>    C4DPolygonsT;
  /// The container type for storing C4D normal vectors.
  typedef FixArray1D<Vector>      C4DNormalsT;


  // references used by the whole conversion process and stored for convenience
  BaseDocument*   mDocument;
  LuxAPI*         mReceiver;
  BaseContainer*  mC4DRenderSettings;
  LuxC4DSettings* mLuxC4DSettings;
  LReal           mC4D2LuxScale;
  LuxParamSet     mTempParamSet;

  // the following data members store the geometry for one object which is
  // cached by extractAndConvertGeometry()
  BaseObject*     mCachedObject;
  C4DPolygonsT    mPolygonCache;
  PointsT         mPointCache;
  NormalsT        mNormalCache;
  ULONG           mQuadCount;

  
  Bool exportFilm(void);
  Bool exportCamera(void);
  Bool exportPixelFilter(void);
  Bool exportSampler(void);
  Bool exportSurfaceIntegrator(void);
  Bool exportAccelerator(void);

  Bool exportLights(void);
  Bool exportTextures(void);
  Bool exportGeometry(void);

  Bool exportLight(BaseObject&   object,
                   const Matrix& globalMatrix);

  Bool exportPolygonObject(PolygonObject& object,
                           const Matrix&  globalMatrix);
  Bool convertGeometry(PolygonObject& object,
                       TrianglesT&    triangles,
                       PointsT&       points,
                       NormalsT&      normals);
  Bool convertAndCacheGeometry(PolygonObject& object);
  Bool convertAndCacheWithoutNormals(PolygonObject& object);
  Bool convertAndCacheWithNormals(PolygonObject& object,
                                  C4DNormalsT&   normals);
};



#endif  // #ifndef __LUXAPICONVERTER_H__