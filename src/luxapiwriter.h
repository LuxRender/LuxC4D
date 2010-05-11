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

#ifndef __LUXAPIWRITER_H__
#define __LUXAPIWRITER_H__  1



#include <c4d.h>

#include "luxapi.h"



/***************************************************************************//*!
 This class implements LuxAPI and writes the passed commands into a .lxs scene
 file.
*//****************************************************************************/
class LuxAPIWriter : public LuxAPI
{
public:

  LuxAPIWriter(void);
  ~LuxAPIWriter(void);

  Bool init(const Filename& sceneFile,
            Bool            useRelativePaths,
            Bool            resume,
            Bool            &sceneFilesExist);
  inline LONG errorStringID(void) const;

  virtual Bool startScene(const char* head);
  virtual Bool endScene(void);

  virtual void processFilePath(FilePath& path);
  virtual Filename getSceneFilename(void);

  virtual Bool setComment(const char* text);
  virtual Bool setComment(const String& text);

  virtual Bool film(IdentifierName     type,
                    const LuxParamSet& paramSet);

  virtual Bool lookAt(const LuxVector& camPos,
                      const LuxVector& trgPos,
                      const LuxVector& upVec);

  virtual Bool camera(IdentifierName     type,
                      const LuxParamSet& paramSet);

  virtual Bool pixelFilter(IdentifierName     type,
                           const LuxParamSet& paramSet);

  virtual Bool sampler(IdentifierName     type,
                       const LuxParamSet& paramSet);

  virtual Bool surfaceIntegrator(IdentifierName     type,
                                 const LuxParamSet& paramSet);

  virtual Bool accelerator(IdentifierName     type,
                           const LuxParamSet& paramSet);

  virtual Bool worldBegin(void);
  virtual Bool worldEnd(void);
  virtual Bool attributeBegin(void);
  virtual Bool attributeEnd(void);
  virtual Bool objectBegin(IdentifierName name);
  virtual Bool objectEnd(void);

  virtual Bool lightGroup(IdentifierName name);
  virtual Bool lightSource(IdentifierName     type,
                           const LuxParamSet& paramSet);
  virtual Bool areaLightSource(IdentifierName     type,
                               const LuxParamSet& paramSet);

  virtual Bool texture(IdentifierName     name,
                       IdentifierName     colorType,
                       IdentifierName     type,
                       const LuxParamSet& paramSet,
                       const LuxMatrix*   trafo);

  virtual Bool makeNamedMaterial(IdentifierName     name,
                                 const LuxParamSet& paramSet);
  virtual Bool namedMaterial(IdentifierName name);
  virtual Bool material(IdentifierName     type,
                        const LuxParamSet& paramSet);

  virtual Bool transform(const LuxMatrix& matrix);
  virtual Bool reverseOrientation(void);

  virtual Bool shape(IdentifierName     type,
                     const LuxParamSet& paramSet);

  virtual Bool portalShape(IdentifierName     type,
                           const LuxParamSet& paramSet);


private:

  typedef const char* SettingNameT;

  Bool                mFilesOpen;
  Filename            mSceneFilename;
  FilePath            mSceneFileDirectory;
  Bool                mUseRelativePaths;
  Bool                mResume;
  AutoAlloc<BaseFile> mSceneFile;
  Filename            mMaterialsFilename;
  AutoAlloc<BaseFile> mMaterialsFile;
  Filename            mObjectsFilename;
  AutoAlloc<BaseFile> mObjectsFile;
  Bool                mWorldStarted;
  LONG                mErrorStringID;
  CHAR                mComment[2048];
  ULONG               mCommentLen;

  void writeComment(BaseFile& file);
  Bool writeLine(BaseFile&   file,
                 const CHAR* text);
  Bool writeSetting(BaseFile&            file,
                    SettingNameT         setting,
                    const IdentifierName identifier);
  Bool writeSetting(BaseFile&          file,
                    SettingNameT       setting,
                    IdentifierName     identifier1,
                    IdentifierName     identifier2,
                    IdentifierName     identifier3,
                    const LuxParamSet& paramSet,
                    Bool                newLine);
};



/// Returns the ID of the current error or 0 if no error occured.
inline LONG LuxAPIWriter::errorStringID(void) const
{
  return mErrorStringID;
}



#endif  // #ifndef __LUXAPIWRITER_H__
