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

#ifndef __LUXTEXTUREMAPPING_H__
#define __LUXTEXTUREMAPPING_H__



#include "autoref.h"
#include "filepath.h"
#include "luxapi.h"



/***************************************************************************//*!
 Base class of a texture mapping implementation.
*//****************************************************************************/
class LuxTextureMapping
{
public:

  /// Returns the maximum number of mapping parameters a texture mapping might
  /// need.
  inline static ULONG maxParamCount();

  /// Returns the transformation matrix of the mapping.
  virtual const LuxMatrix* getTrafo() const =0;

  /// Adds the necessary mapping parameters to the texture parameter set.
  ///
  /// @param[in,out]  paramSet
  ///   The parameter set to which we will add 
  virtual void addToParamSet(LuxParamSet& paramSet) const =0;

  /// Compares two texture mappings and returns TRUE if both are the same
  /// and FALSE if not.
  Bool operator==(const LuxTextureMapping& other) const;


protected:

  enum Type {
    TYPE_UV = 0,
    TYPE_SPHERICAL,
    TYPE_CYLINDRICAL,
    TYPE_PLANAR,
    TYPE_NUMBER
  };

  const Type      mType;
  const LuxString mTypeName;

  /// Constructs the base class and stores the name as string.
  LuxTextureMapping(Type type);


private:

  static const char* cTypeNames[TYPE_NUMBER];

  LuxTextureMapping(const LuxTextureMapping& other)
  : mType(TYPE_UV)
  {}

  LuxTextureMapping& operator=(const LuxTextureMapping& other)
  {
    return *this;
  }
};

typedef AutoRef<LuxTextureMapping>  LuxTextureMappingH;



/***************************************************************************//*!
 Implements storage and export of UV mapping.
*//****************************************************************************/
class LuxUVMapping : public LuxTextureMapping
{
public:

  static const ULONG cMaxParamCount;

  LuxUVMapping(TextureTag& textureTag);

  virtual const LuxMatrix* getTrafo() const;
  virtual void addToParamSet(LuxParamSet& paramSet) const;

  Bool operator==(const LuxUVMapping& other) const;

private:

  /// The texture scaling along the U axis.
  LuxFloat  mUScale;
  /// The texture scaling along the V axis.
  LuxFloat  mVScale;
  /// The texture shift along the U axis.
  LuxFloat  mUShift;
  /// The texture shift along the V axis.
  LuxFloat  mVShift;
};



/***************************************************************************//*!
 Implements storage and export of spherical mapping.
*//****************************************************************************/
class LuxSphericalMapping : public LuxTextureMapping
{
public:

  static const ULONG cMaxParamCount;

  LuxSphericalMapping(TextureTag& textureTag);

  virtual const LuxMatrix* getTrafo() const;
  virtual void addToParamSet(LuxParamSet& paramSet) const;

  Bool operator==(const LuxSphericalMapping& other) const;

private:

  /// Matrix defining the texture space.
  LuxMatrix mTrafo;
  /// The texture scaling along the U axis.
  LuxFloat  mUScale;
  /// The texture scaling along the V axis.
  LuxFloat  mVScale;
  /// The texture shift along the V axis.
  LuxFloat  mVShift;
};



/***************************************************************************//*!
 Implements storage and export of cylindrical mapping.
*//****************************************************************************/
class LuxCylindricalMapping : public LuxTextureMapping
{
public:

  static const ULONG cMaxParamCount;

  LuxCylindricalMapping(TextureTag& textureTag);

  virtual const LuxMatrix* getTrafo() const;
  virtual void addToParamSet(LuxParamSet& paramSet) const;

  Bool operator==(const LuxCylindricalMapping& other) const;

private:

  /// Matrix defining the texture space.
  LuxMatrix mTrafo;
  /// The texture scaling along the U axis.
  LuxFloat  mUScale;
};



/***************************************************************************//*!
 Implements storage and export of planar mapping.
*//****************************************************************************/
class LuxPlanarMapping : public LuxTextureMapping
{
public:

  static const ULONG cMaxParamCount;

  LuxPlanarMapping(TextureTag& textureTag);

  virtual const LuxMatrix* getTrafo() const;
  virtual void addToParamSet(LuxParamSet& paramSet) const;

  Bool operator==(const LuxPlanarMapping& other) const;

private:

  /// Defines the direction of the U-axis.
  LuxVector mUVector;
  /// Defines the direction of the V-axis.
  LuxVector mVVector;
  /// Defines the shift along the U-axis.
  LuxFloat  mUShift;
  /// Defines the shift along the V-axis.
  LuxFloat  mVShift;
};



/*****************************************************************************
 * Implementation of inlined members of the mapping classes.
 *****************************************************************************/

inline ULONG LuxTextureMapping::maxParamCount()
{
  ULONG maxParamCount = 0;

  if (LuxUVMapping::cMaxParamCount > maxParamCount) {
    maxParamCount = LuxUVMapping::cMaxParamCount;
  }
  if (LuxSphericalMapping::cMaxParamCount > maxParamCount) {
    maxParamCount = LuxSphericalMapping::cMaxParamCount;
  }
  if (LuxCylindricalMapping::cMaxParamCount > maxParamCount) {
    maxParamCount = LuxCylindricalMapping::cMaxParamCount;
  }
  if (LuxPlanarMapping::cMaxParamCount > maxParamCount) {
    maxParamCount = LuxPlanarMapping::cMaxParamCount;
  }
  return maxParamCount;
}



#endif  // #ifndef __LUXTEXTUREMAPPING_H__
