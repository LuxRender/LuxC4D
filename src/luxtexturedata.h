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
#include "filepath.h"
#include "luxapi.h"
#include "luxtexturemapping.h"



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



/***************************************************************************//*!
 Base class of a texture parameter container implementation.
*//****************************************************************************/
class LuxTextureData
{
public:

  const LuxTextureType mType;
  LuxTextureMappingH   mMapping;


  inline LuxTextureData(LuxTextureType type) : mType(type) {}
  virtual ~LuxTextureData() {}

  virtual Bool isConstant() const;
  virtual const LuxFloat& constantFloat();
  virtual const LuxColor& constantColor();

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name) =0;
  Bool sendToAPIAndAddToParamSet(LuxAPI&                receiver,
                                 LuxParamSet&           paramSet,
                                 LuxAPI::IdentifierName paramName,
                                 const LuxString&       textureName);


protected:

  Bool sendToAPIHelper(LuxAPI&                receiver,
                       const LuxString&       name,
                       LuxAPI::IdentifierName typeName,
                       LuxParamSet&           paramSet) const;


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

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name);


protected:

  LuxFloat mConstantFloat;
  LuxColor mConstantColor;
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

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name);


protected:

  LuxFloat mConstantFloat;
  LuxColor mConstantColor;
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
  LuxConstantTextureData(const LuxColor& color,
                         LuxFloat        gamma);

  virtual Bool isConstant() const;
  virtual const LuxFloat& constantFloat();
  virtual const LuxColor& constantColor();

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name);
};

typedef AutoRef<LuxConstantTextureData>  LuxConstantTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxImageMapData : public LuxTextureData
{
public:

  enum ImageChannel {
    IMAGE_CHANNEL_NONE = 0,
    IMAGE_CHANNEL_RED,
    IMAGE_CHANNEL_GREEN,
    IMAGE_CHANNEL_BLUE,
    IMAGE_CHANNEL_ALPHA,
    IMAGE_CHANNEL_MEAN,
    IMAGE_CHANNEL_COLORED_MEAN,
    IMAGE_CHANNEL_COUNT
  };

  Filename     mImagePath;
  ImageChannel mChannel;
  LuxFloat     mGamma;


  LuxImageMapData(LuxTextureType type);
  LuxImageMapData(LuxTextureType     type,
                  LuxTextureMappingH mapping,
                  const Filename&    imagePath,
                  LuxFloat           gamma = 1.0,
                  ImageChannel       channel = IMAGE_CHANNEL_NONE);

  virtual Bool sendToAPI(LuxAPI&          receiver,
                         const LuxString& name);
};

typedef AutoRef<LuxImageMapData>  LuxImageMapDataH;



#endif  // #ifndef __LUXTEXTUREDATA_H__
