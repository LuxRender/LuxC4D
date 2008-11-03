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

#include "utilities.h"



/// Makes a parameter visible or hides it.
///
/// @param[in]  description
///   The desciption object the parameter belongs to.
/// @param[in]  paramID
///   The parameter ID to show/hide.
/// @param[in]  params
///   Specifies the parameter element to show/hide.
/// @param[in]  show
///   Set this to TRUE make it visible or to FALSE to hide it.
void ShowParameter(Description* description,
                   LONG         paramID,
                   AtomArray*   params,
                   Bool         show)
{
  BaseContainer* descrData = description->GetParameterI(DescLevel(paramID),
                                                        params);
  if (descrData) {
    descrData->SetBool(DESC_HIDE, !show);
  }
}


///
LONG GetParameterLong(BaseObject& object,
                      LONG        paramID,
                      LONG        preset)
{
  GeData parameter;
  if (object.GetParameter(DescLevel(paramID), parameter, 0)) {
    if (parameter.GetType() == DA_LONG) {
      return parameter.GetLong();
    }
  }
  return preset;
}


///
Real GetParameterReal(BaseObject& object,
                      LONG        paramID,
                      Real        preset)
{
  GeData parameter;
  if (object.GetParameter(DescLevel(paramID), parameter, 0)) {
    if (parameter.GetType() == DA_REAL) {
      return parameter.GetReal();
    }
  }
  return preset;
}


///
Vector GetParameterVector(BaseObject&   object,
                          LONG          paramID,
                          const Vector& preset)
{
  GeData parameter;
  if (object.GetParameter(DescLevel(paramID), parameter, 0)) {
    if (parameter.GetType() == DA_VECTOR) {
      return parameter.GetVector();
    }
  }
  return preset;
}
