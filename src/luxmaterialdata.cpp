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

#include "luxmaterialdata.h"



/*****************************************************************************
 * Implementation of LuxMaterialData.
 *****************************************************************************/

/// Constructs an empty Lux material container by passing in a material
/// info structure which describes the available channels and their
/// attributes.
///
/// @param[in]  info
///   The info structure that describes the properties/channels of the
///   material.
LuxMaterialData::LuxMaterialData(const LuxMaterialInfo& info)
: mInfo(info),
  mBumpSampleDistance(0.0)
{
  // get info and setup channels
  if (!mChannels.init(mInfo.mChannelCount)) {
    ERRLOG(String("LuxMaterialData::init(): could not initialise channels for material '")
             + mInfo.mName + "'");
  }
}


/// Stores a texture in a material channel and activates it.
///
/// @param[in]  channelId
///   The ID of the material channel (must be valid).
/// @param[in]  texture
///   AutoRef to the texture to store in the material channel (texture type
///   must match material channel data type).
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::setChannel(ULONG           channelId,
                                 LuxTextureDataH texture)
{
  // check channel ID
  if (channelId > mChannels.size()) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): invalid channel ID (" +
                               LongToString(channelId) + ")");
  }

  // make sure that the texture AutoRef is valid
  if (!texture) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): no texture passed");
  }

  // make sure that the data type of the texture matches the data type of the
  // material channel
  if (mInfo.mChannelInfos[channelId].mType != texture->mType) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): type mismatch -> can't set texture in channel with ID " +
                               LongToString(channelId));
  }

  // store texture and enable channel
  mChannels[channelId].mTexture = texture;
  mChannels[channelId].mEnabled = TRUE;
  return TRUE;
}


/// Stores a texture in the emission channel and activates it.
///
/// @param[in]  texture
///   AutoRef to the texture to store in the emission channel (texture type
///   must be LUX_COLOR_TEXTURE).
/// @param[in]  lightGroup
///   Name of the light group this emitter belongs to, or an empty string if
///   not specified.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::setEmissionChannel(LuxTextureDataH  texture,
                                         const LuxString& lightGroup)
{
  // make sure that the texture AutoRef is valid
  if (!texture) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setEmissionChannel(): no texture passed");
  }

  // make sure that the data type of the texture matches the data type of the
  // material channel
  if (texture->mType != LUX_COLOR_TEXTURE) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setEmissionChannel(): type mismatch -> can't set emission texture");
  }

  // store texture and enable channel
  mEmissionChannel.mTexture = texture;
  mEmissionChannel.mEnabled = TRUE;
  mLightGroup = lightGroup;
  return TRUE;
}


/// Returns TRUE if the emission channel is active.
Bool LuxMaterialData::hasEmissionChannel(void)
{
  return mEmissionChannel.mEnabled;
}


/// Returns the light group of the emission channel.
const LuxString& LuxMaterialData::getLightGroup(void) const
{
  return mLightGroup;
}


/// Stores a texture in the bump map channel and activates it.
///
/// @param[in]  texture
///   AutoRef to the texture to store in the bump map channel (texture type
///   must be LUX_FLOAT_TEXTURE).
/// @param[in]  sampleDistance
///   The bump map sample distance of the material which is 0.0 by default
///   and therefore the parameter doesn't get exported. Only if it's > 0.0,
///   the parameter gets exported.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::setBumpChannel(LuxTextureDataH texture,
                                     LuxFloat        sampleDistance)
{
  // make sure that the texture AutoRef is valid
  if (!texture) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setBumpChannel(): no texture passed");
  }

  // make sure that the data type of the texture matches the data type of the
  // material channel
  if (texture->mType != LUX_FLOAT_TEXTURE) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setBumpChannel(): type mismatch -> can't set bump texture");
  }

  // store texture and enable channel
  mBumpChannel.mTexture = texture;
  mBumpChannel.mEnabled = TRUE;
  mBumpSampleDistance   = sampleDistance;
  return TRUE;
}


/// Returns TRUE if the bump channel is active.
Bool LuxMaterialData::hasBumpChannel(void)
{
  return mBumpChannel.mEnabled;
}


/// Stores a texture in the alpha channel and activates it.
///
/// @param[in]  texture
///   AutoRef to the texture to store in the alpha channel (texture type
///   must be LUX_FLOAT_TEXTURE).
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::setAlphaChannel(LuxTextureDataH texture,
                                      Bool            inverted)
{
  // make sure that the texture AutoRef is valid
  if (!texture) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setAlphaChannel(): no texture passed");
  }

  // make sure that the data type of the texture matches the data type of the
  // material channel
  if (texture->mType != LUX_FLOAT_TEXTURE) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setAlphaChannel(): type mismatch -> can't set bump texture");
  }

  // store texture and enable channel
  mAlphaChannel.mTexture = texture;
  mAlphaChannel.mEnabled = TRUE;
  mAlphaInverted         = inverted;
  return TRUE;
}


/// Returns TRUE if the alpha channel is active.
Bool LuxMaterialData::hasAlphaChannel(void)
{
  return mAlphaChannel.mEnabled;
}


/// Base implementation of the interface to send the material data to th Lux API.
///
/// @param[in]  receiver
///   The Lux API receiver the material will be sent to.
/// @param[in]  name
///   The name under which the material will be exported. It can then later be
///   referenced by it.
/// @param[in]  underlyingMaterialName  (optional)
///   Pointer to the name of the underlying material (can be NULL), which
///   becomes important if this material has an alpha channel. In that case we
///   will mix it with the underlying material using the alpha channel as amount
///   parameter. If this pointer is NULL, we would then mix with a null material.
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::sendToAPI(LuxAPI&          receiver,
                                const LuxString& name,
                                const LuxString* underlyingMaterialName)
{
  return LuxMaterialData::sendToAPI(receiver, name, underlyingMaterialName, 0);
}


/// Sends the complete material and its textures to a Lux API receiver.
///
/// @param[in]  receiver
///   The Lux API receiver the material will be sent to.
/// @param[in]  name
///   The name under which the material will be exported. It can then later be
///   referenced by it.
/// @param[in]  underlyingMaterialName
///   Pointer to the name of the underlying material (can be NULL), which
///   becomes important if this material has an alpha channel. In that case we
///   will mix it with the underlying material using the alpha channel as amount
///   parameter. If this pointer is NULL, we would then mix with a null material.
/// @param[in]  addParams (optional)
///   Pointer to an additional set of parameters for the material (can be NULL).
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::sendToAPI(LuxAPI&            receiver,
                                const LuxString&   name,
                                const LuxString*   underlyingMaterialName,
                                const LuxParamSet* addParams)
{
  // initialise parameter set (the maximum possible number of parameters is
  // channel count + 3 (material type, bump texture, bump sample distance)
  // + number of additional parameters)
  LuxParamSet paramSet(mInfo.mChannelCount + 3 +
                         (addParams ? addParams->paramNumber() : 0));

  // if there is an alpha channel, the mixed material will get the name that
  // was passed in and the top material will get <name>_top as name
  LuxString noMixMatName(name);
  if (mAlphaChannel.mEnabled) {
    noMixMatName += " raw";
  }

  // add material type to parameter set
  LuxString type(mInfo.mName);
  paramSet.addParam(LUX_STRING, "type", &type);

  // add additional parameters
  if (addParams) {
    paramSet.add(*addParams);
  }

  // loop over active channels, export their textures and add them to parameter
  // set
  FixArray1D<LuxString> textureNames(mChannels.size()+1);
  ULONG textureNameCount = 0;
  for (ULONG channelIx=0; channelIx<mChannels.size(); ++channelIx) {

    // for convenience, store reference to channel in variable
    LuxMaterialChannel&   channel(mChannels[channelIx]);
    const LuxChannelInfo& channelInfo(mInfo.mChannelInfos[channelIx]);

    // if channel is active, export it
    if (channel.mEnabled) {
      textureNames[textureNameCount] = noMixMatName + "." + channelInfo.mTextureSuffix;
      if (!channel.mTexture->sendToAPIAndAddToParamSet(receiver,
                                                       paramSet,
                                                       channelInfo.mName,
                                                       textureNames[textureNameCount]))
      {
        ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): texture export failed");
      }
      ++textureNameCount;
    // otherwise: export defaults, if we should export defaults
    } else if (channelInfo.mExportDefault) {
      if (channelInfo.mType == LUX_COLOR_TEXTURE) {
        paramSet.addParam(LUX_COLOR, channelInfo.mName, (void*)&channelInfo.mDefaultColor);
      } else {
        paramSet.addParam(LUX_FLOAT, channelInfo.mName, (void*)&channelInfo.mDefaultFloat);
      }
    }
  }

  // if bump channel is active, export its texture
  if (mBumpChannel.mEnabled) {
    textureNames[textureNameCount] = noMixMatName + ".bump";
    if (!mBumpChannel.mTexture->sendToAPIAndAddToParamSet(receiver,
                                                          paramSet,
                                                          "bumpmap",
                                                          textureNames[textureNameCount]))
    {
      ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): export of bump texture failed");
    }
    ++textureNameCount;
  }

  // set bump sample distance, if it's > 0.0
  if (mBumpSampleDistance > 0.0) {
    paramSet.addParam(LUX_FLOAT, "bumpmapsampledistance", &mBumpSampleDistance);
  }

  // if emission channel is active, export its texture, but don't add it to
  // material as it will be referenced by an area light command in the object
  // scope
  if (mEmissionChannel.mEnabled) {
    if (mEmissionChannel.mTexture->isConstant()) {
      LuxConstantTextureData constantTexture(mEmissionChannel.mTexture->constantColor(),
                                             1.0);
      if (!constantTexture.sendToAPI(receiver, name + ".L")) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): export of emission texture failed");
      }
    } else {
      if (!mEmissionChannel.mTexture->sendToAPI(receiver, name + ".L")) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): export of emission texture failed");
      }
    }
  }

  // write material
  receiver.makeNamedMaterial(noMixMatName.c_str(), paramSet);

  // if alpha channel is active, export its texture and create a mix material,
  // mixing the underlying material with this material
  if (mAlphaChannel.mEnabled) {
    LuxParamSet mixParamSet(4);
    LuxString   otherMatName;
    // export null material, if no underlying material was specified
    if (!underlyingMaterialName) {
      LuxParamSet nullMatParamSet(1);
      LuxString   nullMatType("null");
      nullMatParamSet.addParam(LUX_STRING, "type", &nullMatType);
      otherMatName = name + " null";
      receiver.makeNamedMaterial(otherMatName.c_str(), nullMatParamSet);
    // otherwise use passed in material name
    } else {
      otherMatName = *underlyingMaterialName;
    }
    // add type of mix material
    LuxString mixType("mix");
    mixParamSet.addParam(LUX_STRING, "type", &mixType);
    // if alpha is not inverted, add name of underlying material as param
    // "namedmaterial1" and name of this material as param "namedmaterial2"
    if (!mAlphaInverted) {
      mixParamSet.addParam(LUX_STRING, "namedmaterial1", &otherMatName);
      mixParamSet.addParam(LUX_STRING, "namedmaterial2", &noMixMatName);
    // if alpha is inverted, add name of underlying material as param
    // "namedmaterial2" and name of this material as param "namedmaterial1"
    } else {
      mixParamSet.addParam(LUX_STRING, "namedmaterial1", &noMixMatName);
      mixParamSet.addParam(LUX_STRING, "namedmaterial2", &otherMatName);
    }
    // export mix material
    LuxString alphaTextureName = name + ".alpha";
    if (!mAlphaChannel.mTexture->sendToAPIAndAddToParamSet(receiver,
                                                           mixParamSet,
                                                           "amount",
                                                           alphaTextureName))
    {
      ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): export of alpha texture failed");
    }
    // write material
    receiver.makeNamedMaterial(name.c_str(), mixParamSet);
  }

  return TRUE;
}



/*****************************************************************************
 * Implementation of LuxGlassData.
 *****************************************************************************/

const LuxChannelInfo LuxGlassData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kr",        "refl",    TRUE, 0.0, LuxColor(0.0) },  // REFLECTION
    { LUX_COLOR_TEXTURE, "Kt",        "trans",   TRUE, 0.0, LuxColor(0.0) },  // TRANSMISSION
    { LUX_FLOAT_TEXTURE, "index",     "ior",     TRUE, 1.5 },                 // IOR
    { LUX_FLOAT_TEXTURE, "cauchyb",   "disp",    FALSE },                     // CAUCHY_B
    { LUX_FLOAT_TEXTURE, "film",      "film",    FALSE },                     // FILM_THICKNESS,
    { LUX_FLOAT_TEXTURE, "filmindex", "filmior", FALSE },                     // FILM_IOR,
  };

const LuxMaterialInfo LuxGlassData::sMaterialInfo =
  { "glass", CHANNEL_COUNT, sChannelInfos };


LuxGlassData::LuxGlassData()
: LuxMaterialData(sMaterialInfo),
  mArchitectural(FALSE)
{}


Bool LuxGlassData::sendToAPI(LuxAPI&          receiver,
                             const LuxString& name,
                             const LuxString* underlyingMaterialName)
{
  LuxParamSet extraParams(1);
  return (extraParams.addParam(LUX_BOOL, "architectural", &mArchitectural) &&
            LuxMaterialData::sendToAPI(receiver,
                                       name,
                                       underlyingMaterialName,
                                       &extraParams));
}



/*****************************************************************************
 * Implementation of LuxRoughGlassData.
 *****************************************************************************/

const LuxChannelInfo LuxRoughGlassData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kr",         "refl",   TRUE, 0.0, LuxColor(0.0) },  // REFLECTION
    { LUX_COLOR_TEXTURE, "Kt",         "trans",  TRUE, 0.0, LuxColor(0.0) },  // TRANSMISSION
    { LUX_FLOAT_TEXTURE, "index",      "ior",    TRUE, 1.5 },                 // IOR
    { LUX_FLOAT_TEXTURE, "cauchyb",    "disp",   FALSE },                     // CAUCHY_B
    { LUX_FLOAT_TEXTURE, "uroughness", "urough", TRUE, 0.0 },                 // UROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough", TRUE, 0.0 },                 // VROUGHNESS
  };

const LuxMaterialInfo LuxRoughGlassData::sMaterialInfo =
  { "roughglass", CHANNEL_COUNT, sChannelInfos };


LuxRoughGlassData::LuxRoughGlassData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxGlossyData.
 *****************************************************************************/

const LuxChannelInfo LuxGlossyData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kd",         "diff",   TRUE, 0.0, LuxColor(0.0) },  // DIFFUSE
    { LUX_COLOR_TEXTURE, "Ks",         "spec",   TRUE, 0.0, LuxColor(0.0) },  // SPECULAR
    { LUX_FLOAT_TEXTURE, "index",      "ior",    FALSE },                     // SPECULAR_IOR
    { LUX_COLOR_TEXTURE, "Ka",         "absorp", FALSE },                     // ABSORPTION
    { LUX_FLOAT_TEXTURE, "d",          "adepth", FALSE },                     // ABSORPTION_DEPTH
    { LUX_FLOAT_TEXTURE, "uroughness", "urough", TRUE, 0.0 },                 // UROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough", TRUE, 0.0 },                 // VROUGHNESS
  };

const LuxMaterialInfo LuxGlossyData::sMaterialInfo =
  { "glossy", CHANNEL_COUNT, sChannelInfos };


LuxGlossyData::LuxGlossyData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxMatteData.
 *****************************************************************************/

const LuxChannelInfo LuxMatteData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kd",      "diff",  TRUE, 0.0, LuxColor(0.0) },  // DIFFUSE
    { LUX_FLOAT_TEXTURE, "sigma",   "sigma", TRUE, 0.0 },                 // SIGMA
  };

const LuxMaterialInfo LuxMatteData::sMaterialInfo =
  { "matte", CHANNEL_COUNT, sChannelInfos };


LuxMatteData::LuxMatteData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxMatteTranslucentData.
 *****************************************************************************/

const LuxChannelInfo LuxMatteTranslucentData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kr",      "diff",  TRUE, 0.0, LuxColor(0.0)  }, // DIFFUSE
    { LUX_COLOR_TEXTURE, "Kt",      "trans", TRUE, 0.0, LuxColor(0.0) },  // TRANSMISSION
    { LUX_FLOAT_TEXTURE, "sigma",   "sigma", TRUE, 0.0 },                 // SIGMA
  };

const LuxMaterialInfo LuxMatteTranslucentData::sMaterialInfo =
  { "mattetranslucent", CHANNEL_COUNT, sChannelInfos };


LuxMatteTranslucentData::LuxMatteTranslucentData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxMetalData.
 *****************************************************************************/

const LuxChannelInfo LuxMetalData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_FLOAT_TEXTURE, "uroughness", "urough", TRUE, 0.0 },   // UROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough", TRUE, 0.0 },   // VROUGHNESS
  };

const LuxMaterialInfo LuxMetalData::sMaterialInfo =
  { "metal", CHANNEL_COUNT, sChannelInfos };


LuxMetalData::LuxMetalData(void)
: LuxMaterialData(sMaterialInfo),
  mIsFilename(FALSE)
{}


Bool LuxMetalData::sendToAPI(LuxAPI&          receiver,
                             const LuxString& name,
                             const LuxString* underlyingMaterialName)
{
  LuxParamSet extraParams(1);
  LuxString   metalName;
  if (!mIsFilename) {
    convert2LuxString(mName, metalName);
  } else if (mName.Content()) {
    FilePath nkFilePath(mName);
    receiver.processFilePath(nkFilePath);
    metalName = nkFilePath.getLuxString();
  }
  if (metalName.size() && !extraParams.addParam(LUX_STRING, "name", &metalName)) {
    return FALSE;
  }
  return LuxMaterialData::sendToAPI(receiver,
                                    name,
                                    underlyingMaterialName,
                                    &extraParams);
}



/*****************************************************************************
 * Implementation of LuxShinyMetalData.
 *****************************************************************************/

const LuxChannelInfo LuxShinyMetalData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kr",         "refl",    TRUE, 0.0, LuxColor(0.0) }, // REFLECTION
    { LUX_COLOR_TEXTURE, "Ks",         "spec",    TRUE, 0.0, LuxColor(0.0) }, // SPECULAR
    { LUX_FLOAT_TEXTURE, "uroughness", "urough",  TRUE, 0.0 },                // UROUGHNESS
    { LUX_FLOAT_TEXTURE, "vroughness", "vrough",  TRUE, 0.0 },                // VROUGHNESS
    { LUX_FLOAT_TEXTURE, "film",       "film",    FALSE },                    // FILM_THICKNESS,
    { LUX_FLOAT_TEXTURE, "filmindex",  "filmior", FALSE },                    // FILM_IOR,
  };

const LuxMaterialInfo LuxShinyMetalData::sMaterialInfo =
  { "shinymetal", CHANNEL_COUNT, sChannelInfos };


LuxShinyMetalData::LuxShinyMetalData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxMirrorData.
 *****************************************************************************/

const LuxChannelInfo LuxMirrorData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kr",        "refl",    TRUE, 0.0, LuxColor(0.0) },  // REFLECTION
    { LUX_FLOAT_TEXTURE, "film",      "film",    FALSE },                     // FILM_THICKNESS,
    { LUX_FLOAT_TEXTURE, "filmindex", "filmior", FALSE },                     // FILM_IOR,
  };

const LuxMaterialInfo LuxMirrorData::sMaterialInfo =
  { "mirror", CHANNEL_COUNT, sChannelInfos };


LuxMirrorData::LuxMirrorData(void)
: LuxMaterialData(sMaterialInfo)
{}



/*****************************************************************************
 * Implementation of LuxCarPaintData.
 *****************************************************************************/

const LuxChannelInfo LuxCarPaintData::sChannelInfos[CHANNEL_COUNT] = 
  {
    { LUX_COLOR_TEXTURE, "Kd",      "diff",   TRUE, 0.0, LuxColor(0.0) }, // DIFFUSE
    { LUX_COLOR_TEXTURE, "Ks1",     "spec1",  TRUE, 0.0, LuxColor(0.0) }, // SPECULAR_1
    { LUX_FLOAT_TEXTURE, "R1",      "r1",     TRUE, 0.0, LuxColor(0.0) }, // R_1
    { LUX_FLOAT_TEXTURE, "M1",      "m1",     TRUE, 0.0, LuxColor(0.0) }, // M_1
    { LUX_COLOR_TEXTURE, "Ks2",     "spec2",  TRUE, 0.0, LuxColor(0.0) }, // SPECULAR_2
    { LUX_FLOAT_TEXTURE, "R2",      "r2",     TRUE, 0.0, LuxColor(0.0) }, // R_2
    { LUX_FLOAT_TEXTURE, "M2",      "m2",     TRUE, 0.0, LuxColor(0.0) }, // M2
    { LUX_COLOR_TEXTURE, "Ks3",     "spec3",  TRUE, 0.0, LuxColor(0.0) }, // SPECULAR_3
    { LUX_FLOAT_TEXTURE, "R3",      "r3",     TRUE, 0.0, LuxColor(0.0) }, // R_3
    { LUX_FLOAT_TEXTURE, "M3",      "m3",     TRUE, 0.0, LuxColor(0.0) }, // M_3
    { LUX_COLOR_TEXTURE, "Ka",      "absorp", FALSE },                    // ABSORPTION
    { LUX_FLOAT_TEXTURE, "d",       "adepth", FALSE },                    // ABSORPTION_DEPTH
  };

const LuxMaterialInfo LuxCarPaintData::sMaterialInfo =
  { "carpaint", CHANNEL_COUNT, sChannelInfos };


LuxCarPaintData::LuxCarPaintData(void)
: LuxMaterialData(sMaterialInfo)
{}
