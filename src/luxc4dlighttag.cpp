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

#include <cstring>

#include <olight.h>

#include "c4d_symbols.h"
#include "luxc4dlighttag.h"
#include "tluxc4dlighttag.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DLightTag.
 *****************************************************************************/


/// Returns a new instance of this class. Will be called by C4D, when the
/// user adds this plugin as a post-effect.
NodeData* LuxC4DLightTag::alloc(void)
{
  return gNew LuxC4DLightTag;
}


/// Registers this plugin in C4D and returns TRUE if it succeeded.
Bool LuxC4DLightTag::registerPlugin(void)
{
  return RegisterTagPlugin(PID_LUXC4D_LIGHT_TAG,
                           GeLoadString(IDS_LUXC4D_LIGHT_TAG),
                           TAG_VISIBLE,
                           alloc,
                           "Tluxc4dlighttag",
                           "icon_light_tag.tif",
                           0);
}


/// Obtains the parameters which are important to Lux from a light. It either
/// reads it from the light object or (if available) from the light tag.
/// I.e. this is the main "interface" of the light tag to the rest of the
/// exporter.
///
/// @param[in]  lightObject
///   Reference of the light object we want to fetch the parameters from.
/// @param[in]  c4d2LuxScale
///   The length conversion scale from C4D to Lux space. It's used for the
///   brightness correction for different falloff radii.
/// @param[out]  parameters
///   Reference of the container structure where the light parameters will
///   be written to.
/// @return
///   TRUE if the parameters were read successfully or FALSE, if an error
///   occured.
Bool LuxC4DLightTag::getLightParameters(BaseObject&      lightObject,
                                        LReal            c4d2LuxScale,
                                        LightParameters& parameters)
{
  // check if object is of the right type
  if (lightObject.GetType() != Olight)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): passed object is no light object");

  // calculate brightness correction factor which we only apply, if the falloff
  // of the C4D light is "inverse" or "inverse square"
  Real falloffCorrection = 1.0f;
  LONG falloffType = getParameterLong(lightObject, LIGHT_DETAILS_FALLOFF);
  if ((falloffType == LIGHT_DETAILS_FALLOFF_INVERSE) ||
      (falloffType == LIGHT_DETAILS_FALLOFF_INVERSESQUARE))
  {
    falloffCorrection = getParameterReal(lightObject,
                                         LIGHT_DETAILS_OUTERDISTANCE,
                                         1.0 / c4d2LuxScale)
                        * c4d2LuxScale;
    if (falloffType == LIGHT_DETAILS_FALLOFF_INVERSESQUARE) {
      falloffCorrection *= falloffCorrection;
    }
  }

  // reset parameters
  memset(&parameters, 0, sizeof(parameters));

  // if we can't find a light tag, read the parameters directly from the object
  BaseTag* tag = lightObject.GetTag(PID_LUXC4D_LIGHT_TAG);
  if (!tag) {
    parameters.mType       = getLightType(lightObject);
    parameters.mBrightness = getParameterReal(lightObject, LIGHT_BRIGHTNESS)
                             * falloffCorrection;
    parameters.mColor      = getParameterVector(lightObject, LIGHT_COLOR);
    switch (parameters.mType) {
      case IDD_LIGHT_TYPE_POINT:
        break;
      case IDD_LIGHT_TYPE_SPOT:
        parameters.mInnerAngle = getParameterReal(lightObject, LIGHT_DETAILS_INNERANGLE);
        parameters.mOuterAngle = getParameterReal(lightObject, LIGHT_DETAILS_OUTERANGLE);
        break;
      case IDD_LIGHT_TYPE_DISTANT:
        break;
      case IDD_LIGHT_TYPE_AREA:
        parameters.mSamples        = 1;
        parameters.mFlippedNormals = FALSE;
        parameters.mShapeType      = c4dShape2LuxShape(getParameterLong(lightObject, LIGHT_AREADETAILS_SHAPE));
        parameters.mShapeObject    = (PolygonObject*)getParameterLink(lightObject,
                                                                      LIGHT_AREADETAILS_OBJECT,
                                                                      Opolygon);
        parameters.mShapeSize      = Vector(getParameterReal(lightObject, LIGHT_AREADETAILS_SIZEX),
                                            getParameterReal(lightObject, LIGHT_AREADETAILS_SIZEY),
                                            getParameterReal(lightObject, LIGHT_AREADETAILS_SIZEZ));
        break;
      default:
        ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): invalid light type determined from light object");
    }
    return TRUE;
  }
  
  // if we could find a light tag, we read the parameters from there:

  // first obtain tag node and its container
  LuxC4DLightTag* tagData = (LuxC4DLightTag*)tag->GetNodeData();
  if (!tagData)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): could not obtain node data from tag");
  BaseContainer* data = tagData->getData();
  if (!data)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): could not obtain base container from tag node");
  BaseDocument* document = tag->GetDocument();
  if (!document)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): could not obtain document from tag");

  // then obtain current light type of tag and read the corresponding values
  parameters.mType = tagData->getLightType();
  switch (parameters.mType) {
    case IDD_LIGHT_TYPE_POINT:
      parameters.mBrightness = data->GetReal  (IDD_POINT_LIGHT_BRIGHTNESS);
      parameters.mColor      = data->GetVector(IDD_POINT_LIGHT_COLOR);
      break;
    case IDD_LIGHT_TYPE_SPOT:
      parameters.mBrightness = data->GetReal  (IDD_SPOT_LIGHT_BRIGHTNESS);
      parameters.mColor      = data->GetVector(IDD_SPOT_LIGHT_COLOR);
      parameters.mInnerAngle = data->GetReal  (IDD_SPOT_LIGHT_INNERANGLE);
      parameters.mOuterAngle = data->GetReal  (IDD_SPOT_LIGHT_OUTERANGLE);
      break;
    case IDD_LIGHT_TYPE_DISTANT:
      parameters.mBrightness = data->GetReal  (IDD_DISTANT_LIGHT_BRIGHTNESS);
      parameters.mColor      = data->GetVector(IDD_DISTANT_LIGHT_COLOR);
      break;
    case IDD_LIGHT_TYPE_AREA:
      parameters.mBrightness     = data->GetReal  (IDD_AREA_LIGHT_BRIGHTNESS);
      parameters.mColor          = data->GetVector(IDD_AREA_LIGHT_COLOR);
      parameters.mFlippedNormals = data->GetBool  (IDD_AREA_LIGHT_FLIP_NORMALS);
      parameters.mShapeType      = data->GetLong  (IDD_AREA_LIGHT_SHAPE);
      parameters.mShapeObject    = (PolygonObject*)data->GetLink(IDD_AREA_LIGHT_OBJECT,
                                                                 document,
                                                                 Opolygon);
      parameters.mShapeSize      = data->GetVector(IDD_AREA_LIGHT_SIZE);
      parameters.mSamples        = data->GetLong  (IDD_AREA_LIGHT_SAMPLES);
      break;
    case IDD_LIGHT_TYPE_SUN:
      parameters.mBrightness = data->GetReal(IDD_SUN_LIGHT_BRIGHTNESS);
      parameters.mSamples    = data->GetLong(IDD_SUN_LIGHT_SAMPLES);
      parameters.mRelSize    = data->GetReal(IDD_SUN_LIGHT_RELSIZE);
      parameters.mTurbidity  = data->GetReal(IDD_SUN_LIGHT_TURBIDITY);
      break;
    case IDD_LIGHT_TYPE_SKY:
      parameters.mBrightness = data->GetReal(IDD_SKY_LIGHT_BRIGHTNESS);
      parameters.mSamples    = data->GetLong(IDD_SKY_LIGHT_SAMPLES);
      parameters.mTurbidity  = data->GetReal(IDD_SKY_LIGHT_TURBIDITY);
      parameters.mAdvanced   = data->GetBool(IDD_SKY_LIGHT_ADVANCED);
      parameters.mA          = data->GetReal(IDD_SKY_LIGHT_A_CONST);
      parameters.mB          = data->GetReal(IDD_SKY_LIGHT_B_CONST);
      parameters.mC          = data->GetReal(IDD_SKY_LIGHT_C_CONST);
      parameters.mD          = data->GetReal(IDD_SKY_LIGHT_D_CONST);
      parameters.mE          = data->GetReal(IDD_SKY_LIGHT_E_CONST);
      break;
    case IDD_LIGHT_TYPE_SUNSKY:
      parameters.mBrightness = data->GetReal(IDD_SUNSKY_LIGHT_BRIGHTNESS);
      parameters.mSamples    = data->GetLong(IDD_SUNSKY_LIGHT_SAMPLES);
      parameters.mRelSize    = data->GetReal(IDD_SUNSKY_LIGHT_RELSIZE);
      parameters.mTurbidity  = data->GetReal(IDD_SUNSKY_LIGHT_TURBIDITY);
      parameters.mAdvanced   = data->GetBool(IDD_SUNSKY_LIGHT_ADVANCED);
      parameters.mA          = data->GetReal(IDD_SUNSKY_LIGHT_A_CONST);
      parameters.mB          = data->GetReal(IDD_SUNSKY_LIGHT_B_CONST);
      parameters.mC          = data->GetReal(IDD_SUNSKY_LIGHT_C_CONST);
      parameters.mD          = data->GetReal(IDD_SUNSKY_LIGHT_D_CONST);
      parameters.mE          = data->GetReal(IDD_SUNSKY_LIGHT_E_CONST);
      break;
    default:
      ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::getLightParameters(): invalid light type determined from light tag");
  }

  // if we should not ignore the C4D falloff, apply it to brightness
  if (!data->GetBool(IDD_LIGHT_IGNORE_FALLOFF_RADIUS)) {
    parameters.mBrightness *= falloffCorrection;
  }

  return TRUE;
}


/// First initialisation of the tag. When this function is called, the tag is
/// not yet assigned to the object and we can't copy the object settings over
/// to the tag yet. That has to be done later. But we can at least set the
/// default values.
///
/// @param[in]  node
///   The tag data node, created by C4D for storing the tag settings.
/// @return
///   TRUE if the initialisation was successful.
Bool LuxC4DLightTag::Init(GeListNode* node)
{
  // obtain container from data node.
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // set light type and falloff flag
  data->SetLong(IDD_LIGHT_TYPE,                  IDD_LIGHT_TYPE_AS_OBJECT);
  data->SetBool(IDD_LIGHT_IGNORE_FALLOFF_RADIUS, FALSE);

  // set point light defaults
  data->SetVector(IDD_POINT_LIGHT_COLOR,      Vector(1.0));
  data->SetReal  (IDD_POINT_LIGHT_BRIGHTNESS, 1.0);

  // set spot light defaults
  data->SetVector(IDD_SPOT_LIGHT_COLOR,      Vector(1.0));
  data->SetReal  (IDD_SPOT_LIGHT_BRIGHTNESS, 1.0);
  data->SetReal  (IDD_SPOT_LIGHT_INNERANGLE, Rad(50.0));
  data->SetReal  (IDD_SPOT_LIGHT_OUTERANGLE, Rad(60.0));

  // set distant light defaults
  data->SetVector(IDD_DISTANT_LIGHT_COLOR,      Vector(1.0));
  data->SetReal  (IDD_DISTANT_LIGHT_BRIGHTNESS, 1.0);

  // set area light defaults
  data->SetVector(IDD_AREA_LIGHT_COLOR,        Vector(1.0));
  data->SetReal  (IDD_AREA_LIGHT_BRIGHTNESS,   1.0);
  data->SetBool  (IDD_AREA_LIGHT_FLIP_NORMALS, FALSE);
  data->SetLong  (IDD_AREA_LIGHT_SAMPLES,      1);
  data->SetLong  (IDD_AREA_LIGHT_SHAPE,        IDD_AREA_LIGHT_SHAPE_RECTANGLE);
  data->SetVector(IDD_AREA_LIGHT_SIZE,         Vector(100.0));

  // set sun light defaults
  data->SetReal(IDD_SUN_LIGHT_BRIGHTNESS, 1.0);
  data->SetLong(IDD_SUN_LIGHT_SAMPLES,    1);
  data->SetReal(IDD_SUN_LIGHT_TURBIDITY,  2.0);
  data->SetReal(IDD_SUN_LIGHT_RELSIZE,    1.0);

  // set sky light defaults
  data->SetReal(IDD_SKY_LIGHT_BRIGHTNESS, 1.0);
  data->SetLong(IDD_SKY_LIGHT_SAMPLES,    1);
  data->SetReal(IDD_SKY_LIGHT_TURBIDITY,  2.0);
  data->SetBool(IDD_SKY_LIGHT_ADVANCED,   FALSE);
  data->SetReal(IDD_SKY_LIGHT_A_CONST,    1.0);
  data->SetReal(IDD_SKY_LIGHT_B_CONST,    1.0);
  data->SetReal(IDD_SKY_LIGHT_C_CONST,    1.0);
  data->SetReal(IDD_SKY_LIGHT_D_CONST,    1.0);
  data->SetReal(IDD_SKY_LIGHT_E_CONST,    1.0);

  // set sun-sky light defaults
  data->SetReal(IDD_SUNSKY_LIGHT_BRIGHTNESS, 1.0);
  data->SetLong(IDD_SUNSKY_LIGHT_SAMPLES,    1);
  data->SetReal(IDD_SUNSKY_LIGHT_TURBIDITY,  2.0);
  data->SetReal(IDD_SUNSKY_LIGHT_RELSIZE,    1.0);
  data->SetBool(IDD_SUNSKY_LIGHT_ADVANCED,   FALSE);
  data->SetReal(IDD_SUNSKY_LIGHT_A_CONST,    1.0);
  data->SetReal(IDD_SUNSKY_LIGHT_B_CONST,    1.0);
  data->SetReal(IDD_SUNSKY_LIGHT_C_CONST,    1.0);
  data->SetReal(IDD_SUNSKY_LIGHT_D_CONST,    1.0);
  data->SetReal(IDD_SUNSKY_LIGHT_E_CONST,    1.0);

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
Bool LuxC4DLightTag::GetDDescription(GeListNode*  node,
                                     Description* description,
                                     LONG&        flags)
{
  // get tag list node
  BaseTag* tag = (BaseTag*)node;
  if (!tag)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::GetDDescription(): no pointer to tag list node passed");

  // get object the tag is assigned to
  BaseObject* object = tag->GetObject();
  if (!object)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::GetDDescription(): no assigned object found");

  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  // if tag wasn't initialised yet, do it now
  if (!data->GetBool(IDD_LIGHT_TAG_OBJECT_SETTINGS_COPIED, FALSE)) {
    if (!copyFromObject(TRUE))  return FALSE;
  }

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::GetDDescription(): could not load description for node");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::GetDDescription(): could not allocate empty AtomArray");
  }

  // show/hide parameters depending on light type
  LONG lightType = getLightType();
  showParameter(description, IDG_POINT_LIGHT,     params, lightType == IDD_LIGHT_TYPE_POINT);
  showParameter(description, IDG_SPOT_LIGHT,      params, lightType == IDD_LIGHT_TYPE_SPOT);
  showParameter(description, IDG_DISTANT_LIGHT,   params, lightType == IDD_LIGHT_TYPE_DISTANT);
  showParameter(description, IDG_AREA_LIGHT,      params, lightType == IDD_LIGHT_TYPE_AREA);
  showParameter(description, IDG_SUN_LIGHT,       params, lightType == IDD_LIGHT_TYPE_SUN);
  showParameter(description, IDG_SKY_LIGHT,       params, lightType == IDD_LIGHT_TYPE_SKY);
  showParameter(description, IDG_SUNSKY_LIGHT,    params, lightType == IDD_LIGHT_TYPE_SUNSKY);

  // show falloff toggle depending on the falloff type
  LONG falloffType = getParameterLong(*object, LIGHT_DETAILS_FALLOFF);
  showParameter(description, IDD_LIGHT_IGNORE_FALLOFF_RADIUS, params,
                (falloffType == LIGHT_DETAILS_FALLOFF_INVERSE) ||
                (falloffType == LIGHT_DETAILS_FALLOFF_INVERSESQUARE));

  // show/hide advanced settings for sky light
  if (lightType == IDD_LIGHT_TYPE_SKY) {
    Bool advanced = data->GetBool(IDD_SKY_LIGHT_ADVANCED);
    showParameter(description, IDD_SKY_LIGHT_A_CONST, params, advanced);
    showParameter(description, IDD_SKY_LIGHT_B_CONST, params, advanced);
    showParameter(description, IDD_SKY_LIGHT_C_CONST, params, advanced);
    showParameter(description, IDD_SKY_LIGHT_D_CONST, params, advanced);
    showParameter(description, IDD_SKY_LIGHT_E_CONST, params, advanced);
  // show/hide advanced settings for sun-sky light
  } else if (lightType == IDD_LIGHT_TYPE_SUNSKY) {
    Bool advanced = data->GetBool(IDD_SUNSKY_LIGHT_ADVANCED);
    showParameter(description, IDD_SUNSKY_LIGHT_A_CONST, params, advanced);
    showParameter(description, IDD_SUNSKY_LIGHT_B_CONST, params, advanced);
    showParameter(description, IDD_SUNSKY_LIGHT_C_CONST, params, advanced);
    showParameter(description, IDD_SUNSKY_LIGHT_D_CONST, params, advanced);
    showParameter(description, IDD_SUNSKY_LIGHT_E_CONST, params, advanced);
  }

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return TRUE;
}


/// Callback function that used to pass messages to the plugin. We use it to
/// catch a click on the button "Copy from Object".
///
/// @param[in]  node
///   The GeListNode associated with the tag data.
/// @param[in]  type
///   The message ID that describes the type of the message.
/// @param[in]  data
///   Pointer to optional arguments passed along with the message.
/// @return
///   Always returns TRUE.
Bool LuxC4DLightTag::Message(GeListNode *node,
                             LONG       type,
                             void       *data)
{
  if ((type != MSG_DESCRIPTION_COMMAND) || !data) {
    return TRUE;
  }
  switch (((DescriptionCommand*)data)->id[0].id) {
    case IDB_COPY_PARAMETERS:
      copyFromObject(FALSE);
      break;
  }
  return TRUE;
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DLightTag.
 *****************************************************************************/


/// Returns the data container of this node.
BaseContainer* LuxC4DLightTag::getData(void)
{
  BaseTag* tag = static_cast<BaseTag*>(Get());
  BaseContainer* data = 0;
  if (tag) {
    data = tag->GetDataInstance();
  }
  if (!data)  ERRLOG_RETURN_VALUE(NULL, "LuxC4DLightTag::getData(): could not obtain base container");
  return data;
}


/// Static function that determines the Lux light type from a C4D light.
///
/// @param[in]  lightObject
///   Reference to the C4D light object.
/// @return
///   One of the Lux light types, specified in "tluxc4dlighttag.h") (e.g.
///   IDD_LIGHT_TYPE_POINT) or -1, if we couldn't determine it.
LONG LuxC4DLightTag::getLightType(BaseObject& lightObject)
{
  switch (getParameterLong(lightObject, LIGHT_TYPE, -1)) {
    case LIGHT_TYPE_OMNI:
      return IDD_LIGHT_TYPE_POINT;
    case LIGHT_TYPE_SPOT:
    case LIGHT_TYPE_SPOTRECT:
      return IDD_LIGHT_TYPE_SPOT;
    case LIGHT_TYPE_DISTANT:
    case LIGHT_TYPE_PARALLEL:
    case LIGHT_TYPE_PARSPOT:
    case LIGHT_TYPE_PARSPOTRECT:
      return IDD_LIGHT_TYPE_DISTANT;
    case LIGHT_TYPE_AREA:
      return IDD_LIGHT_TYPE_AREA;
    default:
      ERRLOG_RETURN_VALUE(-1, "LuxC4DLightTag::getLightType(): unknown light type found in assigned object");
  }
}


/// Determines the Lux light type from the light tag. If the setting is
/// "Same as Object", we convert the C4D light type into the Lux light type.
///
/// @return
///   One of the Lux light types, specified in "tluxc4dlighttag.h") (e.g.
///   IDD_LIGHT_TYPE_POINT) or -1, if we couldn't determine it.
LONG LuxC4DLightTag::getLightType(void)
{
  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  ERRLOG_RETURN_VALUE(-1, "LuxC4DLightTag::GetLightType(): could not obtain tag data container");

  // get tag list node
  BaseTag* tag = static_cast<BaseTag*>(Get());
  if (!tag)  ERRLOG_RETURN_VALUE(-1, "LuxC4DLightTag::GetLightType(): could not obtain tag list node");

  // get object the tag is assigned to
  BaseObject* object = tag->GetObject();
  if (!object)  ERRLOG_RETURN_VALUE(-1, "LuxC4DLightTag::GetLightType(): no assigned object found");

  // if light type in tag is depending on light type of object, determine
  // Lux light type from it
  LONG lightType = data->GetLong(IDD_LIGHT_TYPE);
  if (lightType == IDD_LIGHT_TYPE_AS_OBJECT) {
    return getLightType(*object);
  }

  // ... otherwise just return the selected light type of tag
  return lightType;
}


/// Converts a C4D light shape type into a Lux shape type. This is done to
/// become independant of future changes in C4D area lights.
///
/// @param[in]  c4dShapeType
///   The ID of the C4D area light shape.
/// @return
///   The ID of the corresponding Lux area light shape.
LONG LuxC4DLightTag::c4dShape2LuxShape(LONG c4dShapeType)
{
  switch (c4dShapeType) {
    case LIGHT_AREADETAILS_SHAPE_DISC:        return IDD_AREA_LIGHT_SHAPE_DISC;
    case LIGHT_AREADETAILS_SHAPE_RECTANGLE:   return IDD_AREA_LIGHT_SHAPE_RECTANGLE;
    case LIGHT_AREADETAILS_SHAPE_SPHERE:      return IDD_AREA_LIGHT_SHAPE_SPHERE;
    case LIGHT_AREADETAILS_SHAPE_CYLINDER:    return IDD_AREA_LIGHT_SHAPE_CYLINDER;
    case LIGHT_AREADETAILS_SHAPE_CUBE:        return IDD_AREA_LIGHT_SHAPE_CUBE;
    case LIGHT_AREADETAILS_SHAPE_HEMISPHERE:  return IDD_AREA_LIGHT_SHAPE_HEMISPHERE;
    case LIGHT_AREADETAILS_SHAPE_OBJECT:      return IDD_AREA_LIGHT_SHAPE_OBJECT;
    default:                                  return IDD_AREA_LIGHT_SHAPE_RECTANGLE;
  }
}


/// Helper function to copy all available and usable settings from the C4D
/// light object into the tag. This can be used to reset either the currently
/// selected or all Lux light parameters.
/// 
/// @param[in]  allTypes
///   If set to TRUE, all Lux light parameters (including the invisible ones)
///   that have a representation in the C4D light will be copied. If set FALSE
///   only the currently visible settings.
/// @return
///   TRUE if executed successfully, otherwise FALSE.
Bool LuxC4DLightTag::copyFromObject(Bool allTypes)
{
  // get container for easy access to current values
  BaseContainer* data = getData();
  if (!data)  return -1;

  // get light type
  LONG lightType = getLightType();

  // get tag list node
  BaseTag* tag = static_cast<BaseTag*>(Get());
  if (!tag)  ERRLOG_RETURN_VALUE(-1, "LuxC4DLightTag::CopyFromObject(): could not obtain tag list node");

  // get object the tag is assigned to and its settings
  BaseObject* object = tag->GetObject();
  if (!object)  ERRLOG_RETURN_VALUE(FALSE, "LuxC4DLightTag::CopyFromObject(): no assigned object found");

  // copy point light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_POINT)) {
    data->SetVector(IDD_POINT_LIGHT_COLOR,      getParameterVector(*object, LIGHT_COLOR));
    data->SetReal  (IDD_POINT_LIGHT_BRIGHTNESS, getParameterReal  (*object, LIGHT_BRIGHTNESS));
  }

  // copy spot light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_SPOT)) {
    data->SetVector(IDD_SPOT_LIGHT_COLOR,      getParameterVector(*object, LIGHT_COLOR));
    data->SetReal  (IDD_SPOT_LIGHT_BRIGHTNESS, getParameterReal  (*object, LIGHT_BRIGHTNESS));
    data->SetReal  (IDD_SPOT_LIGHT_INNERANGLE, getParameterReal  (*object, LIGHT_DETAILS_INNERANGLE));
    data->SetReal  (IDD_SPOT_LIGHT_OUTERANGLE, getParameterReal  (*object, LIGHT_DETAILS_OUTERANGLE));
  }

  // copy distant light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_DISTANT)) {
    data->SetVector(IDD_DISTANT_LIGHT_COLOR,      getParameterVector(*object, LIGHT_COLOR));
    data->SetReal  (IDD_DISTANT_LIGHT_BRIGHTNESS, getParameterReal  (*object, LIGHT_BRIGHTNESS));
  }

  // copy area light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_AREA)) {
    data->SetVector(IDD_AREA_LIGHT_COLOR,      getParameterVector(*object, LIGHT_COLOR));
    data->SetReal  (IDD_AREA_LIGHT_BRIGHTNESS, getParameterReal  (*object, LIGHT_BRIGHTNESS));
    data->SetVector(IDD_AREA_LIGHT_SIZE,       Vector(getParameterReal(*object, LIGHT_AREADETAILS_SIZEX),
                                                      getParameterReal(*object, LIGHT_AREADETAILS_SIZEY),
                                                      getParameterReal(*object, LIGHT_AREADETAILS_SIZEZ)));
    LONG shape = getParameterLong(*object, LIGHT_AREADETAILS_SHAPE);
    data->SetLong  (IDD_AREA_LIGHT_SHAPE,      c4dShape2LuxShape(shape));
    if (shape == LIGHT_AREADETAILS_SHAPE_OBJECT) {
      data->SetLink(IDD_AREA_LIGHT_OBJECT, getParameterLink (*object, LIGHT_AREADETAILS_OBJECT));
    }
  }

  // copy sun light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_SUN)) {
    data->SetReal(IDD_SUN_LIGHT_BRIGHTNESS, getParameterReal(*object, LIGHT_BRIGHTNESS));
  }

  // copy sky light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_SKY)) {
    data->SetReal(IDD_SKY_LIGHT_BRIGHTNESS, getParameterReal(*object, LIGHT_BRIGHTNESS));
  }

  // copy sun-sky light settings
  if (allTypes || (lightType == IDD_LIGHT_TYPE_SUNSKY)) {
    data->SetReal(IDD_SUNSKY_LIGHT_BRIGHTNESS, getParameterReal(*object, LIGHT_BRIGHTNESS));
  }

  // set flag that we have copied the object settings
  data->SetBool(IDD_LIGHT_TAG_OBJECT_SETTINGS_COPIED, TRUE);

  return TRUE;
}
