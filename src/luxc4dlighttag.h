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

#ifndef __LUXC4DLIGHTTAG_H__
#define __LUXC4DLIGHTTAG_H__ 1



#include <c4d.h>



#define PID_LUXC4D_LIGHT_TAG  1023210



/***************************************************************************//*!
 This class implements a tag that can be applied to light objects, to allow
 the user to influence the exported light parameters without having to change
 the C4D lights. It also gives the user more control about Lux specific settings
 and adds Lux specific light types (e.g. sun and sky).
*//****************************************************************************/
class LuxC4DLightTag : public TagData
{
  INSTANCEOF(LuxC4DLightTag,TagData)


public:

  /// Container structure in which we return the light settings for all light
  /// types. Only the parameters that are relevant for the specific light
  /// type will be used, the rest will be 0.
  struct LightParameters {

    // used by all light types
    String         mGroup;
    LONG           mType;
    Real           mBrightness;

    // used by point, spot, distant and area lights
    Vector         mColor;

    // used by spot lights
    Real           mInnerAngle;
    Real           mOuterAngle;

    // used by area lights
    Bool           mFlippedNormals;
    LONG           mShapeType;
    PolygonObject* mShapeObject;
    Vector         mShapeSize;

    // used by area, sun, sky, sun-sky and infinite lights
    LONG           mSamples;

    // used by sun light
    Real           mRelSize;

    // used by sun sky and sun-sky lights
    Real           mTurbidity;

    // used by sky and sun-sky lights
    Bool           mAdvanced;
    Real           mA, mB, mC, mD, mE;

    // used by infinite/sky lights
    LONG           mInfiniteType;
    Filename       mSkyTexFilename;
  };


  static NodeData* alloc(void);
  static Bool registerPlugin(void);

  static Bool getLightParameters(BaseObject&      lightObject,
                                 LReal            c4d2LuxScale,
                                 LightParameters& parameters);

  virtual Bool Init(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*  node,
                               Description* description,
                               LONG&        flags);
  virtual Bool Message(GeListNode *node,
                       LONG       type,
                       void       *data);


private:

  BaseContainer* getData(void);

  static LONG getLightType(BaseObject& lightObject);
  LONG getLightType(void);

  static LONG c4dShape2LuxShape(LONG c4dShapeType);

  Bool copyFromObject(Bool allTypes);
};



#endif  // #ifndef __LUXC4DLIGHTTAG_H__
