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

class XPDX12Window;

namespace dx12al {

class Swapchain
{
  public:
    Swapchain(XPDX12Window* window);
    ~Swapchain();
    void create(ID3D12Device*       device,
                ID3D12CommandQueue* commandQueue,
                IDXGIFactory4*      factory,
                int                 bufferCount,
                int                 width,
                int                 height);
    void destroy();

    IDXGISwapChain3* getSwapchain() const;

  private:
    XPDX12Window*                           _window;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> _swapChain;
};

} // namespace dx12al