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

CONTAINER VPluxc4dsettings
{
  NAME    VPluxc4dsettings;


  GROUP IDG_RENDER {
  
    GROUP IDG_SAMPLING {
    
      DEFAULT 1;
  
      GROUP {
        COLUMNS 2;
        LONG IDD_SAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_SAMPLER_RANDOM;
            IDD_SAMPLER_LOWDISCREPANCY;
            0;
            IDD_SAMPLER_METROPOLIS;
            IDD_SAMPLER_ERPT;
          }
        }
        BOOL IDD_ADVANCED_SAMPLER { ANIM OFF; }
      }
      
      GROUP IDG_RANDOM {
        COLUMNS 2;
        LONG IDD_RANDOM_PIXELSAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_PIXELSAMPLER_LOWDISCREPANCY;
            IDD_PIXELSAMPLER_HILBERT;
            IDD_PIXELSAMPLER_VEGAS;
            IDD_PIXELSAMPLER_RANDOM;
            IDD_PIXELSAMPLER_LINEAR;
            IDD_PIXELSAMPLER_TILE;
          }
        }
        STATICTEXT { JOINEND; }
        LONG IDD_RANDOM_SAMPLES_X { ANIM OFF;  MIN 1; }
        LONG IDD_RANDOM_SAMPLES_Y { ANIM OFF;  MIN 1; }
      }
      
      GROUP IDG_LOWDISCREPANCY {
        LONG IDD_LOWDISCREPANCY_PIXELSAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_PIXELSAMPLER_LOWDISCREPANCY;
            IDD_PIXELSAMPLER_HILBERT;
            IDD_PIXELSAMPLER_VEGAS;
            IDD_PIXELSAMPLER_RANDOM;
            IDD_PIXELSAMPLER_LINEAR;
            IDD_PIXELSAMPLER_TILE;
          }
        }
        LONG IDD_LOWDISCREPANCY_SAMPLES { ANIM OFF;  MIN 1; }
      }
      
      GROUP IDG_METROPOLIS {
        REAL IDD_METROPOLIS_LARGE_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
        LONG IDD_METROPOLIS_MAX_CONSEC_REJECTS  { ANIM OFF;  MIN 10;   STEP 10; }
        REAL IDD_METROPOLIS_MICRO_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
        REAL IDD_METROPOLIS_MUTATION_RANGE      { ANIM OFF;  MIN 0.0;  STEP 1.0; }
        LONG IDD_METROPOLIS_INIT_SAMPLES        { ANIM OFF;  MIN 100;  STEP 100; }
        LONG IDD_METROPOLIS_STRATA_WIDTH        { ANIM OFF;  MIN 1; }
        BOOL IDD_METROPOLIS_USE_VARIANCE        { ANIM OFF; }
        BOOL IDD_METROPOLIS_USE_QR              { ANIM OFF; }
      }
      
      GROUP IDG_ERPT {
        LONG IDD_ERPT_INIT_SAMPLES        { ANIM OFF;  MIN 100;  STEP 100; }
        LONG IDD_ERPT_CHAINLENGTH         { ANIM OFF;  MIN 1; }
        REAL IDD_ERPT_MICRO_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0; }
        REAL IDD_ERPT_MUTATION_RANGE      { ANIM OFF;  MIN 0.0;  STEP 1.0; }
      }
    
    } // GROUP IDG_SAMPLING
    
  
    GROUP IDG_INTEGRATION {

      DEFAULT 1;

      LONG IDD_INTEGRATOR {
        ANIM OFF;
        CYCLE {
          IDD_INTEGRATOR_PATH;
          IDD_INTEGRATOR_DISTRIBUTED_PATH;
          IDD_INTEGRATOR_BIDIRECTIONAL;
        }
      }

      GROUP IDG_PATH {
        COLUMNS 2;
        LONG IDD_PATH_MAX_DEPTH { ANIM OFF;  MIN 1; }
        STATICTEXT { }
        LONG IDD_PATH_DIRECT_LIGHT_STRATEGY {
          ANIM OFF;
          CYCLE {
            IDD_DIRECT_LIGHT_STRATEGY_AUTO;
            IDD_DIRECT_LIGHT_STRATEGY_ONE;
            IDD_DIRECT_LIGHT_STRATEGY_ALL;
          }
        }
        STATICTEXT { }
        LONG IDD_PATH_RR_STRATEGY {
          ANIM OFF;
          CYCLE {
            IDD_PATH_RR_STRATEGY_EFFICIENCY;
            IDD_PATH_RR_STRATEGY_PROBABILITY;
            IDD_PATH_RR_STRATEGY_NONE;
          }
        }
        REAL IDD_PATH_RR_CONTINUE_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
      } // GROUP IDG_PATH
      
      GROUP IDG_DISTRIBUTED_PATH {
        GROUP {
          COLUMNS 2;
          LONG IDD_DISTRIBUTED_PATH_DIRECT_LIGHT_STRATEGY {
            ANIM OFF;
            CYCLE {
              IDD_DIRECT_LIGHT_STRATEGY_AUTO;
              IDD_DIRECT_LIGHT_STRATEGY_ONE;
              IDD_DIRECT_LIGHT_STRATEGY_ALL;
            }
          }
          LONG IDD_DISTRIBUTED_PATH_DIRECT_DIRECT_LIGHT_SAMPLES   { ANIM OFF;  MIN 1; }
          STATICTEXT {}
          LONG IDD_DISTRIBUTED_PATH_INDIRECT_DIRECT_LIGHT_SAMPLES { ANIM OFF;  MIN 1; }
        }
        GROUP {
          COLUMNS 2;
          LONG IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_DEPTH   { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_DIFFUSE_REFLECT_SAMPLES { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_DEPTH   { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_DIFFUSE_REFRACT_SAMPLES { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_DEPTH    { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_GLOSSY_REFLECT_SAMPLES  { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_DEPTH    { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_GLOSSY_REFRACT_SAMPLES  { ANIM OFF;  MIN 1; }
          LONG IDD_DISTRIBUTED_PATH_SPECULAR_REFLECT_DEPTH  { ANIM OFF;  MIN 1; }
          STATICTEXT { }
          LONG IDD_DISTRIBUTED_PATH_SPECULAR_REFRACT_DEPTH  { ANIM OFF;  MIN 1; }
          STATICTEXT { }
        }
      } // GROUP IDG_DISTRIBUTED_PATH
      
      GROUP IDG_BIDIRECTIONAL {
        COLUMNS 2;
        LONG IDD_BIDIRECTIONAL_EYE_DEPTH   { ANIM OFF;  MIN 1; }
        LONG IDD_BIDIRECTIONAL_LIGHT_DEPTH { ANIM OFF;  MIN 1; }
        LONG IDD_BIDIRECTIONAL_DIRECT_LIGHT_STRATEGY {
          ANIM OFF;
          CYCLE {
            IDD_DIRECT_LIGHT_STRATEGY_AUTO;
            IDD_DIRECT_LIGHT_STRATEGY_ONE;
            IDD_DIRECT_LIGHT_STRATEGY_ALL;
          }
        }
        STATICTEXT { }
      } // GROUP IDG_BIDIRECTIONAL

    } // GROUP IDG_INTEGRATION


    GROUP IDG_FILTERING {

      DEFAULT 1;

      LONG IDD_PIXEL_FILTER {
        ANIM OFF;
        CYCLE {
          IDD_PIXEL_FILTER_BOX;
          IDD_PIXEL_FILTER_GAUSSIAN;
          IDD_PIXEL_FILTER_MITCHELL;
          IDD_PIXEL_FILTER_SINC;
          IDD_PIXEL_FILTER_TRIANGLE;
        }
      }
      
      GROUP IDG_BOX_FILTER {
        COLUMNS 2;
        REAL IDD_BOX_FILTER_WIDTH  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_BOX_FILTER_HEIGHT { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
      }
      
      GROUP IDG_GAUSSIAN_FILTER {
        COLUMNS 2;
        REAL IDD_GAUSSIAN_FILTER_WIDTH  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_GAUSSIAN_FILTER_HEIGHT { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_GAUSSIAN_FILTER_ALPHA  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        STATICTEXT {}
      }
      
      GROUP IDG_MITCHELL_FILTER {
        COLUMNS 2;
        REAL IDD_MITCHELL_FILTER_WIDTH  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_MITCHELL_FILTER_HEIGHT { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_MITCHELL_FILTER_B      { ANIM OFF;  MIN 0.0;  MAX  1.0;  STEP 0.01; }
        REAL IDD_MITCHELL_FILTER_C      { ANIM OFF;  MIN 0.0;  MAX  1.0;  STEP 0.01; }
      }
      
      GROUP IDG_SINC_FILTER {
        COLUMNS 2;
        REAL IDD_SINC_FILTER_WIDTH  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_SINC_FILTER_HEIGHT { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_SINC_FILTER_TAU    { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        STATICTEXT {}
      }
      
      GROUP IDG_TRIANGLE_FILTER {
        COLUMNS 2;
        REAL IDD_TRIANGLE_FILTER_WIDTH  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_TRIANGLE_FILTER_HEIGHT { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
      }
      
    } // GROUP IDG_FILTERING

  } // GROUP IDG_RENDER
  
  
  GROUP IDG_OUTPUT {
  
    GROUP IDG_FILM {
      DEFAULT 1;
      
      LONG IDD_FILM {
        ANIM OFF;
        CYCLE {
          IDD_FILM_FLEXIMAGE;
        }
      }

      GROUP IDG_FLEXIMAGE {
        COLUMNS 2;
        LONG IDD_FLEXIMAGE_HALT_SPP               { ANIM OFF;  MIN 0; }
        STATICTEXT {}
        REAL IDD_FLEXIMAGE_GAMMA                  { ANIM OFF;  MIN 1.0;  MAX 10.0;  STEP 0.1; }
        BOOL IDD_FLEXIMAGE_TONEMAP_SETTINGS       { ANIM OFF; }
        REAL IDD_FLEXIMAGE_REINHARD_PRESCALE      { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_FLEXIMAGE_REINHARD_POSTSCALE     { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1; }
        REAL IDD_FLEXIMAGE_REINHARD_BURN          { ANIM OFF;  MIN 0.1;  MAX 12.0;  STEP 0.1; }
        STATICTEXT IDD_FLEXIMAGE_REINHARD_DUMMY   {}
        BOOL IDD_FLEXIMAGE_PREMULTIPLY            { ANIM OFF; }
        STATICTEXT {}
        LONG IDD_FLEXIMAGE_DISPLAY_INTERVAL       { ANIM OFF;  MIN 5;  MAX 3600; }
        LONG IDD_FLEXIMAGE_WRITE_INTERVAL         { ANIM OFF;  MIN 5;  MAX 3600; }
        BOOL IDD_FLEXIMAGE_WRITE_TONEMAPPED_TGA   { ANIM OFF; }
        STATICTEXT {}
        BOOL IDD_FLEXIMAGE_WRITE_TONEMAPPED_EXR   { ANIM OFF; }
        BOOL IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_EXR { ANIM OFF; }
        BOOL IDD_FLEXIMAGE_WRITE_TONEMAPPED_IGI   { ANIM OFF; }
        BOOL IDD_FLEXIMAGE_WRITE_UNTONEMAPPED_IGI { ANIM OFF; }
      } // GROUP IDG_FLEXIMAGE
      
    } // GROUP IDG_FILM
  
  } // GROUP IDG_OUTPUT
}
