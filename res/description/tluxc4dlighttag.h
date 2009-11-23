//************************************************************************
//* LuxC4D - CINEMA 4D plug-in for export to LuxRender                   *
//* (http://www.luxrender.net)                                           *
//*                                                                      *
//* Author:                                                              *
//* Marcus Spranger (abstrax)                                            *
//*                                                                      *
//************************************************************************
//*                                                                      *
//* This file is part of LuxC4D.                                         *
//*                                                                      *
//* LuxC4D is free software: you can redistribute it and/or modify       *
//* it under the terms of the GNU General Public License as published by *
//* the Free Software Foundation, either version 3 of the License, or    *
//* (at your option) any later version.                                  *
//*                                                                      *
//* LuxC4D is distributed in the hope that it will be useful,            *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
//* GNU General Public License for more details.                         *
//*                                                                      *
//* You should have received a copy of the GNU General Public License    *
//* along with LuxC4D.  If not, see <http://www.gnu.org/licenses/>.      *
//************************************************************************

#ifndef __TLUXC4DLIGHTTAG_H__
#define __TLUXC4DLIGHTTAG_H__ 1



enum
{
  // ----------------------------------
  // TAG TAB
  // ----------------------------------
  IDD_LIGHT_TYPE = 10100,
  IDD_LIGHT_TAG_OBJECT_SETTINGS_COPIED,
  IDD_LIGHT_IGNORE_FALLOFF_RADIUS,
  IDD_LIGHT_GROUP_NAME,

    // light types
    IDD_LIGHT_TYPE_AS_OBJECT = 0,
    IDD_LIGHT_TYPE_POINT,
    IDD_LIGHT_TYPE_SPOT,
    IDD_LIGHT_TYPE_DISTANT,
    IDD_LIGHT_TYPE_AREA,
    IDD_LIGHT_TYPE_SUN,
    IDD_LIGHT_TYPE_SKY,
    IDD_LIGHT_TYPE_SUNSKY,
    IDD_LIGHT_TYPE_INFINITE,

  // point light parameters
  IDG_POINT_LIGHT = 10200,
  IDD_POINT_LIGHT_COLOR,
  IDD_POINT_LIGHT_BRIGHTNESS,

  // spot light parameters
  IDG_SPOT_LIGHT = 10300,
  IDD_SPOT_LIGHT_COLOR,
  IDD_SPOT_LIGHT_BRIGHTNESS,
  IDD_SPOT_LIGHT_INNERANGLE,
  IDD_SPOT_LIGHT_OUTERANGLE,

  // distant light parameters
  IDG_DISTANT_LIGHT = 10400,
  IDD_DISTANT_LIGHT_COLOR,
  IDD_DISTANT_LIGHT_BRIGHTNESS,

  // area light parameters
  IDG_AREA_LIGHT = 10500,
  IDD_AREA_LIGHT_COLOR,
  IDD_AREA_LIGHT_BRIGHTNESS,
  IDD_AREA_LIGHT_FLIP_NORMALS,
  IDD_AREA_LIGHT_SAMPLES,
  IDD_AREA_LIGHT_SHAPE,
  IDD_AREA_LIGHT_OBJECT,
  IDD_AREA_LIGHT_SIZE,

    // area light shapes
    IDD_AREA_LIGHT_SHAPE_DISC = 0,
    IDD_AREA_LIGHT_SHAPE_RECTANGLE,
    IDD_AREA_LIGHT_SHAPE_SPHERE,
    IDD_AREA_LIGHT_SHAPE_CYLINDER,
    IDD_AREA_LIGHT_SHAPE_CUBE,
    IDD_AREA_LIGHT_SHAPE_HEMISPHERE,
    IDD_AREA_LIGHT_SHAPE_OBJECT,

  // sun light parameters
  IDG_SUN_LIGHT = 10600,
  IDD_SUN_LIGHT_BRIGHTNESS,
  IDD_SUN_LIGHT_SAMPLES,
  IDD_SUN_LIGHT_TURBIDITY,
  IDD_SUN_LIGHT_RELSIZE,

  // sky light parameters
  IDG_SKY_LIGHT = 10700,
  IDD_SKY_LIGHT_BRIGHTNESS,
  IDD_SKY_LIGHT_SAMPLES,
  IDD_SKY_LIGHT_TURBIDITY,
  IDD_SKY_LIGHT_ADVANCED,
  IDD_SKY_LIGHT_A_CONST,
  IDD_SKY_LIGHT_B_CONST,
  IDD_SKY_LIGHT_C_CONST,
  IDD_SKY_LIGHT_D_CONST,
  IDD_SKY_LIGHT_E_CONST,

  // sunsky light parameters
  IDG_SUNSKY_LIGHT = 10800,
  IDD_SUNSKY_LIGHT_BRIGHTNESS,
  IDD_SUNSKY_LIGHT_SAMPLES,
  IDD_SUNSKY_LIGHT_TURBIDITY,
  IDD_SUNSKY_LIGHT_RELSIZE,
  IDD_SUNSKY_LIGHT_ADVANCED,
  IDD_SUNSKY_LIGHT_A_CONST,
  IDD_SUNSKY_LIGHT_B_CONST,
  IDD_SUNSKY_LIGHT_C_CONST,
  IDD_SUNSKY_LIGHT_D_CONST,
  IDD_SUNSKY_LIGHT_E_CONST,

  // infinite light parameters
  IDG_INFINITE_LIGHT = 10900,
  IDD_INFINITE_LIGHT_BRIGHTNESS,
  IDD_INFINITE_LIGHT_SAMPLES,
  IDD_INFINITE_LIGHT_TYPE,

    // types of infinite lights
    IDD_INFINITE_LIGHT_TYPE_AUTO = 0,
    IDD_INFINITE_LIGHT_TYPE_INFINITE,
    IDD_INFINITE_LIGHT_TYPE_INFINITE_IMPORTANCE,

  // other parameters
  IDB_COPY_PARAMETERS

};



#endif
