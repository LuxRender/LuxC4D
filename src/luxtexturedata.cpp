#include "luxtexturedata.h"



Bool LuxTextureData::isConstantTexture() const
{
  return FALSE;
}


Bool LuxTextureData::sendToAPIAndAddToParamSet(LuxAPI&                receiver,
                                               LuxParamSet&           paramSet,
                                               LuxAPI::IdentifierName paramName,
                                               const LuxString&       textureName) const
{
  // special handling of constant textures, which we don't create explicitely
  if (isConstantTexture()) {
    const LuxConstantTextureData* texture =
      (const LuxConstantTextureData*)this;
    if (mType == FLOAT_TEXTURE) {
      if (!paramSet.addParam(LUX_FLOAT, (CHAR*)paramName, (void*)&texture->value())) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add float to parameter set");
      }
    } else {
      if (!paramSet.addParam(LUX_COLOR, (CHAR*)paramName, (void*)&texture->color())) {
        ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add color to parameter set");
      }
    }
    return TRUE;
  }

  // normal case:
  if (!sendToAPI(receiver, textureName.c_str())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not send texture to LuxAPI");
  }
  if (!paramSet.addParam(LUX_TEXTURE, (CHAR*)paramName, (void*)&textureName)) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxTextureData::addToParamSet(): could not add texture to parameter set");
  }
  
  return TRUE;
}


Bool LuxTextureData::sendToAPIHelper(LuxAPI&                receiver,
                                     LuxAPI::IdentifierName name,
                                     LuxAPI::IdentifierName typeName,
                                     const LuxParamSet&     paramSet) const
{
  static const CHAR* cTextureTypeStr[LUX_TEXTURE_TYPE_COUNT] = {
    "float",
    "color"
  };

  return receiver.texture(name,
                          cTextureTypeStr[mType],
                          typeName,
                          paramSet);
}



LuxConstantTextureData::LuxConstantTextureData(LuxFloat value)
: LuxTextureData(FLOAT_TEXTURE),
  mValue(value),
  mColor(0.0f, 0.0f, 0.0f)
{}


LuxConstantTextureData::LuxConstantTextureData(const LuxColor& color)
: LuxTextureData(COLOR_TEXTURE),
  mValue(0.0f),
  mColor(color)
{}


Bool LuxConstantTextureData::isConstantTexture() const
{
  return TRUE;
}


Bool LuxConstantTextureData::sendToAPI(LuxAPI&                receiver,
                                       LuxAPI::IdentifierName name) const
{
  LuxParamSet paramSet(1);

  if (mType == FLOAT_TEXTURE) {
    paramSet.addParam(LUX_FLOAT, "value", (void*)&mValue);
  } else {
    paramSet.addParam(LUX_COLOR, "value", (void*)&mColor);
  }
  return sendToAPIHelper(receiver, name, "constant", paramSet);
}



LuxImageMapData::LuxImageMapData(LuxTextureType  type,
                                 const Filename& path)
: LuxTextureData(type),
  mPath(path)
{}


Bool LuxImageMapData::sendToAPI(LuxAPI&                receiver,
                                LuxAPI::IdentifierName name) const
{
  LuxParamSet paramSet(9);
  LuxString imagePath;

  convert2LuxString(mPath, imagePath);
  paramSet.addParam(LUX_STRING, "filename", &imagePath);
  return sendToAPIHelper(receiver, name, "imagemap", paramSet);
}
