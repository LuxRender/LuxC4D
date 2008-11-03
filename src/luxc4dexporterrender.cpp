#include "luxc4d.h"
#include "luxc4dexporterrender.h"



#define PID_LUXC4D_EXPORTERRENDER 1023253



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DExporterRender.
 *****************************************************************************/

/// Registers this plugin instance in CINEMA 4D.
///
/// @return
///   TRUE if successfull, otherwise FALSE.
Bool LuxC4DExporterRender::registerPlugin(void)
{
  return RegisterCommandPlugin(PID_LUXC4D_EXPORTERRENDER,
                               GeLoadString(IDS_LUXC4D_EXPORTERRENDER),
                               0, "",
                               GeLoadString(IDS_LUXC4D_EXPORTER_DESCR),
                               this);
}


/// Gets called when the user selects the plugin in the menu. The actual export
/// will be called from here.
///
/// @param[in]  document
///   The current document, which will be exported.
/// @return
///   TRUE if successfull, FALSE otherwise.
Bool LuxC4DExporterRender::Execute(BaseDocument* document)
{
  // get Lux executable
  Filename luxPath = gPreferences->GetLuxPath();
  if (!luxPath.Content() || !GeFExist(luxPath)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_INVALID), GEMB_OK);
    return FALSE;
  }

  // export file
  if (!LuxC4DExporter::Execute(document)) {
    return FALSE;
  }

  // call Lux
  if (!GeExecuteProgram(luxPath, mExportedFile)) {
    GeOutString(GeLoadString(IDS_ERROR_LUX_PATH_EXECUTE, luxPath.GetString()), GEMB_OK);
    return FALSE;
  }

  return TRUE;
}
