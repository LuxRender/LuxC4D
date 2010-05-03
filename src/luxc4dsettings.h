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

#ifndef __LUXC4DSETTINGS_H__
#define __LUXC4DSETTINGS_H__  1



#include <c4d.h>

#include "luxparamset.h"



#define PID_LUXC4D_SETTINGS  1023045



/***************************************************************************//*!
 This class is a video post effect which allows the user to set scene specific
 parameters for LuxRender.

 This might change in the future when this plugin will (hopefully) integrate
 Lux into C4D. We then will also rename this class.
*//****************************************************************************/
class LuxC4DSettings : public VideoPostData
{
  INSTANCEOF(LuxC4DSettings,VideoPostData)


public:

  static NodeData* alloc(void);
  static Bool registerPlugin(void);

  virtual Bool Init(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*  node,
                               Description* description,
                               LONG&        flags);
  virtual Bool SetDParameter(GeListNode*   node,
                             const DescID& id,
                             const GeData& value,
                             LONG&         flags);

  void getFilm(const char*& name,
               LuxParamSet& paramSet);
  LONG getOutputFilePathSettings(Filename& userDefined);
  void getPixelFilter(const char*& name,
                      LuxParamSet& paramSet);
  void getSampler(const char*& name,
                  LuxParamSet& paramSet,
                  LONG         xResolution,
                  LONG         yResolution);
  void getSurfaceIntegrator(const char*& name,
                            LuxParamSet& paramSet,
                            Bool&        isBidirectional);
  void getAccelerator(const char*& name,
                      LuxParamSet& paramSet);
  void getExportFilename(BaseDocument& document,
                         Filename&     path,
                         Bool&         overwritingAllowed);
  Real getC4D2LuxScale(void);
  Real getBumpSampleDistance(void);
  Real getTextureGamma(void);
  Real getColorGamma(void);
  Bool useRelativePaths(void);


private:

  /// Helper structure for retrieving a setting from the descriptions and for
  /// converting it to a Lux parameter. It defines the parameter ID and name
  /// and also provides the buffer where the parameter gets stored.
  template<class T>
  struct Descr2Param {
    LONG         mID;
    T            mParam;
    LuxParamName mParamName;

    Descr2Param(LONG id, LuxParamName paramName)
    : mID(id), mParamName(paramName)
    {}
  };


  BaseContainer* getData(void);

  void copyParam(Descr2Param<LuxBool>& descr2Param,
                 LuxParamSet&          paramSet);

  void copyParam(Descr2Param<LuxInteger>& descr2Param,
                 LuxParamSet&             paramSet);

  void copyParam(Descr2Param<LuxFloat>& descr2Param,
                 LuxParamSet&           paramSet,
                 LuxFloat               scaleFactor = 1.0);

  void copyParam(Descr2Param<LuxString>& descr2Param,
                 LuxParamSet&            paramSet,
                 const char**            cycleEntries,
                 LONG                    cycleEntryCount);
};



#endif  // #ifndef __LUXC4DSETTINGS_H__
