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

#include "common.h"



#if _C4D_VERSION < 120


AutoBitmap::AutoBitmap(const String &name)
{
  mBitmap = BaseBitmap::Alloc();
  if (!mBitmap) { return; }
  if (mBitmap->Init(GeGetPluginPath()+String("res")+name) != IMAGE_OK) {
    BaseBitmap::Free(mBitmap);
  }
}


AutoBitmap::~AutoBitmap()
{
  BaseBitmap::Free(mBitmap);
}


#endif  // #if _C4D_VERSION < 120
