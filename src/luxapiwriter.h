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



#include "c4d.h"

#include "luxapi.h"



class LuxAPIWriter : public LuxAPI
{
public:

  LuxAPIWriter(void);
  ~LuxAPIWriter(void);

  Bool init(const Filename& sceneFile);
  inline LONG errorStringID(void) const;

  virtual Bool startScene(const CHAR* head);
  virtual Bool endScene(void);

  virtual Bool film(IdentifierNameT    name,
                    const LuxParamSet& paramSet);

  virtual Bool lookAt(const LuxVectorT& camPos,
                      const LuxVectorT& trgPos,
                      const LuxVectorT& upVec);

  virtual Bool camera(IdentifierNameT    name,
                      const LuxParamSet& paramSet);

  virtual Bool pixelFilter(IdentifierNameT    name,
                           const LuxParamSet& paramSet);

  virtual Bool sampler(IdentifierNameT    name,
                       const LuxParamSet& paramSet);

  virtual Bool surfaceIntegrator(IdentifierNameT    name,
                                 const LuxParamSet& paramSet);

  virtual Bool accelerator(IdentifierNameT    name,
                           const LuxParamSet& paramSet);

  virtual Bool worldBegin(void);
  virtual Bool worldEnd(void);
  virtual Bool attributeBegin(void);
  virtual Bool attributeEnd(void);
  virtual Bool objectBegin(IdentifierNameT name);
  virtual Bool objectEnd(void);

  virtual Bool lightSource(IdentifierNameT    name,
                           const LuxParamSet& paramSet);

  virtual Bool texture(IdentifierNameT    name,
                       IdentifierNameT    colorType,
                       IdentifierNameT    type,
                       const LuxParamSet& paramSet);

  virtual Bool transform(const LuxMatrixT& matrix);

  virtual Bool material(IdentifierNameT    name,
                        const LuxParamSet& paramSet);

  virtual Bool shape(IdentifierNameT    name,
                     const LuxParamSet& paramSet);


private:

  typedef const CHAR* SettingNameT;

  Filename            mSceneFilename;
  AutoAlloc<BaseFile> mSceneFile;
  Bool                mSceneFileOpened;
  LONG                mErrorStringID;

  Bool writeLine(const CHAR* text);
  Bool writeSetting(SettingNameT    setting,
                    IdentifierNameT identifier);
  Bool writeSetting(SettingNameT       setting,
                    IdentifierNameT    identifier1,
                    IdentifierNameT    identifier2,
                    IdentifierNameT    identifier3,
                    const LuxParamSet& paramSet);
};



inline LONG LuxAPIWriter::errorStringID(void) const
{
  return mErrorStringID;
}



#endif  // #ifndef __LUXAPIWRITER_H__
