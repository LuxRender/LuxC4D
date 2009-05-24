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

DIALOG DLG_LUXC4D_PREFERENCES
{
  NAME IDS_LUXC4D_PREFERENCES_TITLE;
  
  SCALE_H;
  GROUP {
  
    BORDERSIZE 5, 5, 5, 5;
    SCALE_H;
    COLUMNS 2;
    
    STATICTEXT { NAME IDS_LUXC4D_PREFS_LUX_PATH; }
    GROUP {
      SCALE_H;
      COLUMNS 2;
      EDITTEXT IDD_LUXC4D_PREFS_LUX_PATH { SCALE_H; }
      BUTTON   IDB_LUXC4D_PREFS_LUX_PATH { NAME IDB_LUXC4D_PREFS_LUX_PATH; }
    }
  }
  
  GROUP {
  
    BORDERSIZE 5, 5, 5, 5;
    CENTER_H;
    
    BUTTON IDB_LUXC4D_PREFS_OK { NAME IDB_LUXC4D_PREFS_OK; }
  }
}
