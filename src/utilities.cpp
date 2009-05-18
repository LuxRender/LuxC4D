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

#include <cstdarg>
#include <cstdio>

#include "utilities.h"



///
void debugLog(const CHAR* format, ...)
{
  static const SizeT cBufferSize = 2048;
  CHAR charBuffer[cBufferSize];

  va_list args;
	va_start(args, format);
  vsprintf(charBuffer, format, args);
	C4DOS.Ge->GeDebugOut("%s", charBuffer);
	va_end(args);
}


///
void debugLog(const String& msg)
{
  static const SizeT cBufferSize = 2048;
  CHAR charBuffer[cBufferSize];

  msg.GetCString(charBuffer, cBufferSize);
  charBuffer[cBufferSize-1] = '\0';
	C4DOS.Ge->GeDebugOut("%s", charBuffer);
}


///
void convert2LuxString(const String& c4dString,
                       LuxString&    luxString)
{
  CHAR buffer[2048];
  c4dString.GetCString(buffer, sizeof(buffer));
  buffer[sizeof(buffer)-1] = '\0';
  luxString = buffer;
}


///
void convert2LuxString(const Filename& c4dPath,
                       LuxString&      luxString)
{
  CHAR buffer[2048];
  c4dPath.GetString().GetCString(buffer, sizeof(buffer));
  buffer[sizeof(buffer)-1] = '\0';

  for (CHAR* c=buffer; *c!='\0'; ++c) {
    if (*c == '\\') {
      *c = '/';
    }
  }

  luxString = buffer;
}


/// Makes a parameter visible or hides it.
///
/// @param[in]  description
///   The desciption object the parameter belongs to.
/// @param[in]  paramID
///   The parameter ID to show/hide.
/// @param[in]  params
///   Specifies the parameter element to show/hide.
/// @param[in]  show
///   Set this to TRUE make it visible or to FALSE to hide it.
void showParameter(Description* description,
                   LONG         paramID,
                   AtomArray*   params,
                   Bool         show)
{
  BaseContainer* descrData = description->GetParameterI(DescLevel(paramID),
                                                        params);
  if (descrData) {
    descrData->SetBool(DESC_HIDE, !show);
  }
}


/// Obtains a description parameter of type LONG of an object.
///
/// @param[in]  atom
///   The atom of which we want to read out the parameter.
/// @param[in]  paramID
///   The ID of the description parameter.
/// @param[in]  preset
///   The value the function returs, if the parameter doesn't exist or is not
///   of type LONG.
/// @return
///   The value of the parameter or the preset value, if we couldn't fetch it.
LONG getParameterLong(C4DAtom& atom,
                      LONG     paramID,
                      LONG     preset)
{
  GeData parameter;
  if (atom.GetParameter(DescLevel(paramID), parameter, 0)) {
    return parameter.GetLong();
  }
  return preset;
}


/// Obtains a description parameter of type Real of an object.
///
/// @param[in]  atom
///   The atom of which we want to read out the parameter.
/// @param[in]  paramID
///   The ID of the description parameter.
/// @param[in]  preset
///   The value the function returs, if the parameter doesn't exist or is not
///   of type Real.
/// @return
///   The value of the parameter or the preset value, if we couldn't fetch it.
Real getParameterReal(C4DAtom& atom,
                      LONG     paramID,
                      Real     preset)
{
  GeData parameter;
  if (atom.GetParameter(DescLevel(paramID), parameter, 0)) {
    return parameter.GetReal();
  }
  return preset;
}


/// Obtains a description parameter of type Vector of an object.
///
/// @param[in]  atom
///   The atom of which we want to read out the parameter.
/// @param[in]  paramID
///   The ID of the description parameter.
/// @param[in]  preset
///   The value the function returs, if the parameter doesn't exist or is not
///   of type Vector.
/// @return
///   The value of the parameter or the preset value, if we couldn't fetch it.
Vector getParameterVector(C4DAtom&      atom,
                          LONG          paramID,
                          const Vector& preset)
{
  GeData parameter;
  if (atom.GetParameter(DescLevel(paramID), parameter, 0)) {
    return parameter.GetVector();
  }
  return preset;
}


///
String getParameterString(C4DAtom&      atom,
                          LONG          paramID,
                          const String& preset)
{
  GeData parameter;
  if (atom.GetParameter(DescLevel(paramID), parameter, 0)) {
    return parameter.GetString();
  }
  return preset;
}


//
Filename getParameterFilename(C4DAtom&        atom,
                              LONG            paramID,
                              const Filename& preset)
{
  GeData parameter;
  if (atom.GetParameter(DescLevel(paramID), parameter, 0)) {
    return parameter.GetFilename();
  }
  return preset;
}


/// Obtains a description parameter of type LINK from an object.
///
/// @param[in]  node
///   The atom node of which we want to read out the parameter.
/// @param[in]  paramID
///   The ID of the description parameter.
/// @param[in]  instanceOf
///   The object type that is allowed in the link.
/// @return
///   The link parameter (BaseList2D*) or o if we couldn't fetch it.
BaseList2D* getParameterLink(GeListNode& node,
                             LONG        paramID,
                             LONG        instanceOf)
{
  GeData parameter;
  if (node.GetParameter(DescLevel(paramID), parameter, 0)) {
    BaseLink* link = parameter.GetBaseLink();
    if (link) {
      return link->GetLink(node.GetDocument(), instanceOf);
    }
  }
  return 0;
}


///
//Filename getRelativePath(const Filename& path,
//                         const Filename& startPath)
//{
//  String pathDirStr = path.GetDirectory().GetString().ToLower();
//  if (pathDir
//  String startDirStr = startPath.GetDirectory().GetString().ToLower();
//
//  LONG startDirLength = startDirStr.GetLength();
//  for (LONG pos=0; pos<startDirLength; ++pos) {
//    if (pathDirStr
//  }
//}
