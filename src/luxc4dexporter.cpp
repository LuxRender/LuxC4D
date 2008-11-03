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

#include "luxapi.h"
#include "luxapiconverter.h"
#include "luxapiwriter.h"
#include "luxc4dexporter.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DExporter.
 *****************************************************************************/

/// Registers this plugin instance in CINEMA 4D.
///
/// @return
///   TRUE if successfull, otherwise FALSE.
Bool LuxC4DExporter::registerPlugin(void)
{
  return RegisterCommandPlugin(PID_LUXC4D_EXPORTER,
                               GeLoadString(IDS_LUXC4D_EXPORTER),
                               0, "",
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
Bool LuxC4DExporter::Execute(BaseDocument* document)
{
  // check if document is valid
  if (!document)  ERRLOG_RETURN_FALSE("LuxC4DExporter::Execute(): no document passed");

  // get LuxC4DSettings video post effect node - if available
  LuxC4DSettings* settingsNode = 0;
  RenderData*     renderData = document->GetActiveRenderData();
  if (renderData) {
    PluginVideoPost* videoPost = renderData->GetFirstVideoPost();
    for (; videoPost; videoPost = videoPost->GetNext()) {
      if (videoPost->GetType() == PID_LUXC4D_SETTINGS) {
        settingsNode = (LuxC4DSettings*)videoPost->GetNodeData();
        break;
      }
    }
  }

  // if we have found a LuxC4DSettings object, get the export filename
  // chosen by the user
  Bool     overwritingAllowed = FALSE;
  if (settingsNode) {
    settingsNode->GetExportFilename(*document, mExportedFile, overwritingAllowed);
  }

  // if the file already exists and overwriting is not allowed, ask the user
  // what to do and ...
  Bool selectFilename = FALSE;
  if (mExportedFile.Content()) {
    if (!overwritingAllowed && GeFExist(mExportedFile)) {
      LONG answer = GeOutString(GeLoadString(IDS_OVERWRITE_FILE_QUERY,
                                             mExportedFile.GetFileString()),
                                GEMB_YESNOCANCEL);
      if (answer == GEMB_R_CANCEL) {
        return FALSE;
      }
      if (answer == GEMB_R_NO) {
        selectFilename = TRUE;
      }
    }
  // otherwise derive export filename from document and ...
  } else {
    mExportedFile.SetDirectory(document->GetDocumentPath());
    mExportedFile.SetFile(document->GetDocumentName());
    mExportedFile.SetSuffix("lxs");
    selectFilename = TRUE;
  }

  // ... get export filename, if needed
  if (selectFilename) {
    if (!mExportedFile.FileSelect(FSTYPE_ANYTHING, GE_SAVE, GeLoadString(IDS_EXPORT_FILENAME_QUERY))) {
      return FALSE;
    }
  }

  // initialise file writer
  LuxAPIWriter apiWriter;
  if (!apiWriter.init(mExportedFile)) {
    GeOutString(GeLoadString(IDS_ERROR_INITIALISE_LUXAPIWRITER, mExportedFile.GetString()), GEMB_OK);
    return FALSE;
  }

  // create exporter and export scene
  LuxAPIConverter converter;
  if (!converter.convertScene(*document, apiWriter)) {
    LONG errorStringID = apiWriter.errorStringID();
    if (errorStringID == 0) {
      errorStringID = IDS_ERROR_CONVERSION;
    }
    GeOutString(GeLoadString(errorStringID), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}
