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

#include "c4d_symbols.h"
#include "luxc4dlighttag.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DLightTag.
 *****************************************************************************/


/// Returns a new instance of this class. Will be called by C4D, when the
/// user adds this plugin as a post-effect.
NodeData* LuxC4DLightTag::alloc(void)
{
  return gNew LuxC4DLightTag;
}


/// Registers this plugin in C4D and returns TRUE if it succeeded.
Bool LuxC4DLightTag::registerPlugin(void)
{
  return RegisterTagPlugin(PID_LUXC4D_LIGHT_TAG,
                           GeLoadString(IDS_LUXC4D_LIGHT_TAG),
                           TAG_VISIBLE,
                           alloc,
                           "Tluxc4dlighttag",
                           0);
}
