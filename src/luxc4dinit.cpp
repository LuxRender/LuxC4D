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

#include "c4d.h"

#include "luxc4dexporter.h"
#include "luxc4dexporterrender.h"
#include "luxc4dpreferences.h"
#include "luxc4dlighttag.h"
#include "luxc4dsettings.h"
#include "utilities.h"



///
LuxC4DPreferences* gPreferences(NULL);



/// Hook that is called during the launch of CINEMA 4D, when the DLL
/// was loaded and the plugins should be initialised and registered.
///
/// @return
///   TRUE if successful, otherwise FALSE.
Bool PluginStart(void)
{
  // initialize global resource object
  if (!resource.Init()) {
    ERRLOG("Global resource object could not be initialized.");
    return FALSE;
  }

  // register LuxC4DPreferences
  gPreferences = gNewNC LuxC4DPreferences;
  if (!gPreferences) {
    ERRLOG("Could not allocate LuxC4DPreferences hook class.");
    return FALSE;
  }
  if (!gPreferences->registerHook()) {
    gDelete(gPreferences);
    ERRLOG("Could not register LuxC4DPreferences .");
    return FALSE;
  }

  // register LuxC4DExporter
  LuxC4DExporter* exporter = gNewNC LuxC4DExporter;
  if (!exporter) {
    ERRLOG("Could not allocate LuxC4DExporter plugin.");
    return FALSE;
  }
  if (!exporter->registerPlugin()) {
    gDelete(exporter);
    ERRLOG("Could not register LuxC4DExporter plugin.");
    return FALSE;
  }

  // register LuxC4DExporterRender
  LuxC4DExporterRender* exporterRender = gNewNC LuxC4DExporterRender;
  if (!exporterRender) {
    ERRLOG("Could not allocate LuxC4DExporterRender plugin.");
    return FALSE;
  }
  if (!exporterRender->registerPlugin()) {
    gDelete(exporterRender);
    ERRLOG("Could not register LuxC4DExporterRender plugin.");
    return FALSE;
  }

  // register LuxC4DSettings
  if (!LuxC4DSettings::registerPlugin()) {
    ERRLOG("Could not register LuxC4DSettings plugin.");
    return FALSE;
  }

  // register LuxC4DLightTag
  if (!LuxC4DLightTag::registerPlugin()) {
    ERRLOG("Could not register LuxC4DLightTag plugin.");
    return FALSE;
  }

  return TRUE;
}


/// Hook that is called during the shut down of CINEMA 4D. Here we can
/// deallocate all resources, that are not owned by CINEMA 4D.
void PluginEnd(void)
{
  gDelete(gPreferences);
}


/// Hook that is called for different messages.
Bool PluginMessage(LONG id, void *data)
{
  return FALSE;
}
