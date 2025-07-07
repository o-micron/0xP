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

#include <Renderer/DX12/dx12al/CommandList.hpp>

namespace dx12al {

CommandList::CommandList() {}

CommandList::~CommandList() {}

void
CommandList::create(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator)
{
    DX12_ASSERT(device->CreateCommandList(
      0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&_commandList)));
    DX12_ASSERT(_commandList->Close());
}

void
CommandList::destroy()
{
    _commandList.Reset();
}

ID3D12GraphicsCommandList*
CommandList::getCommandList() const
{
    return _commandList.Get();
}

} // namespace dx12al
