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

#include <DataPipeline/XPShaderBuffer.h>

XPShaderBuffer::XPShaderBuffer(XPShaderAsset* const shaderAsset, uint32_t id)
  : _shaderAsset(shaderAsset)
  , _id(id)
{
}

XPShaderBuffer::~XPShaderBuffer() {}

XPShaderAsset*
XPShaderBuffer::getShaderAsset() const
{
    return _shaderAsset;
}

uint32_t
XPShaderBuffer::getId() const
{
    return _id;
}

const std::vector<std::unique_ptr<XPShader>>&
XPShaderBuffer::getShaders() const
{
    return _shaders;
}

void
XPShaderBuffer::setShaders(std::vector<std::unique_ptr<XPShader>>&& shaders)
{
    _shaders.clear();
    while (!shaders.empty()) {
        std::unique_ptr<XPShader> shader = std::move(shaders.back());
        shaders.pop_back();
        _shaders.emplace_back(std::move(shader));
    }
}
