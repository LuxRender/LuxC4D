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

#ifndef __LUXTYPES_H__
#define __LUXTYPES_H__  1



#include <string>



#ifndef MAXLONG
#define MAXLONG (MAXLONGl)
#endif



/// Each enumeration specifies one type used by the Lux API.
/// NOTE: Changes here must also be applied to cTokenType of
///       LuxAPIWriter::writeSetting().
enum LuxParamType {
  LUX_BOOL = 0,
  LUX_INTEGER,
  LUX_FLOAT,
  LUX_VECTOR,
  LUX_COLOR,
  LUX_POINT,
  LUX_UV,
  LUX_NORMAL,
  LUX_TRIANGLE,
  LUX_QUAD,
  LUX_STRING,
  LUX_TEXTURE,
  LUX_TYPE_NUMBER
};

///
typedef const CHAR* LuxParamName;

///
typedef void* LuxParamRef;

///
typedef LONG  LuxParamNumber;


/// This type represents a boolean value of the Lux API.
typedef bool        LuxBool;
/// This type represents an integer value of the Lux API.
typedef int         LuxInteger;
/// This type represents a float value of the Lux API.
typedef float       LuxFloat;
/// This type represents a string of the Lux API.
typedef std::string LuxString;



/***************************************************************************//*!
 This structure stores a 2D vector (used for example for UV coordinates.
*//****************************************************************************/
struct LuxVector2D
{
  LuxFloat x, y;


  ///
  inline LuxVector2D()
  {}

  ///
  inline LuxVector2D(LuxFloat xy)
  : x(xy), y(xy)
  {}

  ///
  inline LuxVector2D(LuxFloat ix, LuxFloat iy)
  : x(ix), y(iy)
  {}

  ///
  inline LuxVector2D& operator=(const Vector& other)
  {
    x=other.x;
    y=other.y;
    return *this;
  }

  ///
  inline LuxVector2D operator-(const LuxVector2D& other) const
  {
    return LuxVector2D(x-other.x, y-other.y);
  }
};



/***************************************************************************//*!
 This structure is equivalent to the vector type of the Lux API.
*//****************************************************************************/
struct LuxVector
{
  LuxFloat x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxVector()
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] xyz  The value that will be stored in X, Y and Z components.
  inline LuxVector(LuxFloat xyz)
  : x(xyz), y(xyz), z(xyz)
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] ix  The X component.
  /// @param[in] iy  The Y component.
  /// @param[in] iz  The Z component.
  inline LuxVector(LuxFloat ix, LuxFloat iy, LuxFloat iz)
  : x(ix), y(iy), z(iz)
  {}

  /// Constructor that automatically converts a C4D vector into a Lux vector.
  /// @param[in] c4dVector  The C4D vector to convert.
  inline LuxVector(const Vector& c4dVector)
  : x(c4dVector.x), y(c4dVector.z), z(c4dVector.y)
  {}
};



/***************************************************************************//*!
 This structure is equivalent to the point type of the Lux API.
*//****************************************************************************/
struct LuxPoint
{
  LuxFloat x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxPoint()
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] xyz  The value that will be stored in X, Y and Z components.
  inline LuxPoint(LuxFloat xyz)
  : x(xyz), y(xyz), z(xyz)
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] ix  The X component.
  /// @param[in] iy  The Y component.
  /// @param[in] iz  The Z component.
  inline LuxPoint(LuxFloat ix, LuxFloat iy, LuxFloat iz)
  : x(ix), y(iy), z(iz)
  {}
  
  /// Constructor that automatically converts a C4D vector into a Lux point.
  /// @param[in] c4dVector  The C4D vector to convert.
  inline LuxPoint(const Vector& c4dVector)
  {
    *this = c4dVector;
  }

  /// Copies and converts a C4D vector into a Lux point.
  /// @param[in] c4dVector  The C4D vector to convert.
  inline LuxPoint& operator=(const Vector& c4dVector)
  {
    x = c4dVector.x;
    y = c4dVector.z;
    z = c4dVector.y;
    return *this;
  }
};



/***************************************************************************//*!
 This structure is equivalent to the normal type of the Lux API.
*//****************************************************************************/
struct LuxNormal
{
  LuxFloat x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxNormal()
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] xyz  The value that will be stored in X, Y and Z components.
  inline LuxNormal(LuxFloat xyz)
  : x(xyz), y(xyz), z(xyz)
  {}

  /// Constructor that also initialises the data members.
  /// @param ix  The X component.
  /// @param iy  The Y component.
  /// @param iz  The Z component.
  inline LuxNormal(LuxFloat ix, LuxFloat iy, LuxFloat iz)
  : x(ix), y(iy), z(iz)
  {}

  /// Constructor that automatically converts a C4D vector into a Lux normal.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxNormal(const Vector& c4dVector)
  {
    *this = c4dVector;
  }

  /// Copies and converts a C4D vector into a Lux normal.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxNormal& operator=(const Vector& c4dVector)
  {
    x = c4dVector.x;
    y = c4dVector.z;
    z = c4dVector.y;
    return *this;
  }
};



/***************************************************************************//*!
 This structure is equivalent to the matrix type of the Lux API.
*//****************************************************************************/
struct LuxMatrix
{
  LuxFloat values[16];


  /// Default constructor. The members are NOT cleared.
  inline LuxMatrix()
  {}

  /// Constructor that creates a matching Lux matrix out of a C4D matrix.
  ///
  /// @param[in]  c4dMatrix
  ///   The C4D matrix object that should be converted into a Lux matrix.
  /// @param[in]  scale
  ///   Applies global scale factor to matrix.
  inline LuxMatrix(const Matrix& c4dMatrix, Real scale=1.0)
  {
    // X C4D axis => X Lux axis
    values[0]  = c4dMatrix.v1.x;
    values[1]  = c4dMatrix.v1.z;
    values[2]  = c4dMatrix.v1.y;
    values[3]  = 0.0;

    // Z C4D axis => Y Lux axis
    values[4]  = c4dMatrix.v3.x;
    values[5]  = c4dMatrix.v3.z;
    values[6]  = c4dMatrix.v3.y;
    values[7]  = 0.0;

    // Y C4D axis => Z Lux axis
    values[8]  = c4dMatrix.v2.x;
    values[9]  = c4dMatrix.v2.z;
    values[10] = c4dMatrix.v2.y;
    values[11] = 0.0;

    // Lux pos == C4D pos
    values[12] = c4dMatrix.off.x * scale;
    values[13] = c4dMatrix.off.z * scale;
    values[14] = c4dMatrix.off.y * scale;
    values[15] = 1.0;
  }

};



/***************************************************************************//*!
 This structure is equivalent to the color type of the Lux API.
*//****************************************************************************/
struct LuxColor
{
  LuxFloat c[3];


  /// Default constructor. The members are NOT cleared.
  inline LuxColor()
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] rgb  The value that will ve stored in red, green and blue component.
  inline LuxColor(LuxFloat rgb)
  {
    c[0] = rgb;
    c[1] = rgb;
    c[2] = rgb;
  }

  /// Constructor that also initialises the data members.
  /// @param[in] r  The red component.
  /// @param[in] g  The green component.
  /// @param[in] b  The blue component.
  inline LuxColor(LuxFloat r, LuxFloat g, LuxFloat b)
  {
    c[0] = r;
    c[1] = g;
    c[2] = b;
  }

  /// Constructor that automatically converts a C4D vector into a Lux color.
  /// @param[in] c4dVector  The C4D vector to convert.
  inline LuxColor(const Vector& c4dVector)
  {
    c[0] = c4dVector.x;
    c[1] = c4dVector.y;
    c[2] = c4dVector.z;
  }

  ///
  inline LuxColor operator*(const LuxFloat& scalar) const
  {
    return LuxColor(c[0] * scalar, c[1] * scalar, c[2] * scalar);
  }

  ///
  inline LuxColor operator^(const LuxColor& other) const
  {
    return LuxColor(c[0] * other.c[0], c[1] * other.c[1], c[2] * other.c[2]);
  }

  ///
  inline LuxColor operator+(const LuxColor& other) const
  {
    return LuxColor(c[0] + other.c[0], c[1] + other.c[1], c[2] + other.c[2]);
  }
};



#endif  // #ifndef __LUXTYPES_H__
