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
class Synchronization
{
  public:
    Synchronization();
    ~Synchronization();
    void createFence(ID3D12Device* device);
    void destroyFence();
    void waitForGPU(ID3D12CommandQueue* commandQueue);
    void moveToNextFrame(ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapchain);
    UINT getFrameIndex() const;

  private:
    Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
    HANDLE _fenceHandle; // fired by the fence when the GPU signals it, CPU waits on this event handle
    UINT64 _fenceValues[XP_DX12_BUFFER_COUNT];
    UINT   _frameIndex;
};
} // namespace dx12al
