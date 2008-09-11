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

#include "luxc4d.h"
#include "luxapi.h"
#include "luxapiwriter.h"
#include "luxexporter.h"



#define ERRLOG_RETURN(msg)   { GePrint((msg)); return FALSE; }



/*****************************************************************************
 * Implementation of public member functions of class LuxC4D.
 *****************************************************************************/

/// Standard constructor. Does nothing at the moment.
LuxC4D::LuxC4D(void)
{}


/// Registers this plugin instance in CINEMA 4D.
///
/// @return
///   TRUE if successfull, otherwise FALSE.
Bool LuxC4D::registerPlugin(void)
{
  return RegisterCommandPlugin(PID_LUXC4D,
                               GeLoadString(IDS_LUXC4D),
                               0, "",
                               GeLoadString(IDS_LUXC4D_DESCR),
                               this);
}


/// Gets called when the user selects the plugin in the menu. The actual export
/// will be called from here.
///
/// @param[in]  document
///   The current document, which will be exported.
/// @return
///   TRUE if successfull, FALSE otherwise.
Bool LuxC4D::Execute(BaseDocument* document)
{
  // check if document is valid
  if (!document)  ERRLOG_RETURN("LuxC4D::Execute(): no document passed");

  // get filename of document
  Filename path;
  path.SetDirectory(document->GetDocumentPath());
  path.SetFile(document->GetDocumentName());
  path.SetSuffix("lxs");

  // get export filename
  if (!path.FileSelect(FSTYPE_ANYTHING, GE_SAVE, "Where should we save the scene?")) {
    return FALSE;
  }

  // initialise file writer
  LuxAPIWriter  apiWriter;
  if (!apiWriter.init(path)) {
    GeOutString(GeLoadString(IDS_ERROR_INITIALISE_LUXAPIWRITER, path.GetString()), GEMB_OK);
    return FALSE;
  }

  // create exporter and export scene
  LuxExporter exporter;
  if (!exporter.exportScene(*document, apiWriter)) {
    LONG errorStringID = apiWriter.errorStringID();
    if (errorStringID == 0) {
      errorStringID = IDS_ERROR_CONVERSION;
    }
    GeOutString(GeLoadString(errorStringID), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}
