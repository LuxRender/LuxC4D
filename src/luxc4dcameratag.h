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

#ifndef __LUXC4DCAMERATAG_H__
#define __LUXC4DCAMERATAG_H__ 1



#include <c4d.h>

#include "common.h"
#include "luxapi.h"
#include "luxparamset.h"
#include "luxtypes.h"



#define PID_LUXC4D_CAMERA_TAG  1024019



/***************************************************************************//*!
 This class implements a tag that can be applied to camera objects, to allow
 the user to set additional camera parameters, that are not available in standard
 C4D cameras.
*//****************************************************************************/
class LuxC4DCameraTag : public TagData
{
  INSTANCEOF(LuxC4DCameraTag,TagData)


public:

  enum CameraType {
    CAMERA_TYPE_PERSPECTIVE = 0,
    CAMERA_TYPE_ORTHOGRAPHIC,
    CAMERA_TYPE_NUMBER
  };

  struct CameraParameters {
    CameraType mType;
    LuxFloat   mFov;
    LuxBool    mScreenWindowSet;
    LuxFloat   mScreenWindow[4];
    LuxBool    mNearClippingSet;
    LuxFloat   mNearClipping;
    LuxBool    mDofEnabled;
    LuxFloat   mLensRadius;
    LuxBool    mAutofocus;
    LuxFloat   mFocalDistance;
    LuxInteger mBladeNumber;
    LuxInteger mLensSamplingType;
    LuxInteger mExponentialPower;
  };


  static NodeData* alloc(void);
  static Bool registerPlugin(void);

  static Bool getCameraParameters(CameraObject&     camera,
                                  LReal             c4d2LuxScale,
                                  BaseContainer&    c4dRenderSettings,
                                  CameraParameters& parameters);

  virtual Bool Init(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*     node,
                               Description*    description,
                               DESCFLAGS_DESC& flags);


private:

  BaseContainer* getData(void);
};



#endif // #ifndef __LUXC4DCAMERATAG_H__
