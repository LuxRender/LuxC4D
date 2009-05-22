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


/// Constructs a new LuxC4D preferences dialog.
LuxC4DPreferencesDialog::LuxC4DPreferencesDialog(void)
{}


///
Bool LuxC4DPreferencesDialog::CreateLayout(void)
{
  return LoadDialogResource(DLG_LUXC4D_PREFERENCES, 0, 0);
}


///
Bool LuxC4DPreferencesDialog::InitValues(void)
{
  SetFilename(IDD_LUXC4D_PREFS_LUX_PATH,
              &gPreferences->mSettings,
              IDV_LUXC4D_PREFS_LUX_PATH);
  return TRUE;
}


///
Bool LuxC4DPreferencesDialog::Command(LONG                 id,
                                      const BaseContainer& msg)
{
  // do actions depending on the changed/clicked dialog control
  switch (id) {
    // if Lux path was changed directly, store its value in preferences
    // container
    case IDD_LUXC4D_PREFS_LUX_PATH:
      GetFilename(id, &gPreferences->mSettings, IDV_LUXC4D_PREFS_LUX_PATH);
      gPreferences->saveSettings();
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
          gPreferences->mSettings.SetFilename(IDV_LUXC4D_PREFS_LUX_PATH, luxPath);
          gPreferences->saveSettings();
        }
      }
      break;
    //
    case IDB_LUXC4D_PREFS_OK:
      Close(TRUE);
      break;
    default:
      break;
  }

  return TRUE;
}


///
void LuxC4DPreferencesDialog::Run(void)
{
  Open(100, 100, 500, 0);
}



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DPreferences.
 *****************************************************************************/


///
LuxC4DPreferences::LuxC4DPreferences()
: mDialog(0)
{
  BaseContainer* container = GetWorldPluginData(PID_LUXC4D_PREFERENCES);
  if (container) {
    mSettings = *container;
  }
}


///
LuxC4DPreferences::~LuxC4DPreferences(void)
{
  gDelete(mDialog);
}


///
Bool LuxC4DPreferences::Execute(BaseDocument* doc)
{
  if (!mDialog) {
    mDialog = gNewNC LuxC4DPreferencesDialog;
    if (!mDialog) {
      ERRLOG_RETURN_VALUE(FALSE, "LuxC4DPreferences::Execute(): could not allocate preferences dialog");
    }
  }

  mDialog->Run();

  return TRUE;
}


///
Bool LuxC4DPreferences::registerPlugin(void)
{
  return RegisterCommandPlugin(PID_LUXC4D_PREFERENCES,
                               GeLoadString(IDS_LUXC4D_PREFERENCES),
                               0, 0,
                               GeLoadString(IDS_LUXC4D_PREFERENCES_DESCR),
                               this);
}


///
void LuxC4DPreferences::saveSettings(void)
{
  SetWorldPluginData(PID_LUXC4D_PREFERENCES, mSettings, FALSE);
}


/// Returns the full path to the Lux executable, stored in the LuxC4D
/// preferences.
Filename LuxC4DPreferences::getLuxPath(void)
{
  return mSettings.GetFilename(IDV_LUXC4D_PREFS_LUX_PATH);
}
