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

#ifdef XP_RUNTIME_COMPILER
    #include "shaders/XPShaderTypes.h"
#else
    #include <Renderer/Vulkan/XPShaderTypes.h>
    #include <Utilities/XPMacros.h>
    #include <Utilities/XPMaths.h>
    #include <Utilities/XPPlatforms.h>

    #include <vector>

struct XPVulkanMeshObject;
#endif

struct XPVulkanGPUData
{
#ifndef __GLSL__
    std::vector<XPMat4<float>> modelMatrices;
    std::vector<XPBoundingBox> boundingBoxes;
    std::vector<unsigned int>  perMeshNodeIndices;
#else
    float4x4*         modelMatrices;
    XPGPUBoundingBox* boundingBoxes;
    uint*             modelMatricesIndices;
#endif

// EXTENSION, CPU ONLY
// TODO: MAKE SURE YOU DON't RELY ON CALLING meshObjects.size() !!
// USE THE numSubMeshes instead, we allocate a large unused chunk at first !
#ifndef __GLSL__
    std::vector<XPVulkanMeshObject*> meshObjects;
    std::vector<uint32_t>            meshNodesIds;
    uint32_t                         numDrawCallsVertices;
    uint32_t                         numTotalDrawCallsVertices;
    uint32_t                         numDrawCalls;
    uint32_t                         numTotalDrawCalls;
    uint32_t                         numMeshNodes;
    uint32_t                         numSubMeshes;
    XPVec2<unsigned int>             gridSize;
    XPVec2<unsigned int>             threadsPerThreadgroup;
#endif
};

// STATIC ASSERTIONS TO MAKE SURE STRUCTURES MATCH IN SIZE AND ALIGNMENT, CPU ONLY
#ifndef __GLSL__
static_assert(sizeof(XPMat4<float>) == sizeof(float4x4));
static_assert(alignof(XPMat4<float>) == alignof(float4x4));

static_assert(sizeof(XPBoundingBox) == sizeof(XPGPUBoundingBox));
static_assert(alignof(XPBoundingBox) == alignof(XPGPUBoundingBox));
#endif
