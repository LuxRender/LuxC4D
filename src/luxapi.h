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

#ifndef __LUXAPI_H__
#define __LUXAPI_H__  1



#include "c4d.h"

#include "luxtypes.h"
#include "luxparamset.h"



/***************************************************************************//*!
 This class encapsulates the interface of the Lux API. An implementation might
 write the statements to a file or send it directly to the Lux renderer.
*//****************************************************************************/
class LuxAPI
{
public:

  /// The type for storing the name of a statement, e.g. "Camera".
  typedef const CHAR*  StatementNameT;
  /// The type for storing the identifier of a statement, e.g. "perspective".
  typedef const CHAR*  IdentifierNameT;


  /// Should be called before any other LuxAPI functions are called. It allows
  /// the implementations to do some initial work.
  ///
  /// @param[in]  head
  ///   This optional string can contain any comments or heading text that may
  ///   be interesting for the LuxAPI implementation (e.g. a file writer).
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool startScene(const CHAR* head) =0;

  /// Should be called after the last LuxAPI call of the current scene. It
  /// allows the implementation to do some cleanup if needed.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool endScene(void) =0;


  /// Sets the film buffer.
  ///
  /// @param[in]  name
  ///   The name of the film buffer(e.g. "fleximage").
  /// @param[in]  paramSet
  ///   The parameters
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool film(IdentifierNameT    name,
                    const LuxParamSet& paramSet) =0;

  /// Defines a position and orientation in space, e.g. used by the camera.
  ///
  /// @param[in]  camPos
  ///   The position of the object/camera.
  /// @param[in]  trgPos
  ///   The position of the point the object/camera is facing, i.e. the +Z axis
  ///   will point in that direction.
  /// @param[in]  upVec
  ///   Tells the object/camera where is "up", i.e. the +Y axis will lie in the
  ///   same plane as the Z axis and the up-vector.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool lookAt(const LuxVectorT& camPos,
                      const LuxVectorT& trgPos,
                      const LuxVectorT& upVec) =0;

  /// Specifies the camera.
  ///
  /// @param[in]  name
  ///   The type name of the camera (e.g. "perspective").
  /// @param[in]  paramSet
  ///   The additional camera parameters (e.g. "fov").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool camera(IdentifierNameT    name,
                      const LuxParamSet& paramSet) =0;

  /// Specifies the pixel filter.
  ///
  /// @param[in]  name
  ///   The type name of the pixel filter (e.g. "mitchell").
  /// @param[in]  paramSet
  ///   The additional pixel filter parameters (e.g. "xwidth").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool pixelFilter(IdentifierNameT    name,
                           const LuxParamSet& paramSet) =0;

  /// Specifies the sampler.
  ///
  /// @param[in]  name
  ///   The type name of the sampler (e.g. "metropolis").
  /// @param[in]  paramSet
  ///   The additional sampler parameters (e.g. "largemutationprob").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool sampler(IdentifierNameT    name,
                       const LuxParamSet& paramSet) =0;

  /// Specifies the surface integrator.
  ///
  /// @param[in]  name
  ///   The type name of the surface integrator (e.g. "path").
  /// @param[in]  paramSet
  ///   The additional surface integrator parameters (e.g. "maxdepth").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool surfaceIntegrator(IdentifierNameT    name,
                                 const LuxParamSet& paramSet) =0;

  /// Specifies the ray accelerator structure.
  ///
  /// @param[in]  name
  ///   The type name of the accelerator structure (e.g. "kdtree").
  /// @param[in]  paramSet
  ///   The additional accelerator parameters (e.g. "intersectcost").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool accelerator(IdentifierNameT    name,
                           const LuxParamSet& paramSet) =0;


  virtual Bool worldBegin(void) =0;
  virtual Bool worldEnd(void) =0;
  virtual Bool attributeBegin(void) =0;
  virtual Bool attributeEnd(void) =0;
  virtual Bool objectBegin(IdentifierNameT name) =0;
  virtual Bool objectEnd(void) =0;

  virtual Bool lightSource(IdentifierNameT    name,
                           const LuxParamSet& paramSet) =0;
  virtual Bool texture(IdentifierNameT    name,
                       IdentifierNameT    colorType,
                       IdentifierNameT    type,
                       const LuxParamSet& paramSet) =0;
  virtual Bool transform(const LuxMatrixT& matrix) =0;
  virtual Bool material(IdentifierNameT    name,
                        const LuxParamSet& paramSet) =0;
  virtual Bool shape(IdentifierNameT    name,
                     const LuxParamSet& paramSet) =0;
};



#endif  // #ifndef __LUXAPI_H__
