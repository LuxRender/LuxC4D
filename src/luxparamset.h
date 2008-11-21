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

#ifndef __LUXPARAMSET_H__
#define __LUXPARAMSET_H__  1



#include "c4d.h"

#include "luxtypes.h"



/***************************************************************************//*!
 This class stores a set of parameters for one statement. The values can be
 consumed by an implementation of LuxAPI. As the parameters have the same data
 structure as what the Lux API expects, they can be passed directly to it.

 The parameters are stored in fix-sized arrays as there will be only a limited
 amount of parameters per statement.
*//****************************************************************************/
class LuxParamSet
{
public:

  typedef LONG  ParamNumberT;
  typedef CHAR* ParamNameT;
  typedef void* ParamPointerT;


  /// Each enumeration specifies one type used by the Lux API.
  enum ParamTypeT {
    LUX_BOOL = 0,
    LUX_INTEGER,
    LUX_FLOAT,
    LUX_VECTOR,
    LUX_COLOR,
    LUX_POINT,
    LUX_NORMAL,
    LUX_TRIANGLE,
    LUX_QUAD,
    LUX_STRING,
    LUX_TEXTURE,
    LUX_NUMBER
  };


  LuxParamSet(ParamNumberT maxParamNumber);
  ~LuxParamSet();

  Bool addParam(ParamTypeT    type,
                ParamNameT    name,
                ParamPointerT pointer,
                ULONG         arraySize=1);
  void clear();

  inline ParamNumberT         ParamNumber() const  { return mParamNumber; }
  inline const ParamTypeT*    ParamTypes() const  { return mParamTypes; }
  inline const ParamNameT*    ParamNames() const  { return mParamNames; }
  inline const ParamPointerT* ParamPointers() const  { return mParamPointers; }
  inline const ULONG*         ParamArraySizes() const  { return mParamArraySizes; }


private:

  ParamNumberT    mMaxParamNumber;
  ParamNumberT    mParamNumber;
  ParamTypeT*     mParamTypes;
  ParamNameT*     mParamNames;
  ParamPointerT*  mParamPointers;
  ULONG*          mParamArraySizes;


  LuxParamSet() {}
  LuxParamSet(const LuxParamSet& other) {}
  LuxParamSet& operator=(const LuxParamSet& other) {}
};


#endif  // #ifndef __LUXPARAMSET_H__
