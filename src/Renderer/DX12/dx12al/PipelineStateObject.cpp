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

#include <Renderer/DX12/dx12al/PipelineStateObject.hpp>

namespace dx12al {

PipelineStateObject::PipelineStateObject() {}

PipelineStateObject::~PipelineStateObject() {}

void
PipelineStateObject::create(ID3D12Device* device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
    DX12_ASSERT(device->CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), (void**)_pso.GetAddressOf()));
}

void
PipelineStateObject::destroy()
{
    _pso.Reset();
}

ID3D12PipelineState*
PipelineStateObject::getPSO() const
{
    return _pso.Get();
}

} // namespace dx12al
