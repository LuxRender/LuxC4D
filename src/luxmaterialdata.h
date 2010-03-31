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
  Bool           mExportDefault;
  LuxFloat       mDefaultFloat;
  LuxColor       mDefaultColor;
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

  /// Clears the texture and disables the channel.
  inline void clear(void)
  {
    mTexture.clear();
    mEnabled = FALSE;
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
 This generic class is the base class for storing a Lux material. It implements
 most of the functionality for handling channel settings as they are always the
 same. The derived classes only contain the explicit description of the channels
 and additional parameters (if available).
*//****************************************************************************/
class LuxMaterialData
{
public:

  Bool setChannel(ULONG           channelId,
                  LuxTextureDataH texture);
  Bool setEmissionChannel(LuxTextureDataH  texture,
                          const LuxString& lightGroup);
  Bool hasEmissionChannel(void);
  const LuxString& getLightGroup(void) const;
  Bool setBumpChannel(LuxTextureDataH texture,
                      LuxFloat        sampleDistance = 0.0);
  Bool hasBumpChannel(void);
  Bool setAlphaChannel(LuxTextureDataH texture,
                       Bool            inverted);
  Bool hasAlphaChannel(void);

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name,
                         const LuxString* underlyingMaterialName=0);


protected:

  const LuxMaterialInfo&         mInfo;
  FixArray1D<LuxMaterialChannel> mChannels;
  LuxMaterialChannel             mEmissionChannel;
  LuxString                      mLightGroup;
  LuxMaterialChannel             mBumpChannel;
  LuxFloat                       mBumpSampleDistance;
  LuxMaterialChannel             mAlphaChannel;
  Bool                           mAlphaInverted;

  LuxMaterialData(const LuxMaterialInfo& info);

  Bool sendToAPI(LuxAPI&            receiver,
                 const LuxString&   name,
                 const LuxString*   underlyingMaterialName,
                 const LuxParamSet* addParams);


private:

  LuxMaterialData(const LuxMaterialData& other) : mInfo(other.mInfo) {}
  LuxMaterialData& operator=(const LuxMaterialData& other)  {}
};

typedef AutoRef<LuxMaterialData>  LuxMaterialDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux glass material.
*//****************************************************************************/
class LuxGlassData : public LuxMaterialData
{
public:

  enum ChannelId {
    REFLECTION = 0,   // LUX_COLOR_TEXTURE
    TRANSMISSION,     // LUX_COLOR_TEXTURE
    IOR,              // LUX_FLOAT_TEXTURE
    CAUCHY_B,         // LUX_FLOAT_TEXTURE
    FILM_THICKNESS,   // LUX_FLOAT_TEXTURE
    FILM_IOR,         // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxBool mArchitectural;

  LuxGlassData(void);
  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name,
                         const LuxString* underlyingMaterialName=0);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxGlassData>  LuxGlassDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux glass material.
*//****************************************************************************/
class LuxRoughGlassData : public LuxMaterialData
{
public:

  enum ChannelId {
    REFLECTION = 0,   // LUX_COLOR_TEXTURE
    TRANSMISSION,     // LUX_COLOR_TEXTURE
    IOR,              // LUX_FLOAT_TEXTURE
    CAUCHY_B,         // LUX_FLOAT_TEXTURE
    UROUGHNESS,       // LUX_FLOAT_TEXTURE
    VROUGHNESS,       // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxRoughGlassData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxRoughGlassData>  LuxRoughGlassDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux glossy material.
*//****************************************************************************/
class LuxGlossyData : public LuxMaterialData
{
public:

  enum ChannelId {
    DIFFUSE = 0,        // LUX_COLOR_TEXTURE
    SPECULAR,           // LUX_COLOR_TEXTURE
    SPECULAR_IOR,       // LUX_FLOAT_TEXTURE
    ABSORPTION,         // LUX_COLOR_TEXTURE
    ABSORPTION_DEPTH,   // LUX_FLOAT_TEXTURE
    UROUGHNESS,         // LUX_FLOAT_TEXTURE
    VROUGHNESS,         // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxGlossyData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxGlossyData>  LuxGlossyDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux matte material.
*//****************************************************************************/
class LuxMatteData : public LuxMaterialData
{
public:

  enum ChannelId {
    DIFFUSE = 0,    // LUX_COLOR_TEXTURE
    SIGMA,          // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxMatteData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxMatteData>  LuxMatteDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux matte translucent material.
*//****************************************************************************/
class LuxMatteTranslucentData : public LuxMaterialData
{
public:

  enum ChannelId {
    DIFFUSE = 0,    // LUX_COLOR_TEXTURE
    TRANSMISSION,   // LUX_COLOR_TEXTURE
    SIGMA,          // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxMatteTranslucentData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxMatteTranslucentData>  LuxMatteTranslucentDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux metal material.
*//****************************************************************************/
class LuxMetalData : public LuxMaterialData
{
public:

  enum ChannelId {
    UROUGHNESS = 0,   // LUX_FLOAT_TEXTURE
    VROUGHNESS,       // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  String mName;
  Bool   mIsFilename;

  LuxMetalData(void);
  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name,
                         const LuxString* underlyingMaterialName=0);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxMetalData>  LuxMetalDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux shiny metal material.
*//****************************************************************************/
class LuxShinyMetalData : public LuxMaterialData
{
public:

  enum ChannelId {
    REFLECTION = 0,   // LUX_COLOR_TEXTURE
    SPECULAR,         // LUX_COLOR_TEXTURE
    UROUGHNESS,       // LUX_FLOAT_TEXTURE
    VROUGHNESS,       // LUX_FLOAT_TEXTURE
    FILM_THICKNESS,   // LUX_FLOAT_TEXTURE
    FILM_IOR,         // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxShinyMetalData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxShinyMetalData>  LuxShinyMetalDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux mirror material.
*//****************************************************************************/
class LuxMirrorData : public LuxMaterialData
{
public:

  enum ChannelId {
    REFLECTION = 0,   // LUX_COLOR_TEXTURE
    FILM_THICKNESS,   // LUX_FLOAT_TEXTURE
    FILM_IOR,         // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxMirrorData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxMirrorData>  LuxMirrorDataH;



/***************************************************************************//*!
 This class stores the parameters of a Lux car paint material.
*//****************************************************************************/
class LuxCarPaintData : public LuxMaterialData
{
public:

  enum ChannelId {
    DIFFUSE = 0,      // LUX_COLOR_TEXTURE
    SPECULAR_1,       // LUX_COLOR_TEXTURE
    R1,               // LUX_FLOAT_TEXTURE
    M1,               // LUX_FLOAT_TEXTURE
    SPECULAR_2,       // LUX_COLOR_TEXTURE
    R2,               // LUX_FLOAT_TEXTURE
    M2,               // LUX_FLOAT_TEXTURE
    SPECULAR_3,       // LUX_COLOR_TEXTURE
    R3,               // LUX_FLOAT_TEXTURE
    M3,               // LUX_FLOAT_TEXTURE
    ABSORPTION,       // LUX_COLOR_TEXTURE
    ABSORPTION_DEPTH, // LUX_FLOAT_TEXTURE
    CHANNEL_COUNT
  };

  LuxCarPaintData(void);


private:

  static const LuxChannelInfo  sChannelInfos[CHANNEL_COUNT];
  static const LuxMaterialInfo sMaterialInfo;
};

typedef AutoRef<LuxCarPaintData>  LuxCarPaintDataH;



#endif  // #ifndef __LUXMATERIALDATA_H__