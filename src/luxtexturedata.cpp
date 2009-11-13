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


void LuxTextureData::add2DMapping(LuxParamSet& paramSet)
{
  paramSet.addParam(LUX_STRING, "mapping", &mMapping.mMappingType);
  paramSet.addParam(LUX_FLOAT,  "uscale", &mMapping.mUScale);
  paramSet.addParam(LUX_FLOAT,  "vscale", &mMapping.mVScale);
  paramSet.addParam(LUX_FLOAT,  "udelta", &mMapping.mUShift);
  paramSet.addParam(LUX_FLOAT,  "vdelta", &mMapping.mVShift);
}


Bool LuxTextureData::sendToAPIHelper(LuxAPI&                receiver,
                                     const LuxString&       name,
                                     LuxAPI::IdentifierName typeName,
                                     const LuxParamSet&     paramSet) const
{
  static const CHAR* cTextureTypeStr[LUX_TEXTURE_TYPE_COUNT] = {
    "float",
    "color"
  };

  return receiver.texture(name.c_str(),
                          cTextureTypeStr[mType],
                          typeName,
                          paramSet);
}


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

  if ((mTexture1->mType != mType) || (mTexture2->mType != mType)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxScaleTextureData::sendToAPI(): mTexture1 or mTexture2 have invalid texture type");
  }

  LuxParamSet paramSet(2);
  LuxString texture1Name = name + ".tex1";
  LuxString texture2Name = name + ".tex2";
  if (!mTexture1->sendToAPIAndAddToParamSet(receiver, paramSet, "tex1", texture1Name) ||
      !mTexture2->sendToAPIAndAddToParamSet(receiver, paramSet, "tex2", texture2Name))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxScaleTextureData::sendToAPI(): could not export textures");
  }
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

  if ((mTexture1->mType != mType) || (mTexture2->mType != mType)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMixTextureData::sendToAPI(): mTexture1 or mTexture2 have invalid texture type");
  }

  LuxParamSet paramSet(3);
  LuxString texture1Name = name + ".tex1";
  LuxString texture2Name = name + ".tex2";
  if (!mTexture1->sendToAPIAndAddToParamSet(receiver, paramSet, "tex1", texture1Name) ||
      !mTexture2->sendToAPIAndAddToParamSet(receiver, paramSet, "tex2", texture2Name))
  {
    ERRLOG_RETURN_VALUE(FALSE, "LuxMixTextureData::sendToAPI(): could not export textures");
  }
  paramSet.addParam(LUX_FLOAT, "amount", &mAmount);
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


LuxConstantTextureData::LuxConstantTextureData(const LuxColor& color)
: LuxTextureData(LUX_COLOR_TEXTURE),
  mColor(color)
{}


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

  if (mType == LUX_FLOAT_TEXTURE) {
    paramSet.addParam(LUX_FLOAT, "value", &mFloat);
  } else {
    paramSet.addParam(LUX_COLOR, "value", &mColor);
  }
  return sendToAPIHelper(receiver, name, "constant", paramSet);
}



/*****************************************************************************
 * Implementation of member functions of class LuxImageMapData.
 *****************************************************************************/

LuxImageMapData::LuxImageMapData(LuxTextureType type)
: LuxTextureData(type),
  mGamma(1.0)
{}


LuxImageMapData::LuxImageMapData(LuxTextureType        type,
                                 const TextureMapping& mapping,
                                 const FilePath&       imagePath,
                                 LuxFloat              gamma)
: LuxTextureData(type),
  mImagePath(imagePath.getLuxString()),
  mGamma(gamma)
{
  mMapping = mapping;
}


Bool LuxImageMapData::sendToAPI(LuxAPI&          receiver,
                                const LuxString& name)
{
  LuxParamSet paramSet(10);

  paramSet.addParam(LUX_STRING, "filename", &mImagePath);
  if (fabsf(mGamma-1.0) > 0.001) {
    paramSet.addParam(LUX_FLOAT, "gamma", &mGamma);
  }
  add2DMapping(paramSet);
  return sendToAPIHelper(receiver, name, "imagemap", paramSet);
}
