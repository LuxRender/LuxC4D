/************************************************************************
 * LuxC4D - CINEMA 4D plug-in for export to LuxRender                   *
 * (http://www.luxrender.net)                                           *
 *                                                                      *
 * Author:                                                              *
 * Marcus Spranger (abstrax)                                            *
 *                                                                      *
 ************************************************************************
 *                                                                      *
 * This file is part of LuxC4D.                                         *
 *                                                                      *
 * LuxC4D is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * LuxC4D is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with LuxC4D.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#include "c4d_symbols.h"
#include "luxc4dpreferences.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPreferencesDialog.
 *****************************************************************************/

LuxC4DPreferencesDialog::LuxC4DPreferencesDialog(PrefsDialogHookClass* hook)
: PrefsDlg_Base(DLG_LUXC4D_PREFERENCES, hook)
{}


void LuxC4DPreferencesDialog::SetValues(BaseContainer* data)
{
  if (!data)  return;
  SetFilename(IDD_LUXC4D_PREFS_LUX_PATH, data, IDV_LUXC4D_PREFS_LUX_PATH);
}


LONG LuxC4DPreferencesDialog::CommandValues(LONG                 id,
                                            const BaseContainer& msg,
                                            BaseContainer*       data)
{
  if (!data)  return FALSE;
  switch (id) {
    case IDD_LUXC4D_PREFS_LUX_PATH:
      GetFilename(id, data, IDV_LUXC4D_PREFS_LUX_PATH);
      break;
    case IDB_LUXC4D_PREFS_LUX_PATH:
      {
        Filename luxPath;
        GetFilename(IDD_LUXC4D_PREFS_LUX_PATH, luxPath);
        if (luxPath.FileSelect(FSTYPE_ANYTHING, 0, GeLoadString(IDS_LUXC4D_PREFS_LUX_PATH_FS_TITLE))) {
          SetFilename(IDD_LUXC4D_PREFS_LUX_PATH, luxPath);
          data->SetFilename(IDV_LUXC4D_PREFS_LUX_PATH, luxPath);
        }
      }
      break;
    default:
      break;
  }
  return TRUE;
}



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPreferences.
 *****************************************************************************/


Bool LuxC4DPreferences::registerHook(void)
{
  return Register(PID_LUXC4D_PREFERENCES,
                  GeLoadString(IDS_LUXC4D_PREFERENCES),
                  0,
                  0);
}


SubDialog* LuxC4DPreferences::Alloc(void)
{
  return gNewNC LuxC4DPreferencesDialog(this);
}


void LuxC4DPreferences::Free(SubDialog* dlg)
{
  gDelete(dlg);
}


Filename LuxC4DPreferences::GetLuxPath(void)
{
  Filename luxPath;

  BaseContainer* data = GetData();
  if (data) {
    luxPath = data->GetFilename(IDV_LUXC4D_PREFS_LUX_PATH);
  }

  return luxPath;
}
