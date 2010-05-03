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

#ifndef __LUXC4DPORTALTAG_H__
#define __LUXC4DPORTALTAG_H__ 1



#include <c4d.h>

#include "luxapi.h"
#include "luxparamset.h"
#include "luxtypes.h"



#define PID_LUXC4D_PORTAL_TAG  1024020



/***************************************************************************//*!
 This class implements a tag that can be applied to geometry objects. The
 objects that have a portal tag, will be exported as Lux light portals.
*//****************************************************************************/
class LuxC4DPortalTag : public TagData
{
  INSTANCEOF(LuxC4DPortalTag,TagData)


public:

  static NodeData* alloc(void);
  static Bool registerPlugin(void);

  virtual Bool Init(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*  node,
                               Description* description,
                               LONG&        flags);
#if _C4D_VERSION<115
  virtual Bool GetDEnabling(GeListNode*          node,
                            const DescID&        id,
                            GeData&              t_data,
                            LONG                 flags,
                            const BaseContainer* itemdesc);
#else
  virtual Bool GetDEnabling(GeListNode*          node,
                            const DescID&        id,
                            const GeData&        t_data,
                            LONG                 flags,
                            const BaseContainer* itemdesc);
#endif


private:

  BaseContainer* getData(void);
};



#endif // #ifndef __LUXC4DPORTALTAG_H__
