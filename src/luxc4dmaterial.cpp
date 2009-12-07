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

#include <customgui_matpreview.h>

#include "c4d_symbols.h"
#include "luxc4dmaterial.h"
#include "mluxc4dmaterial.h"
#include "utilities.h"



/*****************************************************************************
 * Implementation of public member functions of class LuxC4DMaterial.
 *****************************************************************************/

///
NodeData* LuxC4DMaterial::alloc(void)
{
  return gNewNC LuxC4DMaterial;
}


///
Bool LuxC4DMaterial::registerPlugin(void)
{
  return RegisterMaterialPlugin(PID_LUXC4D_MATERIAL,
                                GeLoadString(IDS_LUXC4D_MATERIAL),
                                0,
                                LuxC4DMaterial::alloc,
                                "Mluxc4dmaterial",
                                0);
}


///
Bool LuxC4DMaterial::Init(GeListNode* node)
{
  // initialise update counter
  mUpdateCount = 0;

  // obtain container from node
  BaseContainer* data = getData();
  if (!data)  return FALSE;

  data->SetLong  (IDD_METAL_TYPE,                         IDD_METAL_TYPE_ALUMINIUM);
  data->SetVector(IDD_DIFFUSE_COLOR,                      Vector(0.8));
  data->SetReal  (IDD_DIFFUSE_SHADER_STRENGTH,            1.0);
  data->SetReal  (IDD_DIFFUSE_BRIGHTNESS,                 1.0);
  data->SetReal  (IDD_DIFFUSE_SIGMA_VALUE,                0.0);
  data->SetVector(IDD_REFLECTION_COLOR,                   Vector(0.5));
  data->SetReal  (IDD_REFLECTION_SHADER_STRENGTH,         1.0);
  data->SetReal  (IDD_REFLECTION_BRIGHTNESS,              1.0);
  data->SetVector(IDD_SPECULAR_COLOR,                     Vector(0.5));
  data->SetReal  (IDD_SPECULAR_SHADER_STRENGTH,           1.0);
  data->SetReal  (IDD_SPECULAR_BRIGHTNESS,                1.0);
  data->SetReal  (IDD_SPECULAR_IOR_VALUE,                 0.0);
  data->SetVector(IDD_CARPAINT_SPECULAR_COLOR1,           Vector(0.0003, 0.0003, 0.0003));
  data->SetReal  (IDD_CARPAINT_SPECULAR_SHADER_STRENGTH1, 1.0);
  data->SetReal  (IDD_CARPAINT_SPECULAR_BRIGHTNESS1,      1.0);
  data->SetReal  (IDD_CARPAINT_R1,                        0.049);
  data->SetReal  (IDD_CARPAINT_M1,                        1.0);
  data->SetVector(IDD_CARPAINT_SPECULAR_COLOR2,           Vector(0.013, 0.011, 0.0083));
  data->SetReal  (IDD_CARPAINT_SPECULAR_SHADER_STRENGTH2, 1.0);
  data->SetReal  (IDD_CARPAINT_SPECULAR_BRIGHTNESS2,      1.0);
  data->SetReal  (IDD_CARPAINT_R2,                        0.45);
  data->SetReal  (IDD_CARPAINT_M2,                        0.15);
  data->SetVector(IDD_CARPAINT_SPECULAR_COLOR3,           Vector(0.049, 0.042, 0.037));
  data->SetReal  (IDD_CARPAINT_SPECULAR_SHADER_STRENGTH3, 1.0);
  data->SetReal  (IDD_CARPAINT_SPECULAR_BRIGHTNESS3,      1.0);
  data->SetReal  (IDD_CARPAINT_R3,                        0.17);
  data->SetReal  (IDD_CARPAINT_M3,                        0.015);
  data->SetVector(IDD_COATING_ABSORPTION_COLOR,           Vector(0.0));
  data->SetReal  (IDD_COATING_ABSORPTION_SHADER_STRENGTH, 1.0);
  data->SetReal  (IDD_COATING_ABSORPTION_STRENGTH,        1.0);
  data->SetReal  (IDD_COATING_ABSORPTION_DEPTH_VALUE,     0.0);
  data->SetVector(IDD_TRANSMISSION_COLOR,                 Vector(0.8));
  data->SetReal  (IDD_TRANSMISSION_SHADER_STRENGTH,       1.0);
  data->SetReal  (IDD_TRANSMISSION_STRENGTH,              1.0);
  data->SetBool  (IDD_TRANSMISSION_ARCHITECTURAL,         FALSE);
  data->SetReal  (IDD_TRANSMISSION_CAUCHYB,               0.0);
  data->SetReal  (IDD_IOR_VALUE,                          1.5);
  data->SetBool  (IDD_ROUGHNESS_ASYMETRIC,                FALSE);
  data->SetReal  (IDD_ROUGHNESS_VALUE,                    0.1);
  data->SetReal  (IDD_ROUGHNESS_U_VALUE,                  0.1);
  data->SetReal  (IDD_ROUGHNESS_V_VALUE,                  0.1);
  data->SetReal  (IDD_THIN_FILM_THICKNESS,                500.0);
  data->SetReal  (IDD_THIN_FILM_IOR,                      1.5);
  data->SetReal  (IDD_BUMP_HEIGHT,                        1.0);
  data->SetVector(IDD_EMISSION_COLOR,                     Vector(1.0));
  data->SetReal  (IDD_EMISSION_SHADER_STRENGTH,           1.0);
  data->SetReal  (IDD_EMISSION_BRIGHTNESS,                1.0);

  return TRUE;
}


///
Bool LuxC4DMaterial::GetDDescription(GeListNode*  node,
                                     Description* description,
                                     LONG&        flags)
{
  // obtain container from node
  BaseContainer* data = getData();
  if (!data) { return FALSE; }

  // load the description from resource file
  if (!description->LoadDescription(node->GetType())) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DMaterial::GetDDescription(): could not load description for node");
  }

  // allocate empty atom array, which can be used for passing parameters into GetParameterI()
  AutoAlloc<AtomArray> params;
  if (!params) {
    ERRLOG_RETURN_VALUE(FALSE, "LuxC4DMaterial::GetDDescription(): could not allocate empty AtomArray");
  }

  // get material type and enable channels depending on it
  LONG materialType = data->GetLong(IDD_MATERIAL_TYPE);
  switch (materialType) {
    case IDD_MATERIAL_TYPE_GLASS:
      toggleChannel(IDD_TOGGLE_METAL,              IDG_METAL,              FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_DIFFUSE,            IDG_DIFFUSE,            FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_REFLECTION,         IDG_REFLECTION,         TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_SPECULAR,           IDG_SPECULAR,           FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR1, IDG_CARPAINT_SPECULAR1, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR2, IDG_CARPAINT_SPECULAR2, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR3, IDG_CARPAINT_SPECULAR3, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_COATING_ABSORPTION, IDG_COATING_ABSORPTION, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_TRANSMISSION,       IDG_TRANSMISSION,       TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_THIN_FILM,          IDG_THIN_FILM,          TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_ROUGHNESS,          IDG_ROUGHNESS,          FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_BUMP,               IDG_BUMP,               TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_EMISSION,           IDG_EMISSION,           TRUE,  data, description, params);
      break;
    case IDD_MATERIAL_TYPE_ROUGH_GLASS:
      break;
    case IDD_MATERIAL_TYPE_GLOSSY:
      break;
    case IDD_MATERIAL_TYPE_MATTE:
      showParameter(description, IDG_SIGMA, params, TRUE);
      toggleChannel(IDD_TOGGLE_METAL,              IDG_METAL,              FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_DIFFUSE,            IDG_DIFFUSE,            TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_REFLECTION,         IDG_REFLECTION,         FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_SPECULAR,           IDG_SPECULAR,           FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR1, IDG_CARPAINT_SPECULAR1, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR2, IDG_CARPAINT_SPECULAR2, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR3, IDG_CARPAINT_SPECULAR3, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_COATING_ABSORPTION, IDG_COATING_ABSORPTION, FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_TRANSMISSION,       IDG_TRANSMISSION,       FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_THIN_FILM,          IDG_THIN_FILM,          FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_ROUGHNESS,          IDG_ROUGHNESS,          FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_BUMP,               IDG_BUMP,               TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_EMISSION,           IDG_EMISSION,           TRUE,  data, description, params);
      break;
    case IDD_MATERIAL_TYPE_MATTE_TRANSLUCENT:
      break;
    case IDD_MATERIAL_TYPE_METAL:
      break;
    case IDD_MATERIAL_TYPE_SHINY_METAL:
      break;
    case IDD_MATERIAL_TYPE_MIRROR:
      break;
    case IDD_MATERIAL_TYPE_CAR_PAINT:
      showParameter(description, IDG_SIGMA, params, FALSE);
      toggleChannel(IDD_TOGGLE_METAL,              IDG_METAL,              FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_DIFFUSE,            IDG_DIFFUSE,            TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_REFLECTION,         IDG_REFLECTION,         FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_SPECULAR,           IDG_SPECULAR,           FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR1, IDG_CARPAINT_SPECULAR1, TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR2, IDG_CARPAINT_SPECULAR2, TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_CARPAINT_SPECULAR3, IDG_CARPAINT_SPECULAR3, TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_COATING_ABSORPTION, IDG_COATING_ABSORPTION, TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_TRANSMISSION,       IDG_TRANSMISSION,       FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_THIN_FILM,          IDG_THIN_FILM,          FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_ROUGHNESS,          IDG_ROUGHNESS,          FALSE, data, description, params);
      toggleChannel(IDD_TOGGLE_BUMP,               IDG_BUMP,               TRUE,  data, description, params);
      toggleChannel(IDD_TOGGLE_EMISSION,           IDG_EMISSION,           TRUE,  data, description, params);
      break;
  }

  // set flag and return
  flags |= DESCFLAGS_DESC_LOADED;
  return SUPER::GetDDescription(node, description, flags);
}


///
Bool LuxC4DMaterial::GetDParameter(GeListNode*   node,
                                   const DescID& id,
                                   GeData&       value,
                                   LONG&         flags)
{
  // get containter from node
  BaseContainer* data = ((PluginMaterial*)node)->GetDataInstance();

  // special handling for some parameters:
  switch (id[0].id) {
    case MATERIAL_PREVIEW:
      // setup material preview setup itself
      return GetDParameterPreview(data,
                                  &value,
                                  flags,
                                  MATERIAL_PREVIEW,
                                  mUpdateCount,
                                  (BaseMaterial*)node);
  }

  return SUPER::GetDParameter(node, id, value, flags);
}


///
Bool LuxC4DMaterial::SetDParameter(GeListNode*   node,
                                   const DescID& id,
                                   const GeData& value,
                                   LONG&         flags)
{
  // everytime this function is called, something has changed -> keep track of
  // it in counter
  ++mUpdateCount;

  // get containter from node
  BaseContainer *data = ((PluginMaterial*)node)->GetDataInstance();

  // special handling for some parameters:
  switch (id[0].id) {
    case MATERIAL_PREVIEW:
      // send data to material preview
      return SetDParameterPreview(data, &value, flags, MATERIAL_PREVIEW);
  }

  return SUPER::SetDParameter(node, id, value, flags);
}


///
Bool LuxC4DMaterial::Message(GeListNode* node,
                             LONG        type,
                             void*       data)
{
  // keep track of update messages which is usually sent after a parameter change
  if (type == MSG_UPDATE) {
    ++mUpdateCount;
  }

  return TRUE;
}


///
Bool LuxC4DMaterial::CopyTo(NodeData*   dest,
                            GeListNode* srcNode,
                            GeListNode* destNode,
                            LONG        flags,
                            AliasTrans* aliasTrans)
{
  ((LuxC4DMaterial*)dest)->mUpdateCount = mUpdateCount;
  return NodeData::CopyTo(dest, srcNode, destNode, flags, aliasTrans);
}


/// Is called only once per rendering - even if there are several instances of
/// this material.
LONG LuxC4DMaterial::InitRender(PluginMaterial*   mat,
                                InitRenderStruct* irs)
{
  // store material colour
  //BaseContainer *data = mat->GetDataInstance();
  //mColour = data->GetVector(IDD_MATTE_COLOUR);

  //// check if there is a VolumeData available
  //if (!irs || !irs->vd)  return LOAD_UNKNOWN;

  //// store lights and area lights
  //LONG lightCount = irs->vd->GetLightCount();
  //mStandardLights.init(0, lightCount >> 1);
  //mAreaLights.init(0, lightCount >> 1);
  //for (LONG lightIndex=0; lightIndex<lightCount; ++lightIndex) {
  //  RayLight* light = irs->vd->GetLight(lightIndex);
  //  if (light && light->calc_illum && (light->color != 0.0)) {
  //    if (light->type == RT_LT_AREA) {
  //      mAreaLights.push(light);
  //    } else {
  //      mStandardLights.push(light);
  //    }
  //  }
  //}

  return LOAD_OK;
}


///
void LuxC4DMaterial::FreeRender(PluginMaterial* mat)
{
  //mStandardLights.erase();
  //mAreaLights.erase();
}


///
void LuxC4DMaterial::CalcSurface(PluginMaterial* mat,
                                 VolumeData*     volumeData)
{
  //Vector lightColour, lightVector;

  //// loop over all standard lights and accumulate Oren-Nayar radiance
  //volumeData->col = 0.0;
  //for (SizeT light=0; light<mStandardLights.size(); ++light) {
  //  if (volumeData->IlluminateSurfacePoint(mStandardLights[light],
  //                                         &lightColour,
  //                                         &lightVector,
  //                                         volumeData->p,
  //                                         volumeData->bumpn,
  //                                         volumeData->n,
  //                                         volumeData->orign,
  //                                         SV(volumeData->ray->v),
  //                                         volumeData->calc_shadow,
  //                                         volumeData->lhit,
  //                                         volumeData->raybits,
  //                                         TRUE))
  //  {
  //    volumeData->col += (mColour^lightColour)*(volumeData->bumpn*lightVector);
  //  }
  //}

  //// loop over area lights and accumulate estimated Oren-Nayer radiance
  //Vector areaDiffuse, areaSpecular;
  //for (SizeT light=0; light<mAreaLights.size(); ++light) {
  //  volumeData->CalcArea(mAreaLights[light],
  //                       FALSE,
  //                       TRUE,
  //                       0.0,
  //                       SV(volumeData->ray->v),
  //                       volumeData->p,
  //                       volumeData->bumpn,
  //                       volumeData->orign,
  //                       volumeData->raybits,
  //                       &areaDiffuse,
  //                       &areaSpecular);
  //  volumeData->col += areaDiffuse^mColour;
  //}

  //// lower clipping due to negative lights
  //if (volumeData->col.x < 0)  volumeData->col.x = 0.0;
  //if (volumeData->col.y < 0)  volumeData->col.y = 0.0;
  //if (volumeData->col.z < 0)  volumeData->col.z = 0.0;
}


LuxMaterialDataH LuxC4DMaterial::getLuxMaterialData(void)
{
  // obtain container from node
  BaseContainer* data = getData();
  if (!data) { return FALSE; }

  // get material type and enable channels depending on it
  LONG materialType = data->GetLong(IDD_MATERIAL_TYPE);
  switch (materialType) {
    case IDD_MATERIAL_TYPE_GLASS:
      {
        LuxGlassDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_ROUGH_GLASS:
      {
        LuxRoughGlassDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_GLOSSY:
      {
        LuxRoughGlassDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_MATTE:
      {
        LuxRoughGlassDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_MATTE_TRANSLUCENT:
      {
        LuxMatteTranslucentDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_METAL:
      {
        LuxMetalDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_SHINY_METAL:
      {
        LuxShinyMetalDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_MIRROR:
      {
        LuxMirrorDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
    case IDD_MATERIAL_TYPE_CAR_PAINT:
      {
        LuxCarPaintDataH materialData;
        if (materialData.allocate()) {
          // TODO ...
        }
        return materialData;
      }
  }
  
  return LuxMaterialDataH();
}



/*****************************************************************************
 * Implementation of private member functions of class LuxC4DMaterial.
 *****************************************************************************/

/// Returns the data container of this node.
BaseContainer* LuxC4DMaterial::getData(void)
{
  BaseContainer* data = 0;
  BaseList2D* listNode = static_cast<BaseList2D*>(Get());
  if (listNode) {
    data = listNode->GetDataInstance();
  }
  if (!data) {
    ERRLOG_RETURN_VALUE(NULL, "LuxC4DMaterial::getData(): could not obtain base container");
  }
  return data;
}


///
void LuxC4DMaterial::toggleChannel(LONG           channelToggleId,
                                   LONG           channelGroupId,
                                   Bool           enabled,
                                   BaseContainer* data,
                                   Description*   description,
                                   AtomArray*     params)
{
  if (!enabled) {
    showParameter(description, channelToggleId, params, FALSE);
    showParameter(description, channelGroupId,  params, FALSE);
  } else {
    showParameter(description, channelToggleId, params, TRUE);
    showParameter(description, channelGroupId,  params, data->GetBool(channelToggleId));
  }
}
