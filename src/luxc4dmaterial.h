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

#ifndef __LUXC4DMATERIAL_H__
#define __LUXC4DMATERIAL_H__ 1



#include <c4d.h>
#include "common.h"
#include "luxmaterialdata.h"



#define PID_LUXC4D_MATERIAL   1023559



/***************************************************************************//*!
*//****************************************************************************/
class LuxC4DMaterial : public MaterialData
{
  INSTANCEOF(LuxC4DMaterial, MaterialData)


public:

  static NodeData* alloc(void);
  static Bool registerPlugin(void);

  virtual Bool Init(GeListNode* node);
  virtual Bool GetDDescription(GeListNode*     node,
                               Description*    description,
                               DESCFLAGS_DESC& flags);
  virtual Bool GetDParameter(GeListNode*    node,
                             const DescID&  id,
                             GeData&        value,
                             DESCFLAGS_GET& flags);
  virtual Bool SetDParameter(GeListNode*    node,
                             const DescID&  id,
                             const GeData&  value,
                             DESCFLAGS_SET& flags);
  virtual Bool Message(GeListNode* node,
                       LONG        type,
                       void*       data);
  virtual Bool CopyTo(NodeData*   dest,
                      GeListNode* srcNode,
                      GeListNode* destNode,
                      COPYFLAGS   flags,
                      AliasTrans* aliasTrans);

  LuxMaterialDataH getLuxMaterialData(LuxTextureMappingH mapping,
                                      LReal              c4d2LuxScale,
                                      Real               colorGamma,
                                      Real               textureGamma,
                                      Real               bumpSampleDistance);


private:

  LONG mUpdateCount;


  BaseContainer* getData(void);
  void toggleChannel(LONG           channelToggleId,
                     LONG           channelGroupId,
                     Bool           enabled,
                     BaseContainer* data,
                     Description*   description,
                     AtomArray*     params);

  LuxTextureDataH getTextureFromShader(BaseContainer&            data,
                                       LONG                      shaderId,
                                       LuxTextureType            textureType,
                                       LuxTextureMappingH        mapping,
                                       LuxFloat                  textureGamma,
                                       LuxImageMapData::Channel  channel = LuxImageMapData::IMAGE_CHANNEL_NONE,
                                       LuxImageMapData::WrapType wrapType = LuxImageMapData::WRAP_TYPE_NONE) const;

  LuxTextureDataH getColorTexture(LONG               toggleId,
                                  LONG               colorId,
                                  LONG               shaderId,
                                  LONG               shaderStrengthId,
                                  LONG               brightnessId,
                                  BaseContainer&     data,
                                  LuxTextureMappingH mapping,
                                  LuxFloat           colorGamma,
                                  LuxFloat           textureGamma) const;

  void getColorChannel(ULONG              channelId,
                       LONG               toggleId,
                       LONG               colorId,
                       LONG               shaderId,
                       LONG               shaderStrengthId,
                       LONG               brightnessId,
                       BaseContainer&     data,
                       LuxTextureMappingH mapping,
                       LuxFloat           colorGamma,
                       LuxFloat           textureGamma,
                       LuxMaterialData&   materialData) const;

  LuxTextureDataH getFloatTexture(LONG                     toggleId,
                                  LONG                     valueId,
                                  LONG                     shaderId,
                                  BaseContainer&           data,
                                  LuxTextureMappingH       mapping,
                                  LReal                    scaleFactor = 1.0,
                                  LuxImageMapData::Channel channel = LuxImageMapData::IMAGE_CHANNEL_NONE) const;

  void getFloatChannel(ULONG              channelId,
                       LONG               toggleId,
                       LONG               valueId,
                       LONG               shaderId,
                       BaseContainer&     data,
                       LuxTextureMappingH mapping,
                       LuxMaterialData&   materialData,
                       LReal              scaleFactor = 1.0) const;

  void getBumpChannel(BaseContainer&     data,
                      LuxTextureMappingH mapping,
                      LuxMaterialData&   materialData,
                      LReal              scaleFactor,
                      Real               bumpSampleDistance) const;

  void getEmissionChannel(BaseContainer&     data,
                          LuxTextureMappingH mapping,
                          LuxFloat           colorGamma,
                          LuxFloat           textureGamma,
                          LuxMaterialData&   materialData) const;

  void getAlphaChannel(BaseContainer&     data,
                       LuxTextureMappingH mapping,
                       LuxMaterialData&   materialData) const;

  void setCarpaintPreset(BaseContainer& data,
                         LONG           preset);
};



#endif  // #ifndef __LUXC4DMATERIAL_H__
