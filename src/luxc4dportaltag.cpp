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
#include "common.h"
#include "luxapi.h"
#include "luxc4dportaltag.h"
#include "tluxc4dportaltag.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPortalTag.
 *****************************************************************************/


/// Allocates a new instance of LuxC4DPortalTag which the caller owns.
/// This function is usually called by C4D when a new portal tag is created.
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
                           AutoBitmap("icon_portal_tag.tif"),
                           0);
}


/// Initialises the tag parameters.
/// (see C4D API documentation for parameters)
Bool LuxC4DPortalTag::Init(GeListNode* node)
{
  // obtain container from data node.
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // initialise parameters
  data->SetBool(IDD_PORTAL_ENABLED,        TRUE);
  data->SetBool(IDD_PORTAL_SIMPLIFY,       FALSE);
  data->SetLong(IDD_PORTAL_FACE_DIRECTION, IDD_PORTAL_FACE_DIR_X_PLUS);
  data->SetBool(IDD_PORTAL_EXPORT_OBJECT,  FALSE);
  data->SetBool(IDD_PORTAL_FLIP_NORMALS,   FALSE);

  return TRUE;
}


/// Overwritten function that is called to load a description. We use it to
/// hide descriptions, we don't want to show due to the selection of specific
/// parameters.
/// (see C4D API documentation for parameters)
Bool LuxC4DPortalTag::GetDDescription(GeListNode*     node,
                                      Description*    description,
                                      DESCFLAGS_DESC& flags)
{
  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DPortalTag::GetDDescription(): could not load description for node");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DPortalTag::GetDDescription(): could not allocate empty AtomArray");
  }

  // hide options if tag is disabled
  showParameter(description, IDG_PORTAL_OPTIONS, params, data->GetBool(IDD_PORTAL_ENABLED));

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return SUPER::GetDDescription(node, description, flags);
}


/// Overwritten function to enable/disable description controls depending on
/// the current options.
/// (see C4D API documentation for parameters)
#if _C4D_VERSION<115
Bool LuxC4DPortalTag::GetDEnabling(GeListNode*          node,
                                   const DescID&        id,
                                   GeData&              t_data,
                                   LONG                 flags,
                                   const BaseContainer* itemdesc)
#else
Bool LuxC4DPortalTag::GetDEnabling(GeListNode*          node,
                                   const DescID&        id,
                                   const GeData&        t_data,
                                   DESCFLAGS_ENABLE     flags,
                                   const BaseContainer* itemdesc)
#endif
{
  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  //
  if (id == DescID(IDD_PORTAL_FACE_DIRECTION)) {
    return data->GetBool(IDD_PORTAL_SIMPLIFY);
  }

  return SUPER::GetDEnabling(node, id, t_data, flags, itemdesc);
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
