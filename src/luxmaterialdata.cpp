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



LuxMaterialData::LuxMaterialData(const LuxMaterialInfo& info)
: mInfo(&info)
{
  if (!mChannels.init(mInfo->mChannelCount)) {
    ERRLOG(String("LuxMaterialData::LuxMaterialData(): could not initialise channels for material '")
                  + mInfo->mName + "'");
  }
}


Bool LuxMaterialData::setChannel(ULONG           channelId,
                                 LuxTextureDataH texture)
{
  if (channelId > mChannels.size()) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): invalid channel ID (" +
                               LongToString(channelId) + ")");
  }
  if (mInfo->mChannelInfos[channelId].mType != texture->mType) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::setChannel(): type mismatch -> can't set texture in channel with ID " +
                               LongToString(channelId));
  }
  mChannels[channelId].mTexture = texture;
  mChannels[channelId].mEnabled = TRUE;
  return TRUE;
}


Bool LuxMaterialData::sendToAPI(LuxAPI&          receiver,
                                const LuxString& name)
{
  // initialise parameter set (the maximum possible number of parameters is
  // channel count + 1 (for the material type))
  LuxParamSet paramSet(mInfo->mChannelCount + 1);

  // add material type to parameter set
  LuxString type(mInfo->mName);
  paramSet.addParam(LUX_STRING, "type", &type);

  // loop over channels and add them to parameter set
  FixArray1D<LuxString> textureNames(mInfo->mChannelCount);
  ULONG textureNameCount = 0;
  for (ULONG channelIx=0; channelIx<mInfo->mChannelCount; ++channelIx) {

    // for convenience, store reference to channel in variable
    LuxMaterialChannel& channel(mChannels[channelIx]);

    // skip unused channels
    if (channel.mEnabled) {
      textureNames[textureNameCount] =
        name + "." + mInfo->mChannelInfos[channelIx].mTextureSuffix;
      if (!channel.mTexture->sendToAPIAndAddToParamSet(receiver,
                                                       paramSet,
                                                       mInfo->mChannelInfos[channelIx].mName,
                                                       textureNames[textureNameCount]))
      {
        ERRLOG_RETURN_VALUE(FALSE, "LuxMaterialData::sendToAPI(): texture export failed");
      }
      ++textureNameCount;
    }
  }

  // 
  receiver.makeNamedMaterial(name.c_str(), paramSet);

  return TRUE;
}
