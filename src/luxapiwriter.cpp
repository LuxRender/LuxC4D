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

#include <cstdio>

#include "c4d_symbols.h"
#include "luxapiwriter.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxAPIWriter.
 *****************************************************************************/


/// Constructs a new instance.
LuxAPIWriter::LuxAPIWriter()
: mFilesOpen(FALSE),
  mWorldStarted(FALSE),
  mErrorStringID(0)
{}


/// Destroys the instance. If the scene file wasn't closed before, it will be
/// closed and a warning is dumped.
LuxAPIWriter::~LuxAPIWriter(void)
{
  // if output files are still open for some reason, finish and close them
  if (mFilesOpen) {
    ERRLOG("LuxAPIWriter::~LuxAPIWriter(): scene file wasn't closed properly");
    endScene();
  }
}


/// Initialises the instance and sets the scene file name. The scene file won't
/// be opened yet, but if there is still a file opened, it gets closed
/// (together with a warning on the console). To actually open the output file
/// call startScene().
///
/// @param[in]  sceneFile
///   The file name of the next scene file to export into.
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::init(const Filename &sceneFile)
{
  // if there is already an open file, finish it and close it
  if (mFilesOpen) {
    ERRLOG("LuxAPIWriter::init(): scene file wasn't closed properly before");
    if (!endScene()) {
      ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                             "LuxAPIWriter::init():  could not close old scene file -> can't create new scene file");
    }
  }

  // just store the filenames - they will be opened later
  mSceneFilename = sceneFile;
  mWorldStarted = FALSE;
  mErrorStringID = 0;
  return TRUE;
}


Bool LuxAPIWriter::startScene(const CHAR* head)
{
  // if there is already an active session, close it
  if (mFilesOpen) {
    ERRLOG("LuxAPIWriter::startScene(): scene file wasn't closed properly before");
    if (!endScene()) {
      ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                             "LuxAPIWriter::startScene(): could not close old scene file -> can't create new scene file");
    }
  }

  // make sure that the filename actually contains something
  if (!mSceneFilename.Content()) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_INTERNAL,
                           "LuxAPIWriter::startScene(): no filename has been set");
  }

  // derive the filenames for the materials and objects files from the scene
  // filename 
  mMaterialsFilename = mSceneFilename;
  mMaterialsFilename.SetSuffix("lxm");
  mObjectsFilename = mSceneFilename;
  mObjectsFilename.SetSuffix("lxo");

  // open files
  if (!mSceneFile->Open(mSceneFilename, GE_WRITE, FILE_DIALOG) ||
      !mMaterialsFile->Open(mMaterialsFilename, GE_WRITE, FILE_DIALOG) ||
      !mObjectsFile->Open(mObjectsFilename, GE_WRITE, FILE_DIALOG))
  {
    mSceneFile->Close();
    mMaterialsFile->Close();
    mObjectsFile->Close();
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::startScene(): could not open file '" + mSceneFilename.GetString() + "'");
  }
  mFilesOpen = TRUE;

  // write header comments
  return writeLine(*mSceneFile, head) &&
         writeLine(*mSceneFile, "\n\n# Global Settings\n") &&
         writeLine(*mMaterialsFile, "# Materials File\n") &&
         writeLine(*mObjectsFile, "# Geometry File\n");
}


Bool LuxAPIWriter::endScene(void)
{
  // don't do anything if nothing was opened
  if (!mFilesOpen)  return TRUE;

  // write the inclusion of the the materials and objects files into the scene
  static const ULONG cBufferSize = 1024;
  CHAR buffer[cBufferSize];
  Bool success = TRUE;
  success &= writeLine(*mSceneFile, "\n# The Scene");
  success &= writeLine(*mSceneFile, "\nWorldBegin\n\n");
  ( String("Include \"")
    + mMaterialsFilename.GetFileString() 
    + String("\"") ).GetCString(buffer, cBufferSize, StUTF8);
  success &= writeLine(*mSceneFile, buffer);
  ( String("Include \"")
    + mObjectsFilename.GetFileString() 
    + String("\"") ).GetCString(buffer, cBufferSize, StUTF8);
  success &= writeLine(*mSceneFile, buffer);
  success &= writeLine(*mSceneFile, "\n\nWorldEnd\n");

  // close the files
  success &= mSceneFile->Close();
  success &= mMaterialsFile->Close();
  success &= mObjectsFile->Close();

  // check if everything was done correctly
  if (!success) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::endScene(): could not finish scene file or close all files");
  }

  mFilesOpen = FALSE;
  return TRUE;
}


Bool LuxAPIWriter::lookAt(const LuxVectorT& camPos,
                          const LuxVectorT& trgPos,
                          const LuxVectorT& upVec)
{
  const static ULONG  sBufferSize(256);
  CHAR                buffer[sBufferSize];

  LONG len = sprintf(buffer,
                     "LookAt %.8g %.8g %.8g  %.8g %.8g %.8g  %.8g %.8g %.8g\n",
                     camPos.x, camPos.y, camPos.z,
                     trgPos.x, trgPos.y, trgPos.z,
                     upVec.x,  upVec.y,  upVec.z);
  if (!mSceneFile->WriteBytes(buffer, len)) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::lookAt(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::film(IdentifierNameT    name,
                        const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "Film", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::camera(IdentifierNameT     name,
                          const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "Camera", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::pixelFilter(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "PixelFilter", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::sampler(IdentifierNameT    name,
                           const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "Sampler", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::surfaceIntegrator(IdentifierNameT    name,
                                     const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "SurfaceIntegrator", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::accelerator(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting(*mSceneFile, "Accelerator", name, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::worldBegin(void)
{
  mWorldStarted = TRUE;
  return TRUE;
}


Bool LuxAPIWriter::worldEnd(void)
{
  mWorldStarted = FALSE;
  return TRUE;
}


Bool LuxAPIWriter::attributeBegin(void)
{
  return writeLine(*mObjectsFile, "\nAttributeBegin");
}


Bool LuxAPIWriter::attributeEnd(void)
{
  return writeLine(*mObjectsFile, "AttributeEnd");
}


Bool LuxAPIWriter::objectBegin(IdentifierNameT name)
{
  return writeSetting(*mObjectsFile, "\nObjectBegin", name);
}


Bool LuxAPIWriter::objectEnd(void)
{
  return writeLine(*mObjectsFile, "ObjectEnd");
}


Bool LuxAPIWriter::lightSource(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting(*mObjectsFile, "\nLightSource", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::areaLightSource(IdentifierNameT    name,
                                   const LuxParamSet& paramSet)
{
  return writeSetting(*mObjectsFile, "AreaLightSource", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::texture(IdentifierNameT    name,
                           IdentifierNameT    colorType,
                           IdentifierNameT    type,
                           const LuxParamSet& paramSet)
{
  return writeSetting(*mMaterialsFile, "\nTexture", name, colorType, type, paramSet, TRUE);
}


Bool LuxAPIWriter::makeNamedMaterial(IdentifierNameT    name,
                                 const LuxParamSet& paramSet)
{
  return writeSetting(*mMaterialsFile, "MakeNamedMaterial", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::namedMaterial(IdentifierNameT name)
{
  return writeSetting(*mObjectsFile, "NamedMaterial", name);
}


Bool LuxAPIWriter::material(IdentifierNameT    name,
                            const LuxParamSet& paramSet)
{
  return writeSetting(*mObjectsFile, "Material", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::transform(const LuxMatrixT& matrix)
{
  const static ULONG  sBufferSize(360);
  CHAR                buffer[sBufferSize];

  LONG len = sprintf(buffer,
                     "Transform [%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g]\n",
                     matrix.values[0],  matrix.values[1],  matrix.values[2],  matrix.values[3], 
                     matrix.values[4],  matrix.values[5],  matrix.values[6],  matrix.values[7], 
                     matrix.values[8],  matrix.values[9],  matrix.values[10], matrix.values[11], 
                     matrix.values[12], matrix.values[13], matrix.values[14], matrix.values[15]);
  BaseFile* outFile = mWorldStarted ? mObjectsFile : mSceneFile;
  if (!outFile->WriteBytes(buffer, len)) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::transform(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::reverseOrientation(void)
{
  return writeLine(*mObjectsFile, "ReverseOrientation");
}


Bool LuxAPIWriter::shape(IdentifierNameT    name,
                         const LuxParamSet& paramSet)
{
  return writeSetting(*mObjectsFile, "Shape", name, 0, 0, paramSet, TRUE);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxAPIWriter.
 *****************************************************************************/


/// Writes a string and terminates it with a line feed.
///
/// @param[in]  file
///   Reference to the while into which the line should be written.
/// @param[in]  text
///   The text to write. (can be NULL)
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeLine(
  BaseFile&   file,
  const CHAR* text)
{
  if (text) {
    LONG len = (LONG)strlen(text);
    if (len > 0) {
      if (!file.WriteBytes(text, len)) {
        ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                               "LuxAPIWriter::writeLine(): writing to file failed");
      }
    }
  }

  if (!file.WriteChar('\n')) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::writeLine(): writing to file failed");
  }
  return TRUE;
}


/// Writes a simple statement of the form
///   <setting> [identifier]
///
/// @param[in]  file
///   Reference to the while into which the statement should be written.
/// @param[in]  setting
///   The specifier of the setting (e.g. "ObjectBegin"). (must not be NULL)
/// @param[in]  identifier1
///   The first identifier (e.g. the object name). (can be NULL)
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeSetting(
  BaseFile&       file,
  SettingNameT    setting,
  IdentifierNameT identifier)
{
  if (!file.WriteBytes(setting, (VLONG)strlen(setting)) ||
      (identifier &&
       (!file.WriteBytes(" \"", 2) ||
        !file.WriteBytes(identifier, (VLONG)strlen(identifier)) ||
        !file.WriteBytes("\"\n", 2))))
  {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::writeSetting(): writing to file failed");
  }

  return TRUE;
}


/// Writes a complete statement of the form
///   <setting> <identifier1>? <identifier2>? <identifier3>? <parameter>*
/// If a parameter is a value array, the values will be in separate lines.
///
/// @param[in]  file
///   Reference to the while into which the statement should be written.
/// @param[in]  setting
///   The specifier of the setting (e.g. "Texture"). (must not be NULL)
/// @param[in]  identifier1
///   The first identifier (e.g. the texture name). (can be NULL)
/// @param[in]  identifier2
///   The second identifier (e.g. the color type "color"). (can be NULL)
/// @param[in]  identifier3
///   The third identifier (e.g. the texture type "constant"). (can be NULL)
/// @param[in]  paramSet
///   The additional parameters for this statement.
/// @param[in]  newLine
///   Inserts a line feed after each parameter.
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeSetting(
  BaseFile&          file,
  SettingNameT       setting,
  IdentifierNameT    identifier1,
  IdentifierNameT    identifier2,
  IdentifierNameT    identifier3,
  const LuxParamSet& paramSet,
  Bool               newLine)
{
  // these are the type identifiers used in the Lux file format
  static const struct {
    CHAR* nameStr;
    ULONG nameStrLen;
  } cTokenType[LUX_NUMBER] =
    { {"bool ", 5},
      {"integer ", 8},
      {"float ", 6},
      {"vector ", 7},
      {"color ", 6},
      {"point ", 6},
      {"normal ", 7},
      {"integer ", 8},
      {"integer ", 8},
      {"string ", 7},
      {"texture ", 8} };

  Bool success = TRUE;

  // write setting identifier
  success &= file.WriteBytes(setting, (VLONG)strlen(setting));
  if (identifier1) {
    success &= file.WriteBytes(" \"", 2);
    success &= file.WriteBytes(identifier1, (VLONG)strlen(identifier1));
    success &= file.WriteChar('"');
  }
  if (identifier2) {
    success &= file.WriteBytes(" \"", 2);
    success &= file.WriteBytes(identifier2, (VLONG)strlen(identifier2));
    success &= file.WriteChar('"');
  }
  if (identifier3) {
    success &= file.WriteBytes(" \"", 2);
    success &= file.WriteBytes(identifier3, (VLONG)strlen(identifier3));
    success &= file.WriteChar('"');
  }
  if (newLine) {
    success &= file.WriteChar('\n');
  }

  // write parameters
  CHAR          valueString[128];
  VLONG         valueStringLen;
  LuxParamTypeT tokenType;
  LuxParamNameT tokenName;
  LuxParamRefT  tokenValue;
  ULONG         tokenArraySize;
  for (LuxParamNumberT c=0; c<paramSet.ParamNumber(); ++c) {

    // for convenience store token data in local variables
    tokenType      = paramSet.ParamTypes()[c];
    tokenName      = paramSet.ParamNames()[c];
    tokenValue     = paramSet.ParamValues()[c];
    tokenArraySize = paramSet.ParamArraySizes()[c];

    // write token name and type
    success &= file.WriteBytes(" \"", 2);
    success &= file.WriteBytes(cTokenType[tokenType].nameStr, cTokenType[tokenType].nameStrLen);
    success &= file.WriteBytes(tokenName, (VLONG)strlen(tokenName));
    success &= file.WriteChar('"');

    // write token values
    if (tokenArraySize > 1) {
      success &= file.WriteBytes(" [\n", 3);
    } else {
      success &= file.WriteBytes(" [", 2);
    }
    switch (tokenType) {
      case LUX_BOOL:
        {
          const LuxBoolT* values = (const LuxBoolT*)tokenValue;
          if (tokenArraySize == 1) {
            if (values[0]) {
              success &= file.WriteBytes("\"true\"", 6);
            } else {
              success &= file.WriteBytes("\"false\"", 7);
            }
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              if (values[i]) {
                success &= file.WriteBytes("\"true\"\n", 7);
              } else {
                success &= file.WriteBytes("\"false\"\n", 8);
              }
            }
          }
          break;
        }
      case LUX_INTEGER:
        {
          const LuxIntegerT* values = (const LuxIntegerT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%d", values[0]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%d\n", values[i]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_FLOAT:
        {
          const LuxFloatT* values = (const LuxFloatT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g", values[0]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g\n", values[i]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_VECTOR:
        {
          const LuxVectorT* values = (const LuxVectorT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_COLOR:
        {
          const LuxColorT* values = (const LuxColorT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].c[0], values[0].c[1], values[0].c[2]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].c[0], values[i].c[1], values[i].c[2]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_POINT:
        {
          const LuxPointT* values = (const LuxPointT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_NORMAL:
        {
          const LuxNormalT* values = (const LuxNormalT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_TRIANGLE:
        {
          const LuxIntegerT* values = (const LuxIntegerT*)tokenValue;
          if (tokenArraySize == 3) {
            valueStringLen = sprintf(valueString, "%d %d %d",
                                     values[0], values[1], values[2]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=2; i<tokenArraySize; i+=3) {
              valueStringLen = sprintf(valueString, "%d %d %d\n",
                                       values[i-2], values[i-1], values[i]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_QUAD:
        {
          const LuxIntegerT* values = (const LuxIntegerT*)tokenValue;
          if (tokenArraySize == 4) {
            valueStringLen = sprintf(valueString, "%d %d %d %d",
                                     values[0], values[1], values[2], values[3]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=3; i<tokenArraySize; i+=4) {
              valueStringLen = sprintf(valueString, "%d %d %d %d\n",
                                       values[i-3], values[i-2], values[i-1], values[i]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_STRING:
      case LUX_TEXTURE:
        {
          const LuxStringT* values = (const LuxStringT*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = (VLONG)values[0].size();
            if (valueStringLen) {
              success &= file.WriteChar('"');
              success &= file.WriteBytes(values[0].c_str(), valueStringLen);
              success &= file.WriteChar('"');
            }
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = (VLONG)values[i].size();
              if (valueStringLen) {
                success &= file.WriteChar('"');
                success &= file.WriteBytes(values[i].c_str(), valueStringLen);
                success &= file.WriteBytes("\"\n", 2);
              }
            }
          }
          break;
        }
      default:
        ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_INTERNAL,
                               "LuxAPIWriter::writeSetting(): invalid type specifier in token name");
    }
    success &= file.WriteChar(']');
    if (newLine)  success &= file.WriteChar('\n');
  }

  // write line feed, to finish statement and skip to the next line
  if (!newLine)  success &= file.WriteChar('\n');

  // check if some of the write operations have failed
  if (!success) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::writeSetting(): writing to file failed");
  }
  return TRUE;
}
