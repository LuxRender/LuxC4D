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
#include "filepath.h"
#include "luxc4dexporterrender.h"
#include "luxc4dpreferences.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DExporterRender.
 *****************************************************************************/

/// Registers this plugin instance in CINEMA 4D.
///
/// @return
///   TRUE if successfull, otherwise FALSE.
Bool LuxC4DExporterRender::registerPlugin(void)
{
  return RegisterCommandPlugin(PID_LUXC4D_EXPORTERRENDER,
                               GeLoadString(IDS_LUXC4D_EXPORTERRENDER),
                               0,
                               AutoBitmap("icon_export_render.tif"),
                               GeLoadString(IDS_LUXC4D_EXPORTER_DESCR),
                               this);
}


/// Gets called when the user selects the plugin in the menu. The actual export
/// will be called from here.
///
/// @param[in]  document
///   The current document, which will be exported.
/// @return
///   TRUE if successfull, FALSE otherwise.
Bool LuxC4DExporterRender::Execute(BaseDocument* document)
{
  return exportAndRender(document, FALSE);
}



/*****************************************************************************
 * Implementation of protected member functions of class LuxC4DExporterRender.
 *****************************************************************************/

Bool LuxC4DExporterRender::exportAndRender(BaseDocument* document,
                                           Bool          resumeOnly)
{
  // get Lux executable
  Filename luxPath = gPreferences->getLuxPath();
  if (!luxPath.Content()) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EMPTY), GEMB_OK);
    return FALSE;
  }

#ifdef __MAC
  // on MacOS, LuxRender is stored in a bundle 
  if (GeFExist(luxPath, TRUE)) {
    luxPath += "Contents";
    luxPath += "MacOS";
    luxPath += "LuxRender";
  }
#endif
  
  // check if the LuxRender application path is valid
  if (!GeFExist(luxPath, FALSE)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_DOESNT_EXIST, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  // export file
  if (!exportScene(document, resumeOnly)) {
    return FALSE;
  }

  // call LuxRender
  if (!executeProgram(luxPath, mExportedFile)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EXECUTE, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}
