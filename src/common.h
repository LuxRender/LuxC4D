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

#ifndef __COMMON_H__
#define __COMMON_H__ 1



#include "c4d.h"
#include "customgui_datetime.h"



/*****************************************************************************
 * Common types
 *****************************************************************************/

typedef VULONG SizeT;



/*****************************************************************************
 * Backwards compatibility
 *****************************************************************************/

#if _C4D_VERSION < 115
#define MINLONGl  MINLONG
#define MAXLONGl  MAXLONG
#endif


#if _C4D_VERSION < 120


class AutoBitmap
{
  public:

    AutoBitmap(const String& name);
    ~AutoBitmap();
    
    operator BaseBitmap* () const { return mBitmap; }


  private:

    BaseBitmap *mBitmap;
};


#define BaseVideoPost                         PluginVideoPost

#define COPYFLAGS                             LONG
#define COPYFLAGS_0                           0,
#define COPYFLAGS_NO_HIERARCHY                COPY_NO_HIERARCHY,
#define COPYFLAGS_NO_ANIMATION                COPY_NO_ANIMATION,
#define COPYFLAGS_NO_BITS                     COPY_NO_BITS,
#define COPYFLAGS_NO_BRANCHES                 COPY_NO_BRANCHES,
#define COPYFLAGS_DOCUMENT                    COPY_DOCUMENT,
#define COPYFLAGS_NO_NGONS                    COPY_NONGONS,
#if _C4D_VERSION >= 115
#define COPYFLAGS_RECURSIONCHECK              COPY_RECURSIONCHECK,
#endif

#define DateTime                              tagDateTime
#define GetDateTimeNow(dateTime)              DateTimeNow(dateTime)

#define DESCFLAGS_DESC                        LONG

#define DESCFLAGS_ENABLE                      LONG
#define DESCFLAGS_ENABLE_0                    0

#define DESCFLAGS_GET                         LONG
#define DESCFLAGS_GET_0                       0
#define DESCFLAGS_GET_PARAM_GET               DESCFLAGS_PARAM_GET
#define DESCFLAGS_GET_NO_GLOBALDATA           DESCFLAGS_NO_GLOBALDATA
#define DESCFLAGS_GET_NO_GEDATADEFAULTVALUE   DESCFLAGS_NO_GEDATADEFAULTVALUE

#define DESCFLAGS_SET                         LONG
#define DESCFLAGS_SET_0                       0
#define DESCFLAGS_SET_PARAM_SET               DESCFLAGS_PARAM_SET
#define DESCFLAGS_SET_USERINTERACTION         DESCFLAGS_PARAM_SET
#define DESCFLAGS_SET_DONTCHECKMINMAX         DESCFLAGS_DONTCHECKMINMAX
#define DESCFLAGS_SET_DONTAFFECTINHERITANCE   DESCFLAGS_DONTAFFECTINHERITANCE
#define DESCFLAGS_SET_FORCESET                DESCFLAGS_FORCESET

#define EXECUTIONPRIORITY_INITIAL             EXECUTION_INITIAL
#define EXECUTIONPRIORITY_ANIMATION           EXECUTION_ANIMATION
#define EXECUTIONPRIORITY_ANIMATION_NLA       EXECUTION_ANIMATION
#define EXECUTIONPRIORITY_EXPRESSION          EXECUTION_EXPRESSION
#define EXECUTIONPRIORITY_DYNAMICS            EXECUTION_DYNAMICS
#define EXECUTIONPRIORITY_GENERATOR           EXECUTION_GENERATOR

#define FILEDIALOG                            LONG
#define FILEDIALOG_NONE                       FILE_NODIALOG
#define FILEDIALOG_ANY                        FILE_DIALOG
#define FILEDIALOG_IGNOREOPEN                 FILE_IGNOREOPEN

#define FILEOPEN                              LONG
#define FILEOPEN_APPEND                       GE_APPEND
#define FILEOPEN_READ                         GE_READ
#define FILEOPEN_WRITE                        GE_WRITE
#define FILEOPEN_READWRITE                    GE_READWRITE
#define FILEOPEN_READ_NOCACHE                 GE_READ
#define FILEOPEN_SHAREDREAD                   GE_READ

#define FILESELECT                            LONG
#define FILESELECT_LOAD                       0
#define FILESELECT_SAVE                       GE_SAVE
#define FILESELECT_DIRECTORY                  GE_DIRECTORY

#define FILESELECTTYPE                        LONG
#define FILESELECTTYPE_ANYTHING               FSTYPE_ANYTHING
#define FILESELECTTYPE_IMAGES                 FSTYPE_IMAGES
#define FILESELECTTYPE_SCENES                 FSTYPE_SCENES
#define FILESELECTTYPE_COFFEE                 FSTYPE_COFFEE

#define FreeChildren                          FreeChilds

#define GEMB_R                                LONG

#define IMAGERESULT                           LONG
#define IMAGERESULT_OK                        IMAGE_OK
#define IMAGERESULT_NOTEXISTING               IMAGE_NOTEXISTING
#define IMAGERESULT_WRONGTYPE                 IMAGE_WRONGTYPE
#define IMAGERESULT_OUTOFMEMORY               IMAGE_NOMEM
#define IMAGERESULT_FILEERROR                 IMAGE_DISKERROR
#define IMAGERESULT_FILESTRUCTURE             IMAGE_FILESTRUCT

#define INITRENDERRESULT                      LONG
#define INITRENDERRESULT_OK                   LOAD_OK
#define INITRENDERRESULT_OUTOFMEMORY          LOAD_NOMEM
#define INITRENDERRESULT_ASSETMISSING         LOAD_NOTFOUND
#define INITRENDERRESULT_UNKNOWNERROR         LOAD_UNKNOWN
#define INITRENDERRESULT_THREADEDLOCK         LOAD_DOUBLENAME

#define MINREALr                              MINREAL
#define MAXREALr                              MAXREAL

#define STRINGENCODING                        StringEncoding
#define STRINGENCODING_XBIT                   StXbit
#define STRINGENCODING_8BIT                   St8bit
#define STRINGENCODING_7BIT                   St7bit
#define STRINGENCODING_7BITHEX                St7bithex
#define STRINGENCODING_UTF8                   StUTF8
#define STRINGENCODING_HTML                   StHTML

#define UVWHandle                             const void*

#define SReal                                 Real
#define SVector                               Vector
#define SMatrix                               Matrix

inline SVector toSV(const SVector& other)  { return other; }
inline SVector toSV(const LVector& other)  { return SV(other); }
inline LVector toLV(const SVector& other)  { return LV(other); }
inline LVector toLV(const LVector& other)  { return other; }
inline Vector  toV (const SVector& other)  { return other; }
inline Vector  toV (const LVector& other)  { return SV(other); }

inline SMatrix toSM(const SMatrix& other)  { return other; }
inline SMatrix toSM(const LMatrix& other)  { return SM(other); }
inline LMatrix toLM(const SMatrix& other)  { return LM(other); }
inline LMatrix toLM(const LMatrix& other)  { return other; }
inline Matrix  toM (const SMatrix& other)  { return other; }
inline Matrix  toM (const LMatrix& other)  { return SM(other); }

#else

inline SVector toSV(const SVector& other)  { return other; }
inline SVector toSV(const LVector& other)  { return other.ToSV(); }
inline LVector toLV(const SVector& other)  { return other.ToLV(); }
inline LVector toLV(const LVector& other)  { return other; }
inline Vector  toV (const SVector& other)  { return other.ToLV(); }
inline Vector  toV (const LVector& other)  { return other; }

inline SMatrix toSM(const SMatrix& other)  { return other; }
inline SMatrix toSM(const LMatrix& other)  { return other.ToSM(); }
inline LMatrix toLM(const SMatrix& other)  { return other.ToLM(); }
inline LMatrix toLM(const LMatrix& other)  { return other; }
inline Matrix  toM (const SMatrix& other)  { return other.ToLM(); }
inline Matrix  toM (const LMatrix& other)  { return other; }

#endif


inline bool fileSelect(Filename&      path,
                       FILESELECTTYPE type,
                       FILESELECT     flags,
                       const String&  title)
{
#if _C4D_VERSION >= 120
  return path.FileSelect(type, flags, title);
#else
  return path.FileSelect(type, flags, &title);
#endif
}


#endif
