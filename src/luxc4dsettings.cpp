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
#include "luxc4dsettings.h"
#include "luxparamset.h"
#include "vpluxc4dsettings.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DSettings.
 *****************************************************************************/


/// Returns a new instance of this class. Will be called by C4D, when the
/// user adds this plugin as a post-effect.
NodeData* LuxC4DSettings::alloc(void)
{
  return gNew LuxC4DSettings;
}


/// Registers this plugin in C4D and returns TRUE if it succeeded.
Bool LuxC4DSettings::registerPlugin(void)
{
  return RegisterVideoPostPlugin(PID_LUXC4D_SETTINGS,
                                 GeLoadString(IDS_LUXC4D_SETTINGS),
                                 0,
                                 alloc,
                                 "VPluxc4dsettings",
                                 0,
                                 VPPRIORITY_EXTERNAL);
}


/// Overwritten function that initialises all description parameters with the
/// defaults.;
///
/// @param[in]  node
///   The data node, created by C4D for storing the settings.
/// @return
///   TRUE if the initailisation was successful.
Bool LuxC4DSettings::Init(GeListNode* node)
{
  // get current document
  BaseDocument* document = node->GetDocument();

  // obtain active render settings
  BaseContainer* c4dRenderSettings = 0;
  if (document) {
    RenderData* renderData = document->GetActiveRenderData();
    if (renderData) {
      c4dRenderSettings = renderData->GetDataInstance();
    }
  }

  // obtain container from data node.
  BaseContainer* data = ((BaseObject*)node)->GetDataInstance();
  if (!data)  ERRLOG_RETURN_FALSE("LuxC4DSettings::Init(): No base container found.");

  // set sampler defaults
  data->SetLong(IDD_SAMPLER,                        IDD_SAMPLER_LOWDISCREPANCY);
  data->SetLong(IDD_RANDOM_PIXELSAMPLER,            IDD_PIXELSAMPLER_RANDOM);
  data->SetLong(IDD_RANDOM_SAMPLES_X,               2);
  data->SetLong(IDD_RANDOM_SAMPLES_Y,               2);
  data->SetLong(IDD_LOWDISCREPANCY_PIXELSAMPLER,    IDD_PIXELSAMPLER_LOWDISCREPANCY);
  data->SetLong(IDD_LOWDISCREPANCY_SAMPLES,         4);
  data->SetLong(IDD_METROPOLIS_INIT_SAMPLES,        100000);
  data->SetLong(IDD_METROPOLIS_MAX_CONSEC_REJECTS,  256);
  data->SetReal(IDD_METROPOLIS_LARGE_MUTATION_PROB, 0.25);
  data->SetReal(IDD_METROPOLIS_MICRO_MUTATION_PROB, 0.0);
  data->SetReal(IDD_METROPOLIS_MUTATION_RANGE,      56.0);
  data->SetLong(IDD_METROPOLIS_STRATA_WIDTH,        256);
  data->SetBool(IDD_METROPOLIS_USE_VARIANCE,        FALSE);
  data->SetBool(IDD_METROPOLIS_USE_QR,              FALSE);
  data->SetLong(IDD_ERPT_INIT_SAMPLES,              100000);
  data->SetLong(IDD_ERPT_CHAINLENGTH,               512);
  data->SetReal(IDD_ERPT_MICRO_MUTATION_PROB,       0.5);
  data->SetReal(IDD_ERPT_MUTATION_RANGE,            36.0);

  // set integrator defaults
  data->SetLong(IDD_INTEGRATOR,                                     IDD_INTEGRATOR_PATH);
  data->SetLong(IDD_PATH_MAX_DEPTH,                                 12);
  data->SetLong(IDD_PATH_DIRECT_LIGHT_STRATEGY,                     IDD_DIRECT_LIGHT_STRATEGY_AUTO);
  data->SetLong(IDD_PATH_RR_STRATEGY,                               IDD_PATH_RR_STRATEGY_EFFICIENCY);
  data->SetReal(IDD_PATH_RR_CONTINUE_PROB,                          0.65);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIRECT_LIGHT_STRATEGY,         IDD_DIRECT_LIGHT_STRATEGY_AUTO);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIRECT_DIRECT_LIGHT_SAMPLES,   1);
  data->SetLong(IDD_DISTRIBUTED_PATH_INDIRECT_DIRECT_LIGHT_SAMPLES, 1);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_DEPTH,         3);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_SAMPLES,       1);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_DEPTH,         5);
  data->SetLong(IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_SAMPLES,       1);
  data->SetLong(IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_DEPTH,          2);
  data->SetLong(IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_SAMPLES,        1);
  data->SetLong(IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_DEPTH,          5);
  data->SetLong(IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_SAMPLES,        1);
  data->SetLong(IDD_DISTRIBUTED_PATH_SPECULAR_REFLECT_DEPTH,        2);
  data->SetLong(IDD_DISTRIBUTED_PATH_SPECULAR_REFRACT_DEPTH,        5);
  data->SetLong(IDD_BIDIRECTIONAL_EYE_DEPTH,                        8);
  data->SetLong(IDD_BIDIRECTIONAL_LIGHT_DEPTH,                      8);
  data->SetLong(IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY,            IDD_DIRECT_LIGHT_STRATEGY_AUTO);

  // set pixel filter defaults
  data->SetLong(IDD_PIXEL_FILTER,           IDD_PIXEL_FILTER_MITCHELL);
  data->SetReal(IDD_BOX_FILTER_WIDTH,       0.5);
  data->SetReal(IDD_BOX_FILTER_HEIGHT,      0.5);
  data->SetReal(IDD_GAUSSIAN_FILTER_WIDTH,  2.0);
  data->SetReal(IDD_GAUSSIAN_FILTER_HEIGHT, 2.0);
  data->SetReal(IDD_GAUSSIAN_FILTER_ALPHA,  2.0);
  data->SetReal(IDD_MITCHELL_FILTER_WIDTH,  2.0);
  data->SetReal(IDD_MITCHELL_FILTER_HEIGHT, 2.0);
  data->SetReal(IDD_MITCHELL_FILTER_B,      1.0/3.0);
  data->SetReal(IDD_MITCHELL_FILTER_C,      1.0/3.0);
  data->SetReal(IDD_SINC_FILTER_WIDTH,      4.0);
  data->SetReal(IDD_SINC_FILTER_HEIGHT,     4.0);
  data->SetReal(IDD_SINC_FILTER_TAU,        3.0);
  data->SetReal(IDD_TRIANGLE_FILTER_WIDTH,  2.0);
  data->SetReal(IDD_TRIANGLE_FILTER_HEIGHT, 2.0);

  // set film parameters
  data->SetLong(IDD_FILM,                             IDD_FILM_FLEXIMAGE);
  data->SetLong(IDD_FLEXIMAGE_HALT_SPP,               0);
  if (c4dRenderSettings) {
    data->SetReal(IDD_FLEXIMAGE_GAMMA,                  c4dRenderSettings->GetReal(RDATA_RENDERGAMMA));
  } else {
#ifdef __MAC
    data->SetReal(IDD_FLEXIMAGE_GAMMA,                  1.8);
#else
    data->SetReal(IDD_FLEXIMAGE_GAMMA,                  2.2);
#endif
  }
  data->SetBool(IDD_FLEXIMAGE_TONEMAP_SETTINGS,       FALSE);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_PRESCALE,      1.0);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_POSTSCALE,     1.2);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_BURN,          6.0);
  data->SetBool(IDD_FLEXIMAGE_PREMULTIPLY,            TRUE);
  data->SetReal(IDD_FLEXIMAGE_DISPLAY_INTERVAL,       12);
  data->SetReal(IDD_FLEXIMAGE_WRITE_INTERVAL,         120);
  data->SetBool(IDD_FLEXIMAGE_WRITE_TONEMAPPED_TGA,   TRUE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_TONEMAPPED_EXR,   FALSE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_EXR, FALSE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_TONEMAPPED_IGI,   FALSE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_IGI, FALSE);

  // set export defaults
  data->SetLong(IDD_WHICH_EXPORT_FILENAME, IDD_ASK_FOR_EXPORT_FILENAME);
  data->SetBool(IDD_ALLOW_OVERWRITING,     FALSE);
  data->SetReal(IDD_SCALE_FACTOR,          0.01);

  return TRUE;
}


/// Overwritten function that frees all allocated resources.
///
/// @param[in]  node
///   The data node, created by C4D for storing the settings.
void LuxC4DSettings::Free(GeListNode* node)
{}


/// Overwritten function that is called to load a description. We use it to
/// hide descriptions, we don't want to show due to the selection of specific
/// parameters.
///
/// @param[in]  node
///   Pointer to the data node.
/// @param[in]  description
///   The description to add the parameters to.
/// @param[out]  flags
///   State flags set during loading of description.
/// @return
///   TRUE if successul, FALSE otherwise.
Bool LuxC4DSettings::GetDDescription(GeListNode*  node,
                                     Description* description,
                                     LONG&        flags)
{
  // get container for easy access to current values
  BaseContainer* nodeData = ((BaseObject*)node)->GetDataInstance();
  if (!nodeData)  ERRLOG_RETURN_FALSE("LuxC4DSettings::GetDDescription(): No base container found.");

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_FALSE("LuxC4DSettings::GetDDescription(): Could not load description for node.");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_FALSE("LuxC4DSettings::GetDDescription(): Could not allocate empty AtomArray.");
  }

  // show/hide sampler parameters
  LONG sampler = nodeData->GetLong(IDD_SAMPLER);
  Bool dumbSampler = (sampler == IDD_SAMPLER_RANDOM) ||
                     (sampler == IDD_SAMPLER_LOWDISCREPANCY);
  ShowParameter(description, IDD_ADVANCED_SAMPLER, params, !dumbSampler);
  ShowParameter(description, IDG_RANDOM,           params, sampler == IDD_SAMPLER_RANDOM);
  ShowParameter(description, IDG_LOWDISCREPANCY,   params, sampler == IDD_SAMPLER_LOWDISCREPANCY);
  ShowParameter(description, IDG_METROPOLIS,       params, sampler == IDD_SAMPLER_METROPOLIS);
  ShowParameter(description, IDG_ERPT,             params, sampler == IDD_SAMPLER_ERPT);
  Bool advanced = nodeData->GetBool(IDD_ADVANCED_SAMPLER);
  if (sampler == IDD_SAMPLER_METROPOLIS) {
    ShowParameter(description, IDD_METROPOLIS_MICRO_MUTATION_PROB, params, advanced);
    ShowParameter(description, IDD_METROPOLIS_MUTATION_RANGE,      params, advanced);
    ShowParameter(description, IDD_METROPOLIS_INIT_SAMPLES,        params, advanced);
    ShowParameter(description, IDD_METROPOLIS_STRATA_WIDTH,        params, advanced);
    ShowParameter(description, IDD_METROPOLIS_USE_VARIANCE,        params, advanced);
    ShowParameter(description, IDD_METROPOLIS_USE_QR,              params, advanced);
  } else if (sampler == IDD_SAMPLER_ERPT) {
    ShowParameter(description, IDD_ERPT_MUTATION_RANGE,            params, advanced);
  }

  // show/hide integrator parameters
  LONG integrator = nodeData->GetLong(IDD_INTEGRATOR);
  ShowParameter(description, IDG_PATH,             params, integrator == IDD_INTEGRATOR_PATH);
  ShowParameter(description, IDG_DISTRIBUTED_PATH, params, integrator == IDD_INTEGRATOR_DISTRIBUTED_PATH);
  ShowParameter(description, IDG_BIDIRECTIONAL,    params, integrator == IDD_INTEGRATOR_BIDIRECTIONAL);
  if (integrator == IDD_INTEGRATOR_PATH) {
    LONG rrStrategy = nodeData->GetLong(IDD_PATH_RR_STRATEGY);
    ShowParameter(description, IDD_PATH_RR_CONTINUE_PROB, params, rrStrategy == IDD_PATH_RR_STRATEGY_PROBABILITY);
  }

  // show/hide pixel filter parameters
  LONG pixelFilter = nodeData->GetLong(IDD_PIXEL_FILTER);
  ShowParameter(description, IDG_BOX_FILTER,      params, pixelFilter == IDD_PIXEL_FILTER_BOX);
  ShowParameter(description, IDG_GAUSSIAN_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_GAUSSIAN);
  ShowParameter(description, IDG_MITCHELL_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_MITCHELL);
  ShowParameter(description, IDG_SINC_FILTER,     params, pixelFilter == IDD_PIXEL_FILTER_SINC);
  ShowParameter(description, IDG_TRIANGLE_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_TRIANGLE);

  // show/hide film parameters
  Bool tonemapSettings = nodeData->GetBool(IDD_FLEXIMAGE_TONEMAP_SETTINGS);
  ShowParameter(description, IDD_FLEXIMAGE_REINHARD_PRESCALE,  params, tonemapSettings);
  ShowParameter(description, IDD_FLEXIMAGE_REINHARD_POSTSCALE, params, tonemapSettings);
  ShowParameter(description, IDD_FLEXIMAGE_REINHARD_BURN,      params, tonemapSettings);
  ShowParameter(description, IDD_FLEXIMAGE_REINHARD_DUMMY,     params, tonemapSettings);

  // show/hide conversion parameters
  LONG exportFilenameMethod = nodeData->GetLong(IDD_WHICH_EXPORT_FILENAME);
  ShowParameter(description, IDD_EXPORT_FILENAME,   params, exportFilenameMethod == IDD_DEFINE_EXPORT_FILENAME);
  ShowParameter(description, IDD_ALLOW_OVERWRITING, params, exportFilenameMethod != IDD_ASK_FOR_EXPORT_FILENAME);

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return TRUE;
}


/// Obtains the film settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::GetFilm(const char*& name,
                             LuxParamSet& paramSet)
{
  // the different film names
  static const char* sFilmNames[IDD_PIXEL_FILTER_NUMBER] = {
      "fleximage"
    };

  // parameters for fleximage
  static Descr2Param<LuxIntegerT> sFleximageHaltSPP        (IDD_FLEXIMAGE_HALT_SPP,               "haltspp");
  static Descr2Param<LuxFloatT>   sFleximageGamma          (IDD_FLEXIMAGE_GAMMA,                  "gamma");
  static Descr2Param<LuxFloatT>   sFleximagePrescale       (IDD_FLEXIMAGE_REINHARD_PRESCALE,      "reinhard_prescale");
  static Descr2Param<LuxFloatT>   sFleximagePostscale      (IDD_FLEXIMAGE_REINHARD_POSTSCALE,     "reinhard_postscale");
  static Descr2Param<LuxFloatT>   sFleximageBurn           (IDD_FLEXIMAGE_REINHARD_BURN,          "reinhard_burn");
  static Descr2Param<LuxBoolT>    sFleximagePremultiply    (IDD_FLEXIMAGE_PREMULTIPLY,            "premultiplyalpha");
  static Descr2Param<LuxIntegerT> sFleximageDisplayInterval(IDD_FLEXIMAGE_DISPLAY_INTERVAL,       "displayinterval");
  static Descr2Param<LuxIntegerT> sFleximageWriteInterval  (IDD_FLEXIMAGE_WRITE_INTERVAL,         "writeinterval");
  static Descr2Param<LuxBoolT>    sFleximageTonemappedTGA  (IDD_FLEXIMAGE_WRITE_TONEMAPPED_TGA,   "write_tonemapped_tga");
  static Descr2Param<LuxBoolT>    sFleximageTonemappedEXR  (IDD_FLEXIMAGE_WRITE_TONEMAPPED_EXR,   "write_tonemapped_exr");
  static Descr2Param<LuxBoolT>    sFleximageUnTonemappedEXR(IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_EXR, "write_untonemapped_exr");
  static Descr2Param<LuxBoolT>    sFleximageTonemappedIGI  (IDD_FLEXIMAGE_WRITE_TONEMAPPED_IGI,   "write_tonemapped_igi");
  static Descr2Param<LuxBoolT>    sFleximageUnTonemappedIGI(IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_IGI, "write_untonemapped_igi");
  

  // set default sampler
  name = sFilmNames[IDD_FILM_FLEXIMAGE];

  // get base container of this node
  BaseContainer* data = GetData();
  if (!data)  return;

  // get pixel filter ...
  LONG film = data->GetLong(IDD_FILM);

  // ... and fetch its settings
  switch (film) {
    // film fleximage
    case IDD_FILM_FLEXIMAGE:
      CopyParam(sFleximageHaltSPP,         paramSet);
      CopyParam(sFleximageGamma,           paramSet);
      if (data->GetBool(IDD_FLEXIMAGE_TONEMAP_SETTINGS)) {
        CopyParam(sFleximagePrescale,  paramSet);
        CopyParam(sFleximagePostscale, paramSet);
        CopyParam(sFleximageBurn,      paramSet);
      }
      CopyParam(sFleximagePremultiply,     paramSet);
      CopyParam(sFleximageDisplayInterval, paramSet);
      CopyParam(sFleximageWriteInterval,   paramSet);
      CopyParam(sFleximageTonemappedTGA,   paramSet);
      CopyParam(sFleximageTonemappedEXR,   paramSet);
      CopyParam(sFleximageUnTonemappedEXR, paramSet);
      CopyParam(sFleximageTonemappedIGI,   paramSet);
      CopyParam(sFleximageUnTonemappedIGI, paramSet);
      break;
    // invalid film -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetFilm(): invalid film found -> using default settings");
  }

  // store pixel filter name
  name = sFilmNames[film];
}


/// Obtains the pixel filter settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::GetPixelFilter(const char*& name,
                                    LuxParamSet& paramSet)
{
  // the different pixel filter names
  static const char* sPixelFilterNames[IDD_PIXEL_FILTER_NUMBER] = {
      "box",
      "gaussian",
      "mitchell",
      "sinc",
      "triangle"
    };

  // parameters for box filter
  static Descr2Param<LuxFloatT> sBoxWidth (IDD_BOX_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloatT> sBoxHeight(IDD_BOX_FILTER_HEIGHT, "ywidth");

  // parameters for Gaussian filter
  static Descr2Param<LuxFloatT> sGaussianWidth (IDD_GAUSSIAN_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloatT> sGaussianHeight(IDD_GAUSSIAN_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloatT> sGaussianAlpha (IDD_GAUSSIAN_FILTER_ALPHA,  "alpha");

  // parameters for Mitchell filter
  static Descr2Param<LuxFloatT> sMitchellWidth (IDD_MITCHELL_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloatT> sMitchellHeight(IDD_MITCHELL_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloatT> sMitchellB     (IDD_MITCHELL_FILTER_B,      "B");
  static Descr2Param<LuxFloatT> sMitchellC     (IDD_MITCHELL_FILTER_C,      "C");

  // parameters for sinc filter
  static Descr2Param<LuxFloatT> sSincWidth (IDD_SINC_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloatT> sSincHeight(IDD_SINC_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloatT> sSincTau   (IDD_SINC_FILTER_TAU,    "tau");

  // parameters for triangle filter
  static Descr2Param<LuxFloatT> sTriangleWidth (IDD_TRIANGLE_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloatT> sTriangleHeight(IDD_TRIANGLE_FILTER_HEIGHT, "ywidth");


  // set default sampler
  name = sPixelFilterNames[IDD_PIXEL_FILTER_MITCHELL];

  // get base container of this node
  BaseContainer* data = GetData();
  if (!data)  return;

  // get pixel filter ...
  LONG pixelFilter = data->GetLong(IDD_PIXEL_FILTER);

  // ... and fetch its settings
  switch (pixelFilter) {
    // box filter 
    case IDD_PIXEL_FILTER_BOX:
      CopyParam(sBoxWidth,  paramSet);
      CopyParam(sBoxHeight, paramSet);
      break;
    // Gaussian filter
    case IDD_PIXEL_FILTER_GAUSSIAN:
      CopyParam(sGaussianWidth,  paramSet);
      CopyParam(sGaussianHeight, paramSet);
      CopyParam(sGaussianAlpha,  paramSet);
      break;
    // Mitchell filter
    case IDD_PIXEL_FILTER_MITCHELL:
      CopyParam(sMitchellWidth,  paramSet);
      CopyParam(sMitchellHeight, paramSet);
      CopyParam(sMitchellB,      paramSet);
      CopyParam(sMitchellC,      paramSet);
      break;
    // sinc filter
    case IDD_PIXEL_FILTER_SINC:
      CopyParam(sSincWidth,  paramSet);
      CopyParam(sSincHeight, paramSet);
      CopyParam(sSincTau,    paramSet);
      break;
    // triangle filter
    case IDD_PIXEL_FILTER_TRIANGLE:
      CopyParam(sTriangleWidth,  paramSet);
      CopyParam(sTriangleHeight, paramSet);
      break;
    // invalid pixel filter -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetPixelFilter(): invalid pixel filter found -> using default settings");
  }

  // store pixel filter name
  name = sPixelFilterNames[pixelFilter];
}


/// Obtains the sampler settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::GetSampler(const char*& name,
                                LuxParamSet& paramSet)
{
  // the different sampler names
  static const char* sSamplerNames[IDD_SAMPLER_NUMBER] = {
      "lowdiscrepancy",
      "random",
      "metropolis",
      "erpt"
    };

  // the different pixel sampler names
  static const char* sPixelSamplerNames[IDD_PIXELSAMPLER_NUMBER] = {
      "lowdiscrepancy",
      "hilbert",
      "vegas",
      "random",
      "linear",
      "tile"
    };

  // parameters for lowdiscrepancy sampler
  static Descr2Param<LuxStringT>  sLowdiscrepancyPixelSampler(IDD_LOWDISCREPANCY_PIXELSAMPLER, "pixelsampler");
  static Descr2Param<LuxIntegerT> sLowdiscrepancyPixelSamples(IDD_LOWDISCREPANCY_SAMPLES,      "pixelsamples");

  // parameters for random sampler
  static Descr2Param<LuxStringT>  sRandomPixelSampler(IDD_RANDOM_PIXELSAMPLER, "pixelsampler");
  static Descr2Param<LuxIntegerT> sRandomXSamples    (IDD_RANDOM_SAMPLES_X,    "xsamples");
  static Descr2Param<LuxIntegerT> sRandomYSamples    (IDD_RANDOM_SAMPLES_Y,    "ysamples");

  // parameters for metropolis sampler
  static Descr2Param<LuxFloatT>   sMetroLargeMutationProb(IDD_METROPOLIS_LARGE_MUTATION_PROB, "largemutationprob");
  static Descr2Param<LuxIntegerT> sMetroMaxConsecRejects (IDD_METROPOLIS_MAX_CONSEC_REJECTS,  "maxconsecrejects");
  static Descr2Param<LuxFloatT>   sMetroMicroMutationProb(IDD_METROPOLIS_MICRO_MUTATION_PROB, "micromutationprob");
  static Descr2Param<LuxFloatT>   sMetroMutationRange    (IDD_METROPOLIS_MUTATION_RANGE,      "mutationrange");
  static Descr2Param<LuxIntegerT> sMetroInitSamples      (IDD_METROPOLIS_INIT_SAMPLES,        "initsamples");
  static Descr2Param<LuxFloatT>   sMetroStrataWidth      (IDD_METROPOLIS_STRATA_WIDTH,        "stratawidth");
  static Descr2Param<LuxBoolT>    sMetroUseVariance      (IDD_METROPOLIS_USE_VARIANCE,        "usevariance");
  static Descr2Param<LuxBoolT>    sMetroUseQR            (IDD_METROPOLIS_USE_QR,              "useqr");

  // parameters for ERPT sampler
  static Descr2Param<LuxIntegerT> sERPTInitSamples      (IDD_ERPT_INIT_SAMPLES,        "initsamples");
  static Descr2Param<LuxIntegerT> sERPTChainLength      (IDD_ERPT_CHAINLENGTH,         "chainlength");
  static Descr2Param<LuxFloatT>   sERPTMicroMutationProb(IDD_ERPT_MICRO_MUTATION_PROB, "micromutationprob");
  static Descr2Param<LuxFloatT>   sERPTMutationRange    (IDD_ERPT_MUTATION_RANGE,      "mutationrange");


  // set default sampler
  name = sSamplerNames[IDD_SAMPLER_LOWDISCREPANCY];

  // get base container of this node
  BaseContainer* data = GetData();
  if (!data)  return;

  // get sampler ...
  LONG sampler = data->GetLong(IDD_SAMPLER);

  // ... and fetch its settings
  switch (sampler) {
    // low discrepancy sampler
    case IDD_SAMPLER_LOWDISCREPANCY:
      CopyParam(sLowdiscrepancyPixelSampler, paramSet,
                sPixelSamplerNames, IDD_PIXELSAMPLER_NUMBER);
      CopyParam(sLowdiscrepancyPixelSamples, paramSet);
      break;
    // random sampler
    case IDD_SAMPLER_RANDOM:
      CopyParam(sRandomPixelSampler, paramSet,
                sPixelSamplerNames, IDD_PIXELSAMPLER_NUMBER);
      CopyParam(sRandomXSamples, paramSet);
      CopyParam(sRandomYSamples, paramSet);
      break;
    // metropolis sampler
    case IDD_SAMPLER_METROPOLIS:
      CopyParam(sMetroLargeMutationProb, paramSet);
      CopyParam(sMetroMaxConsecRejects,  paramSet);
      if (data->GetBool(IDD_ADVANCED_SAMPLER)) {
        CopyParam(sMetroMicroMutationProb, paramSet);
        CopyParam(sMetroMutationRange,     paramSet);
        CopyParam(sMetroInitSamples,       paramSet);
        CopyParam(sMetroStrataWidth,       paramSet);
        CopyParam(sMetroUseVariance,       paramSet);
        CopyParam(sMetroUseQR,             paramSet);
      }
      break;
    // ERPT sampler
    case IDD_SAMPLER_ERPT:
      CopyParam(sERPTInitSamples,       paramSet);
      CopyParam(sERPTChainLength,       paramSet);
      CopyParam(sERPTMicroMutationProb, paramSet);
      if (data->GetBool(IDD_ADVANCED_SAMPLER)) {
        CopyParam(sERPTMutationRange, paramSet);
      }
      break;
    // invalid sampler -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetSampler(): invalid sampler found -> using default settings");
  }

  // store sampler name
  name = sSamplerNames[sampler];
}


/// Obtains the surface integrator settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::GetSurfaceIntegrator(const char*& name,
                                          LuxParamSet& paramSet)
{
  // the different sampler names
  static const char* sIntegratorNames[IDD_INTEGRATOR_NUMBER] = {
      "path",
      "distributedpath",
      "bidirectional"
    };

  // direct light strategies
  static const char* sDirectLightStrategies[IDD_DIRECT_LIGHT_STRATEGY_NUMBER] = {
      "auto",
      "one",
      "all"
    };

  // Russian roulette strategies
  static const char* sRRStrategies[IDD_PATH_RR_STRATEGY_NUMBER] = {
      "efficiency",
      "probability",
      "none"
    };

  // parameters for path integrator
  static Descr2Param<LuxIntegerT> sPathMaxDepth           (IDD_PATH_MAX_DEPTH,             "maxdepth");
  static Descr2Param<LuxStringT>  sPathDirectLightStrategy(IDD_PATH_DIRECT_LIGHT_STRATEGY, "strategy");
  static Descr2Param<LuxStringT>  sPathRRStrategy         (IDD_PATH_RR_STRATEGY,           "rrstrategy");
  static Descr2Param<LuxFloatT>   sPathRRContinueProb     (IDD_PATH_RR_CONTINUE_PROB,      "rrcontinueprob");

  // parameters for distributed path integrator
  static Descr2Param<LuxStringT>  sDistriPathDirectLightStrategy       (IDD_DISTRIBUTED_PATH_DIRECT_LIGHT_STRATEGY,         "strategy");
  static Descr2Param<LuxIntegerT> sDistriPathDirectDirectLightSamples  (IDD_DISTRIBUTED_PATH_DIRECT_DIRECT_LIGHT_SAMPLES,   "directsamples");
  static Descr2Param<LuxIntegerT> sDistriPathInDirectDirectLightSamples(IDD_DISTRIBUTED_PATH_INDIRECT_DIRECT_LIGHT_SAMPLES, "indirectsamples");
  static Descr2Param<LuxIntegerT> sDistriPathDiffuseReflectDepth       (IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_DEPTH,         "diffusereflectdepth");
  static Descr2Param<LuxIntegerT> sDistriPathDiffuseReflectSamples     (IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_SAMPLES,       "diffusereflectsamples");
  static Descr2Param<LuxIntegerT> sDistriPathDiffuseRefractDepth       (IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_DEPTH,         "diffuserefractdepth");
  static Descr2Param<LuxIntegerT> sDistriPathDiffuseRefractSamples     (IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_SAMPLES,       "diffuserefractsamples");
  static Descr2Param<LuxIntegerT> sDistriPathGlossyReflectDepth        (IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_DEPTH,          "glossyreflectdepth");
  static Descr2Param<LuxIntegerT> sDistriPathGlossyReflectSamples      (IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_SAMPLES,        "glossyreflectsamples");
  static Descr2Param<LuxIntegerT> sDistriPathGlossyRefractDepth        (IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_DEPTH,          "glossyrefractdepth");
  static Descr2Param<LuxIntegerT> sDistriPathGlossyRefractSamples      (IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_SAMPLES,        "glossyrefractsamples");
  static Descr2Param<LuxIntegerT> sDistriPathSpecularReflectDepth      (IDD_DISTRIBUTED_PATH_SPECULAR_REFLECT_DEPTH,        "specularreflectdepth");
  static Descr2Param<LuxIntegerT> sDistriPathSpecularRefractDepth      (IDD_DISTRIBUTED_PATH_SPECULAR_REFRACT_DEPTH,        "specularrefractdepth");

  // parameters for bidirectional integrator
  static Descr2Param<LuxIntegerT> sBidirectionalEyeDepth           (IDD_BIDIRECTIONAL_EYE_DEPTH,             "eyedepth");
  static Descr2Param<LuxIntegerT> sBidirectionalLightDepth         (IDD_BIDIRECTIONAL_LIGHT_DEPTH,           "lightdepth");
  static Descr2Param<LuxStringT>  sBidirectionalDirectLightStrategy(IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY, "strategy");


  // set default sampler
  name = sIntegratorNames[IDD_INTEGRATOR_PATH];

  // get base container of this node
  BaseContainer* data = GetData();
  if (!data)  return;

  // get sampler...
  LONG integrator = data->GetLong(IDD_INTEGRATOR);

  // ... and fetch its data
  switch (integrator) {
    // path integrator
    case IDD_INTEGRATOR_PATH:
      CopyParam(sPathMaxDepth, paramSet);
      CopyParam(sPathDirectLightStrategy, paramSet,
                sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
      CopyParam(sPathRRStrategy, paramSet,
                sRRStrategies, IDD_PATH_RR_STRATEGY_NUMBER);
      if (data->GetLong(IDD_PATH_RR_STRATEGY) == IDD_PATH_RR_STRATEGY_PROBABILITY) {
        CopyParam(sPathRRContinueProb, paramSet);
      }
      break;
    // distributed path integrator
    case IDD_INTEGRATOR_DISTRIBUTED_PATH:
      CopyParam(sDistriPathDirectLightStrategy, paramSet,
                sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
      CopyParam(sDistriPathDirectDirectLightSamples,   paramSet);
      CopyParam(sDistriPathInDirectDirectLightSamples, paramSet);
      CopyParam(sDistriPathDiffuseReflectDepth,        paramSet);
      CopyParam(sDistriPathDiffuseReflectSamples,      paramSet);
      CopyParam(sDistriPathDiffuseRefractDepth,        paramSet);
      CopyParam(sDistriPathDiffuseRefractSamples,      paramSet);
      CopyParam(sDistriPathGlossyReflectDepth,         paramSet);
      CopyParam(sDistriPathGlossyReflectSamples,       paramSet);
      CopyParam(sDistriPathGlossyRefractDepth,         paramSet);
      CopyParam(sDistriPathGlossyRefractSamples,       paramSet);
      CopyParam(sDistriPathSpecularReflectDepth,       paramSet);
      CopyParam(sDistriPathSpecularRefractDepth,       paramSet);
      break;
    // bidirectional integrator
    case IDD_INTEGRATOR_BIDIRECTIONAL:
      CopyParam(sBidirectionalEyeDepth,   paramSet);
      CopyParam(sBidirectionalLightDepth, paramSet);
      CopyParam(sBidirectionalDirectLightStrategy, paramSet,
                sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
      break;
    // invalid integrator -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetSurfaceIntegrator(): invalid integrator found -> using default settings");
  }

  // store integrator name
  name = sIntegratorNames[integrator];
}


/// Obtains the filename of the exported scene from the settings. If it's not
/// specified the filename will be empty.
///
/// @param[in]  document
///   The document that should be exported.
/// @param[out]  path
///   The filename for the file to export.
/// @param[out]  overwritingAllowed
///   Will be set to TRUE, if the user wants to export to that filename, even
///   if already a file with that path exists.
void LuxC4DSettings::GetExportFilename(BaseDocument& document,
                                       Filename&     path,
                                       Bool&         overwritingAllowed)
{
  // reset return values
  path               = Filename();
  overwritingAllowed = FALSE;

  // get base container and determine export filename method that was chosen
  BaseContainer* data = GetData();
  if (!data)  return;
  LONG exportFilenameMethod = data->GetLong(IDD_WHICH_EXPORT_FILENAME);

  // if the filename is derived from the C4D scene file, get the C4D filename
  // and make a .lxs file out of it
  if (exportFilenameMethod == IDD_USE_C4D_EXPORT_FILENAME) {
    path.SetDirectory(document.GetDocumentPath());
    path.SetFile(document.GetDocumentName());
    path.SetSuffix("lxs");
  // otherwise, just grab the filename from the input field and set the suffix
  // to .lxs
  } else if (exportFilenameMethod == IDD_DEFINE_EXPORT_FILENAME) {
    path = data->GetFilename(IDD_EXPORT_FILENAME);
    path.SetSuffix("lxs");
  }

  // determine if the userwants overwrite files
  overwritingAllowed = data->GetBool(IDD_ALLOW_OVERWRITING);
}


/// Returns the global scale from the settings object.
LReal LuxC4DSettings::GetC4D2LuxScale(void)
{
  // get base container and return the scale factor from it
  BaseContainer* data = GetData();
  if (!data)  return 0.01;
  return data->GetReal(IDD_SCALE_FACTOR);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DSettings.
 *****************************************************************************/


/// Returns the data container of this node.
BaseContainer* LuxC4DSettings::GetData(void)
{
  BaseContainer *data = 0;
  BaseList2D* listNode = static_cast<BaseList2D*>(Get());
  if (listNode) {
    data = listNode->GetDataInstance();
  }
  if (!data)  ERRLOG_RETURN_FALSE("LuxC4DSettings::GetData(): could not obtain base container");
  return data;
}


/// Copies a bool parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::CopyParam(Descr2Param<LuxBoolT>& descr2Param,
                               LuxParamSet&           paramSet)
{
  // get base container of this object
  BaseContainer* data = GetData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = (data->GetBool(descr2Param.mID) != 0);
  paramSet.addParam(LuxParamSet::LUX_BOOL, descr2Param.mParamName, &descr2Param.mParam);
}


/// Copies an integer parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::CopyParam(Descr2Param<LuxIntegerT>& descr2Param,
                               LuxParamSet&              paramSet)
{
  // get base container of this object
  BaseContainer* data = GetData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = data->GetLong(descr2Param.mID);
  paramSet.addParam(LuxParamSet::LUX_INTEGER, descr2Param.mParamName, &descr2Param.mParam);
}


/// Copies a float parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::CopyParam(Descr2Param<LuxFloatT>& descr2Param,
                               LuxParamSet&            paramSet)
{
  // get base container of this object
  BaseContainer* data = GetData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = data->GetReal(descr2Param.mID);
  paramSet.addParam(LuxParamSet::LUX_FLOAT, descr2Param.mParamName, &descr2Param.mParam);
}


/// Copies a cycle parameter into the Lux parameter set. While doing that it
/// it converts the cycle value into a string.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
/// @param[in]  cycleEntries
///   An array of the cycle entries as C strings.
/// @param[in]  cycleEntryCount
///   The number of entries in the cycle entry array.
void LuxC4DSettings::CopyParam(Descr2Param<LuxStringT>& descr2Param,
                               LuxParamSet&            paramSet,
                               const char**            cycleEntries,
                               LONG                    cycleEntryCount)
{
  // get base container of this object
  BaseContainer* data = GetData();
  if (!data)  return;

  // get and check entry from cycle
  LONG entry = data->GetLong(descr2Param.mID);
  if ((entry<0) || (entry>=cycleEntryCount))
    ERRLOG_RETURN("LuxC4DSettings::CopyCycleParam(): invalid cycle entry found -> using default settings");

  // map entry to string and add that to parameter set
  descr2Param.mParam = cycleEntries[entry];
  paramSet.addParam(LuxParamSet::LUX_STRING, descr2Param.mParamName, &descr2Param.mParam);
}
