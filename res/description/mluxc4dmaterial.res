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
      BOOL IDD_TOGGLE_METAL               { PAGE;  HIDDEN;  PARENTMSG IDG_METAL; }
      BOOL IDD_TOGGLE_CARPAINT            { PAGE;  HIDDEN;  PARENTMSG IDG_CARPAINT; }
      BOOL IDD_TOGGLE_DIFFUSE             { PARENTMSG IDG_DIFFUSE; }
      BOOL IDD_TOGGLE_REFLECTION          { PARENTMSG IDG_REFLECTION; }
      BOOL IDD_TOGGLE_SPECULAR            { PARENTMSG IDG_SPECULAR; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR1  { PARENTMSG IDG_CARPAINT_SPECULAR1; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR2  { PARENTMSG IDG_CARPAINT_SPECULAR2; }
      BOOL IDD_TOGGLE_CARPAINT_SPECULAR3  { PARENTMSG IDG_CARPAINT_SPECULAR3; }
      BOOL IDD_TOGGLE_COATING_ABSORPTION  { PARENTMSG IDG_COATING_ABSORPTION; }
      BOOL IDD_TOGGLE_TRANSMISSION        { PARENTMSG IDG_TRANSMISSION; }
      BOOL IDD_TOGGLE_ROUGHNESS           { PARENTMSG IDG_ROUGHNESS; }
      BOOL IDD_TOGGLE_THIN_FILM           { PARENTMSG IDG_THIN_FILM; }
      BOOL IDD_TOGGLE_BUMP                { PARENTMSG IDG_BUMP; }
      BOOL IDD_TOGGLE_EMISSION            { PARENTMSG IDG_EMISSION; }
      BOOL IDD_TOGGLE_ALPHA               { PARENTMSG IDG_ALPHA; }
    }
  } // GROUP Obaselist
  
  
  GROUP IDG_METAL {
    DEFAULT 1;
    LONG IDD_METAL_TYPE {
      CYCLE {
        IDD_METAL_TYPE_ALUMINIUM;
        IDD_METAL_TYPE_AMORPHOUS_CARBON;
        IDD_METAL_TYPE_COPPER;
        IDD_METAL_TYPE_GOLD;
        IDD_METAL_TYPE_SILVER;
        IDD_METAL_TYPE_NK_FILE;
      }
    }
    FILENAME IDD_METAL_FILENAME { }
  } // GROUP IDG_METAL
  
  
  GROUP IDG_CARPAINT {
    DEFAULT 1;
    LONG IDD_CARPAINT_TYPE {
      CYCLE {
        IDD_CARPAINT_TYPE_CUSTOM;
        IDD_CARPAINT_TYPE_BMW_339;
        IDD_CARPAINT_TYPE_FORD_F8;
        IDD_CARPAINT_TYPE_OPEL_TITAN;
        IDD_CARPAINT_TYPE_POLARIS_SILVER;
        IDD_CARPAINT_TYPE_2K_ACRYLIC_PAINT;
        IDD_CARPAINT_TYPE_BLUE;
        IDD_CARPAINT_TYPE_BLUE_MATTE;
        IDD_CARPAINT_TYPE_WHITE;
      }
    }
  } // GROUP IDG_CARPAINT
  
  
  GROUP IDG_DIFFUSE {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_DIFFUSE_COLOR           { OPEN; }
      REAL       IDD_DIFFUSE_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_DIFFUSE_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_DIFFUSE_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    GROUP IDG_DIFFUSE_SIGMA {
      REAL         IDD_DIFFUSE_SIGMA_VALUE   { MIN 0.0;  MAX 360.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_DIFFUSE_SIGMA_SHADER  { }
    }
  } // GROUP IDG_DIFFUSE
  
  
  GROUP IDG_REFLECTION {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_REFLECTION_COLOR           { OPEN; }
      REAL       IDD_REFLECTION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_REFLECTION_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_REFLECTION_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_REFLECTION
  
  
  GROUP IDG_SPECULAR {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_SPECULAR_COLOR           { OPEN; }
      REAL       IDD_SPECULAR_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_SPECULAR_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_SPECULAR_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    GROUP IDG_SPECULAR_IOR {
      REAL         IDD_SPECULAR_IOR_VALUE   { MIN 0.0;  MAX 10.0;  STEP 0.1;  MINSLIDER 1.0; MAXSLIDER 3.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_SPECULAR_IOR_SHADER  { }
    }
  } // GROUP IDG_SPECULAR
  
  
  GROUP IDG_CARPAINT_SPECULAR1 {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR1           { OPEN; }
      REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH1 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_CARPAINT_SPECULAR_SHADER1          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS1 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_R1  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M1  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR1


  GROUP IDG_CARPAINT_SPECULAR2 {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR2           { OPEN; }
      REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH2 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_CARPAINT_SPECULAR_SHADER2          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS2 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_R2  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M2  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR2


  GROUP IDG_CARPAINT_SPECULAR3 {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_CARPAINT_SPECULAR_COLOR3           { OPEN; }
      REAL       IDD_CARPAINT_SPECULAR_SHADER_STRENGTH3 { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_CARPAINT_SPECULAR_SHADER3          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_SPECULAR_BRIGHTNESS3 { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    REAL IDD_CARPAINT_R3  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
    REAL IDD_CARPAINT_M3  { MIN 0.0;  MAX 1.0;  STEP 0.05;  CUSTOMGUI REALSLIDER; }
  } // GROUP IDG_CARPAINT_SPECULAR3


  GROUP IDG_COATING_ABSORPTION {
    GROUP {
      COLOR      IDD_COATING_ABSORPTION_COLOR           { OPEN; }
      REAL       IDD_COATING_ABSORPTION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_COATING_ABSORPTION_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_COATING_ABSORPTION_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    REAL         IDD_COATING_ABSORPTION_DEPTH_VALUE  { MIN 0.0;  MAX 1.0;  STEP 0.1;  MINSLIDER 0.0; MAXSLIDER 1.0;  CUSTOMGUI REALSLIDER; }
    SHADERLINK  IDD_COATING_ABSORPTION_DEPTH_SHADER { }
  } // GROUP IDG_COATING_ABSORPTION
  
  
  GROUP IDG_TRANSMISSION {
    DEFAULT 1;
    GROUP {
      COLOR      IDD_TRANSMISSION_COLOR           { OPEN; }
      REAL       IDD_TRANSMISSION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_TRANSMISSION_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_TRANSMISSION_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    GROUP IDG_IOR {
      REAL         IDD_IOR_VALUE   { MIN 0.0;  MAX 10.0;  STEP 0.1;  MINSLIDER 1.0; MAXSLIDER 3.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_IOR_SHADER  { }
    }
    SEPARATOR { LINE; }
    GROUP {
      BOOL IDD_TRANSMISSION_ARCHITECTURAL { }
      REAL IDD_TRANSMISSION_CAUCHYB       { MIN 0.0;  MAX 1.0;  STEP 0.01;  CUSTOMGUI REALSLIDER; }
    }
  } // GROUP IDG_TRANSMISSION
  
  
  GROUP IDG_ROUGHNESS {
    BOOL IDD_ROUGHNESS_ASYMETRIC  { }
    GROUP IDG_ROUGHNESS_SYM {
      REAL         IDD_ROUGHNESS_VALUE   { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  STEP 0.01;  MAXSLIDER 20.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_ROUGHNESS_SHADER  { }
    }
    GROUP IDG_ROUGHNESS_ASYM {
      REAL         IDD_ROUGHNESS_U_VALUE   { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  STEP 0.01;  MAXSLIDER 20.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_ROUGHNESS_U_SHADER  { }
      REAL         IDD_ROUGHNESS_V_VALUE   { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  STEP 0.01;  MAXSLIDER 20.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK  IDD_ROUGHNESS_V_SHADER  { }
    }
  } // GROUP IDG_ROUGHNESS


  GROUP IDG_THIN_FILM {
    REAL       IDD_THIN_FILM_THICKNESS         { MIN 200.0;  MAX 1500.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
    SHADERLINK IDD_THIN_FILM_THICKNESS_SHADER  { }
    SEPARATOR { LINE; }
    REAL       IDD_THIN_FILM_IOR        { MIN 1.0;  MAX 6.0;  STEP 0.1;  CUSTOMGUI REALSLIDER; }
    SHADERLINK IDD_THIN_FILM_IOR_SHADER { }
  } // GROUP IDG_THIN_FILM


  GROUP IDG_BUMP {
    REAL       IDD_BUMP_HEIGHT                  { UNIT METER;  STEP 0.1; }
    SHADERLINK IDD_BUMP_SHADER                  { }
  } // GROUP IDG_BUMP


  GROUP IDG_EMISSION {
    GROUP {
      COLOR      IDD_EMISSION_COLOR           { OPEN; }
      REAL       IDD_EMISSION_SHADER_STRENGTH { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  CUSTOMGUI REALSLIDER; }
      SHADERLINK IDD_EMISSION_SHADER          { }
    }
    SEPARATOR { LINE; }
    REAL IDD_EMISSION_BRIGHTNESS    { UNIT PERCENT;  MIN 0.0;  MAX 10000.0;  MAXSLIDER 100.0;  CUSTOMGUI REALSLIDER; }
    SEPARATOR { LINE; }
    STRING IDD_EMISSION_GROUP_NAME  { }
    
  } // GROUP IDG_EMISSION
  

  GROUP IDG_ALPHA {
    REAL       IDD_ALPHA_VALUE    { UNIT PERCENT;  MIN 0.0;  MAX 100.0; STEP 0.1; CUSTOMGUI REALSLIDER; }
    SHADERLINK IDD_ALPHA_SHADER   { }
  } // GROUP IDG_BUMP

  
  //INCLUDE Millum;
  INCLUDE Massign;
}
