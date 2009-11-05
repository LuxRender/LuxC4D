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
  //if (!GeExecuteProgram(luxPath, mExportedFile)) {
  if (!executeProgram(luxPath, mExportedFile)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EXECUTE, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DExporterRender.
 *****************************************************************************/

#ifdef __PC

#include <process.h>

/// This helper launches a progam using a direct OS call. This way we can pass
/// more than only one argument and other stuff (e.g. "--noopengl") in the future.
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
  static const SizeT cStrBufferSize = 2048;

  String  programFileNameStr, sceneFileNameStr;
  wchar_t programFileNameCStr[cStrBufferSize];
  wchar_t programFileNameCStr2[cStrBufferSize];
  wchar_t sceneFileNameCStr[cStrBufferSize];

  // make sure that the assumption is correct that wchar_t == UWORD
  GeAssert( sizeof(wchar_t) == sizeof(UWORD) );

  // copy UNICODE filenames into C strings
  programFileNameStr = programFileName.GetString();
  programFileNameStr.GetUcBlockNull((UWORD*)programFileNameCStr,
                                    cStrBufferSize);
  programFileNameStr = "\"" + programFileNameStr + "\"";
  programFileNameStr.GetUcBlockNull((UWORD*)programFileNameCStr2,
                                    cStrBufferSize);
  sceneFileNameStr = "\"" + sceneFileName.GetString() + "\"";
  sceneFileNameStr.GetUcBlockNull((UWORD*)sceneFileNameCStr,
                                  cStrBufferSize);

  // start new process
  return _wspawnl(_P_NOWAITO,
                  programFileNameCStr,
                  programFileNameCStr2,
                  sceneFileNameCStr,
                  NULL)
                  >= 0 ? TRUE : FALSE;
}

#else

#include <unistd.h>

/// This helper launches a progam using a direct OS call. This way we can pass
/// more than only one argument and other stuff (e.g. "--noopengl") in the future.
///
/// On Mac OS we do this also to make LuxRender actually open the scene file
/// which is not working using GeExecuteProgram(). In builds for C4D R9.6, we
/// also convert the filenames from HFS style to POSIX.
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
  static const SizeT cStrBufferSize = 2048;
  
  String programFileNameStr, sceneFileNameStr;
  char   programFileNameCStr[cStrBufferSize];
  char   programFileNameCStr2[cStrBufferSize];
  char   sceneFileNameCStr[cStrBufferSize];
  
  // make sure that the assumption is correct that wchar_t == UWORD
  GeAssert( sizeof(char) == sizeof(CHAR) );
  
  // copy UNICODE filenames into C strings
  programFileNameStr = programFileName.GetString();
  programFileNameStr.GetCString(programFileNameCStr, cStrBufferSize, StUTF8);
  programFileNameStr = "\"" + programFileNameStr + "\"";
  programFileNameStr.GetCString(programFileNameCStr2, cStrBufferSize, StUTF8);
  sceneFileNameStr = "\"" + sceneFileName.GetString() + "\"";
  sceneFileNameStr.GetCString(sceneFileNameCStr, cStrBufferSize, StUTF8);
  
  // fork to new process and run LuxRender in new process
  switch (fork()) {
    case -1:
      ERRLOG_RETURN_VALUE(FALSE, "fork() failed");
    case 0:
      // this is the child process
      execl(programFileNameCStr, programFileNameCStr2, sceneFileNameCStr, NULL);
      puts("Uh oh! If this prints, execl() must have failed");
      exit(EXIT_FAILURE);
    default:
      // this is the parent process
      return TRUE;
  }
}

#endif  // #ifdef __PC
