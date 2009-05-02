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

void showParameter(Description* description,
                   LONG         paramID,
                   AtomArray*   params,
                   Bool         show);


LONG getParameterLong(BaseObject& object,
                      LONG        paramID,
                      LONG        preset = 0);
Real getParameterReal(BaseObject& object,
                      LONG        paramID,
                      Real        preset = 0.0);
Vector getParameterVector(BaseObject&   object,
                          LONG          paramID,
                          const Vector& preset = Vector());
BaseList2D* getParameterLink(BaseObject& object,
                             LONG        paramID,
                             LONG        instanceOf = 0);



/*****************************************************************************
 * Inlined functions
 *****************************************************************************/

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


inline const Vector* getPoints(PointObject& object)
{
#if _C4D_VERSION>=100
  return object.GetPointR();
#else
  return object.GetPoint();
#endif
}


inline const CPolygon* getPolygons(PolygonObject& object)
{
#if _C4D_VERSION>=100
  return object.GetPolygonR();
#else
  return object.GetPolygon();
#endif
}



#endif  // #ifndef __UTILITIES_H__
