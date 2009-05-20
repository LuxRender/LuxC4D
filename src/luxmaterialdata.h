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

#ifndef __LUXMATERIALDATA_H__
#define __LUXMATERIALDATA_H__



#include "fixarray1d.h"
#include "luxapi.h"
#include "luxtexturedata.h"



/***************************************************************************//*!
*//****************************************************************************/
struct LuxChannelInfo
{
  LuxTextureType mType;
  const CHAR*    mName;
  const CHAR*    mTextureSuffix;
};


/***************************************************************************//*!
*//****************************************************************************/
class LuxMaterialChannel
{
  public:

    LuxTextureDataH mTexture;
    Bool            mEnabled;


    inline LuxMaterialChannel()
    : mEnabled(FALSE)
    {}

    inline LuxMaterialChannel(const LuxMaterialChannel& other)
    {
      *this = other;
    }

    inline LuxMaterialChannel& operator=(const LuxMaterialChannel& other)
    {
      mTexture = other.mTexture;
      mEnabled = other.mEnabled;
      return *this;
    }
};



/***************************************************************************//*!
*//****************************************************************************/
struct LuxMaterialInfo
{
  const CHAR*           mName;
  ULONG                 mChannelCount;
  const LuxChannelInfo* mChannelInfos;
};


/***************************************************************************//*!
*//****************************************************************************/
class LuxMaterialData
{
  public:

    LuxMaterialData(const LuxMaterialInfo& info);

    Bool setChannel(ULONG           channelId,
                    LuxTextureDataH texture);

    Bool sendToAPI(LuxAPI&          receiver,
                   const LuxString& name);


  protected:

    const LuxMaterialInfo*         mInfo;
    FixArray1D<LuxMaterialChannel> mChannels;


  private:

    LuxMaterialData(const LuxMaterialData& other)  {}
    LuxMaterialData& operator=(const LuxMaterialData& other)  {}
};



/*****************************************************************************
 * DESCRIPTION OF THE LUX MATTE MATERIAL
 *****************************************************************************/

enum LuxMatteChannelId {
    LUX_MATTE_DIFFUSE = 0,
    LUX_MATTE_SIGMA,
    LUX_MATTE_BUMP,
    LUX_MATTE_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxMatteChannelInfos[] = 
  {
    // LUX_MATTE_DIFFUSE
    { COLOR_TEXTURE, "Kd",      "Kd"    },
    // LUX_MATTE_SIGMA
    { FLOAT_TEXTURE, "sigma",   "sigma" },
    // LUX_MATTE_BUMP
    { FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxMatteInfo =
  {
    "matte",
    LUX_MATTE_CHANNEL_COUNT,
    gLuxMatteChannelInfos
  };



#endif  // #ifndef __LUXMATERIALDATA_H__