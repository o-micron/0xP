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

// #ifdef XP_RUNTIME_COMPILER
//    #include "shaders/XPShaderTypes.h"
// #else
#include <Renderer/WebGPU/XPShaderTypes.h>
#include <Renderer/WebGPU/wgpu_cpp.h>
#include <Utilities/XPMacros.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <vector>

struct XPWGPUMeshObject;
// #endif

struct XPWGPUGPUData
{
    // FUNCTIONS TO USE WHEN TRANSFERING DATA, CPU ONLY
    // #ifndef __METAL_VERSION__
    void   uploadToGPU();
    void   downloadFromGPU();
    size_t getSpaceInBytes() const;
    // #endif

    // LISTS AND THEIR INDEXERS, CPU AND GPU
    // #ifndef __METAL_VERSION__
    typedef unsigned int       uint;
    std::vector<XPMat4<float>> modelMatrices;
    std::vector<XPBoundingBox> boundingBoxes;
    std::vector<uint>          perMeshNodeIndices;
    std::vector<uint>          sortedNodesIndices;
    // #else
    // matrix_float4x4*  modelMatrices;
    // XPGPUBoundingBox* boundingBoxes;
    // uint*             modelMatricesIndices;
    // uint*             boundingBoxesIndices;
    // #endif

    // EXTENSION, CPU ONLY
    // TODO: MAKE SURE YOU DON't RELY ON CALLING meshObjects.size() !!
    // USE THE numSubMeshes instead, we allocate a large unused chunk at first !
    // #ifndef __METAL_VERSION__
    std::vector<XPWGPUMeshObject*> meshObjects;
    std::vector<uint32_t>          meshNodesIds;
    uint32_t                       numDrawCallsVertices;
    uint32_t                       numTotalDrawCallsVertices;
    uint32_t                       numDrawCalls;
    uint32_t                       numTotalDrawCalls;
    uint32_t                       numMeshNodes;
    uint32_t                       numSubMeshes;
    // MTL::Size                      gridSize;
    // MTL::Size                      threadsPerThreadgroup;
    // #endif
};

// TODO: THE FOLLOWING NEEDS SOME WORK TO USE SAME CODE ACROSS WHILE ENSURING SAME SIZE AND ALIGNMENT ETC ...
//
// STATIC ASSERTIONS TO MAKE SURE STRUCTURES MATCH IN SIZE AND ALIGNMENT, CPU ONLY
// #ifndef __METAL_VERSION__
// static_assert(sizeof(XPMat4<float>) == sizeof(mat4x4f));
// static_assert(alignof(XPMat4<float>) == alignof(mat4x4f));

// static_assert(sizeof(XPBoundingBox) == sizeof(XPGPUBoundingBox));
// static_assert(alignof(XPBoundingBox) == alignof(XPGPUBoundingBox));
// #endif
