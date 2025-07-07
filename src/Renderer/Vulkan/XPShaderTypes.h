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

#if defined(GL_core_profile) || defined(GL_compatibility_profile) || defined(GL_es_profile)
    #define __GLSL__

    #define uint2    uvec2
    #define float2   vec2
    #define float3   vec3
    #define float4   vec4
    #define float3x3 mat3x3
    #define float4x4 mat4x4
#else
    #include <Utilities/XPMaths.h>

    #define uint     unsigned int
    #define uint2    XPVec2<unsigned int>
    #define float2   XPVec2<float>
    #define float3   XPVec3<float>
    #define float4   XPVec4<float>
    #define float3x3 XPMat3<float>
    #define float4x4 XPMat4<float>
#endif

enum GPUPlanes
{
    Left = 0,
    Right,
    Bottom,
    Top,
    Near,
    Far,

    Count,
};

struct alignas(128) XPFrameDataPerObject
{
    // 0
    float4 frustumPoints[8];

    float4x4 modelMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 inverseViewProjectionMatrix;
    float4x4 inverseViewMatrix;

    float4 frustumPlanes[GPUPlanes::Count];
    float4 albedo;
    float  metallic;
    float  roughness;
    uint   objectId;
    uint   flags; // XPFragmentTextureDataSourceFlags
};

// maps to XPBoundingBox
typedef struct XPGPUBoundingBox
{
    float4 points[24];
    float4 minPoint;
    float4 maxPoint;
} XPGPUBoundingBox;

struct XPVertexShaderArgumentBuffer
{
    uint2                viewportSize;
    XPFrameDataPerObject frameDataPerObject;
};

// Buffer index values shared between shader and C code to ensure Metal shader buffer inputs
// match Metal API buffer set calls.
enum XPVertexInputIndex
{
    XPVertexInputIndexPositions = 0,
    XPVertexInputIndexNormals   = 1,
    XPVertexInputIndexUvs       = 2
};
