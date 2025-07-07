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

#include <DataPipeline/XPMeshBuffer.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <Engine/XPAllocators.h>
#include <Utilities/XPMemory.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <iostream>

static int64_t MeshBufferRefCounter = 0;

XPMeshBuffer::XPMeshBuffer(XPMeshAsset* meshAsset, uint32_t id)
  : _meshAsset(meshAsset)
  , _id(id)
  , _backingMemory(nullptr)
  , _positions(nullptr)
  , _normals(nullptr)
  , _texcoords(nullptr)
  , _indices(nullptr)
  , _objects(nullptr)
  , _reservedPositionsCount(0)
  , _reservedNormalsCount(0)
  , _reservedTexcoordsCount(0)
  , _reservedIndicesCount(0)
  , _reservedObjectsCount(0)
{
    ++MeshBufferRefCounter;
}

XPMeshBuffer::~XPMeshBuffer()
{
    deallocateResources();
    --MeshBufferRefCounter;
    std::cout << "MeshBufferRefCounter: " << MeshBufferRefCounter << "\n";
}

uint32_t
XPMeshBuffer::getId() const
{
    return _id;
}

XPMeshAsset*
XPMeshBuffer::getMeshAsset() const
{
    return _meshAsset;
}

XPVec4<float>*
XPMeshBuffer::getPositions() const
{
    return _positions;
}

XPVec4<float>*
XPMeshBuffer::getNormals() const
{
    return _normals;
}

XPVec4<float>*
XPMeshBuffer::getTexcoords() const
{
    return _texcoords;
}

uint32_t*
XPMeshBuffer::getIndices() const
{
    return _indices;
}

XPMeshBufferObject*
XPMeshBuffer::getObjects() const
{
    return _objects;
}

size_t
XPMeshBuffer::getPositionsCount() const
{
    return _reservedPositionsCount;
}

size_t
XPMeshBuffer::getNormalsCount() const
{
    return _reservedNormalsCount;
}

size_t
XPMeshBuffer::getTexcoordsCount() const
{
    return _reservedTexcoordsCount;
}

size_t
XPMeshBuffer::getIndicesCount() const
{
    return _reservedIndicesCount;
}

size_t
XPMeshBuffer::getObjectsCount() const
{
    return _reservedObjectsCount;
}

void
XPMeshBuffer::setPositionsCount(size_t count)
{
    _reservedPositionsCount = count;
}

void
XPMeshBuffer::setNormalsCount(size_t count)
{
    _reservedNormalsCount = count;
}

void
XPMeshBuffer::setTexcoordsCount(size_t count)
{
    _reservedTexcoordsCount = count;
}

void
XPMeshBuffer::setIndicesCount(size_t count)
{
    _reservedIndicesCount = count;
}

void
XPMeshBuffer::setObjectsCount(size_t count)
{
    _reservedObjectsCount = count;
}

XPVec4<float>&
XPMeshBuffer::positionAtIndex(size_t index)
{
    return _positions[index];
}

XPVec4<float>&
XPMeshBuffer::normalAtIndex(size_t index)
{
    return _normals[index];
}

XPVec4<float>&
XPMeshBuffer::texcoordAtIndex(size_t index)
{
    return _texcoords[index];
}

uint32_t&
XPMeshBuffer::indexAtIndex(size_t index)
{
    return _indices[index];
}

XPMeshBufferObject&
XPMeshBuffer::objectAtIndex(size_t index)
{
    return _objects[index];
}

void
XPMeshBuffer::allocateForResources()
{
    assert(_reservedPositionsCount >= 1);
    assert(_reservedNormalsCount >= 1);
    assert(_reservedTexcoordsCount >= 1);
    assert(_reservedIndicesCount >= 1);
    assert(_reservedObjectsCount >= 1);

    size_t numBytes = ((_reservedPositionsCount + 1) * sizeofPositionsType())       // +1 for alignment
                      + ((_reservedNormalsCount + 1) * sizeofNormalsType())         // +1 for alignment
                      + ((_reservedTexcoordsCount + 1) * sizeofTexcoordsType())     // +1 for alignment
                      + ((_reservedIndicesCount + 1) * sizeofIndicesType())         // +1 for alignment
                      + ((_reservedObjectsCount + 1) * sizeof(XPMeshBufferObject)); // +1 for alignment
    _backingMemory = _meshAsset->getFile()->getDataPipelineStore()->getRegistry()->getAllocators()->allocate(numBytes);

    unsigned char* refPtr = _backingMemory;

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec4<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec4<float>)));
    _positions = (XPVec4<float>*)(refPtr);
    refPtr += _reservedPositionsCount * sizeofPositionsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec4<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec4<float>)));
    _normals = (XPVec4<float>*)(refPtr);
    refPtr += _reservedNormalsCount * sizeofNormalsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPVec4<float>));
    assert(XPIsAligned(refPtr, alignof(XPVec4<float>)));
    _texcoords = (XPVec4<float>*)(refPtr);
    refPtr += _reservedTexcoordsCount * sizeofTexcoordsType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(uint32_t));
    assert(XPIsAligned(refPtr, alignof(uint32_t)));
    _indices = (uint32_t*)(refPtr);
    refPtr += _reservedIndicesCount * sizeofIndicesType();

    refPtr = (unsigned char*)XPAlignPointer(refPtr, alignof(XPMeshBufferObject));
    assert(XPIsAligned(refPtr, alignof(XPMeshBufferObject)));
    _objects = (XPMeshBufferObject*)(refPtr);
    refPtr += _reservedObjectsCount * sizeof(XPMeshBufferObject);

    // make sure we don't spill past our allocated area
    assert(refPtr <= (((unsigned char*)_backingMemory) + numBytes));
}

void
XPMeshBuffer::deallocateResources()
{
    if (_backingMemory) {
        _objects                = nullptr;
        _indices                = nullptr;
        _texcoords              = nullptr;
        _normals                = nullptr;
        _positions              = nullptr;
        _backingMemory          = nullptr;
        _reservedPositionsCount = 0;
        _reservedNormalsCount   = 0;
        _reservedTexcoordsCount = 0;
        _reservedIndicesCount   = 0;
        _reservedObjectsCount   = 0;
    }
}

size_t
XPMeshBuffer::sizeofPositionsType()
{
    return sizeof(XPVec4<float>);
}

size_t
XPMeshBuffer::sizeofNormalsType()
{
    return sizeof(XPVec4<float>);
}

size_t
XPMeshBuffer::sizeofTexcoordsType()
{
    return sizeof(XPVec4<float>);
}

size_t
XPMeshBuffer::sizeofIndicesType()
{
    return sizeof(uint32_t);
}
