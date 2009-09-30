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



#include <c4d.h>

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

  LuxParamSet(LuxParamNumber maxParamNumber);
  ~LuxParamSet();

  Bool addParam(LuxParamType       type,
                LuxParamName name,
                LuxParamRef        value,
                ULONG              arraySize=1);
  void clear();

  inline LuxParamNumber      ParamNumber() const  { return mParamNumber; }
  inline const LuxParamType* ParamTypes() const  { return mParamTypes; }
  inline const LuxParamName* ParamNames() const  { return mParamNames; }
  inline const LuxParamRef*  ParamValues() const  { return mParamValues; }
  inline const ULONG*        ParamArraySizes() const  { return mParamArraySizes; }


private:

  LuxParamNumber mMaxParamNumber;
  LuxParamNumber mParamNumber;
  LuxParamType*  mParamTypes;
  LuxParamName*  mParamNames;
  LuxParamRef*   mParamValues;
  ULONG*         mParamArraySizes;


  LuxParamSet() {}
  LuxParamSet(const LuxParamSet& other) {}
  LuxParamSet& operator=(const LuxParamSet& other) {}
};


#endif  // #ifndef __LUXPARAMSET_H__
