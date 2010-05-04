//************************************************************************
//* LuxC4D - CINEMA 4D plug-in for export to LuxRender                   *
//* (http://www.luxrender.net)                                           *
//*                                                                      *
//* Author:                                                              *
//* Marcus Spranger (abstrax)                                            *
//*                                                                      *
//************************************************************************
//*                                                                      *
//* This file is part of LuxC4D.                                         *
//*                                                                      *
//* LuxC4D is free software: you can redistribute it and/or modify       *
//* it under the terms of the GNU General Public License as published by *
//* the Free Software Foundation, either version 3 of the License, or    *
//* (at your option) any later version.                                  *
//*                                                                      *
//* LuxC4D is distributed in the hope that it will be useful,            *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
//* GNU General Public License for more details.                         *
//*                                                                      *
//* You should have received a copy of the GNU General Public License    *
//* along with LuxC4D.  If not, see <http://www.gnu.org/licenses/>.      *
//************************************************************************

#ifndef __VPLUXC4DSETTINGS_H__
#define __VPLUXC4DSETTINGS_H__ 1



enum
{
  // ----------------------------------
  // RENDER TAB
  // ----------------------------------
  IDG_RENDER = 10000,
  

    // ----------------------------------
    // SAMPLING GROUP
    IDG_SAMPLING = 10100,
    IDD_SAMPLER,
    IDD_ADVANCED_SAMPLER,

      // samplers
      IDD_SAMPLER_LOWDISCREPANCY = 0,
      IDD_SAMPLER_RANDOM,
      IDD_SAMPLER_METROPOLIS,
      IDD_SAMPLER_ERPT,
      IDD_SAMPLER_NUMBER,

    //  low discrepancy sampler parameters
    IDG_LOWDISCREPANCY = 10200,
    IDD_LOWDISCREPANCY_PIXELSAMPLER,
    IDD_LOWDISCREPANCY_PIXELSAMPLES,

    // random sampler parameters
    IDG_RANDOM = 10300,
    IDD_RANDOM_PIXELSAMPLER,
    __IDD_RANDOM_SAMPLES_X,             // deprecated
    __IDD_RANDOM_SAMPLES_Y,             // deprecated
    IDD_RANDOM_PIXELSAMPLES,
  
      // pixel samplers
      IDD_PIXELSAMPLER_LOWDISCREPANCY = 0,
      IDD_PIXELSAMPLER_HILBERT,
      IDD_PIXELSAMPLER_VEGAS,
      __IDD_PIXELSAMPLER_RANDOM,        // deprecated
      IDD_PIXELSAMPLER_LINEAR,
      IDD_PIXELSAMPLER_TILE,
      IDD_PIXELSAMPLER_NUMBER,

    // metropolis sampler parameters
    IDG_METROPOLIS = 10400,
    IDD_METROPOLIS_LARGE_MUTATION_PROB,
    IDD_METROPOLIS_MAX_CONSEC_REJECTS,
    __IDD_METROPOLIS_MICRO_MUTATION_PROB, // deprecated
    __IDD_METROPOLIS_MUTATION_RANGE,      // deprecated
    __IDD_METROPOLIS_INIT_SAMPLES,        // deprecated
    __IDD_METROPOLIS_STRATA_WIDTH,        // deprecated
    IDD_METROPOLIS_USE_VARIANCE,
    __IDD_METROPOLIS_USE_QR,              // deprecated
    IDD_METROPOLIS_STRENGTH,
    IDD_METROPOLIS_MUTATION_RANGE_TYPE,
    IDD_METROPOLIS_MUTATION_RANGE_FRACTION,
    IDD_METROPOLIS_MUTATION_RANGE_PIXEL,

      // mutation range types
      IDD_METROPOLIS_MUTATION_RANGE_AS_FRACTION = 0,
      IDD_METROPOLIS_MUTATION_RANGE_IN_PIXEL,

    // ERPT sampler parameters
    IDG_ERPT = 10500,
    __IDD_ERPT_INIT_SAMPLES,            // deprecated
    IDD_ERPT_CHAINLENGTH,
    __IDD_ERPT_MICRO_MUTATION_PROB,     // deprecated
    __IDD_ERPT_MUTATION_RANGE,          // deprecated
    IDD_ERPT_PIXELSAMPLER,
    IDD_ERPT_PIXELSAMPLES,
    IDD_ERPT_MUTATION_RANGE_TYPE,
    IDD_ERPT_MUTATION_RANGE_FRACTION,
    IDD_ERPT_MUTATION_RANGE_PIXEL,

      // mutation range types
      IDD_ERPT_MUTATION_RANGE_AS_FRACTION = 0,
      IDD_ERPT_MUTATION_RANGE_IN_PIXEL,


    // ----------------------------------
    // INTEGRATION GROUP
    IDG_INTEGRATION = 12000,
    IDD_INTEGRATOR,
    IDD_ADVANCED_INTEGRATOR,

      // integrators
      IDD_INTEGRATOR_PATH = 0,
      IDD_INTEGRATOR_DISTRIBUTED_PATH,
      IDD_INTEGRATOR_BIDIRECTIONAL,
      IDD_INTEGRATOR_DIRECT_LIGHTING,
      IDD_INTEGRATOR_NUMBER,

    // path integrator parameters
    IDG_PATH = 12100,
    IDD_PATH_MAX_DEPTH,
    IDD_PATH_DIRECT_LIGHT_STRATEGY,
    IDD_PATH_RR_STRATEGY,
    IDD_PATH_RR_CONTINUE_PROB,
    IDD_PATH_INCLUDE_ENVIRONMENT,

      // direct lighting strategies
      IDD_DIRECT_LIGHT_STRATEGY_AUTO = 0,
      IDD_DIRECT_LIGHT_STRATEGY_ONE,
      IDD_DIRECT_LIGHT_STRATEGY_ALL,
      IDD_DIRECT_LIGHT_STRATEGY_NUMBER,

      // Russian roulette strategies
      IDD_PATH_RR_STRATEGY_EFFICIENCY = 0,
      IDD_PATH_RR_STRATEGY_PROBABILITY,
      IDD_PATH_RR_STRATEGY_NONE,
      IDD_PATH_RR_STRATEGY_NUMBER,

    // distributed path integrator parameters
    IDG_DISTRIBUTED_PATH = 12200,
    IDD_DISTRIBUTED_PATH_DIRECT_LIGHT_STRATEGY,
    IDD_DISTRIBUTED_PATH_DIRECT_DIRECT_LIGHT_SAMPLES,
    IDD_DISTRIBUTED_PATH_INDIRECT_DIRECT_LIGHT_SAMPLES,
    IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_DEPTH,
    IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_SAMPLES,
    IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_DEPTH,
    IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_SAMPLES,
    IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_DEPTH,
    IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_SAMPLES,
    IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_DEPTH,
    IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_SAMPLES,
    IDD_DISTRIBUTED_PATH_SPECULAR_REFLECT_DEPTH,
    IDD_DISTRIBUTED_PATH_SPECULAR_REFRACT_DEPTH,

    // bidirectional integrator parameters
    IDG_BIDIRECTIONAL = 12300,
    IDD_BIDIRECTIONAL_EYE_DEPTH,
    IDD_BIDIRECTIONAL_LIGHT_DEPTH,
    IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY,
    IDD_BIDIRECTIONAL_EYE_RR_THRESHOLD,
    IDD_BIDIRECTIONAL_LIGHT_RR_THRESHOLD,
    IDD_BIDIRECTIONAL_MAX_DEPTH,

    // direct lighting integrator parameters
    IDG_DIRECT_LIGHTING = 12400,
    IDD_DIRECT_LIGHTING_MAX_DEPTH,
    IDD_DIRECT_LIGHTING_STRATEGY,


    // ----------------------------------
    // FILTERING GROUP
    IDG_FILTERING = 14000,
    IDD_PIXEL_FILTER,

      // pixel filters
      IDD_PIXEL_FILTER_BOX = 0,
      IDD_PIXEL_FILTER_GAUSSIAN,
      IDD_PIXEL_FILTER_MITCHELL,
      IDD_PIXEL_FILTER_SINC,
      IDD_PIXEL_FILTER_TRIANGLE,
      IDD_PIXEL_FILTER_NUMBER,

    // box filter parameters
    IDG_BOX_FILTER = 14100,
    IDD_BOX_FILTER_WIDTH,
    IDD_BOX_FILTER_HEIGHT,

    // gaussian filter parameters
    IDG_GAUSSIAN_FILTER = 14200,
    IDD_GAUSSIAN_FILTER_WIDTH,
    IDD_GAUSSIAN_FILTER_HEIGHT,
    IDD_GAUSSIAN_FILTER_ALPHA,

    // mitchell filter parameters
    IDG_MITCHELL_FILTER = 14300,
    IDD_MITCHELL_FILTER_WIDTH,
    IDD_MITCHELL_FILTER_HEIGHT,
    IDD_MITCHELL_FILTER_B,
    IDD_MITCHELL_FILTER_C,
    IDD_MITCHELL_SUPERSAMPLE,

    // sinc filter parameters
    IDG_SINC_FILTER = 14400,
    IDD_SINC_FILTER_WIDTH,
    IDD_SINC_FILTER_HEIGHT,
    IDD_SINC_FILTER_TAU,

    // triangle filter parameters
    IDG_TRIANGLE_FILTER = 14500,
    IDD_TRIANGLE_FILTER_WIDTH,
    IDD_TRIANGLE_FILTER_HEIGHT,


    // ----------------------------------
    // ACCELERATION GROUP
    IDG_ACCELERATION = 15000,
    IDD_ACCELERATION_TYPE,
    IDD_ACCELERATION_ADVANCED,

      // acceleration types
      IDD_ACCELERATION_KDTREE = 0,
      IDD_ACCELERATION_BVH,
      IDD_ACCELERATION_QBVH,
      IDD_ACCELERATION_TYPE_NUMBER,

    // tabreckdtree acceleration
    IDG_ACCELERATION_KDTREE = 15100,
    IDD_KDTREE_INTERSECTION_COST,
    IDD_KDTREE_TRAVERSAL_COST,
    IDD_KDTREE_EMPTY_BONUS,
    IDD_KDTREE_MAX_PRIMITIVES,
    IDD_KDTREE_MAX_DEPTH,

    // bvh acceleration
    IDG_ACCELERATION_BVH = 15200,
    IDD_BVH_TREE_TYPE,
    IDD_BVH_COST_SAMPLES,
    IDD_BVH_INTERSECTION_COST,
    IDD_BVH_TRAVERSAL_COST,
    IDD_BVH_EMPTY_BONUS,

      // bvh tree types
      IDD_BVH_BINARY_TREE = 2,
      IDD_BVH_QUAD_TREE   = 4,
      IDD_BVH_OCT_TREE    = 8,

    // qbvh acceleration
    IDG_ACCELERATION_QBVH = 15300,
    IDD_QBVH_MAX_PRIMITIVES,
    IDD_QBVH_FULL_SWEEP_THRESHOLD,
    IDD_QBVH_SKIP_FACTOR,



  // ----------------------------------
  // OUTPUT TAB
  // ----------------------------------
  IDG_OUTPUT = 20000,

  
    // ----------------------------------
    // FILM GROUP
    IDG_FILM = 20100,
    IDD_FILM,

      // films
      IDD_FILM_FLEXIMAGE = 0,
      IDD_FILM_NUMBER,

    // fleximage parameters
    IDG_FLEXIMAGE = 20200,
    IDD_FLEXIMAGE_HALT_SPP,
    IDD_FLEXIMAGE_GAMMA,
    __IDD_FLEXIMAGE_TONEMAP_SETTINGS,         // unused
    __IDD_FLEXIMAGE_REINHARD_PRESCALE,        // moved down
    __IDD_FLEXIMAGE_REINHARD_POSTSCALE,       // moved down
    __IDD_FLEXIMAGE_REINHARD_BURN,            // moved down
    __IDD_FLEXIMAGE_REINHARD_DUMMY,           // unused
    IDD_FLEXIMAGE_PREMULTIPLY,
    IDD_FLEXIMAGE_DISPLAY_INTERVAL,
    IDD_FLEXIMAGE_WRITE_INTERVAL,
    __IDD_FLEXIMAGE_WRITE_TONEMAPPED_TGA,     // unused
    __IDD_FLEXIMAGE_WRITE_TONEMAPPED_EXR,     // unused
    __IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_EXR,   // unused
    __IDD_FLEXIMAGE_WRITE_TONEMAPPED_IGI,     // unused
    __IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_IGI,   // unused
    IDD_FLEXIMAGE_CLAMP_METHOD,
    IDD_FLEXIMAGE_REJECT_WARMUP,
      
      // fleximage tone mapping group
      IDG_FLEXIMAGE_TONEMAPPING = 21000,
      IDD_FLEXIMAGE_TONEMAP_KERNEL,
      IDG_FLEXIMAGE_REINHARD = 21100,
      IDD_FLEXIMAGE_REINHARD_PRESCALE,
      IDD_FLEXIMAGE_REINHARD_POSTSCALE,
      IDD_FLEXIMAGE_REINHARD_BURN,
      IDG_FLEXIMAGE_LINEAR = 21200,
      IDD_FLEXIMAGE_LINEAR_SENSITIVITY,
      IDD_FLEXIMAGE_LINEAR_EXPOSURE,
      IDD_FLEXIMAGE_LINEAR_FSTOP,
      IDD_FLEXIMAGE_LINEAR_GAMMA,
      IDG_FLEXIMAGE_CONTRAST = 21300,
      IDD_FLEXIMAGE_CONTRAST_YWA,
      IDG_FLEXIMAGE_MAXWHITE = 21400,

      // fleximage file format group
      IDG_FLEXIMAGE_FILE_FORMATS = 22000,
      IDD_FLEXIMAGE_WRITE_EXR,
      IDD_FLEXIMAGE_WRITE_PNG,
      IDD_FLEXIMAGE_WRITE_TGA,
      IDG_FLEXIMAGE_EXR = 22100,
      IDD_FLEXIMAGE_EXR_CHANNELS,
      IDD_FLEXIMAGE_EXR_HALFTYPE,
      IDD_FLEXIMAGE_EXR_COMPRESSION,
      IDD_FLEXIMAGE_EXR_APPLY_IMAGING,
      IDD_FLEXIMAGE_EXR_GAMUT_CLAMP,
      IDD_FLEXIMAGE_EXR_WRITE_ZBUF,
      IDD_FLEXIMAGE_EXR_ZBUF_NORM_TYPE,
      IDG_FLEXIMAGE_PNG = 22200,
      IDD_FLEXIMAGE_PNG_CHANNELS,
      IDD_FLEXIMAGE_PNG_16BIT,
      IDD_FLEXIMAGE_PNG_GAMUT_CLAMP,
      IDG_FLEXIMAGE_TGA = 22300,
      IDD_FLEXIMAGE_TGA_CHANNELS,
      IDD_FLEXIMAGE_TGA_GAMUT_CLAMP,

        // clamp methods
        IDD_CLAMP_METHOD_LUM = 0,
        IDD_CLAMP_METHOD_HUE,
        IDD_CLAMP_METHOD_CUT,
        IDD_CLAMP_METHOD_NUMBER,

        // tonemap kernels
        IDD_TONEMAP_KERNEL_REINHARD = 0,
        IDD_TONEMAP_KERNEL_LINEAR,
        IDD_TONEMAP_KERNEL_CONTRAST,
        IDD_TONEMAP_KERNEL_MAXWHITE,
        IDD_TONEMAP_KERNEL_NUMBER,

        // image channels
        IDD_WRITE_CHANNELS_Y = 0,
        IDD_WRITE_CHANNELS_YA,
        IDD_WRITE_CHANNELS_RGB,
        IDD_WRITE_CHANNELS_RGBA,
        IDD_WRITE_CHANNELS_NUMBER,

        // Z-buffer normalistation types
        IDD_ZBUF_NORM_TYPE_NONE = 0,
        IDD_ZBUF_NORM_TYPE_CAMERA,
        IDD_ZBUF_NORM_TYPE_SCENE,
        IDD_ZBUF_NORM_TYPE_NUMBER,

        // exr compression types
        IDD_EXR_COMPRESSION_RLE = 0,
        IDD_EXR_COMPRESSION_PIZ,
        IDD_EXR_COMPRESSION_ZIP,
        IDD_EXR_COMPRESSION_PXR24,
        IDD_EXR_COMPRESSION_NONE,
        IDD_EXR_COMPRESSION_NUMBER,

      // the way how the filename is determined
      IDD_FLEXIMAGE_FILENAME_TYPE = 22020,
      IDD_FLEXIMAGE_OUTPUT_FILENAME,

        // the different methods to determine the output filename
        IDD_FLEXIMAGE_FILENAME_AS_SCENE_FILE = 0,
        IDD_FLEXIMAGE_FILENAME_AS_C4D_OUTPUT_FILE,
        IDD_FLEXIMAGE_FILENAME_DEFINE_HERE,



  // ----------------------------------
  // EXPORT TAB
  // ----------------------------------
  IDG_EXPORT = 30000,

    // ----------------------------------
    // EXPORT FILENAME GROUP
    IDG_EXPORT_FILENAME = 30100,
    IDD_WHICH_EXPORT_FILENAME,
    IDD_EXPORT_FILENAME,
    IDD_ALLOW_OVERWRITING,

      // the different ways of getting the export filename
      IDD_ASK_FOR_EXPORT_FILENAME = 0,
      IDD_USE_C4D_EXPORT_FILENAME,
      IDD_DEFINE_EXPORT_FILENAME,
      IDD_EXPORT_FILENAME_SELECTION_NUMBER,

    // ----------------------------------
    // SCALING GROUP
    IDG_SCALING = 30200,
    IDD_SCALE_FACTOR,
    IDD_EXPORT_BUMP_SAMPLE_DISTANCE,
    IDD_BUMP_SAMPLE_DISTANCE,
    IDD_TEXTURE_GAMMA_CORRECTION,
    IDD_USE_RELATIVE_PATHS,
    IDD_DO_COLOUR_GAMMA_CORRECTION
};



#endif  // #ifndef __VPLUXC4DSETTINGS_H__
  