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



/// Constructs a new instance.
LuxAPIWriter::LuxAPIWriter()
: mSceneFileOpened(FALSE),
  mErrorStringID(0)
{}


/// Destroys the instance. If the scene file wasn't closed before, it will be
/// closed and a warning is dumped.
LuxAPIWriter::~LuxAPIWriter(void)
{
  // if output fileis still open for some reason, finish it and close it
  if (mSceneFileOpened) {
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
  if (mSceneFileOpened) {
    ERRLOG("LuxAPIWriter::init(): scene file wasn't closed properly before");
    if (!endScene()) {
      ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::init():  could not close old scene file -> can't create new scene file");
    }
  }

  // just store the filename, the file be opened later
  mSceneFilename = sceneFile;
  mErrorStringID = 0;
  return TRUE;
}


Bool LuxAPIWriter::startScene(const CHAR* head)
{
  if (mSceneFileOpened) {
    ERRLOG("LuxAPIWriter::startScene(): scene file wasn't closed properly before");
    if (!endScene()) {
      ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::startScene(): could not close old scene file -> can't create new scene file");
    }
  }
  if (!mSceneFilename.Content()) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_INTERNAL, "LuxAPIWriter::startScene(): no filename has been set");
  }
  if (!mSceneFile->Open(mSceneFilename, GE_WRITE, FILE_DIALOG)) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::startScene(): could not open file '" + mSceneFilename.GetString() + "'");
  }
  mSceneFileOpened = TRUE;
  return writeLine(head) && writeLine("\n\n#Global Settings\n");
}


Bool LuxAPIWriter::endScene(void)
{
  if (!mSceneFileOpened)  return TRUE;
  if (!mSceneFile->Close()) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::endScene(): could not close scene file");
  }
  mSceneFileOpened = FALSE;
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
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::lookAt(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::film(IdentifierNameT    name,
                        const LuxParamSet& paramSet)
{
  return writeSetting("Film", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::camera(IdentifierNameT     name,
                          const LuxParamSet& paramSet)
{
  return writeSetting("Camera", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::pixelFilter(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting("PixelFilter", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::sampler(IdentifierNameT    name,
                           const LuxParamSet& paramSet)
{
  return writeSetting("Sampler", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::surfaceIntegrator(IdentifierNameT    name,
                                     const LuxParamSet& paramSet)
{
  return writeSetting("SurfaceIntegrator", name, 0, 0, paramSet, TRUE);
}


Bool LuxAPIWriter::accelerator(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting("Accelerator", name, 0, 0, paramSet, FALSE);
}


Bool LuxAPIWriter::worldBegin(void)
{
  return writeLine("\n\n#Scene Description\n\nWorldBegin");
}


Bool LuxAPIWriter::worldEnd(void)
{
  return writeLine("\nWorldEnd");
}


Bool LuxAPIWriter::attributeBegin(void)
{
  return writeLine("\nAttributeBegin");
}


Bool LuxAPIWriter::attributeEnd(void)
{
  return writeLine("AttributeEnd");
}


Bool LuxAPIWriter::objectBegin(IdentifierNameT name)
{
  return writeSetting("\nObjectBegin", name);
}


Bool LuxAPIWriter::objectEnd(void)
{
  return writeLine("ObjectEnd");
}


Bool LuxAPIWriter::lightSource(IdentifierNameT    name,
                               const LuxParamSet& paramSet)
{
  return writeSetting("LightSource", name, 0, 0, paramSet, FALSE);
}


Bool LuxAPIWriter::texture(IdentifierNameT    name,
                           IdentifierNameT    colorType,
                           IdentifierNameT    type,
                           const LuxParamSet& paramSet)
{
  return writeSetting("\nTexture", name, colorType, type, paramSet, FALSE);
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
  if (!mSceneFile->WriteBytes(buffer, len)) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::lookAt(): writing to file failed");
  }
  return TRUE;
}


Bool LuxAPIWriter::material(IdentifierNameT    name,
                            const LuxParamSet& paramSet)
{
  return writeSetting("Material", name, 0, 0, paramSet, FALSE);
}


Bool LuxAPIWriter::shape(IdentifierNameT    name,
                         const LuxParamSet& paramSet)
{
  return writeSetting("Shape", name, 0, 0, paramSet, FALSE);
}


/// Writes a string and terminates it with a line feed.
/// @param[in]  text
///   The text to write. (can be NULL)
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeLine(const CHAR* text)
{
  if (text) {
    LONG len = (LONG)strlen(text);
    if (len > 0) {
      if (!mSceneFile->WriteBytes(text, len)) {
        ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::writeLine(): writing to file failed");
      }
    }
  }

  if (!mSceneFile->WriteChar('\n')) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::writeLine(): writing to file failed");
  }
  return TRUE;
}


/// Writes a simple statement of the form
///   <setting> [identifier]
///
/// @param[in]  setting
///   The specifier of the setting (e.g. "ObjectBegin"). (must not be NULL)
/// @param[in]  identifier1
///   The first identifier (e.g. the object name). (can be NULL)
/// @return
///   TRUE if successful, otherwise FALSE.
Bool LuxAPIWriter::writeSetting(SettingNameT    setting,
                                IdentifierNameT identifier)
{
  if (!mSceneFile->WriteBytes(setting, (VLONG)strlen(setting)) ||
      (identifier &&
       (!mSceneFile->WriteBytes(" \"", 2) ||
        !mSceneFile->WriteBytes(identifier, (VLONG)strlen(identifier)) ||
        !mSceneFile->WriteBytes("\"\n", 2))))
  {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::writeSetting(): writing to file failed");
  }

  return TRUE;
}


/// Writes a complete statement of the form
///   <setting> <identifier1>? <identifier2>? <identifier3>? <parameter>*
/// If a parameter is a value array, the values will be in separate lines.
///
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
Bool LuxAPIWriter::writeSetting(SettingNameT       setting,
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
  } cTokenType[LuxParamSet::LUX_NUMBER] =
    { {"bool ", 5},
      {"integer ", 8},
      {"float ", 6},
      {"vector ", 7},
      {"color ", 6},
      {"point ", 6},
      {"normal ", 7},
      {"integer ", 8},
      {"string ", 7},
      {"texture ", 8} };

  Bool success = TRUE;

  // write setting identifier
  success &= mSceneFile->WriteBytes(setting, (VLONG)strlen(setting));
  if (identifier1) {
    success &= mSceneFile->WriteBytes(" \"", 2);
    success &= mSceneFile->WriteBytes(identifier1, (VLONG)strlen(identifier1));
    success &= mSceneFile->WriteChar('"');
  }
  if (identifier2) {
    success &= mSceneFile->WriteBytes(" \"", 2);
    success &= mSceneFile->WriteBytes(identifier2, (VLONG)strlen(identifier2));
    success &= mSceneFile->WriteChar('"');
  }
  if (identifier3) {
    success &= mSceneFile->WriteBytes(" \"", 2);
    success &= mSceneFile->WriteBytes(identifier3, (VLONG)strlen(identifier3));
    success &= mSceneFile->WriteChar('"');
  }
  if (newLine) {
    success &= mSceneFile->WriteChar('\n');
  }

  // write parameters
  CHAR        valueString[128];
  VLONG       valueStringLen;
  LuxParamSet::ParamTypeT    tokenType;
  LuxParamSet::ParamNameT    tokenName;
  LuxParamSet::ParamPointerT tokenPointer;
  ULONG                      tokenArraySize;
  for (LuxParamSet::ParamNumberT c=0; c<paramSet.ParamNumber(); ++c) {

    // for convenience store token data in local variables
    tokenType      = paramSet.ParamTypes()[c];
    tokenName      = paramSet.ParamNames()[c];
    tokenPointer   = paramSet.ParamPointers()[c];
    tokenArraySize = paramSet.ParamArraySizes()[c];

    // write token name and type
    success &= mSceneFile->WriteBytes(" \"", 2);
    success &= mSceneFile->WriteBytes(cTokenType[tokenType].nameStr, cTokenType[tokenType].nameStrLen);
    success &= mSceneFile->WriteBytes(tokenName, (VLONG)strlen(tokenName));
    success &= mSceneFile->WriteChar('"');

    // write token values
    if (tokenArraySize > 1) {
      success &= mSceneFile->WriteBytes(" [\n", 3);
    } else {
      success &= mSceneFile->WriteBytes(" [", 2);
    }
    switch (tokenType) {
      case LuxParamSet::LUX_BOOL:
        {
          const LuxBoolT* values = (const LuxBoolT*)tokenPointer;
          if (tokenArraySize == 1) {
            if (values[0]) {
              success &= mSceneFile->WriteBytes("\"true\"", 6);
            } else {
              success &= mSceneFile->WriteBytes("\"false\"", 7);
            }
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              if (values[i]) {
                success &= mSceneFile->WriteBytes("\"true\"\n", 7);
              } else {
                success &= mSceneFile->WriteBytes("\"false\"\n", 8);
              }
            }
          }
          break;
        }
      case LuxParamSet::LUX_INTEGER:
        {
          const LuxIntegerT* values = (const LuxIntegerT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%d", values[0]);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%d\n", values[i]);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_FLOAT:
        {
          const LuxFloatT* values = (const LuxFloatT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g", values[0]);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g\n", values[i]);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_VECTOR:
        {
          const LuxVectorT* values = (const LuxVectorT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_COLOR:
        {
          const LuxColorT* values = (const LuxColorT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].c[0], values[0].c[1], values[0].c[2]);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].c[0], values[i].c[1], values[i].c[2]);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_POINT:
        {
          const LuxPointT* values = (const LuxPointT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_NORMAL:
        {
          const LuxNormalT* values = (const LuxNormalT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = sprintf(valueString, "%.8g %.8g %.8g",
                                     values[0].x, values[0].y, values[0].z);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = sprintf(valueString, "%.8g %.8g %.8g\n",
                                       values[i].x, values[i].y, values[i].z);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_TRIANGLE:
        {
          const LuxIntegerT* values = (const LuxIntegerT*)tokenPointer;
          if (tokenArraySize == 3) {
            valueStringLen = sprintf(valueString, "%d %d %d",
                                     values[0], values[1], values[2]);
            success &= mSceneFile->WriteBytes(valueString, valueStringLen);
          } else {
            for (ULONG i=2; i<tokenArraySize; i+=3) {
              valueStringLen = sprintf(valueString, "%d %d %d\n",
                                       values[i-2], values[i-1], values[i]);
              success &= mSceneFile->WriteBytes(valueString, valueStringLen);
            }
          }
          break;
        }
      case LuxParamSet::LUX_STRING:
      case LuxParamSet::LUX_TEXTURE:
        {
          const LuxStringT* values = (const LuxStringT*)tokenPointer;
          if (tokenArraySize == 1) {
            valueStringLen = (VLONG)values[0].size();
            if (valueStringLen) {
              success &= mSceneFile->WriteChar('"');
              success &= mSceneFile->WriteBytes(values[0].c_str(), valueStringLen);
              success &= mSceneFile->WriteChar('"');
            }
          } else {
            for (ULONG i=0; i<tokenArraySize; ++i) {
              valueStringLen = (VLONG)values[i].size();
              if (valueStringLen) {
                success &= mSceneFile->WriteChar('"');
                success &= mSceneFile->WriteBytes(values[i].c_str(), valueStringLen);
                success &= mSceneFile->WriteBytes("\"\n", 2);
              }
            }
          }
          break;
        }
      default:
        ERRLOG_ID_RETURN_FALSE(IDS_ERROR_INTERNAL, "LuxAPIWriter::writeSetting(): invalid type specifier in token name");
    }
    success &= mSceneFile->WriteChar(']');
    if (newLine) {
      success &= mSceneFile->WriteChar('\n');
    }
  }

  // write line feed, to finish statement and skip to the next line
  success &= mSceneFile->WriteChar('\n');

  // check if some of the write operations have failed
  if (!success) {
    ERRLOG_ID_RETURN_FALSE(IDS_ERROR_IO, "LuxAPIWriter::writeSetting(): writing to file failed");
  }
  return TRUE;
}
