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
        LONG IDD_RANDOM_PIXELSAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_PIXELSAMPLER_LOWDISCREPANCY;
            IDD_PIXELSAMPLER_HILBERT;
            IDD_PIXELSAMPLER_VEGAS;
            IDD_PIXELSAMPLER_LINEAR;
            IDD_PIXELSAMPLER_TILE;
          }
        }
        LONG IDD_RANDOM_PIXELSAMPLES { ANIM OFF;  MIN 1; }
      }
      
      GROUP IDG_LOWDISCREPANCY {
        LONG IDD_LOWDISCREPANCY_PIXELSAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_PIXELSAMPLER_LOWDISCREPANCY;
            IDD_PIXELSAMPLER_HILBERT;
            IDD_PIXELSAMPLER_VEGAS;
            IDD_PIXELSAMPLER_LINEAR;
            IDD_PIXELSAMPLER_TILE;
          }
        }
        LONG IDD_LOWDISCREPANCY_PIXELSAMPLES { ANIM OFF;  MIN 1; }
      }
      
      GROUP IDG_METROPOLIS {
        REAL IDD_METROPOLIS_STRENGTH            { UNIT PERCENT;  MIN 0.0;  MAX 100.0;  STEP 1.0;  CUSTOMGUI REALSLIDER; }
        STATICTEXT { JOINEND; }
        REAL IDD_METROPOLIS_LARGE_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
        LONG IDD_METROPOLIS_MAX_CONSEC_REJECTS  { ANIM OFF;  MIN 10;   STEP 10; }
        REAL IDD_METROPOLIS_MICRO_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
        BOOL IDD_METROPOLIS_USE_VARIANCE        { ANIM OFF; }
      }
      
      GROUP IDG_ERPT {
        LONG IDD_ERPT_CHAINLENGTH         { ANIM OFF;  MIN 1; }
        REAL IDD_ERPT_MICRO_MUTATION_PROB { ANIM OFF;  MIN 0.0;  MAX 1.0; }
        LONG IDD_ERPT_PIXELSAMPLER {
          ANIM OFF;
          CYCLE {
            IDD_PIXELSAMPLER_LOWDISCREPANCY;
            IDD_PIXELSAMPLER_HILBERT;
            IDD_PIXELSAMPLER_VEGAS;
            IDD_PIXELSAMPLER_LINEAR;
            IDD_PIXELSAMPLER_TILE;
          }
        }
        LONG IDD_ERPT_PIXELSAMPLES        { ANIM OFF;  MIN 1; }
      }
    
    } // GROUP IDG_SAMPLING
    
  
    GROUP IDG_INTEGRATION {

      DEFAULT 1;

      GROUP {
        COLUMNS 2;
        LONG IDD_INTEGRATOR {
          ANIM OFF;
          CYCLE {
            IDD_INTEGRATOR_DIRECT_LIGHTING;
            IDD_INTEGRATOR_PATH;
            IDD_INTEGRATOR_BIDIRECTIONAL;
            IDD_INTEGRATOR_DISTRIBUTED_PATH;
          }
        }
        BOOL IDD_ADVANCED_INTEGRATOR { ANIM OFF; }
      }

      GROUP IDG_PATH {
        LONG IDD_PATH_MAX_DEPTH { ANIM OFF;  MIN 1; }
        BOOL IDD_PATH_INCLUDE_ENVIRONMENT { ANIM OFF; }
        LONG IDD_PATH_DIRECT_LIGHT_STRATEGY {
          ANIM OFF;
          CYCLE {
            IDD_DIRECT_LIGHT_STRATEGY_AUTO;
            IDD_DIRECT_LIGHT_STRATEGY_ONE;
            IDD_DIRECT_LIGHT_STRATEGY_ALL;
          }
        }
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
        LONG IDD_BIDIRECTIONAL_MAX_DEPTH   { ANIM OFF;  MIN 1; }
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
        REAL IDD_BIDIRECTIONAL_EYE_RR_THRESHOLD   { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
        REAL IDD_BIDIRECTIONAL_LIGHT_RR_THRESHOLD { ANIM OFF;  MIN 0.0;  MAX 1.0;  STEP 0.05; }
      } // GROUP IDG_BIDIRECTIONAL

      GROUP IDG_DIRECT_LIGHTING {
        LONG IDD_DIRECT_LIGHTING_MAX_DEPTH { ANIM OFF;  MIN 1; }
        LONG IDD_DIRECT_LIGHTING_STRATEGY {
          ANIM OFF;
          CYCLE {
            IDD_DIRECT_LIGHT_STRATEGY_AUTO;
            IDD_DIRECT_LIGHT_STRATEGY_ONE;
            IDD_DIRECT_LIGHT_STRATEGY_ALL;
          }
        }
      } // GROUP IDG_DIRECT_LIGHTING
      
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
    
    
    GROUP IDG_ACCELERATION {
      DEFAULT 1;
    
      GROUP {
        COLUMNS 2;
        LONG IDD_ACCELERATION_TYPE {
          ANIM OFF;
          CYCLE {
            IDD_ACCELERATION_KDTREE;
            IDD_ACCELERATION_BVH;
            IDD_ACCELERATION_QBVH;
          }
        }
        BOOL IDD_ACCELERATION_ADVANCED  { ANIM OFF; }
      }

      GROUP IDG_ACCELERATION_KDTREE {
        COLUMNS 2;
        LONG IDD_KDTREE_INTERSECTION_COST   { ANIM OFF;  MIN 1;  MAX 1000; }
        LONG IDD_KDTREE_TRAVERSAL_COST      { ANIM OFF;  MIN 1;  MAX 1000; }
        REAL IDD_KDTREE_EMPTY_BONUS         { ANIM OFF;  MIN 0.0;  MAX 100.0;  STEP 0.1; }
        STATICTEXT {}
        LONG IDD_KDTREE_MAX_PRIMITIVES      { ANIM OFF;  MIN 1;  MAX 1000; }
        LONG IDD_KDTREE_MAX_DEPTH           { ANIM OFF;  MIN -1;  MAX 100; }
      }

      GROUP IDG_ACCELERATION_BVH {
        COLUMNS 2;
        LONG IDD_BVH_TREE_TYPE {
          ANIM OFF;
          CYCLE {
            IDD_BVH_BINARY_TREE;
            IDD_BVH_QUAD_TREE;
            IDD_BVH_OCT_TREE;
          }
        }
        STATICTEXT {}
        LONG IDD_BVH_COST_SAMPLES       { ANIM OFF;  MIN 0;  MAX 100; }
        STATICTEXT {}
        LONG IDD_BVH_INTERSECTION_COST  { ANIM OFF;  MIN 1;  MAX 1000; }
        LONG IDD_BVH_TRAVERSAL_COST     { ANIM OFF;  MIN 1;  MAX 1000; }
        REAL IDD_BVH_EMPTY_BONUS        { ANIM OFF;  MIN 0.0;  MAX 100.0;  STEP 0.1; }
        STATICTEXT {}
      } // GROUP IDG_ACCELERATION_BVH

      GROUP IDG_ACCELERATION_QBVH {
        LONG IDD_QBVH_MAX_PRIMITIVES        { ANIM OFF;  MIN 1;  MAX 64; }
        REAL IDD_QBVH_FULL_SWEEP_THRESHOLD  { ANIM OFF;  MIN 1.0;  MAX 256.0;  STEP 1.0; }
        LONG IDD_QBVH_SKIP_FACTOR           { ANIM OFF;  MIN 1;  MAX 10; }
      }
      
    } // GROUP IDG_ACCELERATION

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
        DEFAULT 1;
        
        LONG IDD_FLEXIMAGE_HALT_SPP               { ANIM OFF;  MIN 0; }
        STATICTEXT {}
        LONG IDD_FLEXIMAGE_DISPLAY_INTERVAL       { ANIM OFF;  MIN 5;  MAX 3600; }
        LONG IDD_FLEXIMAGE_WRITE_INTERVAL         { ANIM OFF;  MIN 5;  MAX 3600; }
        REAL IDD_FLEXIMAGE_GAMMA                  { ANIM OFF;  MIN 1.0;  MAX 10.0;  STEP 0.1; }
        BOOL IDD_FLEXIMAGE_PREMULTIPLY            { ANIM OFF; }
        LONG IDD_FLEXIMAGE_CLAMP_METHOD {
          ANIM OFF;
          CYCLE {
            IDD_CLAMP_METHOD_LUM;
            IDD_CLAMP_METHOD_HUE;
            IDD_CLAMP_METHOD_CUT;
          }
        }
        LONG IDD_FLEXIMAGE_REJECT_WARMUP          { ANIM OFF;  MIN 0; }
        
        GROUP IDG_FLEXIMAGE_TONEMAPPING {
          DEFAULT 1;

          LONG IDD_FLEXIMAGE_TONEMAP_KERNEL {
            ANIM OFF;
            CYCLE {
              IDD_TONEMAP_KERNEL_REINHARD;
              IDD_TONEMAP_KERNEL_LINEAR;
              IDD_TONEMAP_KERNEL_CONTRAST;
              IDD_TONEMAP_KERNEL_MAXWHITE;
            }
          }
          
          GROUP IDG_FLEXIMAGE_REINHARD {
            REAL IDD_FLEXIMAGE_REINHARD_PRESCALE  { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1;  CUSTOMGUI REALSLIDER; }
            REAL IDD_FLEXIMAGE_REINHARD_POSTSCALE { ANIM OFF;  MIN 0.0;  MAX 10.0;  STEP 0.1;  CUSTOMGUI REALSLIDER; }
            REAL IDD_FLEXIMAGE_REINHARD_BURN      { ANIM OFF;  MIN 0.1;  MAX 12.0;  STEP 0.1;  CUSTOMGUI REALSLIDER; }
          } // GROUP IDG_FLEXIMAGE_REINHARD
          
          GROUP IDG_FLEXIMAGE_LINEAR {
            REAL IDD_FLEXIMAGE_LINEAR_SENSITIVITY { ANIM OFF;  MIN 0.0;  MAX 1000.0;   STEP 10.0;  CUSTOMGUI REALSLIDER; }
            REAL IDD_FLEXIMAGE_LINEAR_EXPOSURE    { ANIM OFF;  MIN 0.0;  MAX 10000.0;  STEP 1.0;   CUSTOMGUI REALSLIDER;  MAXSLIDER 1000.0; }
            REAL IDD_FLEXIMAGE_LINEAR_FSTOP       { ANIM OFF;  MIN 0.0;  MAX 64.0;     STEP 0.1;   CUSTOMGUI REALSLIDER;  MAXSLIDER 16.0; }
            REAL IDD_FLEXIMAGE_LINEAR_GAMMA       { ANIM OFF;  MIN 0.0;  MAX 5.0;      STEP 0.1;   CUSTOMGUI REALSLIDER; }
          } // GROUP IDG_FLEXIMAGE_LINEAR
          
          GROUP IDG_FLEXIMAGE_CONTRAST {
            REAL IDD_FLEXIMAGE_CONTRAST_YWA       { ANIM OFF;  MIN 0.0;  MAX 1000.0; STEP 0.01; }
          } // GROUP IDG_FLEXIMAGE_CONTRAST
          
        } // GROUP IDG_FLEXIMAGE_TONEMAPPING
        
        
        GROUP IDG_FLEXIMAGE_FILE_FORMATS {
          DEFAULT 1;
          
          GROUP {
            LONG IDD_FLEXIMAGE_FILENAME_TYPE {
              ANIM OFF;
              CYCLE {
                IDD_FLEXIMAGE_FILENAME_AS_SCENE_FILE;
                IDD_FLEXIMAGE_FILENAME_AS_C4D_OUTPUT_FILE;
                IDD_FLEXIMAGE_FILENAME_DEFINE_HERE;
              }
            }
            FILENAME IDD_FLEXIMAGE_OUTPUT_FILENAME  { ANIM OFF;  SAVE; }
          }
  
          SEPARATOR { LINE; }
        
          BOOL IDD_FLEXIMAGE_WRITE_EXR  { ANIM OFF; }
          GROUP IDG_FLEXIMAGE_EXR {
            COLUMNS 2;
          
            LONG IDD_FLEXIMAGE_EXR_CHANNELS {
              ANIM OFF;
              CYCLE {
                IDD_WRITE_CHANNELS_Y;
                IDD_WRITE_CHANNELS_YA;
                IDD_WRITE_CHANNELS_RGB;
                IDD_WRITE_CHANNELS_RGBA;
              }
            }
            BOOL IDD_FLEXIMAGE_EXR_HALFTYPE       { ANIM OFF; }
            LONG IDD_FLEXIMAGE_EXR_COMPRESSION {
              ANIM OFF;
              CYCLE {
                IDD_EXR_COMPRESSION_RLE;
                IDD_EXR_COMPRESSION_PIZ;
                IDD_EXR_COMPRESSION_ZIP;
                IDD_EXR_COMPRESSION_PXR24;
                IDD_EXR_COMPRESSION_NONE;
              }
            }
            STATICTEXT { }
            BOOL IDD_FLEXIMAGE_EXR_APPLY_IMAGING  { ANIM OFF; }
            BOOL IDD_FLEXIMAGE_EXR_GAMUT_CLAMP    { ANIM OFF; }
            BOOL IDD_FLEXIMAGE_EXR_WRITE_ZBUF     { ANIM OFF; }
            LONG IDD_FLEXIMAGE_EXR_ZBUF_NORM_TYPE {
              ANIM OFF;
              CYCLE {
                IDD_ZBUF_NORM_TYPE_NONE;
                IDD_ZBUF_NORM_TYPE_CAMERA;
                IDD_ZBUF_NORM_TYPE_SCENE;
              }
            }
            
          } // GROUP IDG_FLEXIMAGE_EXR
          
          SEPARATOR { LINE; }
          
          BOOL IDD_FLEXIMAGE_WRITE_PNG  { ANIM OFF; }
          GROUP IDG_FLEXIMAGE_PNG {
            COLUMNS 3;

            LONG IDD_FLEXIMAGE_PNG_CHANNELS {
              ANIM OFF;
              CYCLE {
                IDD_WRITE_CHANNELS_Y;
                IDD_WRITE_CHANNELS_YA;
                IDD_WRITE_CHANNELS_RGB;
                IDD_WRITE_CHANNELS_RGBA;
              }
            }
            BOOL IDD_FLEXIMAGE_PNG_16BIT        { ANIM OFF;}
            BOOL IDD_FLEXIMAGE_PNG_GAMUT_CLAMP  { ANIM OFF; }

          } // GROUP IDG_FLEXIMAGE_PNG
          
          SEPARATOR { LINE; }
          
          BOOL IDD_FLEXIMAGE_WRITE_TGA  { ANIM OFF; }
          GROUP IDG_FLEXIMAGE_TGA {
            COLUMNS 2;
            
            LONG IDD_FLEXIMAGE_TGA_CHANNELS {
              ANIM OFF;
              CYCLE {
                IDD_WRITE_CHANNELS_Y;
                IDD_WRITE_CHANNELS_RGB;
                IDD_WRITE_CHANNELS_RGBA;
              }
            }
            BOOL IDD_FLEXIMAGE_TGA_GAMUT_CLAMP  { ANIM OFF; }
          } // GROUP IDG_FLEXIMAGE_TGA
          
        } // GROUP IDG_FLEXIMAGE_FILE_FORMATS
        
      } // GROUP IDG_FLEXIMAGE
      
    } // GROUP IDG_FILM
  
  } // GROUP IDG_OUTPUT
  
  
  GROUP IDG_EXPORT {
  
    LONG IDD_WHICH_EXPORT_FILENAME {
      ANIM OFF;
      CYCLE {
        IDD_ASK_FOR_EXPORT_FILENAME;
        IDD_USE_C4D_EXPORT_FILENAME;
        IDD_DEFINE_EXPORT_FILENAME;
      }
    }
    FILENAME IDD_EXPORT_FILENAME   { ANIM OFF;  SAVE; }
    BOOL     IDD_ALLOW_OVERWRITING { ANIM OFF; }
    
    REAL IDD_SCALE_FACTOR                 { ANIM OFF;  MIN 0.00001;  MAX 100000.0;  STEP 1.0; }
    BOOL IDD_EXPORT_BUMP_SAMPLE_DISTANCE  { ANIM OFF; }
    REAL IDD_BUMP_SAMPLE_DISTANCE         { ANIM OFF;  MIN 0.00001;  MAX 1.0;  STEP 0.00001; }
    REAL IDD_TEXTURE_GAMMA_CORRECTION     { ANIM OFF;  MIN 1.0;  MAX 10.0;  STEP 0.1; }
    BOOL IDD_USE_RELATIVE_PATHS           { ANIM OFF; }
    
  } // GROUP IDG_EXPORT

}
