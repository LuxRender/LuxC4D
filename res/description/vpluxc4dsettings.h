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
    IDD_LOWDISCREPANCY_SAMPLES,

    // random sampler parameters
    IDG_RANDOM = 10300,
    IDD_RANDOM_PIXELSAMPLER,
    IDD_RANDOM_SAMPLES_X,
    IDD_RANDOM_SAMPLES_Y,

      // pixel samplers
      IDD_PIXELSAMPLER_LOWDISCREPANCY = 0,
      IDD_PIXELSAMPLER_HILBERT,
      IDD_PIXELSAMPLER_VEGAS,
      IDD_PIXELSAMPLER_RANDOM,
      IDD_PIXELSAMPLER_LINEAR,
      IDD_PIXELSAMPLER_TILE,
      IDD_PIXELSAMPLER_NUMBER,

    // metropolis sampler parameters
    IDG_METROPOLIS = 10400,
    IDD_METROPOLIS_LARGE_MUTATION_PROB,
    IDD_METROPOLIS_MAX_CONSEC_REJECTS,
    IDD_METROPOLIS_MICRO_MUTATION_PROB,
    IDD_METROPOLIS_MUTATION_RANGE,
    IDD_METROPOLIS_INIT_SAMPLES,
    IDD_METROPOLIS_STRATA_WIDTH,
    IDD_METROPOLIS_USE_VARIANCE,
    IDD_METROPOLIS_USE_QR,

    // ERPT sampler parameters
    IDG_ERPT = 10500,
    IDD_ERPT_INIT_SAMPLES,
    IDD_ERPT_CHAINLENGTH,
    IDD_ERPT_MICRO_MUTATION_PROB,
    IDD_ERPT_MUTATION_RANGE,


    // ----------------------------------
    // INTEGRATION GROUP
    IDG_INTEGRATION = 12000,
    IDD_INTEGRATOR,

      // integrators
      IDD_INTEGRATOR_PATH = 0,
      IDD_INTEGRATOR_DISTRIBUTED_PATH,
      IDD_INTEGRATOR_BIDIRECTIONAL,
      IDD_INTEGRATOR_NUMBER,

    // path integrator parameters
    IDG_PATH = 12100,
    IDD_PATH_MAX_DEPTH,
    IDD_PATH_DIRECT_LIGHT_STRATEGY,
    IDD_PATH_RR_STRATEGY,
    IDD_PATH_RR_CONTINUE_PROB,

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
    IDD_FLEXIMAGE_TONEMAP_SETTINGS,
    IDD_FLEXIMAGE_REINHARD_PRESCALE,
    IDD_FLEXIMAGE_REINHARD_POSTSCALE,
    IDD_FLEXIMAGE_REINHARD_BURN,
    IDD_FLEXIMAGE_REINHARD_DUMMY,
    IDD_FLEXIMAGE_PREMULTIPLY,
    IDD_FLEXIMAGE_DISPLAY_INTERVAL,
    IDD_FLEXIMAGE_WRITE_INTERVAL,
    IDD_FLEXIMAGE_WRITE_TONEMAPPED_TGA,
    IDD_FLEXIMAGE_WRITE_TONEMAPPED_EXR,
    IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_EXR,
    IDD_FLEXIMAGE_WRITE_TONEMAPPED_IGI,
    IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_IGI,
};



#endif  // #ifndef __VPLUXC4DSETTINGS_H__
  