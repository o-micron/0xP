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

#include <Renderer/DX12/XPDX12.h>
#include <Renderer/DX12/dx12al/PipelineStateObject.hpp>
#include <Renderer/DX12/dx12al/RootSignature.hpp>
#include <Renderer/DX12/dx12al/Shader.hpp>

namespace dx12al {

class RootSignature;

class GraphicsPipeline
{
  public:
    GraphicsPipeline();
    ~GraphicsPipeline();
    void                       create(ID3D12Device* device, const char* vsfilename, const char* psfilename);
    void                       destroy();
    const RootSignature&       getRootSignature() const;
    const PipelineStateObject& getPSO() const;
    const Shader&              getVertexShader() const;
    const Shader&              getPixelShader() const;

  private:
    RootSignature       _gBufferRootSignature;
    PipelineStateObject _gBufferPSO;
    Shader              _gBufferVS;
    Shader              _gBufferPS;
};

} // namespace dx12al
