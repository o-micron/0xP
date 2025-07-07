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

#include <Renderer/DX12/dx12al/Synchronization.hpp>

namespace dx12al {

Synchronization::Synchronization()
  : _frameIndex(0)
{
}

Synchronization::~Synchronization() {}

void
Synchronization::createFence(ID3D12Device* device)
{
    // create a GPU fence that will fire an event once the command list has been executed by the command queue.
    DX12_ASSERT(device->CreateFence(_fenceValues[_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
    ++_fenceValues[_frameIndex];
    // And the CPU event that the fence will fire off
    _fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void
Synchronization::destroyFence()
{
    // close the event handle so that fence can actually release()
    CloseHandle(_fenceHandle);
    _fence.Reset();
}

void
Synchronization::waitForGPU(ID3D12CommandQueue* commandQueue)
{
    // Schedule a Signal command in the queue.
    DX12_ASSERT(commandQueue->Signal(_fence.Get(), _fenceValues[_frameIndex]));

    // Wait until the fence has been processed.
    DX12_ASSERT(_fence->SetEventOnCompletion(_fenceValues[_frameIndex], _fenceHandle));
    WaitForSingleObjectEx(_fenceHandle, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    ++_fenceValues[_frameIndex];
}

// Prepare to render the next frame.
void
Synchronization::moveToNextFrame(ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapchain)
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = _fenceValues[_frameIndex];
    DX12_ASSERT(commandQueue->Signal(_fence.Get(), currentFenceValue));

    // Update the frame index.
    _frameIndex = swapchain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (_fence->GetCompletedValue() < _fenceValues[_frameIndex]) {
        DX12_ASSERT(_fence->SetEventOnCompletion(_fenceValues[_frameIndex], _fenceHandle));
        WaitForSingleObjectEx(_fenceHandle, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    _fenceValues[_frameIndex] = currentFenceValue + 1;
}

UINT
Synchronization::getFrameIndex() const
{
    return _frameIndex;
}

} // namespace dx12al
