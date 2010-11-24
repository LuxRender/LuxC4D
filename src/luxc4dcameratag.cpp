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
#include "luxapi.h"
#include "luxc4dcameratag.h"
#include "tluxc4dcameratag.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DCameraTag.
 *****************************************************************************/


/// Allocates a new instance of LuxC4DCameraTag which the caller owns.
/// This function is usually called by C4D when a new camera tag is created.
///
/// @return
///   Pointer to the new instance or NULL, if we ran out of memory.
NodeData* LuxC4DCameraTag::alloc(void)
{
  return gNew LuxC4DCameraTag;
}


/// Registers this TagData plugin.
///
/// @return
///   TRUE if registered successful or FALSE if not.
Bool LuxC4DCameraTag::registerPlugin(void)
{
  return RegisterTagPlugin(PID_LUXC4D_CAMERA_TAG,
                           GeLoadString(IDS_LUXC4D_CAMERA_TAG),
                           TAG_VISIBLE,
                           alloc,
                           "Tluxc4dcameratag",
                           AutoBitmap("icon_camera_tag.tif"),
                           0);
}


/// Determines the Lux camera parameters from a C4D camera object. If a camera
/// tag is assigned to this camera object, we obtain the additional settings
/// from there, too.
///
/// @param[in]  camera
///   Reference to the camera object of which we want to obtain the parameters.
/// @param[in]  c4dLuxScale
///   The conversion scale factor (C4D -> Lux) that will be used for conversion
///   of distances.
/// @param[in]  c4dRenderSettings
///   The data container of the C4D render settings which are used for the
///   export.
/// @param[ou]  parameters
///   Reference to the Lux parameter structure, where we store the converted
///   camera parameters.
/// @return
///   TRUE if the conversion was successful and the returned parameters are
///   valid, FALSE otherwise.
Bool LuxC4DCameraTag::getCameraParameters(CameraObject&     camera,
                                          LReal             c4d2LuxScale,
                                          BaseContainer&    c4dRenderSettings,
                                          CameraParameters& parameters)
{
  // get data container of camera object
  BaseContainer* cameraData = camera.GetDataInstance();
  if (!cameraData)  ERRLOG_RETURN_VALUE(FALSE, "LuxAPIConverter::exportCamera(): couldn't obtain container from camera object");

  // obtain camera parameters
  LONG projection     = cameraData->GetLong(CAMERA_PROJECTION);
  LReal filmWidth     = (LReal)cameraData->GetReal(CAMERAOBJECT_APERTURE);
  LReal focalLength   = (LReal)cameraData->GetReal(CAMERA_FOCUS);
  LReal zoom          = (LReal)cameraData->GetReal(CAMERA_ZOOM);
  LReal focalDistance = (LReal)cameraData->GetReal(CAMERAOBJECT_TARGETDISTANCE);
  LReal filmOffsetX   = (LReal)cameraData->GetReal(CAMERAOBJECT_FILM_OFFSET_X);
  LReal filmOffsetY   = (LReal)cameraData->GetReal(CAMERAOBJECT_FILM_OFFSET_Y);

  // as Lux specifies FOV over the smallest dimension and C4D defines it always
  // over the X axis of the camera, we have to calculate a FOv correction for
  // landscape layout
  LONG xResolution = c4dRenderSettings.GetLong(RDATA_XRES);
  LONG yResolution = c4dRenderSettings.GetLong(RDATA_YRES);
  LReal fovCorrection = (xResolution < yResolution) ? 1.0L : (LReal)yResolution / (LReal)xResolution;

  // determine camera type and calculate field of view and screen window (the region
  // in screenspace we want to render into the result image)
  if (projection == Pperspective) {
    parameters.mType = CAMERA_TYPE_PERSPECTIVE;
    parameters.mFov =
      (LuxFloat)(2.0L * atan(0.5L*fovCorrection*filmWidth / focalLength) / pi * 180.0L);
    if (xResolution < yResolution) {
      parameters.mScreenWindow[0] = -1.0L;
      parameters.mScreenWindow[1] =  1.0L;
      parameters.mScreenWindow[2] = -((LReal)yResolution / (LReal)xResolution);
      parameters.mScreenWindow[3] =  ((LReal)yResolution / (LReal)xResolution);
    } else {
      parameters.mScreenWindow[0] = -((LReal)xResolution / (LReal)yResolution);
      parameters.mScreenWindow[1] =  ((LReal)xResolution / (LReal)yResolution);
      parameters.mScreenWindow[2] = -1.0L;
      parameters.mScreenWindow[3] =  1.0L;
    }
    parameters.mScreenWindowSet = FALSE;
  } else {
    parameters.mType = CAMERA_TYPE_ORTHOGRAPHIC;
    LReal scale = 512.0L * c4d2LuxScale / zoom;
    parameters.mScreenWindow[0] = -1.0L * scale;
    parameters.mScreenWindow[1] =  1.0L * scale;
    parameters.mScreenWindow[2] = -((LReal)yResolution / (LReal)xResolution) * scale;
    parameters.mScreenWindow[3] =  ((LReal)yResolution / (LReal)xResolution) * scale;
    parameters.mScreenWindowSet = TRUE;
  }

  // apply film offset to already calculated render region, if it's != 0%
  if ((fabs(filmOffsetX) > 0.0001L) || (fabs(filmOffsetY) > 0.0001L)) {
    LReal deltaX = filmOffsetX *
                   ((LReal)parameters.mScreenWindow[1] - (LReal)parameters.mScreenWindow[0]);
    parameters.mScreenWindow[0] += deltaX;
    parameters.mScreenWindow[1] += deltaX;
    LReal deltaY = filmOffsetY *
                   ((LReal)parameters.mScreenWindow[3] - (LReal)parameters.mScreenWindow[2]);
    parameters.mScreenWindow[2] -= deltaY;
    parameters.mScreenWindow[3] -= deltaY;
    parameters.mScreenWindowSet = TRUE;
  }

  // get near clipping plane (if set)
  parameters.mNearClippingSet = FALSE;
  if (cameraData->GetBool(CAMERAOBJECT_NEAR_CLIPPING_ENABLE)) {
    Real nearClipDistance = cameraData->GetReal(CAMERAOBJECT_NEAR_CLIPPING);
    if (nearClipDistance > 0.01f) {
      parameters.mNearClippingSet = TRUE;
      parameters.mNearClipping = nearClipDistance * c4d2LuxScale;
    }
  }

  // look for camera tag, if there is none, we are finished and can return
  BaseTag* cameraTag = camera.GetTag(PID_LUXC4D_CAMERA_TAG);
  if (!cameraTag) {
    parameters.mDofEnabled = FALSE;
    return TRUE;
  }

  // get camera tag container and return, if it's there is none
  BaseContainer* tagData = cameraTag->GetDataInstance();

  // get DOF parameters
  parameters.mDofEnabled       = (tagData->GetBool(IDD_ENABLE_DOF) != 0);
  parameters.mLensRadius       = focalLength / (2.0 * tagData->GetReal(IDD_FSTOP)) * 0.001;
  LONG focusType = tagData->GetLong(IDD_FOCUS_TYPE);
  if (focusType == IDD_FOCUS_TYPE_CAMERA) {
    parameters.mAutofocus = FALSE;
  } else if (focusType == IDD_FOCUS_TYPE_AUTOFOCUS) {
    parameters.mAutofocus = TRUE;
  } else if (focusType == IDD_FOCUS_TYPE_OBJECT) {
    parameters.mAutofocus = FALSE;
    BaseObject* focusObject = (BaseObject*)tagData->GetLink(IDD_FOCUS_OBJECT,
                                                            camera.GetDocument(),
                                                            Obase);
    if (focusObject) {
      Matrix focusObjectMat = focusObject->GetMg();
      Matrix cameraMat = camera.GetMgn();
      Vector deltaVec = focusObjectMat.off - cameraMat.off;
      focalDistance = deltaVec * cameraMat.v3;
    }
  }
  parameters.mFocalDistance    = focalDistance * c4d2LuxScale;
  parameters.mBladeNumber      = tagData->GetLong(IDD_BLADE_NUMBER);
  parameters.mLensSamplingType = tagData->GetLong(IDD_LENS_SAMPLING_TYPE);
  parameters.mExponentialPower = tagData->GetLong(IDD_EXPONENTIAL_POWER);

  return TRUE;
}


/// Initialises the tag parameters.
///
/// @return
///   TRUE if registered successful or FALSE if not.
Bool LuxC4DCameraTag::Init(GeListNode* node)
{
  // obtain container from data node.
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // initialise parameters
  data->SetBool(IDD_ENABLE_DOF,         TRUE);
  data->SetReal(IDD_FSTOP,              5.6);
  data->SetBool(IDD_FOCUS_TYPE,         IDD_FOCUS_TYPE_CAMERA);
  data->SetLong(IDD_BLADE_NUMBER,       0);
  data->SetLong(IDD_LENS_SAMPLING_TYPE, IDD_LENS_SAMPLING_UNIFORM);
  data->SetLong(IDD_EXPONENTIAL_POWER,  1);

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
Bool LuxC4DCameraTag::GetDDescription(GeListNode*     node,
                                      Description*    description,
                                      DESCFLAGS_DESC& flags)
{
  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DCameraTag::GetDDescription(): could not load description for node");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DCameraTag::GetDDescription(): could not allocate empty AtomArray");
  }

  // show/hide parameters depending on options
  showParameter(description, IDG_DOF_SUB, params, data->GetBool(IDD_ENABLE_DOF));
  LONG focusType = data->GetLong(IDD_FOCUS_TYPE);
  showParameter(description, IDD_FOCUS_OBJECT, params,
                focusType == IDD_FOCUS_TYPE_OBJECT);
  LONG lensSamplingType = data->GetLong(IDD_LENS_SAMPLING_TYPE);
  showParameter(description, IDD_EXPONENTIAL_POWER, params,
                (lensSamplingType == IDD_LENS_SAMPLING_EXPONENTIAL) ||
                (lensSamplingType == IDD_LENS_SAMPLING_INVERSE_EXPONENTIAL));

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return SUPER::GetDDescription(node, description, flags);
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DCameraTag.
 *****************************************************************************/


/// Returns the data container of this node.
BaseContainer* LuxC4DCameraTag::getData(void)
{
  BaseTag* tag = static_cast<BaseTag*>(Get());
  BaseContainer* data = 0;
  if (tag) {
    data = tag->GetDataInstance();
  }
  if (!data)  ERRLOG_RETURN_VALUE(NULL, "LuxC4DCameraTag::getData(): could not obtain base container");
  return data;
}
