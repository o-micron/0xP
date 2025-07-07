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

#include <simd/simd.h>

#ifdef __METAL_VERSION__
    #include <metal_common>
    #include <metal_math>
    #include <metal_matrix>
    #define XPMetalShaderAttribute(x) attribute(x)
    #define DEVICE                    device
    #define CONSTANT                  constant
#else
    #include <Utilities/XPMaths.h>
typedef __fp16 half;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned int   uint;
    #define float2   XPVec2<float>
    #define float3   XPVec3<float>
    #define float4   XPVec4<float>
    #define float3x3 XPMat3<float>
    #define float4x4 XPMat4<float>
    #define XPMetalShaderAttribute(x)
    #define DEVICE
    #define CONSTANT static constexpr
#endif

// Buffer index values shared between shader and C code to ensure Metal shader buffer inputs
// match Metal API buffer set calls.
enum XPVertexInputIndex
{
    XPVertexInputIndexPositions = 0,
    XPVertexInputIndexNormals   = 1,
    XPVertexInputIndexUvs       = 2
};

// Texture index values shared between shader and C code to ensure Metal shader buffer inputs match
//   Metal API texture set calls
enum XPGBufferIndex
{
    XPGBufferIndexAlbedo    = 0,
    XPGBufferIndexNormal    = 1,
    XPGBufferIndexMetallic  = 2,
    XPGBufferIndexRoughness = 3,
    XPGBufferIndexAmbient   = 4,
};

enum XPMBufferIndex
{
    XPMBufferIndexPositionU                        = 0,
    XPMBufferIndexNormalV                          = 1,
    XPMBufferIndexAlbedo                           = 2,
    XPMBufferIndexMetallicRoughnessAmbientObjectId = 3,
};

struct GBufferVertexIn
{
    float4 position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
    float4 normal [[XPMetalShaderAttribute(XPVertexInputIndexNormals)]];
    float4 texcoord [[XPMetalShaderAttribute(XPVertexInputIndexUvs)]];
};
#define GBufferVertexIn_VertexShaderArgumentBuffer0Index 3

struct LineVertexIn
{
    float4 position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
};
#define LineVertexIn_VertexShaderArgumentBuffer0Index 1

struct MBufferVertexIn
{
    float4 position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
    float4 normal [[XPMetalShaderAttribute(XPVertexInputIndexNormals)]];
    float4 texcoord [[XPMetalShaderAttribute(XPVertexInputIndexUvs)]];
};
#define MBufferVertexIn_VertexShaderArgumentBuffer0Index 3

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

struct XPConstantMeshData
{
    float4 position;
    float4 normal;
    float4 uv;
};

enum XPFragmentTextureDataSourceFlags : uint
{
    XPFragmentDataSourceFlags_None                 = 0,
    XPFragmentDataSourceFlags_AlbedoFromTexture    = 1,
    XPFragmentDataSourceFlags_NormalFromTexture    = 2,
    XPFragmentDataSourceFlags_MetallicFromTexture  = 4,
    XPFragmentDataSourceFlags_RoughnessFromTexture = 8,
    XPFragmentDataSourceFlags_AmbientFromTexture   = 16,
};

// maps to XPBoundingBox
struct XPGPUBoundingBox
{
    float4 points[24];
    float4 minPoint;
    float4 maxPoint;
};

// 128+(64+64+64+64)+(96+16+4+4+4+4)
static_assert(sizeof(XPFrameDataPerObject) == 128 + (64 + 64 + 64 + 64) + (96 + 16 + 4 + 4 + 4 + 4),
              "Might have an alignment issue here");

struct XPVertexShaderArgumentBuffer
{
    packed_uint2         viewportSize;
    XPFrameDataPerObject frameDataPerObject;
};
