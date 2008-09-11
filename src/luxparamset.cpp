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

#include "luxparamset.h"



/// Constructs an empty parameter set and allocates the required space for
/// a fixed amount of parameters.
///
/// @param[in]  maxParamNumber
///   The maximum number of parameters this set can store.
LuxParamSet::LuxParamSet(ParamNumberT maxParamNumber)
: mMaxParamNumber(maxParamNumber),
  mParamNumber(0)
{
  // if the parameter number is too low:
  if (mMaxParamNumber <= 0) {
    // reset everything and return
    mMaxParamNumber = 0;
    mParamTypes = 0;
    mParamNames = 0;
    mParamPointers = 0;
    mParamArraySizes = 0;
    GePrint("LuxParamSet::LuxParamSet(): maxParamNumber was <= 0!");
    return;
  }

  // allocated arrays for the parameter attributes and values
  mParamTypes      = bNewNC ParamTypeT[mMaxParamNumber];
  mParamNames      = bNewNC ParamNameT[mMaxParamNumber];
  mParamPointers   = bNewNC ParamPointerT[mMaxParamNumber];
  mParamArraySizes = bNewNC ULONG[mMaxParamNumber];

  // if we couldn't allocate all arrays:
  if (!mParamTypes || !mParamNames || !mParamPointers || !mParamArraySizes) {
    // reset and deallocate everything and return
    bDelete(mParamTypes);
    bDelete(mParamNames);
    bDelete(mParamPointers);
    bDelete(mParamArraySizes);
    mMaxParamNumber = 0;
    GePrint("LuxParamSet::LuxParamSet(): not enough memory");
    return;
  }
}


/// Destroys the parameter set and deallocates its resources.
LuxParamSet::~LuxParamSet()
{
  bDelete(mParamTypes);
  bDelete(mParamNames);
  bDelete(mParamPointers);
  bDelete(mParamArraySizes);
}


/// Adds a new parameter to the list. The data is still owned by the caller.
///
/// @param[in]  type
///   The data type of the parameter.
/// @param[in]  name
///   The name of the parameter (must not be NULL).
/// @param[in]  pointer
///   The pointer to the parameter (array) (must not be NULL). The caller
///   still owns the memory.
/// @param[in]  arraySize
///   The size of te parameter array (default: 1) (must not be NULL).
Bool LuxParamSet::addParam(ParamTypeT    type,
                           ParamNameT    name,
                           ParamPointerT pointer,
                           ULONG         arraySize)
{
  // check if therer is still some space left
  if (mParamNumber >= mMaxParamNumber) {
    GePrint("LuxParamSet::addParam(): no more space left");
    return FALSE;
  }

  // check if the passed parameters are valid
  if (!name || !pointer || !arraySize) {
    GePrint("LuxParamSet::addParam(): 0 is not allowed for token name, token pointer or token number");
    return FALSE;
  }

  // store the parameter in the arrays
  mParamTypes[mParamNumber]      = type;
  mParamNames[mParamNumber]      = name;
  mParamPointers[mParamNumber]   = pointer;
  mParamArraySizes[mParamNumber] = arraySize;
  ++mParamNumber;
  return TRUE;
}


/// Removes all stored parameters. The parameter set is empty afterwards.
void LuxParamSet::clear()
{
  mParamNumber = 0;
}
