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
#include "luxc4d.h"



/// Hook that is called during the launch of CINEMA 4D, when the DLL
/// was loaded and the plugins should be initialised and registered.
///
/// @return
///   TRUE if successful, otherwise FALSE.
Bool PluginStart(void)
{
  // initialize global resource object
  if (!resource.Init()) {
    GePrint("Global resource object could not be initialized.");
    return FALSE;
  }

  // register QuantumExporter
  LuxC4D* plugin = gNewNC LuxC4D;
  if (!plugin) {
    GePrint("Could not allocate QuantumExporter plugin.");
    return FALSE;
  }
  if (!plugin->registerPlugin()) {
    GePrint("Could not register selector tool.");
    gDelete(plugin);  return FALSE;
  }

  return TRUE;
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
