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

#ifndef __MLUXC4DMATERIAL_H__
#define __MLUXC4DMATERIAL_H__ 1



enum
{
  IDD_MATERIAL_TYPE = 10000,
  
    IDD_MATERIAL_TYPE_GLASS = 0,
    IDD_MATERIAL_TYPE_ROUGH_GLASS,
    IDD_MATERIAL_TYPE_GLOSSY,
    IDD_MATERIAL_TYPE_MATTE,
    IDD_MATERIAL_TYPE_MATTE_TRANSLUCENT,
    IDD_MATERIAL_TYPE_METAL,
    IDD_MATERIAL_TYPE_SHINY_METAL,
    IDD_MATERIAL_TYPE_MIRROR,
    IDD_MATERIAL_TYPE_CAR_PAINT,

  IDT_CHANNELS = 10100,
  IDD_TOGGLE_GLASS,
  IDD_TOGGLE_DIFFUSE,
  IDD_TOGGLE_SPECULAR,
  IDD_TOGGLE_IOR,
  IDD_TOGGLE_REFLECTION,
  IDD_TOGGLE_TRANSMISSION,
  IDD_TOGGLE_EMISSION,
  IDD_TOGGLE_BUMP,
  
  IDG_DIFFUSE = 12000,
  IDD_DIFFUSE_COLOR,
  IDD_DIFFUSE_BRIGHTNESS,
  IDD_DIFFUSE_SHADER,
  IDD_DIFFUSE_SHADER_STRENGTH,
  
  IDG_SPECULAR = 14000,
  IDD_SPECULAR_COLOR,
  IDD_SPECULAR_BRIGHTNESS,
  IDD_SPECULAR_SHADER,
  IDD_SPECULAR_SHADER_STRENGTH,

  IDG_IOR = 16000,
  IDD_IOR_VALUE,
  IDD_IOR_SHADER,
  
  IDG_REFLECTION = 18000,
  IDD_REFLECTION_COLOR,
  IDD_REFLECTION_BRIGHTNESS,
  IDD_REFLECTION_SHADER,
  IDD_REFLECTION_SHADER_STRENGTH,
  
  IDG_TRANSMISSION = 20000,
  IDD_TRANSMISSION_COLOR,
  IDD_TRANSMISSION_BRIGHTNESS,
  IDD_TRANSMISSION_SHADER,
  IDD_TRANSMISSION_SHADER_STRENGTH,
  
  IDG_EMISSION = 22000,
  IDD_EMISSION_COLOR,
  IDD_EMISSION_BRIGHTNESS,
  IDD_EMISSION_SHADER,
  IDD_EMISSION_SHADER_STRENGTH,
  
  IDG_BUMP = 24000
  IDD_BUMP_STRENGTH,
  IDD_BUMP_SHADER,
  IDD_BUMP_SHADER_STRENGTH,
};



#endif  // #ifndef __MLUXC4DMATERIAL_H__
