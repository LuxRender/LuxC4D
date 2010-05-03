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


LuxTextureMapping::LuxTextureMapping(Type        type,
                                     TextureTag& textureTag)
: mType(type),
  mTypeName(cTypeNames[type])
{
  // check if texture should be tiled
  mTiled = getParameterLong(textureTag, TEXTURETAG_TILE);
}


Bool LuxTextureMapping::operator<(const LuxTextureMapping& other) const
{
  if (mType < other.mType) { return TRUE; }
  if (mType == other.mType) {
    switch (mType) {
      case TYPE_UV:
        return (LuxUVMapping&)*this < (LuxUVMapping&)other;
      case TYPE_SPHERICAL:
        return (LuxSphericalMapping&)*this < (LuxSphericalMapping&)other;
      case TYPE_CYLINDRICAL:
        return (LuxCylindricalMapping&)*this < (LuxCylindricalMapping&)other;
      case TYPE_PLANAR:
        return (LuxPlanarMapping&)*this < (LuxPlanarMapping&)other;
      default:
        return FALSE;
    }
  }
  return FALSE;
}



/*****************************************************************************
 * Implementation of member functions of class LuxUVMapping.
 *****************************************************************************/

const ULONG LuxUVMapping::cMaxParamCount(5);


LuxUVMapping::LuxUVMapping(TextureTag& textureTag)
: LuxTextureMapping(TYPE_UV, textureTag),
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


Bool LuxUVMapping::operator<(const LuxUVMapping& other) const
{
  if (mUScale < other.mUScale) {
    return TRUE;
  } else if (mUScale == other.mUScale) {
    if (mVScale < other.mVScale) {
      return TRUE;
    } else if (mVScale == other.mVScale) {
      if (mUShift < other.mUShift) {
        return TRUE;
      } else if (mUShift == other.mUShift) {
        if (mVShift < other.mVShift) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}



/*****************************************************************************
 * Implementation of member functions of class LuxSphericalMapping.
 *****************************************************************************/

const ULONG LuxSphericalMapping::cMaxParamCount(4);


LuxSphericalMapping::LuxSphericalMapping(TextureTag& textureTag,
                                         LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_SPHERICAL, textureTag),
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
  texMat.v1 *= c4d2LuxScale;
  texMat.v2 *= c4d2LuxScale;
  texMat.v3 *= c4d2LuxScale;
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


Bool LuxSphericalMapping::operator<(const LuxSphericalMapping& other) const
{
  if (mTrafo < other.mTrafo) {
    return TRUE;
  } else if (mTrafo == other.mTrafo) {
    if (mUScale < other.mUScale) {
      return TRUE;
    } else if (mUScale == other.mUScale) {
      if (mVScale < other.mVScale) {
        return TRUE;
      } else if (mVScale == other.mVScale) {
        if (mVShift < other.mVShift) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}



/*****************************************************************************
 * Implementation of member functions of class LuxCylindricalMapping.
 *****************************************************************************/

const ULONG LuxCylindricalMapping::cMaxParamCount(2);


LuxCylindricalMapping::LuxCylindricalMapping(TextureTag& textureTag,
                                             LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_CYLINDRICAL, textureTag),
  mUScale(1.0)
{
  // obtain texture transformation matrix
  BaseObject* object = textureTag.GetObject();
  if (!object) {
    ERRLOG_RETURN("LuxSphericalMapping::LuxSphericalMapping(): could not obtain object from texture tag");
  }
  Matrix texMat = object->GetMg() * textureTag.GetMl();

  // get U scale from texture tag
  mUScale = getParameterReal(textureTag, TEXTURETAG_TILESX);

  // apply offset X by rotating the texture coordinate system around the Y axis
  Real uShift = getParameterReal(textureTag, TEXTURETAG_OFFSETX);
  if (((LReal)((LLONG)uShift)) != uShift) {
    texMat = texMat * MatrixRotY(uShift*pi2);
  }

  // apply offset Y by shifting the texture coordinate system along the Y axis
  Real vShift = getParameterReal(textureTag, TEXTURETAG_OFFSETY);
  if (vShift != 0.0) {
    texMat.off -= 2.0 * vShift * texMat.v2;
  }

  // apply V scale by scaling the Y axis
  Real vScale = getParameterReal(textureTag, TEXTURETAG_LENGTHY);
  if (vScale != 1.0) {
    texMat.v2 *= vScale;
  }

  // convert C4D matrix to Lux matrix
  texMat.v1 *= c4d2LuxScale;
  texMat.v2 *= c4d2LuxScale;
  texMat.v3 *= c4d2LuxScale;
  mTrafo = LuxMatrix(texMat, c4d2LuxScale);
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


Bool LuxCylindricalMapping::operator<(const LuxCylindricalMapping& other) const
{
  return (mTrafo < other.mTrafo) ||
         ((mTrafo == other.mTrafo) || (mUScale < other.mUScale));
}



/*****************************************************************************
 * Implementation of member functions of class LuxPlanarMapping.
 *****************************************************************************/

const ULONG LuxPlanarMapping::cMaxParamCount(5);


LuxPlanarMapping::LuxPlanarMapping(TextureTag& textureTag,
                                   LReal       c4d2LuxScale)
: LuxTextureMapping(TYPE_PLANAR, textureTag),
  mUVector(1.0, 0.0, 0.0),
  mVVector(0.0, 1.0, 0.0),
  mUShift(0.0),
  mVShift(0.0)
{
  // obtain texture transformation matrix
  BaseObject* object = textureTag.GetObject();
  if (!object) {
    ERRLOG_RETURN("LuxSphericalMapping::LuxSphericalMapping(): could not obtain object from texture tag");
  }
  Matrix texMat = object->GetMg() * textureTag.GetMl();

  // first shift along X and Y axis to apply texture offset
  Real uShift = getParameterReal(textureTag, TEXTURETAG_OFFSETX);
  if (uShift != 0.0) { texMat.off += (2.0 * uShift) * texMat.v1; }
  Real vShift = getParameterReal(textureTag, TEXTURETAG_OFFSETY);
  if (vShift != 0.0) { texMat.off -= (2.0 * vShift) * texMat.v2; }

  // then scale X and Y axis
  Real uScale = getParameterReal(textureTag, TEXTURETAG_LENGTHX);
  if (uScale != 1.0) { texMat.v1 *= uScale; }
  Real vScale = getParameterReal(textureTag, TEXTURETAG_LENGTHY);
  if (vScale != 1.0) { texMat.v2 *= vScale; }

  // convert texture trafo matrix to lux scale
  texMat = c4d2LuxScale * texMat;
  Vector uAxis = 2.0 * texMat.v1;
  Real   uAxisLen = Len(uAxis);
  Vector vAxis = 2.0 * texMat.v2;
  Real   vAxisLen = Len(vAxis);

  // calculate UV vectors and shifts
  mUVector =  uAxis / (uAxisLen * uAxisLen);
  mVVector = -vAxis / (vAxisLen * vAxisLen);
  mUShift = 0.5 / uScale - texMat.off * uAxis / (uAxisLen * uAxisLen);
  mVShift = 0.5 / vScale + texMat.off * vAxis / (vAxisLen * vAxisLen);
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


Bool LuxPlanarMapping::operator<(const LuxPlanarMapping& other) const
{
  if (mUVector < other.mUVector) {
    return TRUE;
  } else if (mUVector == other.mUVector) {
    if (mVVector < other.mVVector) {
      return TRUE;
    } else if (mVVector == other.mVVector) {
      if (mUShift < other.mUShift) {
        return TRUE;
      } else if (mUShift == other.mUShift) {
        if (mVShift < other.mVShift) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}
