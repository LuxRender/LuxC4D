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

#ifndef __LUXC4DEXPORTER_H__
#define __LUXC4DEXPORTER_H__  1



#include <c4d.h>

#include "c4d_symbols.h"



#define PID_LUXC4D_EXPORTER  1022831



/***************************************************************************//*!
 The CommandData plugin that triggers an export into a .lxs file.
*//****************************************************************************/
class LuxC4DExporter : public CommandData
{
public:

  Bool registerPlugin(void);
  virtual Bool Execute(BaseDocument* document);


protected:

  Filename mExportedFile;

  Bool exportScene(BaseDocument* document,
                   Bool          resumeOnly);
};



#endif  // #ifndef __LUXC4DEXPORTER_H__
