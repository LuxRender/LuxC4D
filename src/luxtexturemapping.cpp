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

#include "luxtexturemapping.h"



/*****************************************************************************
 * Implementation of member functions of class LuxTextureMapping.
 *****************************************************************************/

const char* LuxTextureMapping::cTypeNames[TYPE_NUMBER] = {
    "uv",
    "spherical",
    "cylindrical",
    "planar"
  };


LuxTextureMapping::LuxTextureMapping(Type type)
: mType(type),
  mTypeName(cTypeNames[type])
{}


Bool LuxTextureMapping::operator==(const LuxTextureMapping& other) const
{
  if (mType != other.mType) { return FALSE; }
  switch (mType) {
    case TYPE_UV:
      return (LuxUVMapping&)*this == (LuxUVMapping&)other;
    case TYPE_SPHERICAL:
      return (LuxSphericalMapping&)*this == (LuxSphericalMapping&)other;
    case TYPE_CYLINDRICAL:
      return (LuxCylindricalMapping&)*this == (LuxCylindricalMapping&)other;
    case TYPE_PLANAR:
      return (LuxPlanarMapping&)*this == (LuxPlanarMapping&)other;
    default:
      return FALSE;
  }
}



/*****************************************************************************
 * Implementation of member functions of class LuxUVMapping.
 *****************************************************************************/

const ULONG LuxUVMapping::cMaxParamCount(5);


LuxUVMapping::LuxUVMapping(TextureTag& textureTag)
: LuxTextureMapping(TYPE_UV),
  mUScale(1.0),
  mVScale(1.0),
  mUShift(0.0),
  mVShift(0.0)
{
  // get UV shift and scale from texture tag
  mUShift = getParameterReal(textureTag, TEXTURETAG_OFFSETX);
  mVShift = getParameterReal(textureTag, TEXTURETAG_OFFSETY);
  mUScale = getParameterReal(textureTag, TEXTURETAG_TILESX);
  mVScale = getParameterReal(textureTag, TEXTURETAG_TILESY);

  // convert to Lux system where shift has an inverse semantic
  mUShift *= -mUScale;
  mVShift *= -mVScale;
}


const LuxMatrix* LuxUVMapping::getTrafo() const
{
  return 0;
}


void LuxUVMapping::addToParamSet(LuxParamSet& paramSet) const
{
  paramSet.addParam(LUX_STRING, "mapping", (void*)&mTypeName);
  if (mUScale != 1.0) { paramSet.addParam(LUX_FLOAT, "uscale", (void*)&mUScale); }
  if (mVScale != 1.0) { paramSet.addParam(LUX_FLOAT, "vscale", (void*)&mVScale); }
  if (mUShift != 1.0) { paramSet.addParam(LUX_FLOAT, "udelta", (void*)&mUShift); }
  if (mVShift != 1.0) { paramSet.addParam(LUX_FLOAT, "vdelta", (void*)&mVShift); }
}


Bool LuxUVMapping::operator==(const LuxUVMapping& other) const
{
  return (mUScale == other.mUScale) &&
         (mVScale == other.mVScale) &&
         (mUShift == other.mUShift) &&
         (mVShift == other.mVShift);
}



/*****************************************************************************
 * Implementation of member functions of class LuxSphericalMapping.
 *****************************************************************************/

const ULONG LuxSphericalMapping::cMaxParamCount(4);


LuxSphericalMapping::LuxSphericalMapping(TextureTag& textureTag,
                                         LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_SPHERICAL),
  mUScale(1.0),
  mVScale(1.0),
  mVShift(0.0)
{
  // obtain texture transformation matrix
  BaseObject* object = textureTag.GetObject();
  if (!object) {
    ERRLOG_RETURN("LuxSphericalMapping::LuxSphericalMapping(): could not obtain object from texture tag");
  }
  Matrix texMat = object->GetMg() * textureTag.GetMl();

  // get UV shift and scale from texture tag
  mVShift = getParameterReal(textureTag, TEXTURETAG_OFFSETY);
  mUScale = getParameterReal(textureTag, TEXTURETAG_TILESX);
  mVScale = getParameterReal(textureTag, TEXTURETAG_TILESY);

  // convert to Lux system where shift has an inverse semantic
  mVShift *= -mVScale;

  // U shift must be handled a bit differently: we have to rotate the texture
  // coordinate system around the Y axis
  Real uShift = getParameterReal(textureTag, TEXTURETAG_OFFSETX);
  if (((LReal)((LLONG)uShift)) != uShift) {
    texMat = texMat * MatrixRotY(uShift*pi2);
  }

  // convert C4D matrix to Lux matrix
  mTrafo = LuxMatrix(texMat, c4d2LuxScale);
}


const LuxMatrix* LuxSphericalMapping::getTrafo() const
{
  return &mTrafo;
}


void LuxSphericalMapping::addToParamSet(LuxParamSet& paramSet) const
{
  paramSet.addParam(LUX_STRING, "mapping", (void*)&mTypeName);
  if (mUScale != 1.0) { paramSet.addParam(LUX_FLOAT, "uscale", (void*)&mUScale); }
  if (mVScale != 1.0) { paramSet.addParam(LUX_FLOAT, "vscale", (void*)&mVScale); }
  if (mVShift != 0.0) { paramSet.addParam(LUX_FLOAT, "vdelta", (void*)&mVShift); }
}


Bool LuxSphericalMapping::operator==(const LuxSphericalMapping& other) const
{
  return (mTrafo == other.mTrafo) &&
         (mUScale == other.mUScale) &&
         (mVScale == other.mVScale) &&
         (mVShift == other.mVShift);
}



/*****************************************************************************
 * Implementation of member functions of class LuxCylindricalMapping.
 *****************************************************************************/

const ULONG LuxCylindricalMapping::cMaxParamCount(2);


LuxCylindricalMapping::LuxCylindricalMapping(TextureTag& textureTag,
                                             LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_CYLINDRICAL),
  mUScale(1.0)
{
  // TODO ...
}


const LuxMatrix* LuxCylindricalMapping::getTrafo() const
{
  return &mTrafo;
}


void LuxCylindricalMapping::addToParamSet(LuxParamSet& paramSet) const
{
  paramSet.addParam(LUX_STRING, "mapping", (void*)&mTypeName);
  if (mUScale != 1.0) { paramSet.addParam(LUX_FLOAT, "uscale", (void*)&mUScale); }
}


Bool LuxCylindricalMapping::operator==(const LuxCylindricalMapping& other) const
{
  return (mTrafo == other.mTrafo) &&
         (mUScale == other.mUScale);
}



/*****************************************************************************
 * Implementation of member functions of class LuxPlanarMapping.
 *****************************************************************************/

const ULONG LuxPlanarMapping::cMaxParamCount(5);


LuxPlanarMapping::LuxPlanarMapping(TextureTag& textureTag,
                                   LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_PLANAR),
  mUVector(1.0, 0.0, 0.0),
  mVVector(0.0, 1.0, 0.0),
  mUShift(0.0),
  mVShift(0.0)
{
  // TODO ...
}


const LuxMatrix* LuxPlanarMapping::getTrafo() const
{
  return 0;
}


void LuxPlanarMapping::addToParamSet(LuxParamSet& paramSet) const
{
  paramSet.addParam(LUX_STRING, "mapping", (void*)&mTypeName);
  paramSet.addParam(LUX_VECTOR, "v1", (void*)&mUVector);
  paramSet.addParam(LUX_VECTOR, "v2", (void*)&mVVector);
  if (mUShift != 0.0) { paramSet.addParam(LUX_FLOAT, "udelta", (void*)&mUShift); }
  if (mVShift != 0.0) { paramSet.addParam(LUX_FLOAT, "vdelta", (void*)&mVShift); }
}


Bool LuxPlanarMapping::operator==(const LuxPlanarMapping& other) const
{
  return (mUVector == other.mUVector) && (mVVector == other.mVVector) &&
         (mUShift == other.mUShift) && (mVShift == other.mVShift);
}
