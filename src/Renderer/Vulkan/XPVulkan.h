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

#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <vulkan/vulkan.h>

#include <Renderer/Vulkan/XPShaderTypes.h>

#include <memory>
#include <stdint.h>
#include <string>

class XPScene;
class XPMeshBuffer;
class XPShaderBuffer;
class XPTextureBuffer;
class XPMeshAsset;
class XPTextureAsset;
struct XPVulkanMeshObject;
struct XPVulkanMesh;

namespace val {
struct Buffer;
} // namespace val

struct XPVulkanMeshObject
{
    XPVulkanMeshObject(XPVulkanMesh& mesh, const XPBoundingBox& boundingBox)
      : mesh(mesh)
      , boundingBox(boundingBox)
    {
    }

    XPVulkanMesh& mesh;
    std::string   name;
    uint32_t      vertexOffset;
    uint32_t      indexOffset;
    uint32_t      numIndices;
    XPBoundingBox boundingBox;
};

struct XPVulkanMesh
{
    const XPMeshAsset*              meshAsset = nullptr;
    std::unique_ptr<val::Buffer>    vertexBuffer;
    std::unique_ptr<val::Buffer>    normalBuffer;
    std::unique_ptr<val::Buffer>    uvBuffer;
    std::unique_ptr<val::Buffer>    indexBuffer;
    std::vector<XPVulkanMeshObject> objects;
};

struct XPCameraUBO
{
    float4x4 viewProjectionMatrix;
    float4x4 inverseViewProjectionMatrix;
    float4x4 inverseViewMatrix;
};
