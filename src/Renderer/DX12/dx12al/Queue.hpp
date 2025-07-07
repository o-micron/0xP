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

namespace dx12al {

class Queue
{
  public:
    Queue();
    ~Queue();
    void create(ID3D12Device* device, const wchar_t* debugLabel);
    void destroy();

    ID3D12CommandAllocator* getCommandAllocator(UINT frameIndex) const;
    ID3D12CommandQueue*     getCommandQueue() const;

  private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _commandAllocators[XP_DX12_BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>     _commandQueue;
};

} // namespace dx12al