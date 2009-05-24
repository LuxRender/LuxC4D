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
  // LuxC4DExporter IDs
  IDS_LUXC4D_EXPORTER = 1,
  IDS_LUXC4D_EXPORTER_DESCR,
  IDS_OVERWRITE_FILE_QUERY,
  IDS_EXPORT_FILENAME_QUERY,

  // LuxC4DExporterRender IDs
  IDS_LUXC4D_EXPORTERRENDER = 1000,
  IDS_LUXC4D_EXPORTERRENDER_DESCR,

  // LuxC4DSettings IDs
  IDS_LUXC4D_SETTINGS = 10000,

  // LuxC4DLightTag IDs
  IDS_LUXC4D_LIGHT_TAG = 20000,

  // Material IDs
  IDS_LUXC4D_MATERIAL_GLOSSY = 30000,
  IDS_LUXC4D_MATERIAL_MATTE  = 30100,

  // LuxC4DPreferences IDs
  IDS_LUXC4D_PREFERENCES = 40000,
  IDS_LUXC4D_PREFERENCES_DESCR,
  DLG_LUXC4D_PREFERENCES,
  IDS_LUXC4D_PREFS_TITLE,
  IDS_LUXC4D_PREFS_LUX_PATH,
  IDD_LUXC4D_PREFS_LUX_PATH,
  IDB_LUXC4D_PREFS_LUX_PATH,
  IDB_LUXC4D_PREFS_OK,
  IDS_LUXC4D_PREFS_LUX_PATH_FS_TITLE,

  // container IDs of LuxC4DPreferences
  IDV_LUXC4D_PREFS_LUX_PATH = 0,

  // error strings
  IDS_ERROR_INITIALISE_LUXAPIWRITER = 100000,
  IDS_ERROR_IO,
  IDS_ERROR_CONVERSION,
  IDS_ERROR_INTERNAL,
  IDS_ERROR_LUX_PATH_EMPTY,
  IDS_ERROR_LUX_PATH_DOESNT_EXIST,
  IDS_ERROR_LUX_PATH_EXECUTE
};



#endif  // #ifndef __C4D_SYMBOLS_H__
