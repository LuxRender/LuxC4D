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
 This structure stores a description of a material channel which will be used
 for storing and exporting a material channel.
*//****************************************************************************/
struct LuxChannelInfo
{
  LuxTextureType mType;
  const CHAR*    mName;
  const CHAR*    mTextureSuffix;
};


/***************************************************************************//*!
 This class stores the texture and additional attributes of a material channel.
 Each material has multiple channels.
*//****************************************************************************/
class LuxMaterialChannel
{
  public:

    LuxTextureDataH mTexture;
    Bool            mEnabled;


    /// Default constructor which disables this channel by default.
    inline LuxMaterialChannel()
    : mEnabled(FALSE)
    {}

    /// Copy constructor.
    inline LuxMaterialChannel(const LuxMaterialChannel& other)
    {
      *this = other;
    }

    /// Copy operator.
    inline LuxMaterialChannel& operator=(const LuxMaterialChannel& other)
    {
      mTexture = other.mTexture;
      mEnabled = other.mEnabled;
      return *this;
    }
};



/***************************************************************************//*!
 This structure stores a description of a whole Lux material which will be used
 for storing and exporting a material.
*//****************************************************************************/
struct LuxMaterialInfo
{
  const CHAR*           mName;
  ULONG                 mChannelCount;
  const LuxChannelInfo* mChannelInfos;
};


/***************************************************************************//*!
 This generic class stores a Lux material. By constructing it with a specific
 material description we define the properties of a the wanted Lux material.
*//****************************************************************************/
class LuxMaterialData
{
  public:

    LuxMaterialData(const LuxMaterialInfo& info);

    Bool setChannel(ULONG           channelId,
                    LuxTextureDataH texture);
    Bool setEmissionChannel(LuxTextureDataH texture);
    Bool hasEmissionChannel(void);

    Bool sendToAPI(LuxAPI&          receiver,
                   const LuxString& name);


  protected:

    const LuxMaterialInfo*         mInfo;
    FixArray1D<LuxMaterialChannel> mChannels;
    LuxMaterialChannel             mEmissionChannel;


  private:

    LuxMaterialData(const LuxMaterialData& other)  {}
    LuxMaterialData& operator=(const LuxMaterialData& other)  {}
};



/*****************************************************************************
 * DESCRIPTION OF THE LUX GLASS MATERIAL
 *****************************************************************************/

enum LuxGlassChannelId {
    LUX_GLASS_REFLECTION = 0,
    LUX_GLASS_TRANSMISSION,
    LUX_GLASS_IOR,
    LUX_GLASS_BUMP,
    LUX_GLASS_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxGlassChannelInfos[LUX_GLASS_CHANNEL_COUNT] = 
  {
    // LUX_GLASS_REFLECTION
    { LUX_COLOR_TEXTURE, "Kr",      "refl"  },
    // LUX_GLASS_TRANSMISSION
    { LUX_COLOR_TEXTURE, "Kt",      "trans" },
    // LUX_GLASS_IOR
    { LUX_FLOAT_TEXTURE, "index",   "ior"   },
    // LUX_GLASS_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxGlassInfo =
  {
    "glass",
    LUX_GLASS_CHANNEL_COUNT,
    gLuxGlassChannelInfos
  };



/*****************************************************************************
 * DESCRIPTION OF THE LUX ROUGH GLASS MATERIAL
 *****************************************************************************/

enum LuxRoughGlassChannelId {
    LUX_ROUGH_GLASS_REFLECTION = 0,
    LUX_ROUGH_GLASS_TRANSMISSION,
    LUX_ROUGH_GLASS_IOR,
    LUX_ROUGH_GLASS_UROUGHNESS,
    LUX_ROUGH_GLASS_VROUGHNESS,
    LUX_ROUGH_GLASS_BUMP,
    LUX_ROUGH_GLASS_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxRoughGlassChannelInfos[LUX_ROUGH_GLASS_CHANNEL_COUNT] = 
  {
    // LUX_ROUGH_GLASS_REFLECTION
    { LUX_COLOR_TEXTURE, "Kr",         "refl"  },
    // LUX_ROUGH_GLASS_TRANSMISSION
    { LUX_COLOR_TEXTURE, "Kt",         "trans" },
    // LUX_ROUGH_GLASS_IOR
    { LUX_FLOAT_TEXTURE, "index",      "ior"   },
    // LUX_ROUGH_GLASS_UROUGHNESS
    { LUX_FLOAT_TEXTURE, "uroughness", "urough" },
    // LUX_ROUGH_GLASS_VROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough" },
    // LUX_ROUGH_GLASS_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap",    "bump"  }
  };

static const LuxMaterialInfo gLuxRoughGlassInfo =
  {
    "roughglass",
    LUX_ROUGH_GLASS_CHANNEL_COUNT,
    gLuxRoughGlassChannelInfos
  };



/*****************************************************************************
 * DESCRIPTION OF THE LUX GLOSSY MATERIAL
 *****************************************************************************/

enum LuxGlossyChannelId {
    LUX_GLOSSY_DIFFUSE = 0,
    LUX_GLOSSY_SPECULAR,
    LUX_GLOSSY_UROUGHNESS,
    LUX_GLOSSY_VROUGHNESS,
    LUX_GLOSSY_BUMP,
    LUX_GLOSSY_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxGlossyChannelInfos[LUX_GLOSSY_CHANNEL_COUNT] = 
  {
    // LUX_GLOSSY_DIFFUSE
    { LUX_COLOR_TEXTURE, "Kd",         "diff"   },
    // LUX_GLOSSY_SPECULAR
    { LUX_COLOR_TEXTURE, "Ks",         "spec"   },
    // LUX_GLOSSY_UROUGHNESS
    { LUX_FLOAT_TEXTURE, "uroughness", "urough" },
    // LUX_GLOSSY_UROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough" },
    // LUX_GLOSSY_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap",    "bump"   }
  };

static const LuxMaterialInfo gLuxGlossyInfo =
  {
    "glossy",
    LUX_GLOSSY_CHANNEL_COUNT,
    gLuxGlossyChannelInfos
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

static const LuxChannelInfo gLuxMatteChannelInfos[LUX_MATTE_CHANNEL_COUNT] = 
  {
    // LUX_MATTE_DIFFUSE
    { LUX_COLOR_TEXTURE, "Kd",      "diff"  },
    // LUX_MATTE_SIGMA
    { LUX_FLOAT_TEXTURE, "sigma",   "sigma" },
    // LUX_MATTE_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxMatteInfo =
  {
    "matte",
    LUX_MATTE_CHANNEL_COUNT,
    gLuxMatteChannelInfos
  };



/*****************************************************************************
 * DESCRIPTION OF THE LUX MATTE TRANSLUCENT MATERIAL
 *****************************************************************************/

enum LuxMatteTranslucentChannelId {
    LUX_MATTE_TRANSLUCENT_DIFFUSE = 0,
    LUX_MATTE_TRANSLUCENT_TRANSMISSION,
    LUX_MATTE_TRANSLUCENT_SIGMA,
    LUX_MATTE_TRANSLUCENT_BUMP,
    LUX_MATTE_TRANSLUCENT_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxMatteTranslucentChannelInfos[LUX_MATTE_TRANSLUCENT_CHANNEL_COUNT] = 
  {
    // LUX_MATTE_TRANSLUCENT_DIFFUSE
    { LUX_COLOR_TEXTURE, "Kr",      "diff"  },
    // LUX_MATTE_TRANSLUCENT_TRANSMISSION
    { LUX_COLOR_TEXTURE, "Kt",      "trans" },
    // LUX_MATTE_TRANSLUCENT_SIGMA
    { LUX_FLOAT_TEXTURE, "sigma",   "sigma" },
    // LUX_MATTE_TRANSLUCENT_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxMatteTranslucentInfo =
  {
    "mattetranslucent",
    LUX_MATTE_TRANSLUCENT_CHANNEL_COUNT,
    gLuxMatteTranslucentChannelInfos
  };



/*****************************************************************************
 * DESCRIPTION OF THE LUX MIRROR MATERIAL
 *****************************************************************************/

enum LuxMirrorChannelId {
    LUX_MIRROR_REFLECTION = 0,
    LUX_MIRROR_BUMP,
    LUX_MIRROR_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxMirrorChannelInfos[LUX_MIRROR_CHANNEL_COUNT] = 
  {
    // LUX_MIRROR_REFLECTION
    { LUX_COLOR_TEXTURE, "Kr",      "refl"  },
    // LUX_MIRROR_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxMirrorInfo =
  {
    "mirror",
    LUX_MIRROR_CHANNEL_COUNT,
    gLuxMirrorChannelInfos
  };



/*****************************************************************************
 * DESCRIPTION OF THE LUX SHINY METAL MATERIAL
 *****************************************************************************/

enum LuxShinyMetalChannelId {
    LUX_SHINY_METAL_REFLECTION = 0,
    LUX_SHINY_METAL_SPECULAR,
    LUX_SHINY_METAL_UROUGHNESS,
    LUX_SHINY_METAL_VROUGHNESS,
    LUX_SHINY_METAL_BUMP,
    LUX_SHINY_METAL_CHANNEL_COUNT
  };

static const LuxChannelInfo gLuxShinyMetalChannelInfos[LUX_SHINY_METAL_CHANNEL_COUNT] = 
  {
    // LUX_SHINY_METAL_REFLECTION
    { LUX_COLOR_TEXTURE, "Kr",      "refl"  },
    // LUX_SHINY_METAL_SPECULAR
    { LUX_COLOR_TEXTURE, "Ks",      "spec"  },
    // LUX_SHINY_METAL_UROUGHNESS
    { LUX_FLOAT_TEXTURE, "uroughness", "urough" },
    // LUX_SHINY_METAL_VROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough" },
    // LUX_MATTE_BUMP
    { LUX_FLOAT_TEXTURE, "bumpmap", "bump"  }
  };

static const LuxMaterialInfo gLuxShinyMetalInfo =
  {
    "shinymetal",
    LUX_SHINY_METAL_CHANNEL_COUNT,
    gLuxShinyMetalChannelInfos
  };



#endif  // #ifndef __LUXMATERIALDATA_H__