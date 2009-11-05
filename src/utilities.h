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

#ifndef __UTILITIES_H__
#define __UTILITIES_H__ 1



#include <c4d.h>

#include "luxtypes.h"



/*****************************************************************************
 * Primitive but effective error logging and handling
 *****************************************************************************/

/// Logs/prints an error message.
#define ERRLOG(msg)                                                           \
  { GePrint(msg); }

/// Logs/prints an error message and returns without a return value.
#define ERRLOG_RETURN(msg)                                                    \
  { ERRLOG(msg);  return; }

/// Logs/prints an error message and returns with the specified return value.
#define ERRLOG_RETURN_VALUE(returnValue,msg)                                  \
  { ERRLOG(msg);  return returnValue; }

/// Logs/prints an error message, defined by an ID and returns with the
/// specified return value.
#define ERRLOG_ID_RETURN_VALUE(returnValue,id,msg)                            \
  { mErrorStringID=(id);  ERRLOG(msg);  return returnValue; }



/*****************************************************************************
 * Common types
 *****************************************************************************/

typedef VULONG  SizeT;



/*****************************************************************************
 * Functions
 *****************************************************************************/

void debugLog(const CHAR* format, ...);

void debugLog(const String& msg);

void convert2LuxString(const String&  c4dString,
                       LuxString&     luxString,
                       StringEncoding encoding=StUTF8);

void convert2LuxString(const Filename& c4dPath,
                       LuxString&      luxString,
                       StringEncoding  encoding=StUTF8);

void showParameter(Description* description,
                   LONG         paramID,
                   AtomArray*   params,
                   Bool         show);

LONG getParameterLong(C4DAtom& object,
                      LONG     paramID,
                      LONG     preset = 0);
Real getParameterReal(C4DAtom& object,
                      LONG     paramID,
                      Real     preset = 0.0);
Vector getParameterVector(C4DAtom&      object,
                          LONG          paramID,
                          const Vector& preset = Vector());
String getParameterString(C4DAtom&      atom,
                          LONG          paramID,
                          const String& preset = String());
Filename getParameterFilename(C4DAtom&        atom,
                              LONG            paramID,
                              const Filename& preset = Filename());
BaseList2D* getParameterLink(GeListNode& node,
                             LONG        paramID,
                             LONG        instanceOf = 0);

BaseTag* findTagForParamObject(BaseObject* object,
                               LONG        tagId);

//Filename getRelativePath(const Filename& path,
//                         const Filename& startPath);

#if defined(__MAC) && (_C4D_VERSION<100)
String convertToPosixPath(const Filename& path);
#endif



/*****************************************************************************
 * Inlined functions
 *****************************************************************************/

/// Replacement of Vector::Normalize() as it was added later to Vector and is
/// not available in R9.6.
///
/// @param[in,out]  Vector
///   Reference to the vector that will be normalised.
/// @return
///   A reference to the input vector, which has been normalised, so that you
///   can use this function in more alaborate expressions.
inline Vector& normalize(Vector& v)
{
  Real vlen = Len(v);
  if (vlen != 0.0f) {
    v /= vlen;
  } else {
    v.x = 1.0;
    v.y = 0.0;
    v.z = 0.0;
  }
  return v;
}


/// Wrapper which returns the point positions of a PointObject. This is needed
/// as with R10 the function name changed.
///
/// @param[in]  object
///   Reference to the object of which the point array will be obtained.
/// @return
///   The array of point positions of the object.
inline const Vector* getPoints(PointObject& object)
{
#if _C4D_VERSION>=100
  return object.GetPointR();
#else
  return object.GetPoint();
#endif
}


/// Wrapper which returns the polygons of a PointObject. This is needed
/// as with R10 the function name changed.
///
/// @param[in]  object
///   Reference to the object of which the polygon array will be obtained.
/// @return
///   The array of polygons of the object.
inline const CPolygon* getPolygons(PolygonObject& object)
{
#if _C4D_VERSION>=100
  return object.GetPolygonR();
#else
  return object.GetPolygon();
#endif
}



#endif  // #ifndef __UTILITIES_H__
