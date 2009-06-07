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

CONTAINER Tluxc4dcameratag
{
  NAME Tluxc4dcameratag;
  INCLUDE Obaselist;
  
  GROUP IDG_DOF {
    DEFAULT 1;
    
    BOOL IDD_ENABLE_DOF   { }
    
    GROUP IDG_DOF_SUB {
      REAL IDD_FSTOP        { MIN 0.9;  MAX 64.0;  STEP 0.1;  CUSTOMGUI REALSLIDER; MAXSLIDER 16.0; }
      LONG IDD_FOCUS_TYPE {
        CYCLE {
          IDD_FOCUS_TYPE_CAMERA;
          IDD_FOCUS_TYPE_AUTOFOCUS;
          IDD_FOCUS_TYPE_OBJECT;
        }
      }
      LINK IDD_FOCUS_OBJECT { ACCEPT { Obase; } }
      LONG IDD_BLADE_NUMBER { MIN 0;  MAX 16;  CUSTOMGUI LONGSLIDER; }
      LONG IDD_LENS_SAMPLING_TYPE {
        CYCLE {
          IDD_LENS_SAMPLING_UNIFORM;
          IDD_LENS_SAMPLING_EXPONENTIAL;
          IDD_LENS_SAMPLING_INVERSE_EXPONENTIAL;
          IDD_LENS_SAMPLING_GAUSSIAN;
          IDD_LENS_SAMPLING_INVERSE_GAUSSIAN;
        }
      }
      LONG IDD_EXPONENTIAL_POWER { MIN 0; MAX 512; }
    } // GROUP IDG_DOF_SUB
    
  } // GROUP IDG_DOF
}
