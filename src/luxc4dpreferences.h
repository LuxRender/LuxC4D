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

#ifndef __LUXC4DPREFERENCES_H__
#define __LUXC4DPREFERENCES_H__ 1



#include "c4d.h"
#include "lib_prefs.h"



#define PID_LUXC4D_PREFERENCES  1023251



/***************************************************************************//*!
 This class implements a dialog for setting the global preferences of LuxC4D
 in the CINEMA 4D program preferences.
*//****************************************************************************/
class LuxC4DPreferencesDialog : public PrefsDlg_Base
{
public:

  LuxC4DPreferencesDialog(PrefsDialogHookClass* hook);

  virtual void SetValues(BaseContainer* data);
  virtual LONG CommandValues(LONG                 id,
                             const BaseContainer& msg,
                             BaseContainer*       data);
};



/***************************************************************************//*!
 This class implements a hook for registering and managing a LuxC4D preference
 dialog in the CINEMA 4D program preferences.
*//****************************************************************************/
class LuxC4DPreferences : public PrefsDialogHookClass
{
public:

  Bool registerHook(void);

  virtual SubDialog* Alloc(void);
  virtual void Free(SubDialog* dlg);

  Filename getLuxPath(void);
};



/// Use this global instance to fetch global LuxC4D settings.
extern LuxC4DPreferences* gPreferences;



#endif  // #ifndef __LUXC4DPREFERENCES_H__
