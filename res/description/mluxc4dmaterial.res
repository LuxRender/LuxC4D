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

CONTAINER Mluxc4dmaterial
{
  NAME Mluxc4dmaterial;

  INCLUDE Mpreview;
  INCLUDE Mbase;

  GROUP Obaselist {

    SEPARATOR { }
  
    GROUP {
      LONG IDD_MATERIAL_TYPE {
        CYCLE {
          IDD_MATERIAL_TYPE_GLASS;
          IDD_MATERIAL_TYPE_ROUGH_GLASS;
          IDD_MATERIAL_TYPE_GLOSSY;
          IDD_MATERIAL_TYPE_MATTE;
          IDD_MATERIAL_TYPE_MATTE_TRANSLUCENT;
          IDD_MATERIAL_TYPE_METAL;
          IDD_MATERIAL_TYPE_SHINY_METAL;
          IDD_MATERIAL_TYPE_MIRROR;
          IDD_MATERIAL_TYPE_CAR_PAINT;
        }
      }
    }
  
    SEPARATOR { }

    GROUP {
      BOOL IDD_TOGGLE_DIFFUSE       { PARENTMSG IDG_DIFFUSE; }
      BOOL IDD_TOGGLE_SPECULAR      { PARENTMSG IDG_SPECULAR; }
      BOOL IDD_TOGGLE_REFLECTION    { PARENTMSG IDG_REFLECTION; }
      BOOL IDD_TOGGLE_TRANSMISSION  { PARENTMSG IDG_TRANSMISSION; }
      BOOL IDD_TOGGLE_IOR           { PARENTMSG IDG_IOR; }
      
      BOOL IDD_TOGGLE_EMISSION      { PARENTMSG IDG_EMISSION; }
      BOOL IDD_TOGGLE_BUMP          { PARENTMSG IDG_BUMP; }
    }
  } // GROUP Obaselist
  
  
  GROUP IDG_DIFFUSE {
    
    COLOR IDD_DIFFUSE_COLOR       { OPEN; }
    REAL 	IDD_DIFFUSE_BRIGHTNESS  { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
		SHADERLINK IDD_DIFFUSE_SHADER           { }
		REAL       IDD_DIFFUSE_SHADER_STRENGTH  { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
    
  } // GROUP IDG_DIFFUSE
  
  
  GROUP IDG_SPECULAR {

    COLOR IDD_SPECULAR_COLOR       { OPEN; }
    REAL 	IDD_SPECULAR_BRIGHTNESS  { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
		SHADERLINK IDD_SPECULAR_SHADER           { }
		REAL       IDD_SPECULAR_SHADER_STRENGTH  { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }

  } // GROUP IDG_SPECULAR
  
  
  GROUP IDG_IOR {

    REAL 	IDD_IOR_VALUE  { MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
		SHADERLINK IDD_IOR_SHADER        { }
		REAL       IDD_IOR_SHADER_SCALE  { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }

  } // GROUP IDG_IOR
  
  
  GROUP IDG_REFLECTION {
  } // GROUP IDG_REFLECTION
  
  
  GROUP IDG_TRANSMISSION {
  } // GROUP IDG_TRANSMISSION
  
  
  GROUP IDG_EMISSION {
  } // GROUP IDG_EMISSION
  
  
  GROUP IDG_BUMP {
  } // GROUP IDG_BUMP

  
  INCLUDE Millum;
  INCLUDE Massign;
}
