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

#ifndef __FILEPATH_H__
#define __FILEPATH_H__  1



#include <c4d.h>

#include "dlist.h"



/***************************************************************************//*!
*//****************************************************************************/
class FilePath
{
public:

  FilePath();
  FilePath(const Filename& filename);
  FilePath(const String& pathStr);

  FilePath(const FilePath& other);
  FilePath& operator=(const FilePath& other);

  FilePath& operator=(const Filename& filename);
  FilePath& operator=(const String& pathStr);

  void clear(void);
  inline Bool isEmpty(void) const;
  inline Bool isAbsolute(void) const;

  String getString(void) const;
  String getDirectoryString(void) const;
  String getFileString(void) const;

  LuxString getLuxString(void) const;
  LuxString getLuxDirectoryString(void) const;
  LuxString getLuxFileString(void) const;

  Filename getFilename(void) const;

  void makeRelativeTo(const FilePath& basePath);

  FilePath operator+(const FilePath& other) const;


private:

  typedef DList<String> TokensT;

  CHAR    mDrive;
  Bool    mIsAbsolute;
  TokensT mTokens;


#if defined(__MAC) && (_C4D_VERSION<100)
  LuxString convertToPosixPath(const String& c4dPathStr);
#endif
};



/*****************************************************************************
 * Inlined functions of FilePath
 *****************************************************************************/

inline Bool FilePath::isEmpty(void) const
{
  return (!mDrive && !mIsAbsolute && !mTokens.size());
}


inline Bool FilePath::isAbsolute(void) const
{
  return mIsAbsolute;
}



#endif  // #ifndef __FILEPATH_H__
