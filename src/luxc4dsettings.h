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



#include "c4d.h"
#include "c4d_symbols.h"
#include "luxparamset.h"



#define PID_LUXC4D_SETTINGS  1023045



/***************************************************************************//*!
 This class is a video post effect allows the user to set scene specific
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
  virtual void Free(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*  node,
                               Description* description,
                               LONG&        flags);

  void GetFilm(const char*& name,
               LuxParamSet& paramSet);
  void GetPixelFilter(const char*& name,
                      LuxParamSet& paramSet);
  void GetSampler(const char*& name,
                  LuxParamSet& paramSet);
  void GetSurfaceIntegrator(const char*& name,
                            LuxParamSet& paramSet);
  void GetExportFilename(BaseDocument& document,
                         Filename&     path,
                         Bool&         overwritingAllowed);
  LReal GetC4D2LuxScale(void);


private:

  /// Helper structure for retrieving a setting from the descriptions and for
  /// converting it to a Lux parameter. It defines the parameter ID and name
  /// and also provides the buffer where the parameter gets stored.
  template<class T>
  struct Descr2Param {
    LONG                    mID;
    T                       mParam;
    LuxParamSet::ParamNameT mParamName;

    Descr2Param(LONG id, LuxParamSet::ParamNameT paramName)
    : mID(id), mParamName(paramName)
    {}
  };


  void ShowParameter(Description* description,
                     LONG         paramID,
                     AtomArray*   params,
                     Bool         show);
  
  BaseContainer* GetData(void);

  void CopyParam(Descr2Param<LuxBoolT>& descr2Param,
                 LuxParamSet&           paramSet);

  void CopyParam(Descr2Param<LuxIntegerT>& descr2Param,
                 LuxParamSet&              paramSet);

  void CopyParam(Descr2Param<LuxFloatT>& descr2Param,
                 LuxParamSet&            paramSet);

  void CopyParam(Descr2Param<LuxStringT>& descr2Param,
                 LuxParamSet&            paramSet,
                 const char**            cycleEntries,
                 LONG                    cycleEntryCount);
};



#endif	// #ifndef __LUXC4DSETTINGS_H__
