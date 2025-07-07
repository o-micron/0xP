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

#include <map>
#include <string>

namespace val {

enum EGBufferAttachments
{
    EGBufferAttachments_PositionUTexture,
    EGBufferAttachments_NormalVTexture,
    EGBufferAttachments_AlbedoTexture,
    EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture,

    EGBufferAttachments_Count
};

enum EDescriptorSetLayout
{
    EDescriptorSetLayout_CameraMatrices     = 0,
    EDescriptorSetLayout_GBufferAttachments = 1,

    EDescriptorSetLayout_Count
};

enum EResourceStorageMode
{
    EResourceStorageModeShared     = 0,
    EResourceStorageModeManaged    = 16,
    EResourceStorageModePrivate    = 32,
    EResourceStorageModeMemoryless = 48,
};

enum EGraphicsPipelines
{
    EGraphicsPipelines_GBuffer,
    EGraphicsPipelines_Line,
    EGraphicsPipelines_MBuffer,

    EGraphicsPipelines_Count
};

static const std::map<std::string, EGraphicsPipelines> EGraphicsPipelinesMap = {
    { "GBuffer.surface", EGraphicsPipelines_GBuffer },
    { "Line.surface", EGraphicsPipelines_Line },
    { "MBuffer.surface", EGraphicsPipelines_MBuffer },
};

} // namespace val
