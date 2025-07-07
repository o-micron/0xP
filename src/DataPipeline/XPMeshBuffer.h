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

#include <Utilities/XPMaths.h>
#include <Utilities/XPMemoryPool.h>

#include <memory_resource>
#include <stdint.h>
#include <string>
#include <vector>

class XPMeshAsset;
class XPMeshBuffer;
class XPMaterialBuffer;

struct XPMeshVertex
{
    XPVec4<float> position;
    XPVec4<float> normal;
    XPVec4<float> texcoord;
};

struct XPMeshBufferObject
{
    XPBoundingBox     boundingBox;
    XPMeshBuffer*     meshBuffer;
    XPMaterialBuffer* materialBuffer;
    uint32_t          vertexOffset;
    uint32_t          indexOffset;
    uint32_t          numIndices;
    uint32_t          unused;
    std::string       name;
};

class XPMeshBuffer
{
    XP_MPL_MEMORY_POOL(XPMeshBuffer)

  public:
    explicit XPMeshBuffer(XPMeshAsset* meshAsset, uint32_t id);
    ~XPMeshBuffer();

    [[nodiscard]] XPMeshAsset*        getMeshAsset() const;
    [[nodiscard]] uint32_t            getId() const;
    [[nodiscard]] XPVec4<float>*      getPositions() const;
    [[nodiscard]] XPVec4<float>*      getNormals() const;
    [[nodiscard]] XPVec4<float>*      getTexcoords() const;
    [[nodiscard]] uint32_t*           getIndices() const;
    [[nodiscard]] XPMeshBufferObject* getObjects() const;
    [[nodiscard]] size_t              getPositionsCount() const;
    [[nodiscard]] size_t              getNormalsCount() const;
    [[nodiscard]] size_t              getTexcoordsCount() const;
    [[nodiscard]] size_t              getIndicesCount() const;
    [[nodiscard]] size_t              getObjectsCount() const;
    void                              setPositionsCount(size_t count);
    void                              setNormalsCount(size_t count);
    void                              setTexcoordsCount(size_t count);
    void                              setIndicesCount(size_t count);
    void                              setObjectsCount(size_t count);
    XPVec4<float>&                    positionAtIndex(size_t index);
    XPVec4<float>&                    normalAtIndex(size_t index);
    XPVec4<float>&                    texcoordAtIndex(size_t index);
    uint32_t&                         indexAtIndex(size_t index);
    XPMeshBufferObject&               objectAtIndex(size_t index);
    void                              allocateForResources();
    void                              deallocateResources();

    static size_t sizeofPositionsType();
    static size_t sizeofNormalsType();
    static size_t sizeofTexcoordsType();
    static size_t sizeofIndicesType();

  private:
    XPMeshAsset*        _meshAsset;
    const uint32_t      _id;
    XPVec4<float>*      _positions;
    XPVec4<float>*      _normals;
    XPVec4<float>*      _texcoords;
    uint32_t*           _indices;
    XPMeshBufferObject* _objects;
    unsigned char*      _backingMemory;
    size_t              _reservedPositionsCount;
    size_t              _reservedNormalsCount;
    size_t              _reservedTexcoordsCount;
    size_t              _reservedIndicesCount;
    size_t              _reservedObjectsCount;
};
