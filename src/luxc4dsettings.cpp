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
/// defaults.
///
/// @param[in]  node
///   The data node, created by C4D for storing the settings.
/// @return
///   TRUE if the initialisation was successful.
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

  // get render gamma
  Real renderGamma;
  if (c4dRenderSettings) {
    renderGamma = c4dRenderSettings->GetReal(RDATA_RENDERGAMMA);
  } else {
#ifdef __MAC
    renderGamma = 1.8;
#else
    renderGamma = 2.2;
#endif
  }


  // obtain container from node
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // set sampler defaults
  data->SetLong(IDD_SAMPLER,                        IDD_SAMPLER_METROPOLIS);
  data->SetLong(IDD_RANDOM_PIXELSAMPLER,            IDD_PIXELSAMPLER_VEGAS);
  data->SetLong(IDD_RANDOM_PIXELSAMPLES,            4);
  data->SetLong(IDD_LOWDISCREPANCY_PIXELSAMPLER,    IDD_PIXELSAMPLER_LOWDISCREPANCY);
  data->SetLong(IDD_LOWDISCREPANCY_PIXELSAMPLES,    4);
  data->SetLong(IDD_METROPOLIS_MAX_CONSEC_REJECTS,  512);
  data->SetReal(IDD_METROPOLIS_LARGE_MUTATION_PROB, 0.4);
  data->SetReal(IDD_METROPOLIS_MICRO_MUTATION_PROB, 0.0);
  data->SetBool(IDD_METROPOLIS_USE_VARIANCE,        FALSE);
  data->SetReal(IDD_METROPOLIS_STRENGTH,            0.6);
  data->SetLong(IDD_ERPT_CHAINLENGTH,               512);
  data->SetReal(IDD_ERPT_MICRO_MUTATION_PROB,       0.5);
  data->SetLong(IDD_ERPT_PIXELSAMPLER,              IDD_PIXELSAMPLER_VEGAS);
  data->SetLong(IDD_ERPT_PIXELSAMPLES,              4);

  // set integrator defaults
  data->SetLong(IDD_INTEGRATOR,                                     IDD_INTEGRATOR_BIDIRECTIONAL);
  data->SetLong(IDD_PATH_MAX_DEPTH,                                 10);
  data->SetBool(IDD_PATH_INCLUDE_ENVIRONMENT,                       TRUE);
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
  data->SetLong(IDD_BIDIRECTIONAL_MAX_DEPTH,                        16);
  data->SetLong(IDD_BIDIRECTIONAL_EYE_DEPTH,                        16);
  data->SetLong(IDD_BIDIRECTIONAL_LIGHT_DEPTH,                      16);
  data->SetLong(IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY,            IDD_DIRECT_LIGHT_STRATEGY_AUTO);
  data->SetReal(IDD_BIDIRECTIONAL_EYE_RR_THRESHOLD,                 0.0);
  data->SetReal(IDD_BIDIRECTIONAL_LIGHT_RR_THRESHOLD,               0.0);
  data->SetLong(IDD_DIRECT_LIGHTING_MAX_DEPTH,                      8);
  data->SetLong(IDD_DIRECT_LIGHTING_STRATEGY,                       IDD_DIRECT_LIGHT_STRATEGY_AUTO);

  // set pixel filter defaults
  data->SetLong(IDD_PIXEL_FILTER,           IDD_PIXEL_FILTER_GAUSSIAN);
  data->SetReal(IDD_BOX_FILTER_WIDTH,       0.5);
  data->SetReal(IDD_BOX_FILTER_HEIGHT,      0.5);
  data->SetReal(IDD_GAUSSIAN_FILTER_WIDTH,  1.3);
  data->SetReal(IDD_GAUSSIAN_FILTER_HEIGHT, 1.3);
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

  // set accelerator defaults
  data->SetLong(IDD_ACCELERATION_TYPE,         IDD_ACCELERATION_QBVH);
  data->SetBool(IDD_ACCELERATION_ADVANCED,     FALSE);
  data->SetLong(IDD_KDTREE_INTERSECTION_COST,  80);
  data->SetLong(IDD_KDTREE_TRAVERSAL_COST,     1);
  data->SetReal(IDD_KDTREE_EMPTY_BONUS,        0.2);
  data->SetLong(IDD_KDTREE_MAX_PRIMITIVES,     1);
  data->SetLong(IDD_KDTREE_MAX_DEPTH,          -1);
  data->SetLong(IDD_BVH_TREE_TYPE,             IDD_BVH_QUAD_TREE);
  data->SetLong(IDD_BVH_COST_SAMPLES,          0);
  data->SetLong(IDD_BVH_INTERSECTION_COST,     80);
  data->SetLong(IDD_BVH_TRAVERSAL_COST,        10);
  data->SetReal(IDD_BVH_EMPTY_BONUS,           0.5);
  data->SetLong(IDD_QBVH_MAX_PRIMITIVES,       4);
  data->SetReal(IDD_QBVH_FULL_SWEEP_THRESHOLD, 16);
  data->SetLong(IDD_QBVH_SKIP_FACTOR,          1);


  // set film parameters
  data->SetLong(IDD_FILM,                             IDD_FILM_FLEXIMAGE);
  data->SetLong(IDD_FLEXIMAGE_HALT_SPP,               0);
  data->SetReal(IDD_FLEXIMAGE_GAMMA,                  renderGamma);
  data->SetBool(IDD_FLEXIMAGE_PREMULTIPLY,            FALSE);
  data->SetReal(IDD_FLEXIMAGE_DISPLAY_INTERVAL,       12);
  data->SetReal(IDD_FLEXIMAGE_WRITE_INTERVAL,         120);
  data->SetLong(IDD_FLEXIMAGE_CLAMP_METHOD,           IDD_CLAMP_METHOD_LUM);
  data->SetLong(IDD_FLEXIMAGE_REJECT_WARMUP,          128);
  data->SetLong(IDD_FLEXIMAGE_TONEMAP_KERNEL,         IDD_TONEMAP_KERNEL_MAXWHITE);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_PRESCALE,      1.0);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_POSTSCALE,     1.0);
  data->SetReal(IDD_FLEXIMAGE_REINHARD_BURN,          6.0);
  data->SetReal(IDD_FLEXIMAGE_LINEAR_SENSITIVITY,     50.0);
  data->SetReal(IDD_FLEXIMAGE_LINEAR_EXPOSURE,        5.0);
  data->SetReal(IDD_FLEXIMAGE_LINEAR_FSTOP,           2.8);
  data->SetReal(IDD_FLEXIMAGE_LINEAR_GAMMA,           1.0);
  data->SetReal(IDD_FLEXIMAGE_CONTRAST_YWA,           1.0);
  data->SetLong(IDD_FLEXIMAGE_FILENAME_TYPE,          IDD_FLEXIMAGE_FILENAME_AS_SCENE_FILE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_EXR,              FALSE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_PNG,              TRUE);
  data->SetBool(IDD_FLEXIMAGE_WRITE_TGA,              FALSE);
  data->SetLong(IDD_FLEXIMAGE_EXR_CHANNELS,           IDD_WRITE_CHANNELS_RGBA);
  data->SetBool(IDD_FLEXIMAGE_EXR_HALFTYPE,           TRUE);
  data->SetLong(IDD_FLEXIMAGE_EXR_COMPRESSION,        IDD_EXR_COMPRESSION_PIZ);
  data->SetBool(IDD_FLEXIMAGE_EXR_APPLY_IMAGING,      TRUE);
  data->SetBool(IDD_FLEXIMAGE_EXR_GAMUT_CLAMP,        TRUE);
  data->SetBool(IDD_FLEXIMAGE_EXR_WRITE_ZBUF,         TRUE);
  data->SetLong(IDD_FLEXIMAGE_EXR_ZBUF_NORM_TYPE,     IDD_ZBUF_NORM_TYPE_NONE);
  data->SetLong(IDD_FLEXIMAGE_PNG_CHANNELS,           IDD_WRITE_CHANNELS_RGB);
  data->SetBool(IDD_FLEXIMAGE_PNG_16BIT,              FALSE);
  data->SetBool(IDD_FLEXIMAGE_PNG_GAMUT_CLAMP,        TRUE);
  data->SetLong(IDD_FLEXIMAGE_TGA_CHANNELS,           IDD_WRITE_CHANNELS_RGB);
  data->SetBool(IDD_FLEXIMAGE_TGA_GAMUT_CLAMP,        TRUE);

  // set export defaults
  data->SetLong(IDD_WHICH_EXPORT_FILENAME,       IDD_ASK_FOR_EXPORT_FILENAME);
  data->SetBool(IDD_ALLOW_OVERWRITING,           FALSE);
  data->SetReal(IDD_SCALE_FACTOR,                1.0);
  data->SetBool(IDD_EXPORT_BUMP_SAMPLE_DISTANCE, TRUE);
  data->SetReal(IDD_BUMP_SAMPLE_DISTANCE,        0.001);
  data->SetReal(IDD_TEXTURE_GAMMA_CORRECTION,    renderGamma);
  data->SetBool(IDD_USE_RELATIVE_PATHS,          TRUE);
  data->SetBool(IDD_DO_COLOUR_GAMMA_CORRECTION,  TRUE);


  return TRUE;
}


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
  // obtain container from node
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DSettings::GetDDescription(): could not load description for node");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DSettings::GetDDescription(): could not allocate empty AtomArray");
  }

  // show/hide sampler parameters
  LONG sampler = data->GetLong(IDD_SAMPLER);
  Bool hasAdvanced = (sampler == IDD_SAMPLER_METROPOLIS);
  showParameter(description, IDD_ADVANCED_SAMPLER, params, hasAdvanced);
  showParameter(description, IDG_RANDOM,           params, sampler == IDD_SAMPLER_RANDOM);
  showParameter(description, IDG_LOWDISCREPANCY,   params, sampler == IDD_SAMPLER_LOWDISCREPANCY);
  showParameter(description, IDG_METROPOLIS,       params, sampler == IDD_SAMPLER_METROPOLIS);
  showParameter(description, IDG_ERPT,             params, sampler == IDD_SAMPLER_ERPT);
  Bool advanced = data->GetBool(IDD_ADVANCED_SAMPLER);
  if (sampler == IDD_SAMPLER_METROPOLIS) {
    showParameter(description, IDD_METROPOLIS_STRENGTH,            params, !advanced);
    showParameter(description, IDD_METROPOLIS_LARGE_MUTATION_PROB, params, advanced);
    showParameter(description, IDD_METROPOLIS_MAX_CONSEC_REJECTS,  params, advanced);
    showParameter(description, IDD_METROPOLIS_MICRO_MUTATION_PROB, params, advanced);
    showParameter(description, IDD_METROPOLIS_USE_VARIANCE,        params, advanced);
  }

  // show/hide integrator parameters
  LONG integrator = data->GetLong(IDD_INTEGRATOR);
  hasAdvanced = (integrator == IDD_INTEGRATOR_PATH) ||
                (integrator == IDD_INTEGRATOR_BIDIRECTIONAL) ||
                (integrator == IDD_INTEGRATOR_DIRECT_LIGHTING);
  showParameter(description, IDD_ADVANCED_INTEGRATOR, params, hasAdvanced);
  showParameter(description, IDG_PATH,                params, integrator == IDD_INTEGRATOR_PATH);
  showParameter(description, IDG_DISTRIBUTED_PATH,    params, integrator == IDD_INTEGRATOR_DISTRIBUTED_PATH);
  showParameter(description, IDG_BIDIRECTIONAL,       params, integrator == IDD_INTEGRATOR_BIDIRECTIONAL);
  showParameter(description, IDG_DIRECT_LIGHTING,     params, integrator == IDD_INTEGRATOR_DIRECT_LIGHTING);
  advanced = data->GetBool(IDD_ADVANCED_INTEGRATOR);
  if (integrator == IDD_INTEGRATOR_PATH) {
    Bool rrStrategyProb = (data->GetLong(IDD_PATH_RR_STRATEGY) == IDD_PATH_RR_STRATEGY_PROBABILITY);
    showParameter(description, IDD_PATH_DIRECT_LIGHT_STRATEGY, params, advanced);
    showParameter(description, IDD_PATH_RR_STRATEGY,           params, advanced);
    showParameter(description, IDD_PATH_RR_CONTINUE_PROB,      params, advanced && rrStrategyProb);
  } else if (integrator == IDD_INTEGRATOR_BIDIRECTIONAL) {
    showParameter(description, IDD_BIDIRECTIONAL_EYE_DEPTH,             params, advanced);
    showParameter(description, IDD_BIDIRECTIONAL_LIGHT_DEPTH,           params, advanced);
    showParameter(description, IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY, params, advanced);
    showParameter(description, IDD_BIDIRECTIONAL_EYE_RR_THRESHOLD,      params, advanced);
    showParameter(description, IDD_BIDIRECTIONAL_LIGHT_RR_THRESHOLD,    params, advanced);
    showParameter(description, IDD_BIDIRECTIONAL_MAX_DEPTH,             params, !advanced);
  } else if (integrator == IDD_INTEGRATOR_DIRECT_LIGHTING) {
    showParameter(description, IDD_DIRECT_LIGHTING_STRATEGY, params, advanced);
  }

  // show/hide pixel filter parameters
  LONG pixelFilter = data->GetLong(IDD_PIXEL_FILTER);
  showParameter(description, IDG_BOX_FILTER,      params, pixelFilter == IDD_PIXEL_FILTER_BOX);
  showParameter(description, IDG_GAUSSIAN_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_GAUSSIAN);
  showParameter(description, IDG_MITCHELL_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_MITCHELL);
  showParameter(description, IDG_SINC_FILTER,     params, pixelFilter == IDD_PIXEL_FILTER_SINC);
  showParameter(description, IDG_TRIANGLE_FILTER, params, pixelFilter == IDD_PIXEL_FILTER_TRIANGLE);

  // show/hide advanced accelerator parameters
  LONG accelerator = data->GetLong(IDD_ACCELERATION_TYPE);
  advanced = data->GetBool(IDD_ACCELERATION_ADVANCED);
  showParameter(description, IDG_ACCELERATION_KDTREE, params, advanced && (accelerator == IDD_ACCELERATION_KDTREE));
  showParameter(description, IDG_ACCELERATION_BVH,    params, advanced && (accelerator == IDD_ACCELERATION_BVH));
  showParameter(description, IDG_ACCELERATION_QBVH,   params, advanced && (accelerator == IDD_ACCELERATION_QBVH));

  // show hide film parameters
  LONG tonemapKernel = data->GetLong(IDD_FLEXIMAGE_TONEMAP_KERNEL);
  LONG outputFilenameMethod = data->GetLong(IDD_FLEXIMAGE_FILENAME_TYPE);
  showParameter(description, IDG_FLEXIMAGE_REINHARD, params, tonemapKernel == IDD_TONEMAP_KERNEL_REINHARD);
  showParameter(description, IDG_FLEXIMAGE_LINEAR,   params, tonemapKernel == IDD_TONEMAP_KERNEL_LINEAR);
  showParameter(description, IDG_FLEXIMAGE_CONTRAST, params, tonemapKernel == IDD_TONEMAP_KERNEL_CONTRAST);
  showParameter(description, IDD_FLEXIMAGE_OUTPUT_FILENAME,    params, outputFilenameMethod == IDD_FLEXIMAGE_FILENAME_DEFINE_HERE);
  showParameter(description, IDG_FLEXIMAGE_EXR,                params, data->GetBool(IDD_FLEXIMAGE_WRITE_EXR));
  showParameter(description, IDD_FLEXIMAGE_EXR_ZBUF_NORM_TYPE, params, data->GetBool(IDD_FLEXIMAGE_EXR_WRITE_ZBUF));
  showParameter(description, IDG_FLEXIMAGE_PNG, params, data->GetBool(IDD_FLEXIMAGE_WRITE_PNG));
  showParameter(description, IDG_FLEXIMAGE_TGA, params, data->GetBool(IDD_FLEXIMAGE_WRITE_TGA));

  // show/hide conversion parameters
  LONG exportFilenameMethod = data->GetLong(IDD_WHICH_EXPORT_FILENAME);
  showParameter(description, IDD_EXPORT_FILENAME,   params, exportFilenameMethod == IDD_DEFINE_EXPORT_FILENAME);
  showParameter(description, IDD_ALLOW_OVERWRITING, params, exportFilenameMethod != IDD_ASK_FOR_EXPORT_FILENAME);

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return SUPER::GetDDescription(node, description, flags);
}


///
Bool LuxC4DSettings::SetDParameter(GeListNode*   node,
                                   const DescID& id,
                                   const GeData& value,
                                   LONG&         flags)
{
  // obtain container from node
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // Metropolis + not advanced:
  if ((id == DescID(IDD_METROPOLIS_STRENGTH)) &&
      !data->GetBool(IDD_ADVANCED_SAMPLER))
  {
    // convert the strength to a large mutation probability
    data->SetReal(IDD_METROPOLIS_LARGE_MUTATION_PROB, 1.0 - value.GetReal());
  // Metropolis + advanced:
  } else if ((id == DescID(IDD_METROPOLIS_LARGE_MUTATION_PROB)) &&
             data->GetBool(IDD_ADVANCED_SAMPLER))
  {
    // convert the large mutation probability to strength
    data->SetReal(IDD_METROPOLIS_STRENGTH, 1.0 - value.GetReal());
  }

  // Bidirectional + not advanced:
  if ((id == DescID(IDD_BIDIRECTIONAL_MAX_DEPTH)) &&
      !data->GetBool(IDD_ADVANCED_INTEGRATOR))
  {
    // apply max. depth to eye and light depth
    data->SetLong(IDD_BIDIRECTIONAL_EYE_DEPTH,   value.GetLong());
    data->SetLong(IDD_BIDIRECTIONAL_LIGHT_DEPTH, value.GetLong());
  // Bidirectional + not advanced:
  } else if (((id == DescID(IDD_BIDIRECTIONAL_EYE_DEPTH)) ||
              (id == DescID(IDD_BIDIRECTIONAL_LIGHT_DEPTH))) &&
             data->GetBool(IDD_ADVANCED_INTEGRATOR))
  {
    // set average eye and light depth to max. depth
    LONG average = (data->GetLong(IDD_BIDIRECTIONAL_EYE_DEPTH) +
                    data->GetLong(IDD_BIDIRECTIONAL_LIGHT_DEPTH)) >> 1;
    data->SetLong(IDD_BIDIRECTIONAL_MAX_DEPTH, average);
  }

  return SUPER::SetDParameter(node, id, value, flags);
}


/// Obtains the film settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::getFilm(const char*& name,
                             LuxParamSet& paramSet)
{
  // the different film names
  static const char* sFilmNames[IDD_PIXEL_FILTER_NUMBER] = {
      "fleximage"
    };

  // the different clamp methods
  static const char* sClampMethods[IDD_CLAMP_METHOD_NUMBER] = {
      "lum",
      "hue",
      "cut"
    };

  // the different tonemap kernels
  static const char* sTonemapKernels[IDD_TONEMAP_KERNEL_NUMBER] = {
      "reinhard",
      "linear",
      "contrast",
      "maxwhite"
    };

  // the different image channels
  static const char* sImageChannels[IDD_WRITE_CHANNELS_NUMBER] = {
      "Y",
      "YA",
      "RGB",
      "RGBA"
    };

  // the different Z-buffer normalisation methods
  static const char* sZBufNormTypes[IDD_ZBUF_NORM_TYPE_NUMBER] = {
      "None",
      "Camera Start/End clip",
      "Min/Max"
    };

  // the different compression methods
  static const char* sCompressionTypes[IDD_EXR_COMPRESSION_NUMBER] = {
      "RLE (lossless)",
      "PIZ (lossless)",
      "ZIP (lossless)",
      "Pxr24 (lossy)",
      "None"
    };

  // parameters for fleximage
  static Descr2Param<LuxInteger> sFleximageHaltSPP          (IDD_FLEXIMAGE_HALT_SPP,           "haltspp");
  static Descr2Param<LuxFloat>   sFleximageGamma            (IDD_FLEXIMAGE_GAMMA,              "gamma");
  static Descr2Param<LuxBool>    sFleximagePremultiply      (IDD_FLEXIMAGE_PREMULTIPLY,        "premultiplyalpha");
  static Descr2Param<LuxInteger> sFleximageDisplayInterval  (IDD_FLEXIMAGE_DISPLAY_INTERVAL,   "displayinterval");
  static Descr2Param<LuxInteger> sFleximageWriteInterval    (IDD_FLEXIMAGE_WRITE_INTERVAL,     "writeinterval");
  static Descr2Param<LuxString>  sFleximageClampMethod      (IDD_FLEXIMAGE_CLAMP_METHOD,       "ldr_clamp_method");
  static Descr2Param<LuxInteger> sFleximageRejectWarmup     (IDD_FLEXIMAGE_REJECT_WARMUP,      "reject_warmup");
  static Descr2Param<LuxString>  sFleximageTonemapKernel    (IDD_FLEXIMAGE_TONEMAP_KERNEL,     "tonemapkernel");
  static Descr2Param<LuxFloat>   sFleximageReinhardPrescale (IDD_FLEXIMAGE_REINHARD_PRESCALE,  "reinhard_prescale");
  static Descr2Param<LuxFloat>   sFleximageReinhardPostscale(IDD_FLEXIMAGE_REINHARD_POSTSCALE, "reinhard_postscale");
  static Descr2Param<LuxFloat>   sFleximageReinhardBurn     (IDD_FLEXIMAGE_REINHARD_BURN,      "reinhard_burn");
  static Descr2Param<LuxFloat>   sFleximageLinearSensitivity(IDD_FLEXIMAGE_LINEAR_SENSITIVITY, "linear_sensitivity");
  static Descr2Param<LuxFloat>   sFleximageLinearExposure   (IDD_FLEXIMAGE_LINEAR_EXPOSURE,    "linear_exposure");
  static Descr2Param<LuxFloat>   sFleximageLinearFStop      (IDD_FLEXIMAGE_LINEAR_FSTOP,       "linear_fstop");
  static Descr2Param<LuxFloat>   sFleximageLinearGamma      (IDD_FLEXIMAGE_LINEAR_GAMMA,       "linear_gamma");
  static Descr2Param<LuxFloat>   sFleximageContrastYwa      (IDD_FLEXIMAGE_CONTRAST_YWA,       "contrast_ywa");
  static Descr2Param<LuxBool>    sFleximageWriteEXR         (IDD_FLEXIMAGE_WRITE_EXR,          "write_exr");
  static Descr2Param<LuxBool>    sFleximageWritePNG         (IDD_FLEXIMAGE_WRITE_PNG,          "write_png");
  static Descr2Param<LuxBool>    sFleximageWriteTGA         (IDD_FLEXIMAGE_WRITE_TGA,          "write_tga");
  static Descr2Param<LuxString>  sFleximageEXRChannels      (IDD_FLEXIMAGE_EXR_CHANNELS,       "write_exr_channels");
  static Descr2Param<LuxBool>    sFleximageEXRHalftype      (IDD_FLEXIMAGE_EXR_HALFTYPE,       "write_exr_halftype");
  static Descr2Param<LuxString>  sFleximageEXRCompression   (IDD_FLEXIMAGE_EXR_COMPRESSION,    "write_exr_compressiontype");
  static Descr2Param<LuxBool>    sFleximageEXRApplyImaging  (IDD_FLEXIMAGE_EXR_APPLY_IMAGING,  "write_exr_applyimaging");
  static Descr2Param<LuxBool>    sFleximageEXRGamutClamp    (IDD_FLEXIMAGE_EXR_GAMUT_CLAMP,    "write_exr_gamutclamp");
  static Descr2Param<LuxBool>    sFleximageEXRWriteZBuf     (IDD_FLEXIMAGE_EXR_WRITE_ZBUF,     "write_exr_ZBuf");
  static Descr2Param<LuxString>  sFleximageEXRZBufNormType  (IDD_FLEXIMAGE_EXR_ZBUF_NORM_TYPE, "write_exr_zbuf_normalizationtype");
  static Descr2Param<LuxString>  sFleximagePNGChannels      (IDD_FLEXIMAGE_PNG_CHANNELS,       "write_png_channels");
  static Descr2Param<LuxBool>    sFleximagePNG16Bit         (IDD_FLEXIMAGE_PNG_16BIT,          "write_png_16bit");
  static Descr2Param<LuxBool>    sFleximagePNGGamutClamp    (IDD_FLEXIMAGE_PNG_GAMUT_CLAMP,    "write_png_gamutclamp");
  static Descr2Param<LuxString>  sFleximageTGAChannels      (IDD_FLEXIMAGE_TGA_CHANNELS,       "write_tga_channels");
  static Descr2Param<LuxBool>    sFleximageTGAGamutClamp    (IDD_FLEXIMAGE_TGA_GAMUT_CLAMP,    "write_tga_gamutclamp");
  

  // set default sampler
  name = sFilmNames[IDD_FILM_FLEXIMAGE];

  // get base container of this node
  BaseContainer* data = getData();
  if (!data)  return;

  // get pixel filter ...
  LONG film = data->GetLong(IDD_FILM);

  // ... and fetch its settings
  switch (film) {
    // film fleximage
    case IDD_FILM_FLEXIMAGE:
      copyParam(sFleximageHaltSPP,           paramSet);
      copyParam(sFleximageGamma,             paramSet);
      copyParam(sFleximagePremultiply,       paramSet);
      copyParam(sFleximageDisplayInterval,   paramSet);
      copyParam(sFleximageWriteInterval,     paramSet);
      copyParam(sFleximageClampMethod,       paramSet, sClampMethods,   IDD_CLAMP_METHOD_NUMBER);
      copyParam(sFleximageRejectWarmup,      paramSet);
      copyParam(sFleximageTonemapKernel,     paramSet, sTonemapKernels, IDD_TONEMAP_KERNEL_NUMBER);
      switch (data->GetLong(IDD_FLEXIMAGE_TONEMAP_KERNEL)) {
        case IDD_TONEMAP_KERNEL_REINHARD:
          copyParam(sFleximageReinhardPrescale,  paramSet);
          copyParam(sFleximageReinhardPostscale, paramSet);
          copyParam(sFleximageReinhardBurn,      paramSet);
          break;
        case IDD_TONEMAP_KERNEL_LINEAR:
          copyParam(sFleximageLinearSensitivity, paramSet);
          copyParam(sFleximageLinearExposure,    paramSet, 0.001);
          copyParam(sFleximageLinearFStop,       paramSet);
          copyParam(sFleximageLinearGamma,       paramSet);
          break;
        case IDD_TONEMAP_KERNEL_CONTRAST:
          copyParam(sFleximageContrastYwa,       paramSet);
          break;
      }
      copyParam(sFleximageWriteEXR,          paramSet);
      copyParam(sFleximageWritePNG,          paramSet);
      copyParam(sFleximageWriteTGA,          paramSet);
      if (data->GetBool(IDD_FLEXIMAGE_WRITE_EXR)) {
        copyParam(sFleximageEXRChannels,       paramSet, sImageChannels,    IDD_WRITE_CHANNELS_NUMBER);
        copyParam(sFleximageEXRHalftype,       paramSet);
        copyParam(sFleximageEXRCompression,    paramSet, sCompressionTypes, IDD_EXR_COMPRESSION_NUMBER);
        copyParam(sFleximageEXRApplyImaging,   paramSet);
        copyParam(sFleximageEXRGamutClamp,     paramSet);
        copyParam(sFleximageEXRWriteZBuf,      paramSet);
        copyParam(sFleximageEXRZBufNormType,   paramSet, sZBufNormTypes,    IDD_ZBUF_NORM_TYPE_NUMBER);
      }
      if (data->GetBool(IDD_FLEXIMAGE_WRITE_PNG)) {
        copyParam(sFleximagePNGChannels,       paramSet, sImageChannels,    IDD_WRITE_CHANNELS_NUMBER);
        copyParam(sFleximagePNG16Bit,          paramSet);
        copyParam(sFleximagePNGGamutClamp,     paramSet);
      }
      if (data->GetBool(IDD_FLEXIMAGE_WRITE_TGA)) {
        copyParam(sFleximageTGAChannels,       paramSet, sImageChannels,    IDD_WRITE_CHANNELS_NUMBER);
        copyParam(sFleximageTGAGamutClamp,     paramSet);
      }
      break;
    // invalid film -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetFilm(): invalid film found -> using default settings");
  }

  // store pixel filter name
  name = sFilmNames[film];
}


///
LONG LuxC4DSettings::getOutputFilePathSettings(Filename& userDefined)
{
  // get base container
  BaseContainer* data = getData();
  if (!data) { return IDD_FLEXIMAGE_FILENAME_AS_SCENE_FILE; }

  // read user defined path and return filename method
  userDefined = data->GetFilename(IDD_FLEXIMAGE_OUTPUT_FILENAME);
  return data->GetLong(IDD_FLEXIMAGE_FILENAME_TYPE);
}


/// Obtains the pixel filter settings from the settings object.
///
/// @param[out]  name
///   Will receive the film name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::getPixelFilter(const char*& name,
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
  static Descr2Param<LuxFloat> sBoxWidth (IDD_BOX_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloat> sBoxHeight(IDD_BOX_FILTER_HEIGHT, "ywidth");

  // parameters for Gaussian filter
  static Descr2Param<LuxFloat> sGaussianWidth (IDD_GAUSSIAN_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloat> sGaussianHeight(IDD_GAUSSIAN_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloat> sGaussianAlpha (IDD_GAUSSIAN_FILTER_ALPHA,  "alpha");

  // parameters for Mitchell filter
  static Descr2Param<LuxFloat> sMitchellWidth (IDD_MITCHELL_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloat> sMitchellHeight(IDD_MITCHELL_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloat> sMitchellB     (IDD_MITCHELL_FILTER_B,      "B");
  static Descr2Param<LuxFloat> sMitchellC     (IDD_MITCHELL_FILTER_C,      "C");

  // parameters for sinc filter
  static Descr2Param<LuxFloat> sSincWidth (IDD_SINC_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloat> sSincHeight(IDD_SINC_FILTER_HEIGHT, "ywidth");
  static Descr2Param<LuxFloat> sSincTau   (IDD_SINC_FILTER_TAU,    "tau");

  // parameters for triangle filter
  static Descr2Param<LuxFloat> sTriangleWidth (IDD_TRIANGLE_FILTER_WIDTH,  "xwidth");
  static Descr2Param<LuxFloat> sTriangleHeight(IDD_TRIANGLE_FILTER_HEIGHT, "ywidth");


  // set default sampler
  name = sPixelFilterNames[IDD_PIXEL_FILTER_MITCHELL];

  // get base container of this node
  BaseContainer* data = getData();
  if (!data)  return;

  // get pixel filter ...
  LONG pixelFilter = data->GetLong(IDD_PIXEL_FILTER);

  // ... and fetch its settings
  switch (pixelFilter) {
    // box filter 
    case IDD_PIXEL_FILTER_BOX:
      copyParam(sBoxWidth,  paramSet);
      copyParam(sBoxHeight, paramSet);
      break;
    // Gaussian filter
    case IDD_PIXEL_FILTER_GAUSSIAN:
      copyParam(sGaussianWidth,  paramSet);
      copyParam(sGaussianHeight, paramSet);
      copyParam(sGaussianAlpha,  paramSet);
      break;
    // Mitchell filter
    case IDD_PIXEL_FILTER_MITCHELL:
      copyParam(sMitchellWidth,  paramSet);
      copyParam(sMitchellHeight, paramSet);
      copyParam(sMitchellB,      paramSet);
      copyParam(sMitchellC,      paramSet);
      break;
    // sinc filter
    case IDD_PIXEL_FILTER_SINC:
      copyParam(sSincWidth,  paramSet);
      copyParam(sSincHeight, paramSet);
      copyParam(sSincTau,    paramSet);
      break;
    // triangle filter
    case IDD_PIXEL_FILTER_TRIANGLE:
      copyParam(sTriangleWidth,  paramSet);
      copyParam(sTriangleHeight, paramSet);
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
void LuxC4DSettings::getSampler(const char*& name,
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
  static Descr2Param<LuxString>  sLowdiscrepancyPixelSampler(IDD_LOWDISCREPANCY_PIXELSAMPLER, "pixelsampler");
  static Descr2Param<LuxInteger> sLowdiscrepancyPixelSamples(IDD_LOWDISCREPANCY_PIXELSAMPLES, "pixelsamples");

  // parameters for random sampler
  static Descr2Param<LuxString>  sRandomPixelSampler(IDD_RANDOM_PIXELSAMPLER, "pixelsampler");
  static Descr2Param<LuxInteger> sRandomPixelSamples(IDD_RANDOM_PIXELSAMPLES, "pixelsamples");

  // parameters for metropolis sampler
  static Descr2Param<LuxFloat>   sMetroLargeMutationProb(IDD_METROPOLIS_LARGE_MUTATION_PROB, "largemutationprob");
  static Descr2Param<LuxInteger> sMetroMaxConsecRejects (IDD_METROPOLIS_MAX_CONSEC_REJECTS,  "maxconsecrejects");
  static Descr2Param<LuxFloat>   sMetroMicroMutationProb(IDD_METROPOLIS_MICRO_MUTATION_PROB, "micromutationprob");
  static Descr2Param<LuxBool>    sMetroUseVariance      (IDD_METROPOLIS_USE_VARIANCE,        "usevariance");

  // parameters for ERPT sampler
  static Descr2Param<LuxInteger> sERPTChainLength      (IDD_ERPT_CHAINLENGTH,         "chainlength");
  static Descr2Param<LuxFloat>   sERPTMicroMutationProb(IDD_ERPT_MICRO_MUTATION_PROB, "micromutationprob");
  static Descr2Param<LuxString>  sERPTPixelSampler     (IDD_ERPT_PIXELSAMPLER,        "pixelsampler");
  static Descr2Param<LuxInteger> sERPTPixelSamples     (IDD_ERPT_PIXELSAMPLES,        "pixelsamples");


  // set default sampler
  name = sSamplerNames[IDD_SAMPLER_LOWDISCREPANCY];

  // get base container of this node
  BaseContainer* data = getData();
  if (!data)  return;

  // get sampler ...
  LONG sampler = data->GetLong(IDD_SAMPLER);

  // ... and fetch its settings
  switch (sampler) {
    // low discrepancy sampler
    case IDD_SAMPLER_LOWDISCREPANCY:
      copyParam(sLowdiscrepancyPixelSampler, paramSet,
                sPixelSamplerNames, IDD_PIXELSAMPLER_NUMBER);
      copyParam(sLowdiscrepancyPixelSamples, paramSet);
      break;
    // random sampler
    case IDD_SAMPLER_RANDOM:
      copyParam(sRandomPixelSampler, paramSet,
                sPixelSamplerNames, IDD_PIXELSAMPLER_NUMBER);
      copyParam(sRandomPixelSamples, paramSet);
      break;
    // metropolis sampler
    case IDD_SAMPLER_METROPOLIS:
      copyParam(sMetroLargeMutationProb, paramSet);
      if (data->GetBool(IDD_ADVANCED_SAMPLER)) {
        copyParam(sMetroMaxConsecRejects,  paramSet);
        copyParam(sMetroMicroMutationProb, paramSet);
        copyParam(sMetroUseVariance,       paramSet);
      }
      break;
    // ERPT sampler
    case IDD_SAMPLER_ERPT:
      copyParam(sERPTChainLength,       paramSet);
      copyParam(sERPTMicroMutationProb, paramSet);
      copyParam(sERPTPixelSampler,      paramSet,
                sPixelSamplerNames,     IDD_PIXELSAMPLER_NUMBER);
      copyParam(sERPTPixelSamples,      paramSet);
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
void LuxC4DSettings::getSurfaceIntegrator(const char*& name,
                                          LuxParamSet& paramSet,
                                          Bool&        isBidirectional)
{
  // the different sampler names
  static const char* sIntegratorNames[IDD_INTEGRATOR_NUMBER] = {
      "path",
      "distributedpath",
      "bidirectional",
      "directlighting"
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
  static Descr2Param<LuxInteger> sPathMaxDepth           (IDD_PATH_MAX_DEPTH,             "maxdepth");
  static Descr2Param<LuxString>  sPathDirectLightStrategy(IDD_PATH_DIRECT_LIGHT_STRATEGY, "strategy");
  static Descr2Param<LuxString>  sPathRRStrategy         (IDD_PATH_RR_STRATEGY,           "rrstrategy");
  static Descr2Param<LuxFloat>   sPathRRContinueProb     (IDD_PATH_RR_CONTINUE_PROB,      "rrcontinueprob");
  static Descr2Param<LuxBool>    sPathIncludeEnvironment (IDD_PATH_INCLUDE_ENVIRONMENT,   "includeenvironment");

  // parameters for distributed path integrator
  static Descr2Param<LuxString>  sDistriPathDirectLightStrategy       (IDD_DISTRIBUTED_PATH_DIRECT_LIGHT_STRATEGY,         "strategy");
  static Descr2Param<LuxInteger> sDistriPathDirectDirectLightSamples  (IDD_DISTRIBUTED_PATH_DIRECT_DIRECT_LIGHT_SAMPLES,   "directsamples");
  static Descr2Param<LuxInteger> sDistriPathInDirectDirectLightSamples(IDD_DISTRIBUTED_PATH_INDIRECT_DIRECT_LIGHT_SAMPLES, "indirectsamples");
  static Descr2Param<LuxInteger> sDistriPathDiffuseReflectDepth       (IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_DEPTH,         "diffusereflectdepth");
  static Descr2Param<LuxInteger> sDistriPathDiffuseReflectSamples     (IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_SAMPLES,       "diffusereflectsamples");
  static Descr2Param<LuxInteger> sDistriPathDiffuseRefractDepth       (IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_DEPTH,         "diffuserefractdepth");
  static Descr2Param<LuxInteger> sDistriPathDiffuseRefractSamples     (IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_SAMPLES,       "diffuserefractsamples");
  static Descr2Param<LuxInteger> sDistriPathGlossyReflectDepth        (IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_DEPTH,          "glossyreflectdepth");
  static Descr2Param<LuxInteger> sDistriPathGlossyReflectSamples      (IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_SAMPLES,        "glossyreflectsamples");
  static Descr2Param<LuxInteger> sDistriPathGlossyRefractDepth        (IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_DEPTH,          "glossyrefractdepth");
  static Descr2Param<LuxInteger> sDistriPathGlossyRefractSamples      (IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_SAMPLES,        "glossyrefractsamples");
  static Descr2Param<LuxInteger> sDistriPathSpecularReflectDepth      (IDD_DISTRIBUTED_PATH_SPECULAR_REFLECT_DEPTH,        "specularreflectdepth");
  static Descr2Param<LuxInteger> sDistriPathSpecularRefractDepth      (IDD_DISTRIBUTED_PATH_SPECULAR_REFRACT_DEPTH,        "specularrefractdepth");

  // parameters for bidirectional integrator
  static Descr2Param<LuxInteger> sBidirectionalEyeDepth           (IDD_BIDIRECTIONAL_EYE_DEPTH,             "eyedepth");
  static Descr2Param<LuxInteger> sBidirectionalLightDepth         (IDD_BIDIRECTIONAL_LIGHT_DEPTH,           "lightdepth");
  static Descr2Param<LuxString>  sBidirectionalDirectLightStrategy(IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY, "strategy");
  static Descr2Param<LuxFloat>   sBidirectionalEyeRRThreshold     (IDD_BIDIRECTIONAL_EYE_RR_THRESHOLD,      "eyerrthreshold");
  static Descr2Param<LuxFloat>   sBidirectionalLightRRThreshold   (IDD_BIDIRECTIONAL_LIGHT_RR_THRESHOLD,    "lightrrthreshold");

  // parameters for direct lighting integrator
  static Descr2Param<LuxInteger> sDirectLightingMaxDepth(IDD_DIRECT_LIGHTING_MAX_DEPTH, "maxdepth");
  static Descr2Param<LuxString>  sDirectLightingStrategy(IDD_DIRECT_LIGHTING_STRATEGY,  "strategy");


  // set default integrator
  name = sIntegratorNames[IDD_INTEGRATOR_PATH];
  isBidirectional = FALSE;

  // get base container of this node
  BaseContainer* data = getData();
  if (!data)  return;

  // get integrator ...
  LONG integrator = data->GetLong(IDD_INTEGRATOR);

  // ... and fetch its data
  switch (integrator) {
    // path integrator
    case IDD_INTEGRATOR_PATH:
      copyParam(sPathMaxDepth,            paramSet);
      copyParam(sPathIncludeEnvironment,  paramSet);
      if (data->GetBool(IDD_ADVANCED_INTEGRATOR)) {
        copyParam(sPathDirectLightStrategy, paramSet,
                  sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
        copyParam(sPathRRStrategy, paramSet,
                  sRRStrategies, IDD_PATH_RR_STRATEGY_NUMBER);
        if (data->GetLong(IDD_PATH_RR_STRATEGY) == IDD_PATH_RR_STRATEGY_PROBABILITY) {
          copyParam(sPathRRContinueProb,    paramSet);
        }
      }
      break;
    // distributed path integrator
    case IDD_INTEGRATOR_DISTRIBUTED_PATH:
      copyParam(sDistriPathDirectLightStrategy, paramSet,
                sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
      copyParam(sDistriPathDirectDirectLightSamples,   paramSet);
      copyParam(sDistriPathInDirectDirectLightSamples, paramSet);
      copyParam(sDistriPathDiffuseReflectDepth,        paramSet);
      copyParam(sDistriPathDiffuseReflectSamples,      paramSet);
      copyParam(sDistriPathDiffuseRefractDepth,        paramSet);
      copyParam(sDistriPathDiffuseRefractSamples,      paramSet);
      copyParam(sDistriPathGlossyReflectDepth,         paramSet);
      copyParam(sDistriPathGlossyReflectSamples,       paramSet);
      copyParam(sDistriPathGlossyRefractDepth,         paramSet);
      copyParam(sDistriPathGlossyRefractSamples,       paramSet);
      copyParam(sDistriPathSpecularReflectDepth,       paramSet);
      copyParam(sDistriPathSpecularRefractDepth,       paramSet);
      break;
    // bidirectional integrator
    case IDD_INTEGRATOR_BIDIRECTIONAL:
      copyParam(sBidirectionalEyeDepth,            paramSet);
      copyParam(sBidirectionalLightDepth,          paramSet);
      if (data->GetBool(IDD_ADVANCED_INTEGRATOR)) {
        copyParam(sBidirectionalDirectLightStrategy, paramSet,
                  sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
        copyParam(sBidirectionalEyeRRThreshold,      paramSet);
        copyParam(sBidirectionalLightRRThreshold,    paramSet);
      }
      isBidirectional = TRUE;
      break;
    // direct lighting integrator
    case IDD_INTEGRATOR_DIRECT_LIGHTING:
      copyParam(sDirectLightingMaxDepth, paramSet);
      if (data->GetBool(IDD_ADVANCED_INTEGRATOR)) {
        copyParam(sDirectLightingStrategy, paramSet,
                  sDirectLightStrategies, IDD_DIRECT_LIGHT_STRATEGY_NUMBER);
      }
      break;
    // invalid integrator -> error and return
    default:
      ERRLOG_RETURN("LuxC4DSettings::GetSurfaceIntegrator(): invalid integrator found -> using default settings");
  }

  // store integrator name
  name = sIntegratorNames[integrator];
}


/// Obtains the accelerator settings from the settings object.
///
/// @param[out]  name
///   Will receive the accelerator name.
/// @param[out]  paramSet
///   The set to which the parameters get added.
void LuxC4DSettings::getAccelerator(const char*& name,
                                    LuxParamSet& paramSet)
{
  // the different accelerator names
  static const char* sAcceleratorNames[IDD_ACCELERATION_TYPE_NUMBER] = {
      "kdtree",
      "bvh",
      "qbvh"
    };

  // parameters for kd-tree
  static Descr2Param<LuxInteger> sKdTreeIntersectionCost(IDD_KDTREE_INTERSECTION_COST, "intersectcost");
  static Descr2Param<LuxInteger> sKdTreeTraversalCost   (IDD_KDTREE_TRAVERSAL_COST,    "traversalcost");
  static Descr2Param<LuxFloat>   sKdTreeEmptyBonus      (IDD_KDTREE_EMPTY_BONUS,       "emptybonus");
  static Descr2Param<LuxInteger> sKdTreeMaxPrimitives   (IDD_KDTREE_MAX_PRIMITIVES,    "maxprims");
  static Descr2Param<LuxInteger> sKdTreeMaxDepth        (IDD_KDTREE_MAX_DEPTH,         "maxdepth");

  // parameters for bvh tree
  static Descr2Param<LuxInteger> sBVHIntersectionCost(IDD_BVH_TREE_TYPE,         "treetype");
  static Descr2Param<LuxInteger> sBVHTraversalCost   (IDD_BVH_COST_SAMPLES,      "costsamples");
  static Descr2Param<LuxInteger> sBVHMaxPrimitives   (IDD_BVH_INTERSECTION_COST, "intersectcost");
  static Descr2Param<LuxInteger> sBVHMaxDepth        (IDD_BVH_TRAVERSAL_COST,    "traversalcost");
  static Descr2Param<LuxFloat>   sBVHEmptyBonus      (IDD_BVH_EMPTY_BONUS,       "emptybonus");

  // parameters for qbvh tree
  static Descr2Param<LuxInteger> sQBVHTraversalCost(IDD_QBVH_MAX_PRIMITIVES,       "maxprimsperleaf");
  static Descr2Param<LuxFloat>   sQBVHEmptyBonus   (IDD_QBVH_FULL_SWEEP_THRESHOLD, "fullsweepthreshold");
  static Descr2Param<LuxInteger> sQBVHMaxPrimitives(IDD_QBVH_SKIP_FACTOR,          "skipfactor");


  // set default accelerator
  name = sAcceleratorNames[IDD_ACCELERATION_KDTREE];

  // get base container of this node
  BaseContainer* data = getData();
  if (!data) { return; }

  // get accelerator...
  LONG accelerator = data->GetLong(IDD_ACCELERATION_TYPE);

  // ... and fetch its data, if we have the advanced option enabled
  if (data->GetBool(IDD_ACCELERATION_ADVANCED)) {
    switch (accelerator) {
      // kd-tree
      case IDD_INTEGRATOR_PATH:
        copyParam(sKdTreeIntersectionCost, paramSet);
        copyParam(sKdTreeTraversalCost,    paramSet);
        copyParam(sKdTreeEmptyBonus,       paramSet);
        copyParam(sKdTreeMaxPrimitives,    paramSet);
        copyParam(sKdTreeMaxDepth,         paramSet);
        break;
      // bvh tree
      case IDD_ACCELERATION_BVH:
        copyParam(sBVHIntersectionCost, paramSet);
        copyParam(sBVHTraversalCost,    paramSet);
        copyParam(sBVHMaxPrimitives,    paramSet);
        copyParam(sBVHMaxDepth,         paramSet);
        copyParam(sBVHEmptyBonus,       paramSet);
        break;
      // qbvh tree
      case IDD_ACCELERATION_QBVH:
        copyParam(sQBVHTraversalCost, paramSet);
        copyParam(sQBVHEmptyBonus,    paramSet);
        copyParam(sQBVHMaxPrimitives, paramSet);
        break;
      // invalid accelerator-> error and return
      default:
        ERRLOG_RETURN("LuxC4DSettings::getAccelerator(): invalid accelerator type found -> using default settings");
    }
  }

  // store accelerator name
  name = sAcceleratorNames[accelerator];
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
void LuxC4DSettings::getExportFilename(BaseDocument& document,
                                       Filename&     path,
                                       Bool&         overwritingAllowed)
{
  // reset return values
  path               = Filename();
  overwritingAllowed = FALSE;

  // get base container and determine export filename method that was chosen
  BaseContainer* data = getData();
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
Real LuxC4DSettings::getC4D2LuxScale(void)
{
  // get base container and return the scale factor from it
  BaseContainer* data = getData();
  if (!data) { return 0.01; }
  return data->GetReal(IDD_SCALE_FACTOR) * 0.01;
}

/// Returns the bump sample distance, that should be used for bump mapping or
/// 0.0 if the bump sample distance shouldn't be exported.
Real LuxC4DSettings::getBumpSampleDistance(void)
{
  // get base container and return the bump sample distance from it
  BaseContainer* data = getData();
  if (!data || !data->GetBool(IDD_EXPORT_BUMP_SAMPLE_DISTANCE)) {
    return 0.0;
  }
  return data->GetReal(IDD_BUMP_SAMPLE_DISTANCE);
}


/// Returns the gamma correction that should be used for bitmap textures.
Real LuxC4DSettings::getTextureGamma(void)
{
  // get base container and return the texture gamma from it
  BaseContainer* data = getData();
  if (!data) { return 1.0; }
  return data->GetReal(IDD_TEXTURE_GAMMA_CORRECTION, 1.0);
}


/// Returns the gamma correction that should be used for colours.
Real LuxC4DSettings::getColorGamma(void)
{
  // get base container and return the texture gamma from it
  BaseContainer* data = getData();
  if (!data || !data->GetBool(IDD_DO_COLOUR_GAMMA_CORRECTION)) {
    return 1.0;
  }
  return data->GetReal(IDD_TEXTURE_GAMMA_CORRECTION, 1.0);
}


/// Returns TRUE if file paths (e.g. for textures) should be stored relative to
/// the scene file.
Bool LuxC4DSettings::useRelativePaths(void)
{
  // get base container and return setting of relative path option
  BaseContainer* data = getData();
  if (!data) { return TRUE; }
  return data->GetBool(IDD_USE_RELATIVE_PATHS, TRUE);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DSettings.
 *****************************************************************************/


/// Returns the data container of this node.
BaseContainer* LuxC4DSettings::getData(void)
{
  BaseContainer* data = 0;
  BaseList2D* listNode = static_cast<BaseList2D*>(Get());
  if (listNode) {
    data = listNode->GetDataInstance();
  }
  if (!data)  ERRLOG_RETURN_VALUE(NULL, "LuxC4DSettings::getData(): could not obtain base container");
  return data;
}


/// Copies a bool parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::copyParam(Descr2Param<LuxBool>& descr2Param,
                               LuxParamSet&          paramSet)
{
  // get base container of this object
  BaseContainer* data = getData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = (data->GetBool(descr2Param.mID) != 0);
  paramSet.addParam(LUX_BOOL, descr2Param.mParamName, &descr2Param.mParam);
}


/// Copies an integer parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::copyParam(Descr2Param<LuxInteger>& descr2Param,
                               LuxParamSet&             paramSet)
{
  // get base container of this object
  BaseContainer* data = getData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = data->GetLong(descr2Param.mID);
  paramSet.addParam(LUX_INTEGER, descr2Param.mParamName, &descr2Param.mParam);
}


/// Copies a float parameter into the Lux parameter set.
///
/// @param[in]  descr2Param
///   Structure that contains the parameter ID, name and its value buffer.
/// @param[in]  paramSet
///   The set where the parameter gets added to.
void LuxC4DSettings::copyParam(Descr2Param<LuxFloat>& descr2Param,
                               LuxParamSet&           paramSet,
                               LuxFloat               scaleFactor)
{
  // get base container of this object
  BaseContainer* data = getData();
  if (!data)  return;

  // get float and add it to parameter set
  descr2Param.mParam = data->GetReal(descr2Param.mID) * scaleFactor;
  paramSet.addParam(LUX_FLOAT, descr2Param.mParamName, &descr2Param.mParam);
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
void LuxC4DSettings::copyParam(Descr2Param<LuxString>& descr2Param,
                               LuxParamSet&            paramSet,
                               const char**            cycleEntries,
                               LONG                    cycleEntryCount)
{
  // get base container of this object
  BaseContainer* data = getData();
  if (!data)  return;

  // get and check entry from cycle
  LONG entry = data->GetLong(descr2Param.mID);
  if ((entry<0) || (entry>=cycleEntryCount))
    ERRLOG_RETURN("LuxC4DSettings::CopyCycleParam(): invalid cycle entry found -> using default settings");

  // map entry to string and add that to parameter set
  descr2Param.mParam = cycleEntries[entry];
  paramSet.addParam(LUX_STRING, descr2Param.mParamName, &descr2Param.mParam);
}
