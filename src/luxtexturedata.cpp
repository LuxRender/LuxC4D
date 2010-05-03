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

#include "filepath.h"
#include "luxtexturedata.h"



/*****************************************************************************
 * Implementation of member functions of class LuxTextureData.
 *****************************************************************************/

Bool LuxTextureData::isConstant() const
{
  return FALSE;
}


const LuxFloat& LuxTextureData::constantFloat()
{
  static const LuxFloat cDummy(0.0);
  return cDummy;
}


const LuxColor& LuxTextureData::constantColor()
{
  static const LuxColor cDummy(0.0);
  return cDummy;
}


Bool LuxTextureData::sendToAPIAndAddToParamSet(LuxAPI&                receiver,
                                               LuxParamSet&           paramSet,
                                               LuxAPI::IdentifierName paramName,
                                               const LuxString&       textureName)
{
  // special handling of constant textures, which we don't create explicitely,
  // but store as a parameter
  if (isConstant()) {
    if (mType == LUX_FLOAT_TEXTURE) {
      if (!paramSet.addParam(LUX_FLOAT, (CHAR*)paramName, (void*)&constantFloat())) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add float to parameter set");
      }
    } else {
      if (!paramSet.addParam(LUX_COLOR, (CHAR*)paramName, (void*)&constantColor())) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add color to parameter set");
      }
    }
    return TRUE;
  }

  // normal case of non-constant textures which we set-up as named texture
  if (!sendToAPI(receiver, textureName.c_str())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not send texture to LuxAPI");
  }
  if (!paramSet.addParam(LUX_TEXTURE, (CHAR*)paramName, (void*)&textureName)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add texture to parameter set");
  }
  
  return TRUE;
}


Bool LuxTextureData::sendToAPIHelper(LuxAPI&                receiver,
                                     const LuxString&       name,
                                     LuxAPI::IdentifierName typeName,
                                     LuxParamSet&           paramSet) const
{
  static const CHAR* cTextureTypeStr[LUX_TEXTURE_TYPE_COUNT] = {
    "float",
    "color"
  };

  const LuxMatrix* trafo = 0;
  if (mMapping) {
    mMapping->addToParamSet(paramSet);
    trafo = mMapping->getTrafo();
  }
  if (!receiver.texture(name.c_str(),
                        cTextureTypeStr[mType],
                        typeName,
                        paramSet,
                        trafo))
  {
    return FALSE;
  }
  return TRUE;
}



/*****************************************************************************
 * Implementation of member functions of class LuxScaleTextureData.
 *****************************************************************************/

LuxScaleTextureData::LuxScaleTextureData(LuxTextureType type)
: LuxTextureData(type)
{}


Bool LuxScaleTextureData::isConstant() const
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  return mTexture1->isConstant() && mTexture2->isConstant();
}


const LuxFloat& LuxScaleTextureData::constantFloat()
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  mConstantFloat = mTexture1->constantFloat() * mTexture2->constantFloat();
  return mConstantFloat;
}


const LuxColor& LuxScaleTextureData::constantColor()
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  mConstantColor = mTexture1->constantColor() ^ mTexture2->constantColor();
  return mConstantColor;
}


Bool LuxScaleTextureData::sendToAPI(LuxAPI&          receiver,
                                    const LuxString& name)
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  // make sure that the texture type is correct
  if ((mTexture1->mType != mType) || (mTexture2->mType != mType)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxScaleTextureData::sendToAPI(): mTexture1 or mTexture2 have invalid texture type");
  }

  // don't export scale texture, if one of the textures is constant 1.0
  if (mType == LUX_FLOAT_TEXTURE) {
    if (isConstant()) {
      LuxConstantTextureData constantTexture(constantFloat());
      return constantTexture.sendToAPI(receiver, name);
    } else if (mTexture1->isConstant() && fabs(mTexture1->constantFloat()-1.0) < 0.00001) {
      return mTexture2->sendToAPI(receiver, name);
    } else if (mTexture2->isConstant() && fabs(mTexture2->constantFloat()-1.0) < 0.00001) {
      return mTexture1->sendToAPI(receiver, name);
    }
  }

  // setup parameters
  LuxParamSet paramSet(2);
  LuxString texture1Name = name + ".tex1";
  LuxString texture2Name = name + ".tex2";
  if (!mTexture1->sendToAPIAndAddToParamSet(receiver, paramSet, "tex1", texture1Name) ||
      !mTexture2->sendToAPIAndAddToParamSet(receiver, paramSet, "tex2", texture2Name))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxScaleTextureData::sendToAPI(): could not export textures");
  }

  // remove texture mapping (not needed) and export parameter set
  mMapping = 0;
  return sendToAPIHelper(receiver, name, "scale", paramSet);
}



/*****************************************************************************
 * Implementation of member functions of class LuxMixTextureData.
 *****************************************************************************/

LuxMixTextureData::LuxMixTextureData(LuxTextureType type)
: LuxTextureData(type)
{}


Bool LuxMixTextureData::isConstant() const
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  return mTexture1->isConstant() && mTexture2->isConstant();
}


const LuxFloat& LuxMixTextureData::constantFloat()
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  mConstantFloat = mTexture1->constantFloat() * (1.0 - mAmount) +
                   mTexture2->constantFloat() * mAmount;
  return mConstantFloat;
}


const LuxColor& LuxMixTextureData::constantColor()
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  mConstantColor = mTexture1->constantColor() * (1.0 - mAmount) +
                   mTexture2->constantColor() * mAmount;
  return mConstantColor;
}


Bool LuxMixTextureData::sendToAPI(LuxAPI&          receiver,
                                  const LuxString& name)
{
  GeAssert(mTexture1);
  GeAssert(mTexture2);

  // make sure that both textures have the same type (you can't mix two
  // different texture types)
  if ((mTexture1->mType != mType) || (mTexture2->mType != mType)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMixTextureData::sendToAPI(): mTexture1 or mTexture2 have invalid texture type");
  }

  // setup parameters
  LuxParamSet paramSet(3);
  LuxString texture1Name = name + ".tex1";
  LuxString texture2Name = name + ".tex2";
  if (!mTexture1->sendToAPIAndAddToParamSet(receiver, paramSet, "tex1", texture1Name) ||
      !mTexture2->sendToAPIAndAddToParamSet(receiver, paramSet, "tex2", texture2Name))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMixTextureData::sendToAPI(): could not export textures");
  }
  paramSet.addParam(LUX_FLOAT, "amount", &mAmount);

  // remove texture mapping (not needed) and export parameter set
  mMapping = 0;
  return sendToAPIHelper(receiver, name, "mix", paramSet);
}



/*****************************************************************************
 * Implementation of member functions of class LuxConstantTextureData.
 *****************************************************************************/

LuxConstantTextureData::LuxConstantTextureData(LuxTextureType type)
: LuxTextureData(type)
{}


LuxConstantTextureData::LuxConstantTextureData(LuxFloat value)
: LuxTextureData(LUX_FLOAT_TEXTURE),
  mFloat(value)
{}


LuxConstantTextureData::LuxConstantTextureData(const LuxColor& color,
                                               LuxFloat        gamma)
: LuxTextureData(LUX_COLOR_TEXTURE)
{
  if (gamma != 1.0) {
    mColor.c[0] = pow(color.c[0], gamma);
    mColor.c[1] = pow(color.c[1], gamma);
    mColor.c[2] = pow(color.c[2], gamma);
  } else {
    mColor = color;
  }
}


Bool LuxConstantTextureData::isConstant() const
{
  return TRUE;
}


const LuxFloat& LuxConstantTextureData::constantFloat()
{
  return mFloat;
}


const LuxColor& LuxConstantTextureData::constantColor()
{
  return mColor;
}


Bool LuxConstantTextureData::sendToAPI(LuxAPI&          receiver,
                                       const LuxString& name)
{
  LuxParamSet paramSet(1);

  // setup parameters
  if (mType == LUX_FLOAT_TEXTURE) {
    paramSet.addParam(LUX_FLOAT, "value", &mFloat);
  } else {
    paramSet.addParam(LUX_COLOR, "value", &mColor);
  }

  // remove texture mapping (not needed) and export parameter set
  mMapping = 0;
  return sendToAPIHelper(receiver, name, "constant", paramSet);
}



/*****************************************************************************
 * Implementation of member functions of class LuxUVMaskTextureData.
 *****************************************************************************/

LuxUVMaskTextureData::LuxUVMaskTextureData(LuxTextureType type)
: LuxTextureData(type)
{}


Bool LuxUVMaskTextureData::sendToAPI(LuxAPI&          receiver,
                                     const LuxString& name)
{
  GeAssert(mInnerTex);
  GeAssert(mOuterTex);

  // make sure that both textures have the same type (you can't mix two
  // different texture types)
  if ((mInnerTex->mType != mType) || (mOuterTex->mType != mType)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMixTextureData::sendToAPI(): mInnerTex or mOuterTex have invalid texture type");
  }

  // export child textures
  LuxParamSet paramSet(2 + LuxTextureMapping::maxParamCount());
  LuxString innerTexName = name + ".inner";
  LuxString outerTexName = name + ".outer";
  if (!mInnerTex->sendToAPIAndAddToParamSet(receiver, paramSet, "innertex", innerTexName) ||
      !mOuterTex->sendToAPIAndAddToParamSet(receiver, paramSet, "outertex", outerTexName))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxUVMaskTextureData::sendToAPI(): could not export textures");
  }

  // export UV mask texture
  return sendToAPIHelper(receiver, name, "uvmask", paramSet);
}



/*****************************************************************************
 * Implementation of member functions of class LuxImageMapData.
 *****************************************************************************/

LuxImageMapData::LuxImageMapData(LuxTextureType type)
: LuxTextureData(type),
  mGamma(1.0)
{}


LuxImageMapData::LuxImageMapData(LuxTextureType     type,
                                 LuxTextureMappingH mapping,
                                 const Filename&    imagePath,
                                 LuxFloat           gamma,
                                 Channel            channel,
                                 WrapType           wrapType)
: LuxTextureData(type),
  mImagePath(imagePath),
  mChannel(channel),
  mGamma(gamma),
  mWrapType(wrapType)
{
  mMapping = mapping;
}


Bool LuxImageMapData::sendToAPI(LuxAPI&          receiver,
                                const LuxString& name)
{
  static const char *cChannelNames[IMAGE_CHANNEL_COUNT] = {
    "",
    "red",
    "green",
    "blue",
    "alpha",
    "mean",
    "colored_mean"
  };

  static const char *cWrapTypeNames[WRAP_TYPE_COUNT] = {
    "",
    "repeat",
    "black",
    "white",
    "clamp"
  };

  LuxParamSet paramSet(4 + LuxTextureMapping::maxParamCount());

  // convert and clean up image path
  FilePath processedPath(mImagePath);
  receiver.processFilePath(processedPath);
  LuxString processedPathStr(processedPath.getLuxString());
  paramSet.addParam(LUX_STRING, "filename", &processedPathStr);

  // if we have a valid channel, export it
  LuxString channel;
  if ((mChannel > IMAGE_CHANNEL_NONE) && (mChannel < IMAGE_CHANNEL_COUNT)) {
    // load bitmap and check if it has an alpha channel
    // TODO: cache results
    if (mChannel == IMAGE_CHANNEL_ALPHA) {
      BaseBitmap* bitmap = BaseBitmap::Alloc();
      bitmap->Init(mImagePath);
      if (!bitmap->GetChannelCount()) {
        mChannel = IMAGE_CHANNEL_NONE;
      }
      BaseBitmap::Free(bitmap);
    }
    // if we still want to specify the channel, convert it into a string and
    // add it to the parameter list
    if (mChannel != IMAGE_CHANNEL_NONE) {
      channel = cChannelNames[mChannel];
      paramSet.addParam(LUX_STRING, "channel", &channel);
    }
  }

  // if gamma != 1.0 then export texture gamma
  if (fabsf(mGamma-1.0) > 0.001) {
    paramSet.addParam(LUX_FLOAT, "gamma", &mGamma);
  }

  // if we want wrapping mode "black", add it as additional parameter
  LuxString wrap;
  if (mWrapType != WRAP_TYPE_NONE) {
    wrap = cWrapTypeNames[mWrapType];
    paramSet.addParam(LUX_STRING, "wrap", &wrap);
  }

  // send texture to Lux API
  return sendToAPIHelper(receiver, name, "imagemap", paramSet);
}
