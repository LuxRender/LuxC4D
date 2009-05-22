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



#include "handle.h"
#include "luxapi.h"



enum LuxTextureType {
  LUX_FLOAT_TEXTURE = 0,
  LUX_COLOR_TEXTURE,
  LUX_TEXTURE_TYPE_COUNT
};



/***************************************************************************//*!
*//****************************************************************************/
class LuxTextureData
{
  public:

    const LuxTextureType mType;


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

typedef Handle<LuxTextureData>  LuxTextureDataH;



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

typedef Handle<LuxScaleTextureData>  LuxScaleTextureDataH;



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

typedef Handle<LuxMixTextureData>  LuxMixTextureDataH;



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

typedef Handle<LuxConstantTextureData>  LuxConstantTextureDataH;



/***************************************************************************//*!
*//****************************************************************************/
class LuxImageMapData : public LuxTextureData
{
  public:

    Filename mPath;


    LuxImageMapData(LuxTextureType type);
    LuxImageMapData(LuxTextureType  type,
                    const Filename& path );


  protected:

    virtual Bool sendToAPI(LuxAPI&          receiver,
                           const LuxString& name);
};

typedef Handle<LuxImageMapData>  LuxImageMapDataH;



#endif  // #ifndef __LUXTEXTUREDATA_H__
