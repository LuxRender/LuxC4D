#ifndef __LUXTEXTUREDATA_H__
#define __LUXTEXTUREDATA_H__



#include "handle.h"
#include "luxapi.h"



enum LuxTextureType {
  FLOAT_TEXTURE = 0,
  COLOR_TEXTURE,
  LUX_TEXTURE_TYPE_COUNT
};



class LuxTextureData
{
  public:

    const LuxTextureType mType;


    inline LuxTextureData(LuxTextureType type) : mType(type) {}

    virtual Bool isConstantTexture() const;

    virtual Bool sendToAPI(LuxAPI&                receiver,
                           LuxAPI::IdentifierName name) const =0;

    Bool sendToAPIAndAddToParamSet(LuxAPI&                receiver,
                                   LuxParamSet&           paramSet,
                                   LuxAPI::IdentifierName paramName,
                                   const LuxString&       textureName) const;


  protected:

    Bool sendToAPIHelper(LuxAPI&                receiver,
                         LuxAPI::IdentifierName name,
                         LuxAPI::IdentifierName typeName,
                         const LuxParamSet&     paramSet) const;


  private:

    LuxTextureData& operator=(const LuxTextureData& other) {}
};

typedef Handle<LuxTextureData>  LuxTextureDataH;



class LuxConstantTextureData : public LuxTextureData
{
  public:

    LuxConstantTextureData(LuxFloat value);
    LuxConstantTextureData(const LuxColor& color);

    virtual Bool isConstantTexture() const;
    inline const LuxFloat& value() const { return mValue; }
    inline const LuxColor& color() const { return mColor; }

    virtual Bool sendToAPI(LuxAPI&                receiver,
                           LuxAPI::IdentifierName name) const;


  private:

    LuxFloat mValue;
    LuxColor mColor;
};



class LuxScaleTextureData : public LuxTextureData
{
  public:

    LuxScaleTextureData(LuxFloat scale);
    LuxScaleTextureData(const LuxColor& scale);

    virtual Bool sendToAPI(LuxAPI&                receiver,
                           LuxAPI::IdentifierName name) const;
  
    
  private:
    
    LuxFloat        mValueScale;
    LuxColor        mColorScale;
    LuxTextureDataH mTexture1;
    LuxTextureDataH mTexture2;
};



class LuxImageMapData : public LuxTextureData
{
  public:

    LuxImageMapData(LuxTextureType  type,
                    const Filename& path);

    virtual Bool sendToAPI(LuxAPI&                receiver,
                           LuxAPI::IdentifierName name) const;
  
    
  private:
    
    Filename mPath;
};



#endif  // #ifndef __LUXTEXTUREDATA_H__
