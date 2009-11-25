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



/// Constructs aan empty Lux material container by passing in a material
/// info structure which describes the available channels and their
/// attributes.
///
/// @param[in]  info
///   The info structure that describes the properties/channels of the
///   material.
LuxMaterialData::LuxMaterialData(const LuxMaterialInfo& info)
: mInfo(&info),
  mBumpSampleDistance(0.0)
{
  if (!mChannels.init(mInfo->mChannelCount)) {
    ERRLOG(String("LuxMaterialData::LuxMaterialData(): could not initialise channels for material '")
                  + mInfo->mName + "'");
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
  if (mInfo->mChannelInfos[channelId].mType != texture->mType) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): type mismatch -> can't set texture in channel with ID " +
                               LongToString(channelId));
  }

  // store texture and enable channel
  mChannels[channelId].mTexture = texture;
  mChannels[channelId].mEnabled = TRUE;
  return TRUE;
}


/// Stores a texture in a material channel and activates it.
///
/// @param[in]  texture
///   AutoRef to the texture to store in the emission channel (texture type
///   must be LUX_COLOR_TEXTURE).
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::setEmissionChannel(LuxTextureDataH texture)
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
  return TRUE;
}


/// Returns TRUE if the emission channel is active.
Bool LuxMaterialData::hasEmissionChannel(void)
{
  return mEmissionChannel.mEnabled;
}


/// Sets the bump map sample distance of the material which is 0.0 by default
/// and therefore the parameter doesn't get exported. Only if you set it to
/// a value > 0.000000000001, the parameter gets exported.
void LuxMaterialData::setBumpSampleDistance(LuxFloat bumpSampleDistance)
{
  mBumpSampleDistance = bumpSampleDistance;
}


/// Sends the complete material and its textures to a Lux API receiver.
///
/// @param[in]  receiver
///   The Lux API receiver the material will be sent to.
/// @param[in]  name
///   The name under which the material will be exported. It can then later be
///   referenced by it.
/// @param[in]  addParams
///   Pointer to an additional set of parameters for the material (can be NULL).
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxMaterialData::sendToAPI(LuxAPI&            receiver,
                                const LuxString&   name,
                                const LuxParamSet* addParams)
{
  // initialise parameter set (the maximum possible number of parameters is
  // channel count + 2 (material type, bumpsampledistance) + number of
  // additional parameters)
  LuxParamSet paramSet(mInfo->mChannelCount + 2 +
                       (addParams ? addParams->paramNumber() : 0));

  // add material type to parameter set
  LuxString type(mInfo->mName);
  paramSet.addParam(LUX_STRING, "type", &type);

  // add additional parameters
  if (addParams) {
    paramSet.add(*addParams);
  }

  // loop over active channels, export their textures and add them to parameter
  // set
  FixArray1D<LuxString> textureNames(mInfo->mChannelCount);
  ULONG textureNameCount = 0;
  for (ULONG channelIx=0; channelIx<mInfo->mChannelCount; ++channelIx) {

    // for convenience, store reference to channel in variable
    LuxMaterialChannel&   channel(mChannels[channelIx]);
    const LuxChannelInfo& channelInfo(mInfo->mChannelInfos[channelIx]);

    // if channel is active, export it
    if (channel.mEnabled) {
      textureNames[textureNameCount] = name + "." + channelInfo.mTextureSuffix;
      if (!channel.mTexture->sendToAPIAndAddToParamSet(receiver,
                                                       paramSet,
                                                       channelInfo.mName,
                                                       textureNames[textureNameCount]))
      {
        ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): texture export failed");
      }
      ++textureNameCount;
    // otherwise: export deefaults, if we should export defaults
    } else if (channelInfo.mExportDefault) {
      if (channelInfo.mType == LUX_COLOR_TEXTURE) {
        paramSet.addParam(LUX_COLOR, channelInfo.mName, (void*)&channelInfo.mDefaultColor);
      } else {
        paramSet.addParam(LUX_FLOAT, channelInfo.mName, (void*)&channelInfo.mDefaultFloat);
      }
    }
  }

  // if emission channel is active, export its texture, but don't add it to
  // material as it will be referenced by an area light command in the object
  // scope
  if (mEmissionChannel.mEnabled) {
    if (!mEmissionChannel.mTexture->sendToAPI(receiver, name + ".L")) {
      ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): texture export failed");
    }
  }

  // set bump sample distance, if it's not 0.0
  if (mBumpSampleDistance > 0.000000000001) {
    paramSet.addParam(LUX_FLOAT, "bumpmapsampledistance", &mBumpSampleDistance);
  }

  // write materials
  receiver.makeNamedMaterial(name.c_str(), paramSet);

  return TRUE;
}
