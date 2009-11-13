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

#include "filepath.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class FilePath.
 *****************************************************************************/

FilePath::FilePath()
: mDrive(0),
  mIsAbsolute(FALSE)
{}


FilePath::FilePath(const Filename& filename)
{
  *this = filename.GetString();
}


FilePath::FilePath(const String& pathStr)
{
  *this = pathStr;
}


FilePath::FilePath(const FilePath& other)
{
  *this = other;
}


FilePath& FilePath::operator=(const FilePath& other)
{
  mDrive      = other.mDrive;
  mIsAbsolute = other.mIsAbsolute;
  mTokens     = other.mTokens;
  return *this;
}


FilePath& FilePath::operator=(const Filename& filename)
{
  *this = filename.GetString();
  return *this;
}


FilePath& FilePath::operator=(const String& pathStr)
{
  // clear current file path content
  clear();

  // do nothing of string is empty
  if (!pathStr.Content()) { return *this; }

  // initialise loop vars
  LONG pathStrLen = pathStr.GetLength();
  LONG pos=0, tokenStart=-1;

#ifdef __PC
  // on PC get drive, if specified
  if ((pathStrLen > 1) && (pathStr[1] == ':')) {
    mDrive = pathStr[0];
    if (pathStrLen == 2) { return *this; }
    pos = 2;
    tokenStart = 1;
  }
#endif

  // check if string starts with delimiter -> path is absolute
  if ((pathStr[pos] == '\\') || (pathStr[pos] == '/') || (pathStr[pos] == ':')) {
    mIsAbsolute = TRUE;
    ++pos;
    ++tokenStart;
  }

  // split string into tokens and fix "." as well as ".."
  String token;
  for (;; ++pos) {
    if ((pos >= pathStrLen) ||
        (pathStr[pos] == '\\') || (pathStr[pos] == '/') || (pathStr[pos] == ':'))
    {
      if (pos > tokenStart+1) {
        token = pathStr.SubStr(tokenStart+1, pos-tokenStart-1);
        if (token != ".") {
          if ((token == "..") && mTokens.size() && (mTokens.back() != "..")) {
            mTokens.removeBack();
          } else {
            mTokens.pushBack(token);
          }
        }
      }
      if (pos >= pathStrLen) { break; }
      tokenStart = pos;
    }
  }

  return *this;
}


void FilePath::clear(void)
{
  mDrive      = 0;
  mIsAbsolute = FALSE;
  mTokens.erase();
}


FilePath FilePath::getDirectoryPath(void) const
{
  FilePath result(*this);
  result.mTokens.removeBack();
  return result;
}


String FilePath::getString(void) const
{
  return getDirectoryString() + getFileString();
}


String FilePath::getFileString(void) const
{
  if (mTokens.size()) {
    return mTokens.back();
  } else {
    return String();
  }
}


String FilePath::getDirectoryString(void) const
{
  String result;

#ifdef __PC
  // on PC add drive, if available
  if (mDrive) {
    result += String(1, mDrive);
    result += ":";
  }
#endif

  // if path is absolute add a leading delimiter
  if (mIsAbsolute) { result += gPathDelimiterStr; }

  // build the rest of the string by concatenating tokens
  for (TokensT::ConstIteratorT iter=mTokens.begin(); !iter.isBack(); ++iter) {
    result += *iter;
    result += gPathDelimiterStr;
  }

  return result;
}


LuxString FilePath::getLuxString(void) const
{
#if defined(__MAC) && (_C4D_VERSION<100)

  // special case for C4D R9.6: do a HFS-to-POSIX conversion of the standard
  //                            path string
  return convertToPosixPath(getString());

#else

  return getLuxDirectoryString() + getLuxFileString();

#endif  // defined(__MAC) && (_C4D_VERSION<100)
}


LuxString FilePath::getLuxFileString(void) const
{
  LuxString result;
  convert2LuxString(getFileString(), result);
  return result;
}


LuxString FilePath::getLuxDirectoryString(void) const
{
#if defined(__MAC) && (_C4D_VERSION<100)

  // special case for C4D R9.6: do a HFS-to-POSIX conversion of the standard
  //                            directory string
  return convertToPosixPath(getDirectoryString());

#else

  LuxString token, result;

#ifdef __PC
  // on PC add drive, if available
  if (mDrive) {
    result += mDrive;
    result += ":";
  }
#endif

  // if path is absolute add a leading delimiter
  if (mIsAbsolute) { result += "/"; }

  // build the rest of the string by concatenating tokens
  for (TokensT::ConstIteratorT iter=mTokens.begin(); !iter.isBack(); ++iter) {
    convert2LuxString(*iter, token);
    result += token;
    result += "/";
  }

  return result;

#endif  // defined(__MAC) && (_C4D_VERSION<100)
}


Filename FilePath::getFilename(void) const
{
  return Filename(getString());
}


void FilePath::makeRelativeTo(const FilePath& basePath)
{
  // we can't determine the relation between the two paths if they are not both
  // absolute or both relative or have different drives
  if ((mIsAbsolute != basePath.mIsAbsolute) || (mDrive != basePath.mDrive)) {
    return;
  }

  // relative paths don't have drives and well are not absolute
  mDrive = 0;
  mIsAbsolute = FALSE;
  
  // now determine how much of the start of the token chains are equal
  TokensT::IteratorT myToken;
  TokensT::ConstIteratorT otherToken;
  for (myToken=mTokens.begin(), otherToken=basePath.mTokens.begin();
       myToken.isValid() && otherToken.isValid();
       ++otherToken)
  {
    if (myToken->LexCompare(*otherToken) == 0) {
      mTokens.remove(myToken);
    } else {
      break;
    }
  }

  // now add for every left entry of the other token list an ".." at the start
  for (; otherToken.isValid(); ++otherToken) {
    mTokens.pushFront("..");
  }
}


FilePath FilePath::operator+(const FilePath& other) const
{
  if (other.isAbsolute()) { return other; }

  FilePath result(*this);
  for (TokensT::ConstIteratorT iter=other.mTokens.begin();
       iter.isValid(); ++iter)
  {
    if ((*iter == "..") && result.mTokens.size() && (result.mTokens.back() != "..")) {
      result.mTokens.removeBack();
    } else {
      result.mTokens.pushBack(*iter);
    }
  }
  return result;
}


#if defined(__MAC) && (_C4D_VERSION<100)
#include <CoreFoundation/CoreFoundation.h>

LuxString FilePath::convertToPosixPath(const String& c4dPathStr)
{
  static const SizeT cCStrBufferSize = 4096;
  
  // convert Filename to a plain C string in UTF-8
  CHAR pathCStr[cCStrBufferSize];
  c4dPathStr.GetCString(pathCStr, cCStrBufferSize, StUTF8);
  pathCStr[cCStrBufferSize-1] = '\0';
  
  // convert plain C string to CoreFoundation CFString
  CFStringRef pathCFString = CFStringCreateWithCString(NULL,
                                                       pathCStr,
                                                       kCFStringEncodingUTF8);
  
  // convert CFString to CFURL and release CFString
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL,
                                               pathCFString,
                                               kCFURLHFSPathStyle,
                                               FALSE);
  CFRelease(pathCFString);
  
  // convert CFURL back to a CFString using the POSIX path style and release CFURL
  pathCFString = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
  CFRelease(url);
  
  // convert CFString back to plain C string and release CFString
  CFStringGetCString(pathCFString,
                     pathCStr,
                     cCStrBufferSize,
                     kCFStringEncodingUTF8);
  CFRelease(pathCFString);
  pathCStr[cCStrBufferSize-1] = '\0';
  
  // return a LuxString created from the C string
  return LuxString(pathCStr);
}
#endif  // #if defined(__MAC) && (_C4D_VERSION<100)
