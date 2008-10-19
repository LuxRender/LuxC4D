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

#ifndef __C4D_SYMBOLS_H__
#define __C4D_SYMBOLS_H__



enum
{
  // basic strings for initialization
  IDS_LUXC4D_EXPORTER = 1,
  IDS_LUXC4D_EXPORTER_DESCR,

  //
  IDS_LUXC4D_SETTINGS = 1000,

  //
  IDS_ERROR_INITIALISE_LUXAPIWRITER = 100000,
  IDS_ERROR_IO,
  IDS_ERROR_CONVERSION,
  IDS_ERROR_INTERNAL
};



#endif  // #ifndef __C4D_SYMBOLS_H__
