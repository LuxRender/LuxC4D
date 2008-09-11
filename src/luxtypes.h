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


/// This type represents a boolean value of the Lux API.
typedef bool        LuxBoolT;
/// This type represents an integer value of the Lux API.
typedef int         LuxIntegerT;
/// This type represents a float value of the Lux API.
typedef float       LuxFloatT;
/// This type represents a string of the Lux API.
typedef std::string LuxStringT;


/***************************************************************************//*!
 This structure is equivalent to the vector type of the Lux API.
*//****************************************************************************/
typedef struct LuxVector
{
  LuxFloatT x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxVector()
  {}

  /// Constructor that also initialises the data members.
  /// @param[in] ix  The X component.
  /// @param[in] iy  The Y component.
  /// @param[in] iz  The Z component.
  inline LuxVector(LuxFloatT ix, LuxFloatT iy, LuxFloatT iz)
  : x(ix), y(iy), z(iz)
  {}

  /// Constructor that automatically converts a C4D vector into a Lux vector.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxVector(const Vector& c4dVector)
  : x(c4dVector.x), y(c4dVector.z), z(c4dVector.y)
  {}
} LuxVectorT;


/***************************************************************************//*!
 This structure is equivalent to the point type of the Lux API.
*//****************************************************************************/
typedef struct LuxPoint
{
  LuxFloatT x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxPoint()
  {}

  /// Constructor that also initialises the data members.
  /// @param ix  The X component.
  /// @param iy  The Y component.
  /// @param iz  The Z component.
  inline LuxPoint(LuxFloatT ix, LuxFloatT iy, LuxFloatT iz)
  : x(ix), y(iy), z(iz)
  {}
  
  /// Constructor that automatically converts a C4D vector into a Lux point.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxPoint(const Vector& c4dVector)
  {
    *this = c4dVector;
  }

  /// Copies and converts a C4D vector into a Lux point.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxPoint& operator=(const Vector& c4dVector)
  {
    x = c4dVector.x;
    y = c4dVector.z;
    z = c4dVector.y;
    return *this;
  }
} LuxPointT;


/***************************************************************************//*!
 This structure is equivalent to the normal type of the Lux API.
*//****************************************************************************/
typedef struct LuxNormal
{
  LuxFloatT x, y, z;


  /// Default constructor. The members are NOT cleared.
  inline LuxNormal()
  {}

  /// Constructor that also initialises the data members.
  /// @param ix  The X component.
  /// @param iy  The Y component.
  /// @param iz  The Z component.
  inline LuxNormal(LuxFloatT ix, LuxFloatT iy, LuxFloatT iz)
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
} LuxNormalT;


/***************************************************************************//*!
 This structure is equivalent to the matrix type of the Lux API.
*//****************************************************************************/
typedef struct LuxMatrix
{
  LuxFloatT values[16];


  /// Constructor that creates a matching Lux matrix out of a C4D matrix.
  ///
  /// @param[in]  c4dMatrix
  ///   The C4D matrix object that should be converted into a Lux matrix.
  inline LuxMatrix(const Matrix& c4dMatrix)
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
    values[12] = c4dMatrix.off.x;
    values[13] = c4dMatrix.off.z;
    values[14] = c4dMatrix.off.y;
    values[15] = 1.0;
  }
} LuxMatrixT;


/***************************************************************************//*!
 This structure is equivalent to the color type of the Lux API.
*//****************************************************************************/
typedef struct LuxColor
{
  LuxFloatT c[3];


  /// Default constructor. The members are NOT cleared.
  inline LuxColor()
  {}

  /// Constructor that also initialises the data members.
  /// @param r  The red component.
  /// @param g  The green component.
  /// @param b  The blue component.
  inline LuxColor(LuxFloatT r, LuxFloatT g, LuxFloatT b)
  {
    c[0] = r;
    c[1] = g;
    c[2] = b;
  }

  /// Constructor that automatically converts a C4D vector into a Lux color.
  /// @param c4dVector  The C4D vector to convert.
  inline LuxColor(const Vector& c4dVector)
  {
    c[0] = c4dVector.x;
    c[1] = c4dVector.y;
    c[2] = c4dVector.z;
  }
} LuxColorT;



#endif  // #ifndef __LUXTYPES_H__
