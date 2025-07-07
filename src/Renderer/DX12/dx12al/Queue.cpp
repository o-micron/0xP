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

#include <Renderer/DX12/dx12al/Queue.hpp>

namespace dx12al {

Queue::Queue() {}

Queue::~Queue() {}

void
Queue::create(ID3D12Device* device, const wchar_t* debugLabel)
{
    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

    DX12_ASSERT(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)));
    NAME_D3D12_OBJECT(_commandQueue, debugLabel);

    for (UINT i = 0; i < XP_DX12_BUFFER_COUNT; ++i) {
        DX12_ASSERT(
          device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocators[i])));
    }
}

void
Queue::destroy()
{
    for (UINT i = 0; i < XP_DX12_BUFFER_COUNT; ++i) { _commandAllocators[i].Reset(); }
    _commandQueue.Reset();
}

ID3D12CommandAllocator*
Queue::getCommandAllocator(UINT frameIndex) const
{
    return _commandAllocators[frameIndex].Get();
}

ID3D12CommandQueue*
Queue::getCommandQueue() const
{
    return _commandQueue.Get();
}

} // namespace dx12al
