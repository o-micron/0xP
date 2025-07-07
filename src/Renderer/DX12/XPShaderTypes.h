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

#ifdef __cplusplus
    #include <array>
typedef std::array<float, 16> float4x4;
    #define ALIGN_256
#else
    #define ALIGN_256
#endif

struct ALIGN_256 CameraMatrices
{
    float4x4 viewProjectionMatrix;
    float4x4 inverseViewProjectionMatrix;
    float4x4 inverseViewMatrix;
    float4x4 unused;
};
#ifdef __cplusplus
static_assert((sizeof(CameraMatrices) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
#endif

struct ALIGN_256 MeshMatrices
{
    float4x4 modelMatrix;
    float4x4 unused[3];
};
#ifdef __cplusplus
static_assert((sizeof(MeshMatrices) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
#endif