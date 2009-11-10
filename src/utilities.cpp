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

#include "dlist.h"
#include "utilities.h"



#ifdef __PC
  static const CHAR gPathDelimiter = '\\';
#else
  static const CHAR gPathDelimiter = '/';
#endif



/// Works like printf, but writes the string to the debug console.
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


/// Writes a string to the debug console.
void debugLog(const String& msg)
{
  static const SizeT cBufferSize = 2048;
  CHAR charBuffer[cBufferSize];

  msg.GetCString(charBuffer, cBufferSize);
  charBuffer[cBufferSize-1] = '\0';
	C4DOS.Ge->GeDebugOut("%s", charBuffer);
}


/// Converts a C4D String to a LuxString.
///
/// @param[in]  c4dString
///   The C4D String object to convert.
/// @param[out]  luxString
///   The LuxString where the content of the C4D String will be stored.
void convert2LuxString(const String&  c4dString,
                       LuxString&     luxString)
{
#ifdef __PC
  const StringEncoding encoding = StXbit;
#else
  const StringEncoding encoding = StUTF8;
#endif
  CHAR buffer[2048];

  c4dString.GetCString(buffer, sizeof(buffer), encoding);
  buffer[sizeof(buffer)-1] = '\0';
  luxString = buffer;
}


/// Converts a C4D Filename to a LuxString. It also replaces '\' with '/'.
///
/// @param[in]  c4dPath
///   The C4D Filename object to convert.
/// @param[out]  luxString
///   The LuxString where the content of the C4D Filename will be stored.
void convert2LuxString(const Filename& c4dPath,
                       LuxString&      luxString)
{
#if defined(__PC)
  String c4dString = c4dPath.GetString();
  for (LONG pos=-1; c4dString.FindFirst('\\', &pos, pos+1); ) {
    c4dString[pos] = '/';
  }
#elif defined(__MAC) && (_C4D_VERSION<100)
  String c4dString = convertToPosixPath(const Filename& path);
#else
  String c4dString = c4dPath.GetString();
#endif

  convert2LuxString(c4dString, luxString);
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


/// Obtains a description parameter of type LONG of a C4DAtom.
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


/// Obtains a description parameter of type Real of a C4DAtom.
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


/// Obtains a description parameter of type Vector of a C4DAtom.
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


/// Obtains a description parameter of type String of a C4DAtom.
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


/// Obtains a description parameter of type Filename of a C4DAtom.
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


/// Traverses the generated hierarchy of a parametric object upwards
/// and looks for the first occurence of a tag of a specific type.
///
/// @param[in]  object
///   The object of which the generator hierarchy will we traversed.
/// @param[in]  tagId
///   The ID of the tag to look for.
/// @return
///   The pointer to the first found tag of the specified type or NULL
///   if we couldn't find it.
BaseTag* findTagForParamObject(BaseObject* object,
                               LONG        tagId)
{
  BaseTag* tag;
  for (; object; object=object->GetCacheParent()) {
    tag = object->GetTag(tagId);
    if (tag) { return tag; }
  }
  return 0;
}


///
Bool isAbsolutePathString(const String& pathStr)
{
#ifdef __PC
  if ((pathStr.GetLength() > 1) && (pathStr[1] == ':')) {
    return TRUE;
  }
  const CHAR cDelimiter = '\\';
#else
  const CHAR cDelimiter = '/';
#endif
  if ((pathStr.GetLength() > 0) && (pathStr[0] == cDelimiter)) {
    return TRUE;
  }
  return FALSE;
}


///
String cleanupFilenameString(const String& str)
{
  typedef DList<String> TokensT;

  // do nothing of string is empty
  if (!str.Content()) { return str; }

  // check if string starts with delimiter
  Bool startsWithDelimiter=FALSE;
  if ((str[0] == '\\') || (str[0] == '/')) {
    startsWithDelimiter = TRUE;
  }

  // split string into tokens
  TokensT tokens;
  LONG    pos, tokenStart=1;
  for (tokenStart=-1, pos=0; pos<str.GetLength(); ++pos) {
    if ((str[pos] == '\\') || (str[pos] == '/')) {
      if (pos > tokenStart+1) {
        tokens.pushBack(str.SubStr(tokenStart+1, pos-tokenStart-1));
      }
      tokenStart = pos;
    }
  }
  if (pos > tokenStart+1) {
    tokens.pushBack(str.SubStr(tokenStart+1, pos-tokenStart-1));
  }

  // loop over tokens and fix "." and ".."
  for (TokensT::IteratorT iter=tokens.begin(); iter.isValid();) {
    if (*iter == ".") {
      tokens.remove(iter);
    } else if ((*iter == "..") && (!iter.isFront())) {
      --iter;
      tokens.remove(iter);
      tokens.remove(iter);
    } else {
      ++iter;
    }
  }

  // rebuild clean string out of tokens
  String result;
  if (startsWithDelimiter) {
    result = String(1, gPathDelimiter);
  }
  for (TokensT::IteratorT iter=tokens.begin(); iter.isValid(); ++iter) {
    result += *iter;
    if (!iter.isBack()) { result += String(1, gPathDelimiter); }
  }

  return result;
}


///
Filename joinFilenames(const Filename& first,
                       const Filename& second)
{
  // join the strings of both paths and store them as a new Filename
  String joinedStr(first.GetString() +
                   String(1, gPathDelimiter) +
                   second.GetString());

  return Filename(cleanupFilenameString(joinedStr));
}


///
Filename getRelativePath(const Filename& path,
                         const Filename& basePath)
{
  //String pathDirStr = path.GetDirectory().GetString().ToLower();
  //if (pathDir
  //String startDirStr = startPath.GetDirectory().GetString().ToLower();

  //LONG startDirLength = startDirStr.GetLength();
  //for (LONG pos=0; pos<startDirLength; ++pos) {
  //  if (pathDirStr
  //}

  return path;
}



#if defined(__MAC) && (_C4D_VERSION<100)
#include <CoreFoundation/CoreFoundation.h>

/// This function converts a filename of C4D R9.x, which is 
String convertToPosixPath(const Filename& path)
{
  static const SizeT cCStrBufferSize = 4096;
  
  // convert Filename to a plain C string in UTF-8
  String filenameStr = path.GetString();
  CHAR   filenameCStr[cCStrBufferSize];
  filenameStr.GetCString(filenameCStr, cCStrBufferSize-2, StUTF8);
  filenameCStr[cCStrBufferSize-1] = '\0';
  
  // convert plain C string to CoreFoundation CFString
  CFStringRef filenameCFString = CFStringCreateWithCString(NULL,
                                                           filenameCStr,
                                                           kCFStringEncodingUTF8);
  
  // convert CFString to CFURL and release CFString
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL,
                                               filenameCFString,
                                               kCFURLHFSPathStyle,
                                               FALSE);
  CFRelease(filenameCFString);
  
  // convert CFURL back to a CFString using the POSIX path style and release CFURL
  filenameCFString = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
  CFRelease(url);
  
  // convert CFString back to plain C string and release CFString
  CFStringGetCString(filenameCFString,
                     filenameCStr,
                     cCStrBufferSize,
                     kCFStringEncodingUTF8);
  CFRelease(filenameCFString);
  filenameCStr[cCStrBufferSize-1] = '\0';
  
  // return a String created from the C string
  return String(filenameCStr, StUTF8);
}

#endif  // #if defined(__MAC) && (_C4D_VERSION<100)
