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



#include <c4d.h>

#include "filepath.h"
#include "luxtypes.h"
#include "luxparamset.h"



/***************************************************************************//*!
 This class encapsulates the interface of the Lux API. An implementation might
 write the statements to a file or send it directly to the Lux renderer.
*//****************************************************************************/
class LuxAPI
{
public:

  /// The type for storing the identifier of a statement, e.g. "perspective".
  typedef const char*  IdentifierName;


  /// Should be called before any other LuxAPI functions are called. It allows
  /// the implementations to do some initial work.
  ///
  /// @param[in]  head
  ///   This optional string can contain any comments or heading text that may
  ///   be interesting for the LuxAPI implementation (e.g. a file writer).
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool startScene(const char* head = NULL) =0;

  /// Should be called after the last LuxAPI call of the current scene. It
  /// allows the implementation to do some cleanup if needed.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool endScene(void) =0;


  /// Processes a file path, e.g. makes it relative to the scene file.
  ///
  /// @param[in,out]  path
  ///   The path that should be processed.
  virtual void processFilePath(FilePath& path) =0;

  ///
  virtual Filename getSceneFilename(void) =0;


  /// Specifies the comment for the next Lux API command. This should be used
  /// by an exporter implementation that writes Lux scene files and should be
  /// ignored by an integration of Lux into C4D.
  ///
  /// The general convention is that you specify a comment and call the API
  /// command and after the command call the comment buffer is empty again. If
  /// the API implementation is writing to several files, the comment and the
  /// the command should go into the same files.
  ///
  /// @param[in]  text
  ///   The string that should be used as comment.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool setComment(const char* text) =0;

  /// The same as LuxAPI::setComment(const char*), except that you specify the
  /// comment via a String.
  ///
  ///
  /// @param[in]  text
  ///   The string that should be used as comment.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool setComment(const String& text) =0;


  /// Sets the film buffer.
  ///
  /// @param[in]  type
  ///   The type name of the film buffer (at the moment only "fleximage").
  /// @param[in]  paramSet
  ///   The parameters of the film buffer (e.g. "xresolution").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool film(IdentifierName     type,
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
  virtual Bool lookAt(const LuxVector& camPos,
                      const LuxVector& trgPos,
                      const LuxVector& upVec) =0;

  /// Specifies the camera.
  ///
  /// @param[in]  name
  ///   The type name of the camera (e.g. "perspective").
  /// @param[in]  paramSet
  ///   The additional camera parameters (e.g. "fov").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool camera(IdentifierName     type,
                      const LuxParamSet& paramSet) =0;

  /// Specifies the pixel filter.
  ///
  /// @param[in]  type
  ///   The type name of the pixel filter (e.g. "mitchell").
  /// @param[in]  paramSet
  ///   The additional pixel filter parameters (e.g. "xwidth").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool pixelFilter(IdentifierName     type,
                           const LuxParamSet& paramSet) =0;

  /// Specifies the sampler.
  ///
  /// @param[in]  type
  ///   The type name of the sampler (e.g. "metropolis").
  /// @param[in]  paramSet
  ///   The additional sampler parameters (e.g. "largemutationprob").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool sampler(IdentifierName     type,
                       const LuxParamSet& paramSet) =0;

  /// Specifies the surface integrator.
  ///
  /// @param[in]  type
  ///   The type name of the surface integrator (e.g. "path").
  /// @param[in]  paramSet
  ///   The additional surface integrator parameters (e.g. "maxdepth").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool surfaceIntegrator(IdentifierName     type,
                                 const LuxParamSet& paramSet) =0;

  /// Specifies the ray accelerator structure.
  ///
  /// @param[in]  type
  ///   The type name of the accelerator structure (e.g. "kdtree").
  /// @param[in]  paramSet
  ///   The additional accelerator parameters (e.g. "intersectcost").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool accelerator(IdentifierName     type,
                           const LuxParamSet& paramSet) =0;


  /// Starts the actual scene description. This has to be called before any
  /// of the functions below are called.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool worldBegin(void) =0;

  /// Ends the actual scene description. After calling this, none of the
  /// functions below should be called anymore.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool worldEnd(void) =0;

  /// Begins a new level on the attribute stack. By calling attributeEnd()
  /// you can go back to the previous level and restore its attributes.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool attributeBegin(void) =0;

  /// Closes the current level on the attribute stack and restores the previous
  /// level.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool attributeEnd(void) =0;

  /// Begins a new named level on the attribute stack - similar to
  /// attributeBegin(). By giving this level a name, we can refer to it later
  /// and create instances of it.
  ///
  /// @param[in]  name
  ///   The name of the object by which you can refer to it.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool objectBegin(IdentifierName name) =0;

  /// Closes the current named level on the attribute stack and restores the
  /// previous level.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool objectEnd(void) =0;


  /// Defines the group of all lights defined in current scope. The default is
  /// "_default".
  ///
  /// @param[in]  name
  ///   The name of the light group.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool lightGroup(IdentifierName name) =0;

  /// Adds a light source object to the current scope.
  ///
  /// @param[in]  type
  ///   The type name of the light object (e.g. "sunsky").
  /// @param[in]  paramSet
  ///   The additional light parameters (e.g. "gain").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool lightSource(IdentifierName     type,
                           const LuxParamSet& paramSet) =0;

  /// Adds an area light source object to the current scope. The shape that is
  /// defined in the same scope will then be used as area light geometry.
  ///
  /// @param[in]  type
  ///   The type name of the area light object (at the moment only "area").
  /// @param[in]  paramSet
  ///   The additional area light parameters (e.g. "nsamples").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool areaLightSource(IdentifierName     type,
                               const LuxParamSet& paramSet) =0;

  /// Adds a named texture to the current scope. Materials can then refer (/use)
  /// it via its name.
  ///
  /// @param[in]  name
  ///   A user-defined name of the texture, which will be used for referencing
  ///   in materials.
  /// @param[in]  colorType
  ///   The color type which basically defines the data type of the texture
  ///   (at the moment only "float" or "color").
  /// @param[in]  type
  ///   The type name of the texture (e.g. "constant").
  /// @param[in]  paramSet
  ///   The additional texture parameters (e.g. "value").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool texture(IdentifierName     name,
                       IdentifierName     colorType,
                       IdentifierName     type,
                       const LuxParamSet& paramSet) =0;

  /// Adds a named material, which can then be refered to by objects.
  ///
  /// NOTE: The material type has to be defined in the parameter set using
  ///       the string parameter "type".
  ///
  /// @param[in]  name
  ///   The name of the new material. It should be unique to make the material
  ///   reference anambiguous.
  /// @param[in]  paramSet
  ///   The additional parameters of the material.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool makeNamedMaterial(IdentifierName     name,
                                 const LuxParamSet& paramSet) =0;

  /// Sets the material referenced by its name as the current material for the
  /// current scope.
  ///
  /// @param[in]  name
  ///   The name of the material to use (must be the name of a material that
  ///   was created by LuxAPI::makeNamedMaterial(IdentifierName,
  ///   const LuxParamSet&) ).
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool namedMaterial(IdentifierName name) =0;

  /// Defines the current material for the current scope.
  ///
  /// @param[in]  type
  ///   The type name of the material (e.g. "glossy").
  /// @param[in]  paramSet
  ///   The additional material parameters (e.g. "Ks").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool material(IdentifierName     type,
                        const LuxParamSet& paramSet) =0;

  /// Transforms the coordinate space of the current scope.
  ///
  /// @param[in]  matrix
  ///   The matrix that should be used for the transformation.
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool transform(const LuxMatrix& matrix) =0;

  /// Reverses the orientation of the normals of all following objects, of the
  /// current scope.
  ///
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool reverseOrientation(void) =0;

  /// Adds a geometry object to the current scope.
  ///
  /// @param[in]  type
  ///   The type name of the object (e.g. "mesh").
  /// @param[in]  paramSet
  ///   The additional parameters of the object (e.g. "triindices").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool shape(IdentifierName     type,
                     const LuxParamSet& paramSet) =0;

  /// Adds a portal geometry object to the current scope.
  ///
  /// @param[in]  type
  ///   The type name of the object (e.g. "mesh").
  /// @param[in]  paramSet
  ///   The additional parameters of the object (e.g. "triindices").
  /// @return
  ///   TRUE if executed successfully, otherwise FALSE.
  virtual Bool portalShape(IdentifierName     type,
                           const LuxParamSet& paramSet) =0;
};



#endif  // #ifndef __LUXAPI_H__
