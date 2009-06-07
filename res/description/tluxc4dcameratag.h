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

#ifndef __TLUXC4DCAMERATAG_H__
#define __TLUXC4DCAMERATAG_H__ 1



enum
{
  // ----------------------------------
  // DOF TAB
  // ----------------------------------
  IDG_DOF = 10100,
  IDD_ENABLE_DOF,
  IDG_DOF_SUB,
  IDD_FSTOP,
  IDD_FOCUS_TYPE,
  IDD_FOCUS_OBJECT,
  IDD_BLADE_NUMBER,
  IDD_LENS_SAMPLING_TYPE,
  IDD_EXPONENTIAL_POWER,

    IDD_FOCUS_TYPE_CAMERA = 0,
    IDD_FOCUS_TYPE_AUTOFOCUS,
    IDD_FOCUS_TYPE_OBJECT,
    IDD_FOCUS_TYPE_NUMBER,
  
    IDD_LENS_SAMPLING_UNIFORM = 0,
    IDD_LENS_SAMPLING_EXPONENTIAL,
    IDD_LENS_SAMPLING_INVERSE_EXPONENTIAL,
    IDD_LENS_SAMPLING_GAUSSIAN,
    IDD_LENS_SAMPLING_INVERSE_GAUSSIAN,
    IDD_LENS_SAMPLING_NUMBER
};



#endif
