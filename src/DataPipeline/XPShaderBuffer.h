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

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

class XPShaderAsset;

enum XPEShaderStageType
{
    XPEShaderStageType_Vertex   = 1,
    XPEShaderStageType_Fragment = 2,
    XPEShaderStageType_Compute  = 4
};

struct XPShaderStage
{
    XPEShaderStageType type;
    std::string        entryFunction;
};

struct XPShader
{
    std::vector<uint8_t>       sourceCode;
    std::vector<XPShaderStage> stages;
};

class XPShaderBuffer
{
    XP_MPL_MEMORY_POOL(XPShaderBuffer)

  public:
    explicit XPShaderBuffer(XPShaderAsset* const shaderAsset, uint32_t id);
    ~XPShaderBuffer();

    [[nodiscard]] XPShaderAsset*                                getShaderAsset() const;
    [[nodiscard]] uint32_t                                      getId() const;
    [[nodiscard]] const std::vector<std::unique_ptr<XPShader>>& getShaders() const;
    void setShaders(std::vector<std::unique_ptr<XPShader>>&& shaders);

  private:
    XPShaderAsset* const                   _shaderAsset;
    const uint32_t                         _id;
    std::vector<std::unique_ptr<XPShader>> _shaders;
};
