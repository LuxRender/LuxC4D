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

#ifndef __LUXC4DLIGHTTAG_H__
#define __LUXC4DLIGHTTAG_H__ 1



#include "c4d.h"



#define PID_LUXC4D_LIGHT_TAG  1023210



/***************************************************************************//*!
*//****************************************************************************/
class LuxC4DLightTag : public TagData
{
  INSTANCEOF(LuxC4DLightTag,TagData)


public:

  static NodeData* alloc(void);
  static Bool registerPlugin(void);


};



#endif  // #ifndef __LUXC4DLIGHTTAG_H__
