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
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxParamSet.
 *****************************************************************************/


/// Constructs an empty parameter set and allocates the required space for
/// a fixed amount of parameters.
///
/// @param[in]  maxParamNumber
///   The maximum number of parameters this set can store.
LuxParamSet::LuxParamSet(LuxParamNumber maxParamNumber)
: mMaxParamNumber(maxParamNumber),
  mParamNumber(0)
{
  // if the parameter number is too low:
  if (mMaxParamNumber <= 0) {
    // reset everything and return
    mMaxParamNumber  = 0;
    mParamTypes      = 0;
    mParamNames      = 0;
    mParamValues     = 0;
    mParamArraySizes = 0;
    ERRLOG_RETURN("LuxParamSet::LuxParamSet(): maxParamNumber was <= 0!");
  }

  // allocated arrays for the parameter attributes and values
  mParamTypes      = bNewNC LuxParamType[mMaxParamNumber];
  mParamNames      = bNewNC LuxParamName[mMaxParamNumber];
  mParamValues     = bNewNC LuxParamRef[mMaxParamNumber];
  mParamArraySizes = bNewNC ULONG[mMaxParamNumber];

  // if we couldn't allocate all arrays:
  if (!mParamTypes || !mParamNames || !mParamValues || !mParamArraySizes) {
    // reset and deallocate everything and return
    bDelete(mParamTypes);
    bDelete(mParamNames);
    bDelete(mParamValues);
    bDelete(mParamArraySizes);
    mMaxParamNumber = 0;
    ERRLOG_RETURN("LuxParamSet::LuxParamSet(): not enough memory");
  }
}


/// Destroys the parameter set and deallocates its resources.
LuxParamSet::~LuxParamSet()
{
  bDelete(mParamTypes);
  bDelete(mParamNames);
  bDelete(mParamValues);
  bDelete(mParamArraySizes);
}


/// Adds a new parameter to the list. The data is still owned by the caller.
///
/// @param[in]  type
///   The data type of the parameter.
/// @param[in]  name
///   The name of the parameter (must not be NULL).
/// @param[in]  value
///   The pointer to the parameter value (array) (must not be NULL). The caller
///   still owns the memory.
/// @param[in]  arraySize
///   The size of te parameter array (default: 1) (must not be NULL).
Bool LuxParamSet::addParam(LuxParamType type,
                           LuxParamName name,
                           LuxParamRef  value,
                           ULONG        arraySize)
{
  // check if therer is still some space left
  if (mParamNumber >= mMaxParamNumber) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxParamSet::addParam(): no more space left");
  }

  // check if the passed parameters are valid
  if (!name || !value || !arraySize) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxParamSet::addParam(): 0 is not allowed for token name, token pointer or token number");
  }

  // store the parameter in the arrays
  mParamTypes[mParamNumber]      = type;
  mParamNames[mParamNumber]      = name;
  mParamValues[mParamNumber]     = value;
  mParamArraySizes[mParamNumber] = arraySize;
  ++mParamNumber;
  return TRUE;
}


/// Appends all parameters of another parameter set.
///
/// @param[in]  other
///   The orher parameter set.
/// @return
///   TRUE if executed successfully, FALSE otherwise.
Bool LuxParamSet::add(const LuxParamSet& other)
{
  // check if there is enough space in set
  if (mParamNumber+other.mParamNumber > mMaxParamNumber) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxParamSet::add(): not enough space to add all parameters from other set");
  }

  // copy parameters
  for (LuxParamNumber c=0; c<other.mParamNumber; ++c) {
    mParamTypes[mParamNumber]      = other.mParamTypes[c];
    mParamNames[mParamNumber]      = other.mParamNames[c];
    mParamValues[mParamNumber]     = other.mParamValues[c];
    mParamArraySizes[mParamNumber] = other.mParamArraySizes[c];
    ++mParamNumber;
  }

  return FALSE;
}


/// Removes all stored parameters. The parameter set is empty afterwards.
void LuxParamSet::clear()
{
  mParamNumber = 0;
}
