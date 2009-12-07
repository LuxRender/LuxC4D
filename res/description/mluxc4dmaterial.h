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
    IDD_MATERIAL_TYPE_NUMBER,

  IDG_CHANNELS = 10100,
  IDD_TOGGLE_METAL,
  IDD_TOGGLE_DIFFUSE,
  IDD_TOGGLE_REFLECTION,
  IDD_TOGGLE_SPECULAR,
  IDD_TOGGLE_CARPAINT_SPECULAR1,
  IDD_TOGGLE_CARPAINT_SPECULAR2,
  IDD_TOGGLE_CARPAINT_SPECULAR3,
  IDD_TOGGLE_COATING_ABSORPTION,
  IDD_TOGGLE_TRANSMISSION,
  IDD_TOGGLE_ROUGHNESS,
  IDD_TOGGLE_THIN_FILM,
  IDD_TOGGLE_BUMP,
  IDD_TOGGLE_EMISSION,


  IDG_METAL = 12000,
  IDD_METAL_TYPE,
  IDD_METAL_FILENAME,

    IDD_METAL_TYPE_ALUMINIUM = 0,
    IDD_METAL_TYPE_AMORPHOUS_CARBON,
    IDD_METAL_TYPE_COPPER,
    IDD_METAL_TYPE_GOLD,
    IDD_METAL_TYPE_SILVER,
    IDD_METAL_TYPE_NK_FILE,
    IDD_METAL_TYPE_NUMBER,

  IDG_DIFFUSE = 12100,
  IDD_DIFFUSE_COLOR,
  IDD_DIFFUSE_SHADER_STRENGTH,
  IDD_DIFFUSE_SHADER,
  IDD_DIFFUSE_BRIGHTNESS,

    IDG_SIGMA = 12200,
    IDD_DIFFUSE_SIGMA_VALUE,
	  IDD_DIFFUSE_SIGMA_SHADER,

  IDG_REFLECTION = 12300,
  IDD_REFLECTION_COLOR,
  IDD_REFLECTION_SHADER_STRENGTH,
  IDD_REFLECTION_SHADER,
  IDD_REFLECTION_BRIGHTNESS,

  IDG_SPECULAR = 12400,
  IDD_SPECULAR_COLOR,
  IDD_SPECULAR_SHADER_STRENGTH,
  IDD_SPECULAR_SHADER,
  IDD_SPECULAR_BRIGHTNESS,

    IDG_SPECULAR_IOR = 12500,
    IDD_SPECULAR_IOR_VALUE,
    IDD_SPECULAR_IOR_SHADER,

  IDG_CARPAINT_SPECULAR1 = 12600,
  IDD_CARPAINT_SPECULAR_COLOR1,
  IDD_CARPAINT_SPECULAR_SHADER_STRENGTH1,
  IDD_CARPAINT_SPECULAR_SHADER1,
  IDD_CARPAINT_SPECULAR_BRIGHTNESS1,
  IDD_CARPAINT_R1,
  IDD_CARPAINT_M1,

  IDG_CARPAINT_SPECULAR2 = 12700,
  IDD_CARPAINT_SPECULAR_COLOR2,
  IDD_CARPAINT_SPECULAR_SHADER_STRENGTH2,
  IDD_CARPAINT_SPECULAR_SHADER2,
  IDD_CARPAINT_SPECULAR_BRIGHTNESS2,
  IDD_CARPAINT_R2,
  IDD_CARPAINT_M2,

  IDG_CARPAINT_SPECULAR3 = 12800,
  IDD_CARPAINT_SPECULAR_COLOR3,
  IDD_CARPAINT_SPECULAR_SHADER_STRENGTH3,
  IDD_CARPAINT_SPECULAR_SHADER3,
  IDD_CARPAINT_SPECULAR_BRIGHTNESS3,
  IDD_CARPAINT_R3,
  IDD_CARPAINT_M3,

  IDG_COATING_ABSORPTION = 12900,
  IDD_COATING_ABSORPTION_COLOR,
  IDD_COATING_ABSORPTION_SHADER_STRENGTH,
  IDD_COATING_ABSORPTION_SHADER,
  IDD_COATING_ABSORPTION_STRENGTH,
  IDD_COATING_ABSORPTION_DEPTH_VALUE,
  IDD_COATING_ABSORPTION_DEPTH_SHADER,

  IDG_TRANSMISSION = 13000,
  IDD_TRANSMISSION_COLOR,
  IDD_TRANSMISSION_SHADER_STRENGTH,
  IDD_TRANSMISSION_SHADER,
  IDD_TRANSMISSION_STRENGTH,
  IDD_TRANSMISSION_ARCHITECTURAL,
  IDD_TRANSMISSION_CAUCHYB,

    IDG_IOR = 13100,
    IDD_IOR_VALUE,
    IDD_IOR_SHADER,
  
  IDG_ROUGHNESS = 13200,
  IDD_ROUGHNESS_ASYMETRIC,

    IDG_ROUGHNESS_SYM = 13300,
    IDD_ROUGHNESS_VALUE,
    IDD_ROUGHNESS_SHADER,

    IDG_ROUGHNESS_U = 13400,
    IDD_ROUGHNESS_U_VALUE,
    IDD_ROUGHNESS_U_SHADER,

    IDG_ROUGHNESS_V = 13500,
    IDD_ROUGHNESS_V_VALUE,
    IDD_ROUGHNESS_V_SHADER,

  IDG_THIN_FILM = 13600,
  IDD_THIN_FILM_THICKNESS,
  IDD_THIN_FILM_THICKNESS_SHADER,
  IDD_THIN_FILM_IOR,
  IDD_THIN_FILM_IOR_SHADER,
  
  IDG_BUMP = 13700,
  IDD_BUMP_HEIGHT,
  IDD_BUMP_SHADER,

  IDG_EMISSION = 13800,
  IDD_EMISSION_COLOR,
  IDD_EMISSION_SHADER_STRENGTH,
  IDD_EMISSION_SHADER,
  IDD_EMISSION_BRIGHTNESS,
  
};



#endif  // #ifndef __MLUXC4DMATERIAL_H__
