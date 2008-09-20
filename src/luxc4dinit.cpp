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



/// Hook that is called during the launch of CINEMA 4D, when the DLL
/// was loaded and the plugins should be initialised and registered.
///
/// @return
///   TRUE if successful, otherwise FALSE.
Bool PluginStart(void)
{
  LuxC4DExporter* exporter(0);

  // initialize global resource object
  if (!resource.Init()) {
    GePrint("Global resource object could not be initialized.");
    goto ERROR;
  }

  // register QuantumExporter
  exporter = gNewNC LuxC4DExporter;
  if (!exporter) {
    GePrint("Could not allocate LuxC4DExporter plugin.");
    goto ERROR;
  }
  if (!exporter->registerPlugin()) {
    GePrint("Could not register LuxC4DExporter plugin.");
    goto ERROR;
  }

  return TRUE;

ERROR:

    gDelete(exporter);
    return FALSE;
}


/// Hook that is called during the shut down of CINEMA 4D. Here we can
/// deallocate all resources, that are not owned by CINEMA 4D.
void PluginEnd(void)
{}


/// Hook that is called for different messages.
Bool PluginMessage(LONG id, void *data)
{
  return FALSE;
}
