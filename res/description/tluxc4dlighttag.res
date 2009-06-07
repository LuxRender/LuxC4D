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

CONTAINER Tluxc4dlighttag
{
  NAME Tluxc4dlighttag;
  INCLUDE Tbase;
  
  GROUP ID_TAGPROPERTIES {
  
    LONG IDD_LIGHT_TYPE {
      CYCLE {
        IDD_LIGHT_TYPE_AS_OBJECT;
        IDD_LIGHT_TYPE_POINT;
        IDD_LIGHT_TYPE_SPOT;
        IDD_LIGHT_TYPE_DISTANT;
        IDD_LIGHT_TYPE_AREA;
        IDD_LIGHT_TYPE_SUN;
        IDD_LIGHT_TYPE_SKY;
        IDD_LIGHT_TYPE_SUNSKY;
      }
    }
    BOOL IDD_LIGHT_IGNORE_FALLOFF_RADIUS  { }
    
    GROUP IDG_POINT_LIGHT {
      DEFAULT 1;
      
      COLOR IDD_POINT_LIGHT_COLOR         { }
      REAL  IDD_POINT_LIGHT_BRIGHTNESS    { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
    }
    
    GROUP IDG_SPOT_LIGHT {
      DEFAULT 1;
      
      COLOR IDD_SPOT_LIGHT_COLOR              { }
      REAL  IDD_SPOT_LIGHT_BRIGHTNESS         { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      REAL  IDD_SPOT_LIGHT_INNERANGLE         { UNIT DEGREE;  MIN 0.0;  MAX 180.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      REAL  IDD_SPOT_LIGHT_OUTERANGLE         { UNIT DEGREE;  MIN 0.0;  MAX 180.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
    }
  
    GROUP IDG_DISTANT_LIGHT {
      DEFAULT 1;
      
      COLOR IDD_DISTANT_LIGHT_COLOR         { }
      REAL  IDD_DISTANT_LIGHT_BRIGHTNESS    { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
    }
    
    GROUP IDG_AREA_LIGHT {
      DEFAULT 1;
      
      COLOR IDD_AREA_LIGHT_COLOR          { }
      REAL  IDD_AREA_LIGHT_BRIGHTNESS     { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      BOOL  IDD_AREA_LIGHT_FLIP_NORMALS   { }
      LONG  IDD_AREA_LIGHT_SAMPLES        { MIN 1; }
      LONG  IDD_AREA_LIGHT_SHAPE          {
        CYCLE {
          IDD_AREA_LIGHT_SHAPE_DISC;
          IDD_AREA_LIGHT_SHAPE_RECTANGLE;
          IDD_AREA_LIGHT_SHAPE_SPHERE;
          IDD_AREA_LIGHT_SHAPE_CYLINDER;
          IDD_AREA_LIGHT_SHAPE_CUBE;
          IDD_AREA_LIGHT_SHAPE_HEMISPHERE;
          IDD_AREA_LIGHT_SHAPE_OBJECT;
        }
      }
      LINK   IDD_AREA_LIGHT_OBJECT         { ACCEPT { Opolygon; } }
      VECTOR IDD_AREA_LIGHT_SIZE           { MIN 0.0;  UNIT METER; }
    }   // GROUP IDG_AREA_LIGHT
    
    GROUP IDG_SUN_LIGHT {
      DEFAULT 1;

      REAL IDD_SUN_LIGHT_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      LONG IDD_SUN_LIGHT_SAMPLES    { MIN 1; }
      REAL IDD_SUN_LIGHT_TURBIDITY  { MIN 1.0;  MAX 30.0; }
      REAL IDD_SUN_LIGHT_RELSIZE    { MIN 0.0;  MAX 180.0; }
    }
    
    GROUP IDG_SKY_LIGHT {
      DEFAULT 1;

      REAL IDD_SKY_LIGHT_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      LONG IDD_SKY_LIGHT_SAMPLES    { MIN 1; }
      REAL IDD_SKY_LIGHT_TURBIDITY  { MIN 1.0;  MAX 30.0; }
      BOOL IDD_SKY_LIGHT_ADVANCED   { }
      REAL IDD_SKY_LIGHT_A_CONST    { MIN 0.0; }
      REAL IDD_SKY_LIGHT_B_CONST    { MIN 0.0; }
      REAL IDD_SKY_LIGHT_C_CONST    { MIN 0.0; }
      REAL IDD_SKY_LIGHT_D_CONST    { MIN 0.0; }
      REAL IDD_SKY_LIGHT_E_CONST    { MIN 0.0; }
    }
    
    GROUP IDG_SUNSKY_LIGHT {
      DEFAULT 1;

      REAL IDD_SUNSKY_LIGHT_BRIGHTNESS { UNIT PERCENT;  MIN 0.0;  MINSLIDER 0.0;  MAXSLIDER 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
      LONG IDD_SUNSKY_LIGHT_SAMPLES    { MIN 1; }
      REAL IDD_SUNSKY_LIGHT_TURBIDITY  { MIN 1.0;  MAX 30.0; }
      REAL IDD_SUNSKY_LIGHT_RELSIZE    { MIN 0.0;  MAX 180.0; }
      BOOL IDD_SUNSKY_LIGHT_ADVANCED   { }
      REAL IDD_SUNSKY_LIGHT_A_CONST    { MIN 0.0; }
      REAL IDD_SUNSKY_LIGHT_B_CONST    { MIN 0.0; }
      REAL IDD_SUNSKY_LIGHT_C_CONST    { MIN 0.0; }
      REAL IDD_SUNSKY_LIGHT_D_CONST    { MIN 0.0; }
      REAL IDD_SUNSKY_LIGHT_E_CONST    { MIN 0.0; }
    }
    
    SEPARATOR { LINE; }
    
    BUTTON IDB_COPY_PARAMETERS {}
    
  } // GROUP ID_TAGPROPERTIES
}
