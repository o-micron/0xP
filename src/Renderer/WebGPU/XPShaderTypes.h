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
typedef __fp16         half;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned int   uint;
#define vec2f   XPVec2<float>
#define vec3f   XPVec3<float>
#define vec4f   XPVec4<float>
#define mat3x3f XPMat3<float>
#define mat4x4f XPMat4<float>

// Buffer index values shared between shader and C code to ensure webgpu shader buffer inputs
// match webgpu API buffer set calls.
typedef enum XPVertexInputIndex
{
    XPVertexInputIndexPositions    = 0,
    XPVertexInputIndexNormals      = 1,
    XPVertexInputIndexUvs          = 2,
    XPVertexInputIndexViewportSize = 3,
    XPVertexInputFrameData         = 4
} XPVertexInputIndex;

// Texture index values shared between shader and C code to ensure webgpu shader buffer inputs match
// WebGPU API texture set calls
typedef enum XPGBufferIndex
{
    XPGBufferIndexAlbedo    = 0,
    XPGBufferIndexNormal    = 1,
    XPGBufferIndexMetallic  = 2,
    XPGBufferIndexRoughness = 3,
    XPGBufferIndexAmbient   = 4,
} XPGBufferIndex;

typedef enum XPMBufferIndex
{
    XPMBufferIndexPositionU                        = 0,
    XPMBufferIndexNormalV                          = 1,
    XPMBufferIndexAlbedo                           = 2,
    XPMBufferIndexMetallicRoughnessAmbientObjectId = 3,
} XPMBufferIndex;

struct GBufferVertexIn
{
    vec4f position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
    vec4f normal [[XPMetalShaderAttribute(XPVertexInputIndexNormals)]];
    vec4f texcoord [[XPMetalShaderAttribute(XPVertexInputIndexUvs)]];
};

struct LineVertexIn
{
    vec4f position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
};

struct MBufferVertexIn
{
    vec4f position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
    vec4f normal [[XPMetalShaderAttribute(XPVertexInputIndexNormals)]];
    vec4f texcoord [[XPMetalShaderAttribute(XPVertexInputIndexUvs)]];
};

struct OcclusionCullingVertexIn
{
    vec4f position [[XPMetalShaderAttribute(XPVertexInputIndexPositions)]];
};

typedef struct
{
    mat4x4f modelMatrix;
    mat4x4f viewProjectionMatrix;
    mat4x4f viewMatrix;
    mat4x4f inverseViewMatrix;
    mat4x4f projectionMatrix;
    mat4x4f inverseProjectionMatrix;
    vec4f   albedo;
    float   metallic;
    float   roughness;
    float   time;
    uint    objectId;
    uint    flags; // XPFragmentTextureDataSourceFlags
} XPFrameDataPerObject;

typedef struct
{
    vec4f position;
    vec4f normal;
    vec4f uv;
} XPConstantMeshData;

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
    vec4f minPoint;
    vec4f maxPoint;
    vec4f points[24];
};
