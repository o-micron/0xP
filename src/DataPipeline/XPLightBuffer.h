/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#pragma once

#include <Utilities/XPPlatforms.h>

#include <DataPipeline/XPEnums.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPMemoryPool.h>

#include <string>

class XPSceneAsset;
class XPLightBuffer;

struct XPLightBufferObject
{
    XPLightBuffer* lightBuffer;
    uint32_t       locationIndex;
    uint32_t       directionIndex;
    uint32_t       ambientIndex;
    uint32_t       diffuseIndex;
    uint32_t       specularIndex;
    uint32_t       intensityIndex;
    uint32_t       attenuationIndex;
    uint32_t       QuadraticIndex;
    uint32_t       typeIndex;
    uint32_t       angleInnerConeIndex;
    uint32_t       angleOuterConeIndex;
    std::string    name;
    XPVec3<float>  location;
    XPVec3<float>  direction;
    XPVec3<float>  ambient;
    XPVec3<float>  diffuse;
    XPVec3<float>  specular;
    float          intensity;
    float          attenuationConstant;
    float          attenuationLinear;
    float          attenuationQuadratic;
    XPELightType   type;
    float          angleInnerCone;
    float          angleOuterCone;
};

class XPLightBuffer
{
    XP_MPL_MEMORY_POOL(XPLightBuffer)

  public:
    explicit XPLightBuffer(XPSceneAsset* sceneAsset, uint32_t id);
    ~XPLightBuffer();

    [[nodiscard]] XPSceneAsset*        getSceneAsset() const;
    [[nodiscard]] uint32_t             getId() const;
    [[nodiscard]] XPVec3<float>*       getLocations() const;
    [[nodiscard]] XPVec3<float>*       getDirections() const;
    [[nodiscard]] XPVec3<float>*       getAmbients() const;
    [[nodiscard]] XPVec3<float>*       getDiffuses() const;
    [[nodiscard]] XPVec3<float>*       getSpeculars() const;
    [[nodiscard]] float*               getIntensities() const;
    [[nodiscard]] float*               getAttenuationConstants() const;
    [[nodiscard]] float*               getAttenuationLinears() const;
    [[nodiscard]] float*               getAttenuationQuadratics() const;
    [[nodiscard]] XPELightType*        getTypes() const;
    [[nodiscard]] float*               getAnglesInnerCone() const;
    [[nodiscard]] float*               getAnglesOuterCone() const;
    [[nodiscard]] XPLightBufferObject* getObjects() const;
    [[nodiscard]] size_t               getLocationsCount() const;
    [[nodiscard]] size_t               getDirectionsCount() const;
    [[nodiscard]] size_t               getAmbientsCount() const;
    [[nodiscard]] size_t               getDiffusesCount() const;
    [[nodiscard]] size_t               getSpecularsCount() const;
    [[nodiscard]] size_t               getIntensitiesCount() const;
    [[nodiscard]] size_t               getAttenuationConstantsCount() const;
    [[nodiscard]] size_t               getAttenuationLinearsCount() const;
    [[nodiscard]] size_t               getAttenuationQuadraticsCount() const;
    [[nodiscard]] size_t               getTypesCount() const;
    [[nodiscard]] size_t               getAnglesInnerConeCount() const;
    [[nodiscard]] size_t               getAnglesOuterConeCount() const;
    [[nodiscard]] size_t               getObjectsCount() const;
    void                               setLocationsCount(size_t count);
    void                               setDirectionsCount(size_t count);
    void                               setAmbientsCount(size_t count);
    void                               setDiffusesCount(size_t count);
    void                               setSpecularsCount(size_t count);
    void                               setIntensitiesCount(size_t count);
    void                               setAttenuationConstantsCount(size_t count);
    void                               setAttenuationLinearsCount(size_t count);
    void                               setAttenuationQuadraticsCount(size_t count);
    void                               setTypesCount(size_t count);
    void                               setAnglesInnerConeCount(size_t count);
    void                               setAnglesOuterConeCount(size_t count);
    void                               setObjectsCount(size_t count);
    XPVec3<float>&                     locationAtIndex(size_t index);
    XPVec3<float>&                     directionAtIndex(size_t index);
    XPVec3<float>&                     ambientAtIndex(size_t index);
    XPVec3<float>&                     diffuseAtIndex(size_t index);
    XPVec3<float>&                     specularAtIndex(size_t index);
    float&                             intensityAtIndex(size_t index);
    float&                             attenuationConstantAtIndex(size_t index);
    float&                             attenuationLinearAtIndex(size_t index);
    float&                             attenuationQuadraticAtIndex(size_t index);
    XPELightType&                      typeAtIndex(size_t index);
    float&                             angleInnerConeAtIndex(size_t index);
    float&                             angleOuterConeAtIndex(size_t index);
    XPLightBufferObject&               objectAtIndex(size_t index);
    void                               allocateForResources();
    void                               deallocateForResources();

    static size_t sizeofLocationsType();
    static size_t sizeofDirectionsType();
    static size_t sizeofAmbientsType();
    static size_t sizeofDiffusesType();
    static size_t sizeofSpecularsType();
    static size_t sizeofIntensitiesType();
    static size_t sizeofAttenuationConstantsType();
    static size_t sizeofAttenuationLinearsType();
    static size_t sizeofAttenuationQuadraticsType();
    static size_t sizeofTypesType();
    static size_t sizeofAnglesInnerConeType();
    static size_t sizeofAnglesOuterConeType();

  private:
    XPSceneAsset*        _sceneAsset;
    uint32_t             _id;
    XPVec3<float>*       _locations;
    XPVec3<float>*       _directions;
    XPVec3<float>*       _ambients;
    XPVec3<float>*       _diffuses;
    XPVec3<float>*       _speculars;
    float*               _intensities;
    float*               _attenuationConstants;
    float*               _attenuationLinears;
    float*               _attenuationQuadratics;
    XPELightType*        _types;
    float*               _anglesInnerCone;
    float*               _anglesOuterCone;
    XPLightBufferObject* _objects;
    unsigned char*       _backingMemory;
    size_t               _reservedLocationsCount;
    size_t               _reservedDirectionsCount;
    size_t               _reservedAmbientsCount;
    size_t               _reservedDiffusesCount;
    size_t               _reservedSpecularsCount;
    size_t               _reservedIntensitiesCount;
    size_t               _reservedAttenuationConstantsCount;
    size_t               _reservedAttenuationLinearsCount;
    size_t               _reservedAttenuationQuadraticsCount;
    size_t               _reservedTypesCount;
    size_t               _reservedAnglesInnerConeCount;
    size_t               _reservedAnglesOuterConeCount;
    size_t               _reservedObjectsCount;
};