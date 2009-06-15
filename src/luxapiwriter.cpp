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
{
  mCommentLen = 0;
}


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
  mCommentLen = 0;
  return TRUE;
}


/// Starts a new scene - see LuxAPI::startScene(const char*).
Bool LuxAPIWriter::startScene(const char* head)
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


/// End the current scene - see LuxAPI::endScene(void).
Bool LuxAPIWriter::endScene(void)
{
  // don't do anything if nothing was opened
  if (!mFilesOpen)  return TRUE;

  // write the inclusion of the the materials and objects files into the scene
  static const ULONG cBufferSize = 1024;
  CHAR buffer[cBufferSize];
  Bool success = TRUE;
  success &= writeLine(*mSceneFile, "\n# The Scene");
  success &= writeLine(*mSceneFile, "WorldBegin\n");
  ( String("Include \"")
    + mMaterialsFilename.GetFileString() 
    + String("\"") ).GetCString(buffer, cBufferSize, StUTF8);
  success &= writeLine(*mSceneFile, buffer);
  ( String("Include \"")
    + mObjectsFilename.GetFileString() 
    + String("\"") ).GetCString(buffer, cBufferSize, StUTF8);
  success &= writeLine(*mSceneFile, buffer);
  success &= writeLine(*mSceneFile, "\nWorldEnd");

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


/// Specifies the comment for the next command - see
/// LuxAPI::setComment(const char*).
Bool LuxAPIWriter::setComment(const char* text)
{
  for (mCommentLen=0; mCommentLen<sizeof(mComment); ++mCommentLen) {
    if ((mComment[mCommentLen] = text[mCommentLen]) == 0) {
      break;
    }
  }
  mComment[sizeof(mComment)-1] = '\0';
  return TRUE;
}


/// Specifies the comment for the next command - see
/// LuxAPI::setComment(const String&).
Bool LuxAPIWriter::setComment(const String& text)
{
  mCommentLen = text.GetCString(mComment, sizeof(mComment), StUTF8);
  mComment[sizeof(mComment)-1] = '\0';
  return TRUE;
}


/// Sets up a coordinate transformation for the current scope based on a
/// viewing direction - see
/// LuxAPI::lookAt(const LuxVector&, const LuxVector&, const LuxVector&).
Bool LuxAPIWriter::lookAt(const LuxVector& camPos,
                          const LuxVector& trgPos,
                          const LuxVector& upVec)
{
  const static ULONG  sBufferSize(256);
  CHAR                buffer[sBufferSize];

  // write comment, if there is one
  writeComment(mSceneFile);

  // create C string with LookAt + parameters
  LONG len = sprintf(buffer,
                     "LookAt %.8g %.8g %.8g  %.8g %.8g %.8g  %.8g %.8g %.8g\n",
                     camPos.x, camPos.y, camPos.z,
                     trgPos.x, trgPos.y, trgPos.z,
                     upVec.x,  upVec.y,  upVec.z);

  // write LookAt string
  if (!mSceneFile->WriteBytes(buffer, len)) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::lookAt(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::film(const IdentifierName type,
                        const LuxParamSet&   paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "Film", type, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::camera(IdentifierName     type,
                          const LuxParamSet& paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "Camera", type, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::pixelFilter(IdentifierName     type,
                               const LuxParamSet& paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "PixelFilter", type, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::sampler(IdentifierName     type,
                           const LuxParamSet& paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "Sampler", type, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::surfaceIntegrator(IdentifierName     type,
                                     const LuxParamSet& paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "SurfaceIntegrator", type, 0, 0, paramSet, TRUE) &&
         writeLine(*mSceneFile, 0);
}


Bool LuxAPIWriter::accelerator(IdentifierName     type,
                               const LuxParamSet& paramSet)
{
  writeComment(mSceneFile);
  return writeSetting(*mSceneFile, "Accelerator", type, 0, 0, paramSet, TRUE) &&
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
  mObjectsFile->WriteChar('\n');
  writeComment(mObjectsFile);
  return writeLine(*mObjectsFile, "AttributeBegin");
}


Bool LuxAPIWriter::attributeEnd(void)
{
  writeComment(mObjectsFile);
  return writeLine(*mObjectsFile, "AttributeEnd\n");
}


Bool LuxAPIWriter::objectBegin(const IdentifierName name)
{
  mObjectsFile->WriteChar('\n');
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "\nObjectBegin", name);
}


Bool LuxAPIWriter::objectEnd(void)
{
  writeComment(mObjectsFile);
  return writeLine(*mObjectsFile, "ObjectEnd");
}


Bool LuxAPIWriter::lightSource(IdentifierName     type,
                               const LuxParamSet& paramSet)
{
  mObjectsFile->WriteChar('\n');
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "LightSource", type, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::areaLightSource(IdentifierName     type,
                                   const LuxParamSet& paramSet)
{
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "AreaLightSource", type, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::texture(IdentifierName     name,
                           IdentifierName     colorType,
                           IdentifierName     type,
                           const LuxParamSet& paramSet)
{
  mObjectsFile->WriteChar('\n');
  writeComment(mMaterialsFile);
  return writeSetting(*mMaterialsFile, "Texture", name, colorType, type, paramSet, TRUE);
}


Bool LuxAPIWriter::makeNamedMaterial(IdentifierName     name,
                                     const LuxParamSet& paramSet)
{
  writeComment(mMaterialsFile);
  Bool success = writeSetting(*mMaterialsFile, "MakeNamedMaterial", name, 0, 0, paramSet, TRUE);
  success &= mMaterialsFile->WriteChar('\n');
  return success;
}


Bool LuxAPIWriter::namedMaterial(IdentifierName name)
{
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "NamedMaterial", name);
}


Bool LuxAPIWriter::material(IdentifierName     type,
                            const LuxParamSet& paramSet)
{
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "Material", type, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::transform(const LuxMatrix& matrix)
{
  const static ULONG  sBufferSize(360);
  CHAR                buffer[sBufferSize];

  BaseFile* outFile = mWorldStarted ? mObjectsFile : mSceneFile;

  // write buffered comment, if there is one
  writeComment(*outFile);

  LONG len = sprintf(buffer,
                     "Transform [%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g  "
                                "%.8g %.8g %.8g %.8g]\n",
                     matrix.values[0],  matrix.values[1],  matrix.values[2],  matrix.values[3], 
                     matrix.values[4],  matrix.values[5],  matrix.values[6],  matrix.values[7], 
                     matrix.values[8],  matrix.values[9],  matrix.values[10], matrix.values[11], 
                     matrix.values[12], matrix.values[13], matrix.values[14], matrix.values[15]);
  if (!outFile->WriteBytes(buffer, len)) {
    ERRLOG_ID_RETURN_VALUE(FALSE, IDS_ERROR_IO,
                           "LuxAPIWriter::transform(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::reverseOrientation(void)
{
  writeComment(mObjectsFile);
  return writeLine(*mObjectsFile, "ReverseOrientation");
}


Bool LuxAPIWriter::shape(IdentifierName     type,
                         const LuxParamSet& paramSet)
{
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "Shape", type, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::portalShape(IdentifierName     type,
                               const LuxParamSet& paramSet)
{
  writeComment(mObjectsFile);
  return writeSetting(*mObjectsFile, "PortalShape", type, 0, 0, paramSet, TRUE);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxAPIWriter.
 *****************************************************************************/


///
void LuxAPIWriter::writeComment(BaseFile& file)
{
  if (mCommentLen) {
    file.WriteBytes((void*)"# ", 2);
    file.WriteBytes((void*)mComment, mCommentLen);
    file.WriteChar('\n');
    mCommentLen = 0;
  }
}


/// Writes a string and terminates it with a line feed.
///
/// @param[in]  file
///   Reference to the while into which the line should be written.
/// @param[in]  text
///   The text to write. (can be NULL)
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeLine(BaseFile&   file,
                             const CHAR* text)
{
  if (text) {
    LONG len = (LONG)strlen(text);
    if (len > 0) {
      if (!file.WriteBytes((void*)text, len)) {
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
Bool LuxAPIWriter::writeSetting(BaseFile&      file,
                                SettingNameT   setting,
                                IdentifierName identifier)
{
  if (!file.WriteBytes((void*)setting, (VLONG)strlen(setting)) ||
      (identifier &&
       (!file.WriteBytes((void*)" \"", 2) ||
        !file.WriteBytes((void*)identifier, (VLONG)strlen(identifier)) ||
        !file.WriteBytes((void*)"\"\n", 2))))
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
Bool LuxAPIWriter::writeSetting(BaseFile&          file,
                                SettingNameT       setting,
                                IdentifierName     identifier1,
                                IdentifierName     identifier2,
                                IdentifierName     identifier3,
                                const LuxParamSet& paramSet,
                                Bool               newLine)
{
  // These are the type identifiers used in the Lux file format.
  // NOTE: Changes in LuxParamType must be applied here too.
  static const struct {
    CHAR* nameStr;
    ULONG nameStrLen;
  } cTokenType[LUX_TYPE_NUMBER] =
    { {"bool ", 5},
      {"integer ", 8},
      {"float ", 6},
      {"vector ", 7},
      {"color ", 6},
      {"point ", 6},
      {"float ", 6},
      {"normal ", 7},
      {"integer ", 8},
      {"integer ", 8},
      {"string ", 7},
      {"texture ", 8} };

  Bool success = TRUE;

  // write setting identifier
  success &= file.WriteBytes((void*)setting, (VLONG)strlen(setting));
  if (identifier1) {
    success &= file.WriteBytes((void*)" \"", 2);
    success &= file.WriteBytes((void*)identifier1, (VLONG)strlen(identifier1));
    success &= file.WriteChar('"');
  }
  if (identifier2) {
    success &= file.WriteBytes((void*)" \"", 2);
    success &= file.WriteBytes((void*)identifier2, (VLONG)strlen(identifier2));
    success &= file.WriteChar('"');
  }
  if (identifier3) {
    success &= file.WriteBytes((void*)" \"", 2);
    success &= file.WriteBytes((void*)identifier3, (VLONG)strlen(identifier3));
    success &= file.WriteChar('"');
  }
  if (newLine) {
    success &= file.WriteChar('\n');
  }

  // write parameters
  CHAR          valueString[128];
  VLONG         valueStringLen;
  LuxParamType tokenType;
  LuxParamName tokenName;
  LuxParamRef  tokenValue;
  ULONG         tokenArraySize;
  for (LuxParamNumber c=0; c<paramSet.ParamNumber(); ++c) {

    // for convenience store token data in local variables
    tokenType      = paramSet.ParamTypes()[c];
    tokenName      = paramSet.ParamNames()[c];
    tokenValue     = paramSet.ParamValues()[c];
    tokenArraySize = paramSet.ParamArraySizes()[c];

    // write token name and type
    success &= file.WriteBytes((void*)" \"", 2);
    success &= file.WriteBytes(cTokenType[tokenType].nameStr, cTokenType[tokenType].nameStrLen);
    success &= file.WriteBytes(tokenName, (VLONG)strlen(tokenName));
    success &= file.WriteChar('"');

    // write token values
    if (tokenArraySize > 1) {
      success &= file.WriteBytes((void*)" [\n", 3);
    } else {
      success &= file.WriteBytes((void*)" [", 2);
    }
    switch (tokenType) {
      case LUX_BOOL:
        {
          const LuxBool* values = (const LuxBool*)tokenValue;
          if (tokenArraySize == 1) {
            if (values[0]) {
              success &= file.WriteBytes((void*)"\"true\"", 6);
            } else {
              success &= file.WriteBytes((void*)"\"false\"", 7);
            }
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              if (values[i]) {
                success &= file.WriteBytes((void*)"\"true\"\n", 7);
              } else {
                success &= file.WriteBytes((void*)"\"false\"\n", 8);
              }
            }
          }
          break;
        }
      case LUX_INTEGER:
        {
          const LuxInteger* values = (const LuxInteger*)tokenValue;
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
          const LuxFloat* values = (const LuxFloat*)tokenValue;
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
          const LuxVector* values = (const LuxVector*)tokenValue;
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
          const LuxColor* values = (const LuxColor*)tokenValue;
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
          const LuxPoint* values = (const LuxPoint*)tokenValue;
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
      case LUX_UV:
        {
          const LuxFloat* values = (const LuxFloat*)tokenValue;
          if (tokenArraySize == 2) {
            valueStringLen = sprintf(valueString, "%.8g %.8g",
                                     values[0], values[1]);
            success &= file.WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=1; i<tokenArraySize; i+=2) {
              valueStringLen = sprintf(valueString, "%.8g %.8g\n",
                                       values[i-1], values[i]);
              success &= file.WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LUX_NORMAL:
        {
          const LuxNormal* values = (const LuxNormal*)tokenValue;
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
          const LuxInteger* values = (const LuxInteger*)tokenValue;
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
          const LuxInteger* values = (const LuxInteger*)tokenValue;
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
          const LuxString* values = (const LuxString*)tokenValue;
          if (tokenArraySize == 1) {
            valueStringLen = (VLONG)values[0].size();
            success &= file.WriteChar('"');
            if (valueStringLen) {
              LuxString mangled = values[0];
              for (VLONG c=0; c<valueStringLen; ++c) {
                if (mangled[c] == '"')  mangled[c] = '_';
              }
              success &= file.WriteBytes((void*)mangled.c_str(), valueStringLen);
            }
            success &= file.WriteChar('"');
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = (VLONG)values[i].size();
              success &= file.WriteChar('"');
              if (valueStringLen) {
                LuxString mangled = values[0];
                for (VLONG c=0; c<valueStringLen; ++c) {
                  if (mangled[c] == '"')  mangled[c] = '_';
                }
                success &= file.WriteBytes((void*)values[i].c_str(), valueStringLen);
              }
              success &= file.WriteBytes((void*)"\"\n", 2);
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
