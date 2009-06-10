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
#include "luxapi.h"
#include "luxc4dportaltag.h"
#include "tluxc4dportaltag.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPortalTag.
 *****************************************************************************/


/// Allocates a new instance of LuxC4DPortalTag which the caller owns.
/// This function is usually called by C4D when a new tag is created.
///
/// @return
///   Pointer to the new instance or NULL, if we ran out of memory.
NodeData* LuxC4DPortalTag::alloc(void)
{
  return gNew LuxC4DPortalTag;
}


/// Registers this TagData plugin.
///
/// @return
///   TRUE if registered successful or FALSE if not.
Bool LuxC4DPortalTag::registerPlugin(void)
{
  return RegisterTagPlugin(PID_LUXC4D_PORTAL_TAG,
                           GeLoadString(IDS_LUXC4D_PORTAL_TAG),
                           TAG_VISIBLE,
                           alloc,
                           "Tluxc4dportaltag",
                           "icon_portal_tag.tif",
                           0);
}


/// Initialises the tag parameters.
///
/// @return
///   TRUE if registered successful or FALSE if not.
Bool LuxC4DPortalTag::Init(GeListNode* node)
{
  // obtain container from data node.
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // initialise parameters
  data->SetBool(IDD_FLIP_NORMALS, FALSE);

  return TRUE;
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DPortalTag.
 *****************************************************************************/


/// Returns the data container of this node.
BaseContainer* LuxC4DPortalTag::getData(void)
{
  BaseTag* tag = static_cast<BaseTag*>(Get());
  BaseContainer* data = 0;
  if (tag) {
    data = tag->GetDataInstance();
  }
  if (!data)  ERRLOG_RETURN_VALUE(NULL, "LuxC4DPortalTag::getData(): could not obtain base container");
  return data;
}
