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

#ifndef __LUXTEXTUREDATA_H__
#define __LUXTEXTUREDATA_H__



#include "autoref.h"
#include "luxapi.h"



/// At the moment we support 2 different types of textures:
///   LUX_FLOAT_TEXTURE: The value of a specific location in the texture is a
///                      floating point scalar.
///   LUX_COLOR_TEXTURE: The value of a specific location in the texture is a
///                      floating point RGB colour.
enum LuxTextureType {
  LUX_FLOAT_TEXTURE = 0,
  LUX_COLOR_TEXTURE,
  LUX_TEXTURE_TYPE_COUNT
};


/// This structure stores texture mapping parameters that are relevant to Lux.
struct TextureMapping {
  /// The mapping type of the texture - already as string.
  LuxString mMappingType;
  /// If TRUE the texture mapping parameters will NOT be exported and the Lux
  /// defaults get applied.
  Bool      mHasDefaultParams;
  /// The texture scaling along the U axis.
  LuxFloat  mUScale;
  /// The texture scaling along the V axis.
  LuxFloat  mVScale;
  /// The texture shift along the U axis.
  LuxFloat  mUShift;
  /// The texture shift along the V axis.
  LuxFloat  mVShift;

  /// Default constructor. The texture mapping parameters will be disabled.
  TextureMapping() : mMappingType("uv"), mHasDefaultParams(TRUE)  {}
};
  


/***************************************************************************//*!
 Base class of a texture parameter container implementation.
*//****************************************************************************/
class LuxTextureData
{
  public:

    const LuxTextureType mType;
    TextureMapping       mMapping;


    inline LuxTextureData(LuxTextureType type) : mType(type) {}
    virtual ~LuxTextureData() {}

    Bool sendToAPIAndAddToParamSet(LuxAPI&                receiver,
                                   LuxParamSet&           paramSet,
                                   LuxAPI::IdentifierName paramName,
                                   const LuxString&       textureName);

    virtual Bool isConstant() const;
    virtual const LuxFloat& constantFloat();
    virtual const LuxColor& constantColor();


  protected:

    void add2DMapping(LuxParamSet& paramSet);

    Bool sendToAPIHelper(LuxAPI&                receiver,
                         const LuxString&       name,
                         LuxAPI::IdentifierName typeName,
                         const LuxParamSet&     paramSet) const;

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name) =0;


  private:

    /// We don't need and don't want a copy operator.
    LuxTextureData& operator=(const LuxTextureData& other) {}
};

typedef AutoRef<LuxTextureData>  LuxTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxScaleTextureData : public LuxTextureData
{
  public:

    LuxTextureDataH mTexture1;
    LuxTextureDataH mTexture2;


    LuxScaleTextureData(LuxTextureType type);

    virtual Bool isConstant() const;
    virtual const LuxFloat& constantFloat();
    virtual const LuxColor& constantColor();


  protected:

    LuxFloat mConstantFloat;
    LuxColor mConstantColor;

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name);
};

typedef AutoRef<LuxScaleTextureData>  LuxScaleTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxMixTextureData : public LuxTextureData
{
  public:

    LuxTextureDataH mTexture1;
    LuxTextureDataH mTexture2;
    LuxFloat        mAmount;


    LuxMixTextureData(LuxTextureType type);

    virtual Bool isConstant() const;
    virtual const LuxFloat& constantFloat();
    virtual const LuxColor& constantColor();


  protected:

    LuxFloat mConstantFloat;
    LuxColor mConstantColor;

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name);
};

typedef AutoRef<LuxMixTextureData>  LuxMixTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxConstantTextureData : public LuxTextureData
{
  public:

    LuxFloat mFloat;
    LuxColor mColor;


    LuxConstantTextureData(LuxTextureType type);
    LuxConstantTextureData(LuxFloat value);
    LuxConstantTextureData(const LuxColor& color);

    virtual Bool isConstant() const;
    virtual const LuxFloat& constantFloat();
    virtual const LuxColor& constantColor();


  protected:

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name);
};

typedef AutoRef<LuxConstantTextureData>  LuxConstantTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxImageMapData : public LuxTextureData
{
  public:

    Filename       mPath;


    LuxImageMapData(LuxTextureType type);
    LuxImageMapData(LuxTextureType        type,
                    const TextureMapping& mapping,
                    const Filename&       path );


  protected:

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name);
};

typedef AutoRef<LuxImageMapData>  LuxImageMapDataH;



#endif  // #ifndef __LUXTEXTUREDATA_H__
