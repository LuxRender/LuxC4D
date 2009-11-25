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
  
    SEPARATOR { LINE; }

    GROUP {
      BOOL IDD_TOGGLE_DIFFUSE             { PARENTMSG IDG_DIFFUSE; }
      BOOL IDD_TOGGLE_REFLECTION          { PARENTMSG IDG_REFLECTION; }
      BOOL IDD_TOGGLE_COATING_ABSORPTION  { PARENTMSG IDG_COATING_ABSORPTION; }
      BOOL IDD_TOGGLE_TRANSMISSION        { PARENTMSG IDG_TRANSMISSION; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR1  { PARENTMSG IDG_CARPAINT_SPECULAR1; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR2  { PARENTMSG IDG_CARPAINT_SPECULAR2; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR3  { PARENTMSG IDG_CARPAINT_SPECULAR3; }
      BOOL IDD_TOGGLE_ROUGHNESS           { PARENTMSG IDG_ROUGHNESS; }
      BOOL IDD_TOGGLE_THIN_FILM           { PARENTMSG IDG_THIN_FILM; }
      BOOL IDD_TOGGLE_BUMP                { PARENTMSG IDG_BUMP; }
      BOOL IDD_TOGGLE_EMISSION            { PARENTMSG IDG_EMISSION; }
    }
  } // GROUP Obaselist
  
  
  GROUP IDG_DIFFUSE {
    GROUP {
      COLOR      IDD_DIFFUSE_COLOR           { OPEN; }
		  REAL       IDD_DIFFUSE_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_DIFFUSE_SHADER          { }
    }
		SEPARATOR { }
    REAL IDD_DIFFUSE_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_DIFFUSE
  
  
  GROUP IDG_REFLECTION {
    GROUP {
      COLOR      IDD_REFLECTION_COLOR           { OPEN; }
		  REAL       IDD_REFLECTION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_REFLECTION_SHADER          { }
    }
		SEPARATOR { }
    REAL IDD_REFLECTION_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_REFLECTION
  
  
  GROUP IDG_COATING_ABSORPTION {
    GROUP {
      COLOR      IDD_COATING_ABSORPTION_COLOR           { OPEN; }
		  REAL       IDD_COATING_ABSORPTION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_COATING_ABSORPTION_SHADER          { }
    }
		SEPARATOR { }
    REAL IDD_COATING_ABSORPTION_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_COATING_ABSORPTION
  
  
  GROUP IDG_TRANSMISSION {
    GROUP {
      COLOR      IDD_TRANSMISSION_COLOR           { OPEN; }
		  REAL       IDD_TRANSMISSION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_TRANSMISSION_SHADER          { }
    }
		SEPARATOR { }
    REAL IDD_TRANSMISSION_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
	  GROUP IDG_IOR {
      REAL 	      IDD_IOR_VALUE   { MIN 0.0;  MAX 10.0;  STEP 0.1;  MINSLIDER 1.0; MAXSLIDER 3.0;  CUSTOMGUI REALSLIDER; }
	    SHADERLINK  IDD_IOR_SHADER  { }
	  }
		SEPARATOR { }
		GROUP {
		  BOOL IDD_TRANSMISSION_ARCHITECTURAL { }
		  REAL IDD_TRANSMISSION_CAUCHYB       { MIN 0.0;  MAX 1.0;  CUSTOMGUI REALSLIDER; }
		}
  } // GROUP IDG_TRANSMISSION
  
  
  GROUP IDG_CARPAINT_SPECULAR1 {
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR1           { OPEN; }
		  REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH1 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_CARPAINT_SPECULAR_SHADER1          { }
    }
		SEPARATOR { }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS1 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
    REAL IDD_CARPAINT_R1  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M1  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR1


  GROUP IDG_CARPAINT_SPECULAR2 {
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR2           { OPEN; }
		  REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH2 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_CARPAINT_SPECULAR_SHADER2          { }
    }
		SEPARATOR { }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS2 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
    REAL IDD_CARPAINT_R2  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M2  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR2


  GROUP IDG_CARPAINT_SPECULAR3 {
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR3           { OPEN; }
		  REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH3 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
		  SHADERLINK IDD_CARPAINT_SPECULAR_SHADER3          { }
    }
		SEPARATOR { }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS3 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
		SEPARATOR { }
    REAL IDD_CARPAINT_R3  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M3  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR3


  GROUP IDG_ROUGHNESS {
  } // GROUP IDG_ROUGHNESS


  GROUP IDG_THIN_FILM {
  } // GROUP IDG_THIN_FILM


  GROUP IDG_BUMP {
  } // GROUP IDG_BUMP


  GROUP IDG_EMISSION {
  } // GROUP IDG_EMISSION
  
  
  INCLUDE Millum;
  INCLUDE Massign;
}
