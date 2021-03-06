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

#ifndef __TLUXC4DPORTALTAG_H__
#define __TLUXC4DPORTALTAG_H__ 1



enum
{
  // ----------------------------------
  // TAG TAB
  // ----------------------------------
  IDD_PORTAL_ENABLED = 10000,

  IDG_PORTAL_OPTIONS = 10100,
  IDD_PORTAL_SIMPLIFY,
  IDD_PORTAL_FACE_DIRECTION,
  IDD_PORTAL_EXPORT_OBJECT,
  IDD_PORTAL_FLIP_NORMALS,

    // the face directions
    IDD_PORTAL_FACE_DIR_X_PLUS = 0,
    IDD_PORTAL_FACE_DIR_X_MINUS,
    IDD_PORTAL_FACE_DIR_Y_PLUS,
    IDD_PORTAL_FACE_DIR_Y_MINUS,
    IDD_PORTAL_FACE_DIR_Z_PLUS,
    IDD_PORTAL_FACE_DIR_Z_MINUS,
    IDD_PORTAL_FACE_DIR_NUMBER
};



#endif
