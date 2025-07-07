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

#include <DataPipeline/XPLightBuffer.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPSceneAsset.h>
#include <Engine/XPAllocators.h>
#include <Engine/XPRegistry.h>
#include <Utilities/XPMemory.h>

static int64_t LightBufferRefCounter = 0;

XPLightBuffer::XPLightBuffer(XPSceneAsset* sceneAsset, uint32_t id)
  : _sceneAsset(sceneAsset)
  , _id(id)
  , _locations(nullptr)
  , _directions(nullptr)
  , _ambients(nullptr)
  , _diffuses(nullptr)
  , _speculars(nullptr)
  , _intensities(nullptr)
  , _attenuationConstants(nullptr)
  , _attenuationLinears(nullptr)
  , _attenuationQuadratics(nullptr)
  , _types(nullptr)
  , _anglesInnerCone(nullptr)
  , _anglesOuterCone(nullptr)
  , _objects(nullptr)
  , _backingMemory(nullptr)
  , _reservedLocationsCount(0)
  , _reservedDirectionsCount(0)
  , _reservedAmbientsCount(0)
  , _reservedDiffusesCount(0)
  , _reservedSpecularsCount(0)
  , _reservedIntensitiesCount(0)
  , _reservedAttenuationConstantsCount(0)
  , _reservedAttenuationLinearsCount(0)
  , _reservedAttenuationQuadraticsCount(0)
  , _reservedTypesCount(0)
  , _reservedAnglesInnerConeCount(0)
  , _reservedAnglesOuterConeCount(0)
  , _reservedObjectsCount(0)
{
    ++LightBufferRefCounter;
}

XPLightBuffer::~XPLightBuffer()
{
    deallocateForResources();
    --LightBufferRefCounter;
    std::cout << "LightBufferRefCounter: " << LightBufferRefCounter << "\n";
}

XPSceneAsset*
XPLightBuffer::getSceneAsset() const
{
    return _sceneAsset;
}

uint32_t
XPLightBuffer::getId() const
{
    return _id;
}

XPVec3<float>*
XPLightBuffer::getLocations() const
{
    return _locations;
}

XPVec3<float>*
XPLightBuffer::getDirections() const
{
    return _directions;
}

XPVec3<float>*
XPLightBuffer::getAmbients() const
{
    return _ambients;
}

XPVec3<float>*
XPLightBuffer::getDiffuses() const
{
    return _diffuses;
}

XPVec3<float>*
XPLightBuffer::getSpeculars() const
{
    return _speculars;
}

float*
XPLightBuffer::getIntensities() const
{
    return _intensities;
}

float*
XPLightBuffer::getAttenuationConstants() const
{
    return _attenuationConstants;
}

float*
XPLightBuffer::getAttenuationLinears() const
{
    return _attenuationLinears;
}

float*
XPLightBuffer::getAttenuationQuadratics() const
{
    return _attenuationQuadratics;
}

XPELightType*
XPLightBuffer::getTypes() const
{
    return _types;
}

float*
XPLightBuffer::getAnglesInnerCone() const
{
    return _anglesInnerCone;
}

float*
XPLightBuffer::getAnglesOuterCone() const
{
    return _anglesOuterCone;
}

XPLightBufferObject*
XPLightBuffer::getObjects() const
{
    return _objects;
}

size_t
XPLightBuffer::getLocationsCount() const
{
    return _reservedLocationsCount;
}

size_t
XPLightBuffer::getDirectionsCount() const
{
    return _reservedDirectionsCount;
}

size_t
XPLightBuffer::getAmbientsCount() const
{
    return _reservedAmbientsCount;
}

size_t
XPLightBuffer::getDiffusesCount() const
{
    return _reservedDiffusesCount;
}

size_t
XPLightBuffer::getSpecularsCount() const
{
    return _reservedSpecularsCount;
}

size_t
XPLightBuffer::getIntensitiesCount() const
{
    return _reservedIntensitiesCount;
}

size_t
XPLightBuffer::getAttenuationConstantsCount() const
{
    return _reservedAttenuationConstantsCount;
}

size_t
XPLightBuffer::getAttenuationLinearsCount() const
{
    return _reservedAttenuationLinearsCount;
}

size_t
XPLightBuffer::getAttenuationQuadraticsCount() const
{
    return _reservedAttenuationQuadraticsCount;
}

size_t
XPLightBuffer::getTypesCount() const
{
    return _reservedTypesCount;
}

size_t
XPLightBuffer::getAnglesInnerConeCount() const
{
    return _reservedAnglesInnerConeCount;
}

size_t
XPLightBuffer::getAnglesOuterConeCount() const
{
    return _reservedAnglesOuterConeCount;
}

size_t
XPLightBuffer::getObjectsCount() const
{
    return _reservedObjectsCount;
}

void
XPLightBuffer::setLocationsCount(size_t count)
{
    _reservedLocationsCount = count;
}

void
XPLightBuffer::setDirectionsCount(size_t count)
{
    _reservedDirectionsCount = count;
}

void
XPLightBuffer::setAmbientsCount(size_t count)
{
    _reservedAmbientsCount = count;
}

void
XPLightBuffer::setDiffusesCount(size_t count)
{
    _reservedDiffusesCount = count;
}

void
XPLightBuffer::setSpecularsCount(size_t count)
{
    _reservedSpecularsCount = count;
}

void
XPLightBuffer::setIntensitiesCount(size_t count)
{
    _reservedIntensitiesCount = count;
}

void
XPLightBuffer::setAttenuationConstantsCount(size_t count)
{
    _reservedAttenuationConstantsCount = count;
}

void
XPLightBuffer::setAttenuationLinearsCount(size_t count)
{
    _reservedAttenuationLinearsCount = count;
}

void
XPLightBuffer::setAttenuationQuadraticsCount(size_t count)
{
    _reservedAttenuationQuadraticsCount = count;
}

void
XPLightBuffer::setTypesCount(size_t count)
{
    _reservedTypesCount = count;
}

void
XPLightBuffer::setAnglesInnerConeCount(size_t count)
{
    _reservedAnglesInnerConeCount = count;
}

void
XPLightBuffer::setAnglesOuterConeCount(size_t count)
{
    _reservedAnglesOuterConeCount = count;
}

void
XPLightBuffer::setObjectsCount(size_t count)
{
    _reservedObjectsCount = count;
}

XPVec3<float>&
XPLightBuffer::locationAtIndex(size_t index)
{
    return _locations[index];
}

XPVec3<float>&
XPLightBuffer::directionAtIndex(size_t index)
{
    return _directions[index];
}

XPVec3<float>&
XPLightBuffer::ambientAtIndex(size_t index)
{
    return _ambients[index];
}

XPVec3<float>&
XPLightBuffer::diffuseAtIndex(size_t index)
{
    return _diffuses[index];
}

XPVec3<float>&
XPLightBuffer::specularAtIndex(size_t index)
{
    return _speculars[index];
}

float&
XPLightBuffer::intensityAtIndex(size_t index)
{
    return _intensities[index];
}

float&
XPLightBuffer::attenuationConstantAtIndex(size_t index)
{
    return _attenuationConstants[index];
}

float&
XPLightBuffer::attenuationLinearAtIndex(size_t index)
{
    return _attenuationLinears[index];
}

float&
XPLightBuffer::attenuationQuadraticAtIndex(size_t index)
{
    return _attenuationQuadratics[index];
}

XPELightType&
XPLightBuffer::typeAtIndex(size_t index)
{
    return _types[index];
}

float&
XPLightBuffer::angleInnerConeAtIndex(size_t index)
{
    return _anglesInnerCone[index];
}

float&
XPLightBuffer::angleOuterConeAtIndex(size_t index)
{
    return _anglesOuterCone[index];
}

XPLightBufferObject&
XPLightBuffer::objectAtIndex(size_t index)
{
    return _objects[index];
}

void
XPLightBuffer::allocateForResources()
{
    assert(_reservedLocationsCount >= 1);
    assert(_reservedDirectionsCount >= 1);
    assert(_reservedAmbientsCount >= 1);
    assert(_reservedDiffusesCount >= 1);
    assert(_reservedSpecularsCount >= 1);
    assert(_reservedIntensitiesCount >= 1);
    assert(_reservedAttenuationConstantsCount >= 1);
    assert(_reservedAttenuationLinearsCount >= 1);
    assert(_reservedAttenuationQuadraticsCount >= 1);
    assert(_reservedTypesCount >= 1);
    assert(_reservedAnglesInnerConeCount >= 1);
    assert(_reservedAnglesOuterConeCount >= 1);
    assert(_reservedObjectsCount >= 1);

    size_t numBytes =
      ((_reservedLocationsCount + 1) * sizeofLocationsType())                           // +1 for alignment
      + ((_reservedDirectionsCount + 1) * sizeofDirectionsType())                       // +1 for alignment
      + ((_reservedAmbientsCount + 1) * sizeofAmbientsType())                           // +1 for alignment
      + ((_reservedDiffusesCount + 1) * sizeofDiffusesType())                           // +1 for alignment
      + ((_reservedSpecularsCount + 1) * sizeofSpecularsType())                         // +1 for alignment
      + ((_reservedIntensitiesCount + 1) * sizeofIntensitiesType())                     // +1 for alignment
      + ((_reservedAttenuationConstantsCount + 1) * sizeofAttenuationConstantsType())   // +1 for alignment
      + ((_reservedAttenuationLinearsCount + 1) * sizeofAttenuationLinearsType())       // +1 for alignment
      + ((_reservedAttenuationQuadraticsCount + 1) * sizeofAttenuationQuadraticsType()) // +1 for alignment
      + ((_reservedTypesCount + 1) * sizeofTypesType())                                 // +1 for alignment
      + ((_reservedAnglesInnerConeCount + 1) * sizeofAnglesInnerConeType())             // +1 for alignment
      + ((_reservedAnglesOuterConeCount + 1) * sizeofAnglesOuterConeType())             // +1 for alignment
      + ((_reservedObjectsCount + 1) * sizeof(XPLightBufferObject));                    // +1 for alignment

    _backingMemory = _sceneAsset->getFile()->getDataPipelineStore()->getRegistry()->getAllocators()->allocate(numBytes);

    unsigned char* refPtr = _backingMemory;

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec3<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec3<float>)));
    _locations = (XPVec3<float>*)(refPtr);
    refPtr += _reservedLocationsCount * sizeofLocationsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec3<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec3<float>)));
    _directions = (XPVec3<float>*)(refPtr);
    refPtr += _reservedDirectionsCount * sizeofDirectionsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec3<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec3<float>)));
    _ambients = (XPVec3<float>*)(refPtr);
    refPtr += _reservedAmbientsCount * sizeofAmbientsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec3<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec3<float>)));
    _diffuses = (XPVec3<float>*)(refPtr);
    refPtr += _reservedDiffusesCount * sizeofDiffusesType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec3<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec3<float>)));
    _speculars = (XPVec3<float>*)(refPtr);
    refPtr += _reservedSpecularsCount * sizeofSpecularsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _intensities = (float*)(refPtr);
    refPtr += _reservedIntensitiesCount * sizeofIntensitiesType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _attenuationConstants = (float*)(refPtr);
    refPtr += _reservedAttenuationConstantsCount * sizeofAttenuationConstantsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _attenuationLinears = (float*)(refPtr);
    refPtr += _reservedAttenuationLinearsCount * sizeofAttenuationLinearsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _attenuationQuadratics = (float*)(refPtr);
    refPtr += _reservedAttenuationQuadraticsCount * sizeofAttenuationQuadraticsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPELightType));
    assert(XPIsAligned(refPtr, alignof(XPELightType)));
    _types = (XPELightType*)(refPtr);
    refPtr += _reservedTypesCount * sizeofTypesType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _anglesInnerCone = (float*)(refPtr);
    refPtr += _reservedAnglesInnerConeCount * sizeofAnglesInnerConeType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(float));
    assert(XPIsAligned(refPtr, alignof(float)));
    _anglesOuterCone = (float*)(refPtr);
    refPtr += _reservedAnglesOuterConeCount * sizeofAnglesOuterConeType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPLightBufferObject));
    assert(XPIsAligned(refPtr, alignof(XPLightBufferObject)));
    _objects = (XPLightBufferObject*)(refPtr);
    refPtr += _reservedObjectsCount * sizeof(XPLightBufferObject);

    // make sure we don't spill past our allocated area
    assert(refPtr <= (((unsigned char*)_backingMemory) + numBytes));
}

void
XPLightBuffer::deallocateForResources()
{
    if (_backingMemory) {
        _objects               = nullptr;
        _anglesOuterCone       = nullptr;
        _anglesInnerCone       = nullptr;
        _types                 = nullptr;
        _attenuationQuadratics = nullptr;
        _attenuationLinears    = nullptr;
        _attenuationConstants  = nullptr;
        _intensities           = nullptr;
        _speculars             = nullptr;
        _diffuses              = nullptr;
        _ambients              = nullptr;
        _directions            = nullptr;
        _locations             = nullptr;
        _backingMemory         = nullptr;
    }
}

size_t
XPLightBuffer::sizeofLocationsType()
{
    return sizeof(XPVec3<float>);
}

size_t
XPLightBuffer::sizeofDirectionsType()
{
    return sizeof(XPVec3<float>);
}

size_t
XPLightBuffer::sizeofAmbientsType()
{
    return sizeof(XPVec3<float>);
}

size_t
XPLightBuffer::sizeofDiffusesType()
{
    return sizeof(XPVec3<float>);
}

size_t
XPLightBuffer::sizeofSpecularsType()
{
    return sizeof(XPVec3<float>);
}

size_t
XPLightBuffer::sizeofIntensitiesType()
{
    return sizeof(float);
}

size_t
XPLightBuffer::sizeofAttenuationConstantsType()
{
    return sizeof(float);
}

size_t
XPLightBuffer::sizeofAttenuationLinearsType()
{
    return sizeof(float);
}

size_t
XPLightBuffer::sizeofAttenuationQuadraticsType()
{
    return sizeof(float);
}

size_t
XPLightBuffer::sizeofTypesType()
{
    return sizeof(XPELightType);
}

size_t
XPLightBuffer::sizeofAnglesInnerConeType()
{
    return sizeof(float);
}

size_t
XPLightBuffer::sizeofAnglesOuterConeType()
{
    return sizeof(float);
}
