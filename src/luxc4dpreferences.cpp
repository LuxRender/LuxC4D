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
#include "utilities.h"



/// Stores a pointer to the global LuxC4DPreferences instance, which will be
/// created during plugin initialisation.
LuxC4DPreferences* gPreferences = 0;



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPreferencesDialog.
 *****************************************************************************/


/// Constructs a new LuxC4D preferences dialog and connects it with the
/// corresponding hook instance.
///
/// @param[in]  hook
///   Pointer to the preferences hook, that allocated the dialog (must not be
///   NULL).
LuxC4DPreferencesDialog::LuxC4DPreferencesDialog(PrefsDialogHookClass* hook)
: PrefsDlg_Base(DLG_LUXC4D_PREFERENCES, hook)
{}


/// Initialises the values in the dialog controls from preferences container.
///
/// @param[in]
///   Pointer to the container of the LuxC4D settings (if NULL, nothing
///   happens).
void LuxC4DPreferencesDialog::SetValues(BaseContainer* data)
{
  // if base container available, copy settings from base container to dialog
  if (!data)  return;
  SetFilename(IDD_LUXC4D_PREFS_LUX_PATH, data, IDV_LUXC4D_PREFS_LUX_PATH);

  // we store a copy of the internal preferences container (which is not always
  // available) in the global plugin container
  SetWorldPluginData(PID_LUXC4D_PREFERENCES, *data, FALSE);
}


/// Callback function which handles interactions of the user with the dialog
/// controls.
///
/// @param[in]  id
///   The ID of the changed control.
/// @param[in]  msg
///   Container with detailed information about the interaction.
/// @param[in]  data
///   Pointer to container, that stores the LuxC4D preferences (if NULL,
///   nothing happens).
/// @return
///   TRUE if successful, FALSE otherwise.
LONG LuxC4DPreferencesDialog::CommandValues(LONG                 id,
                                            const BaseContainer& msg,
                                            BaseContainer*       data)
{
  // no container -> nothing to do
  if (!data)  return FALSE;

  // do actions depending on the changed/clicked dialog control
  switch (id) {
    // if Lux path was changed directly, store its value in preferences
    // container
    case IDD_LUXC4D_PREFS_LUX_PATH:
      GetFilename(id, data, IDV_LUXC4D_PREFS_LUX_PATH);
      break;
    // if the user clicked onto the "..." button, we open a file selection
    // dialog and store the path of the selected file in the preferences
    // container
    case IDB_LUXC4D_PREFS_LUX_PATH:
      {
        Filename luxPath;
        GetFilename(IDD_LUXC4D_PREFS_LUX_PATH, luxPath);
        if (luxPath.FileSelect(FSTYPE_ANYTHING, 0, &GeLoadString(IDS_LUXC4D_PREFS_LUX_PATH_FS_TITLE))) {
          SetFilename(IDD_LUXC4D_PREFS_LUX_PATH, luxPath);
          data->SetFilename(IDV_LUXC4D_PREFS_LUX_PATH, luxPath);
        }
      }
      break;
    default:
      break;
  }

  // we store a copy of the internal preferences container (which is not always
  // available) in the global plugin container
  SetWorldPluginData(PID_LUXC4D_PREFERENCES, *data, TRUE);
  return TRUE;
}



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPreferences.
 *****************************************************************************/


/// Registers the LuxC4D preference hook.
///
/// @return
///   TRUE if successful, FALSE otherwise.
Bool LuxC4DPreferences::registerHook(void)
{
  return Register(PID_LUXC4D_PREFERENCES,
                  GeLoadString(IDS_LUXC4D_PREFERENCES),
                  0,
                  0);
}


/// Returns a newly allocated preferences dialog.
SubDialog* LuxC4DPreferences::Alloc(void)
{
  return gNewNC LuxC4DPreferencesDialog(this);
}


/// Deallocates a preferences dialog.
void LuxC4DPreferences::Free(SubDialog* dlg)
{
  gDelete(dlg);
}


/// Returns the full path to the Lux executable, stored in the LuxC4D
/// preferences.
Filename LuxC4DPreferences::getLuxPath(void)
{
  Filename luxPath;

  // get global plugin container and (if available) read out Lux path
  BaseContainer* data = GetWorldPluginData(PID_LUXC4D_PREFERENCES);
  if (data) {
    luxPath = data->GetFilename(IDV_LUXC4D_PREFS_LUX_PATH);
  }

  return luxPath;
}
