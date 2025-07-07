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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <Utilities/XPLogger.h>
#include <Utilities/XPMaths.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

class XPRegistry;
class XPMeshAsset;
class XPShaderAsset;
class XPScene;

struct XPRendererGraphResourceUsage;
struct XPRendererGraphBuffer;
struct XPRendererGraphTexture;
struct XPRendererGraphTarget;
struct XPRendererGraphResource;
struct XPRendererGraphPass;
struct XPRendererGraph;
struct XPRendererGraphMesh;

struct XPRendererGraphMeshObject
{
    XPRendererGraphMeshObject(XPRendererGraphMesh& mesh, const XPBoundingBox& boundingBox)
      : mesh(mesh)
      , boundingBox(boundingBox)
    {
    }

    XPRendererGraphMesh& mesh;
    std::string          name;
    uint32_t             vertexOffset;
    uint32_t             indexOffset;
    uint32_t             numIndices;
    XPBoundingBox        boundingBox;
};

struct XPRendererGraphMesh
{
    const XPMeshAsset*                     meshAsset = nullptr;
    std::vector<XPRendererGraphMeshObject> objects;
};

enum class EXPRendererGraphResourceAccessMode
{
    ReadOnly,
    ReadWrite
};

struct XPRendererGraphResource
{
    uint64_t id;
};

struct XPRendererGraphPass
{
    uint64_t id;
};

struct XPRendererGraphResourceUsage
{
    EXPRendererGraphResourceAccessMode accessMode;
    XPRendererGraphResource*           resource;
    XPRendererGraphPass*               pass;
};

struct XPRendererGraphBuffer : public XPRendererGraphResource
{
    XPRendererGraphBuffer(size_t numBytes) { data.resize(numBytes); }
    void getData(size_t from, size_t numBytes, uint8_t* ptr)
    {
        assert((numBytes < data.size()) && ((from + numBytes) < data.size()) &&
               "can't copy data which is out of buffer bounds");
        ptr = &data[from];
    }
    void setData(uint8_t* ptr, size_t from, size_t numBytes)
    {
        assert((numBytes < data.size()) && ((from + numBytes) < data.size()) &&
               "can't copy data which is out of buffer bounds");
        memcpy(&data[from], ptr, numBytes);
    }
    std::vector<uint8_t> data;
};

struct XPRendererGraphTexture : public XPRendererGraphResource
{
    XPRendererGraphTexture(const std::string& name)
      : name(name)
    {
    }

    std::string name;
};

struct XPRendererGraphTarget : public XPRendererGraphResource
{};

struct XPRendererGraphCommandBuffer
{
    void beginRecording();
    void submitMeshAssetForDraw(XPMeshAsset* meshAsset);
    void endRecording();

    std::vector<XPMeshAsset*> drawMeshes;
};

struct XPRendererGraphCommandQueue
{
    XPRendererGraphCommandBuffer commandBuffer;
};

enum EXPRendererGraphCommandQueueType
{
    EXPRendererGraphCommandQueueType_Compute,
    EXPRendererGraphCommandQueueType_Graphics,

    EXPRendererGraphCommandQueueType_Count
};

struct XPRendererGraph
{
    XPRendererGraph(XPRegistry* const registry)
      : registry(registry)
    {
    }

    void loadScene(XPScene& scene);

    XPRegistry* const                                                               registry = nullptr;
    std::vector<std::unique_ptr<XPRendererGraphResource>>                           resources;
    std::vector<std::unique_ptr<XPRendererGraphPass>>                               passes;
    std::vector<std::unique_ptr<XPRendererGraphResourceUsage>>                      usages;
    std::array<XPRendererGraphCommandQueue, EXPRendererGraphCommandQueueType_Count> commandQueues;
};