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
                               0, "icon_export_render.tif",
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
  if (!GeFExist(luxPath, FALSE)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_DOESNT_EXIST, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  // export file
  if (!LuxC4DExporter::Execute(document)) {
    return FALSE;
  }

  // call Lux
  if (!executeProgram(luxPath, mExportedFile)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EXECUTE, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DExporterRender.
 *****************************************************************************/

/// This helper launches a progam using a script. There are 2 reasons for this
/// workaround:
///   1. You can't pass more than one parameter to the application if you use
///      GeExecuteProgram().
///   2. GeExecuteProgram() didn't work on MacOS, while trying to launch
///      LuxRender with an .lxs scene file.
/// So what we do now, is to create a launch script (launch.bat on Windows and
/// launch.sh on MacOS) that is then executed.
///
/// At the moment we pass in the scene file name directly, but I will probably
/// extent this function so that you pass in an array of arguments, which can
/// then include a scene file and other stuff (e.g. "--noopengl").
///
/// @param[in]  programFileName
///   The full path of the application to launch.
/// @param[in]  sceneFileName
///   The full path of the scene file to open.
/// @return
///   TRUE if executed successfully (the application was launched), FALSE
///   otherwise.
Bool LuxC4DExporterRender::executeProgram(const Filename& programFileName,
                                          const Filename& sceneFileName)
{
#ifdef __PC
  const String cLaunchScriptName("launch.bat");
  const String cCommandLineSuffix;
#else
  const String cLaunchScriptName("launch.sh");
  const String cCommandLineSuffix(" &");
#endif
  
  // open new script file, which will be stored in the plugin directory
  Filename scriptFileName(GeGetPluginPath());
  scriptFileName += cLaunchScriptName;
  AutoAlloc<BaseFile> scriptFile;
  if (!scriptFile->Open(scriptFileName, GE_WRITE, FILE_NODIALOG)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DExporterRender::executeProgram(): could not open launch script '" + scriptFileName.GetString() + "'" );
  }
  
  // build command line
  String commandLine;
  commandLine = "\"" + programFileName.GetString() + "\"" +
                " \"" + sceneFileName.GetString() + "\"" +
                cCommandLineSuffix + "\n";
  
  // copy command line into CHAR buffer
  SizeT bufferSize = commandLine.GetCStringLen() + 1;
  CHAR* buffer = bNewNC CHAR[bufferSize];
  if (!buffer) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DExporterRender::executeProgram(): not enough memory for output buffer");
  }
  LONG commandLineLen = commandLine.GetCString(buffer, bufferSize);
                         
  // write CHAR buffer and free buffer again
  scriptFile->WriteBytes(buffer, commandLineLen);
  bDelete(buffer);
  
  // close script file and set attributes to make it an executable
  scriptFile->Close();
  GeFSetAttributes(scriptFileName,
                   GE_FILE_ATTRIBUTE_OWNER_R |
                   GE_FILE_ATTRIBUTE_OWNER_W |
                   GE_FILE_ATTRIBUTE_OWNER_X |
                   GE_FILE_ATTRIBUTE_GROUP_R |
                   GE_FILE_ATTRIBUTE_GROUP_W |
                   GE_FILE_ATTRIBUTE_GROUP_X |
                   GE_FILE_ATTRIBUTE_PUBLIC_R |
                   GE_FILE_ATTRIBUTE_PUBLIC_W |
                   GE_FILE_ATTRIBUTE_PUBLIC_X);

  // runs script
  if (!GeExecuteProgram(scriptFileName, Filename())) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EXECUTE, scriptFileName.GetString()), GEMB_OK);
    return FALSE;
  }
    
  return TRUE;
}
