#ifndef __LUXMATERIALDATA_H__
#define __LUXMATERIALDATA_H__



#include "fixarray1d.h"
#include "luxapi.h"
#include "luxtexturedata.h"



struct LuxChannelInfo
{
  LuxTextureType mType;
  const CHAR*    mName;
  const CHAR*    mTextureSuffix;
};


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



struct LuxMaterialInfo
{
  const CHAR*           mName;
  ULONG                 mChannelCount;
  const LuxChannelInfo* mChannelInfos;
};


class LuxMaterialData
{
  public:

    LuxMaterialData(const LuxMaterialInfo& info);

    Bool setChannel(ULONG           channelId,
                    LuxTextureDataH texture);

    Bool sendToAPI(LuxAPI&          receiver,
                   const LuxString& name) const;


  protected:

    const LuxMaterialInfo*         mInfo;
    FixArray1D<LuxMaterialChannel> mChannels;


  private:

    LuxMaterialData(const LuxMaterialData& other)  {}
    LuxMaterialData& operator=(const LuxMaterialData& other)  {}
};



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