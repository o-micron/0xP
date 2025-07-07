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

#include <Renderer/DX12/dx12al/Swapchain.hpp>

#include <Renderer/DX12/XPDX12Renderer.h>
#include <Renderer/DX12/XPDX12Window.h>

#include <D3Dcompiler.h>
#include <Windows.h>
#include <dxgi1_6.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#pragma clang diagnostic pop

namespace dx12al {

Swapchain::Swapchain(XPDX12Window* window)
  : _window(window)
{
}

Swapchain::~Swapchain() {}

void
Swapchain::create(ID3D12Device*       device,
                  ID3D12CommandQueue* commandQueue,
                  IDXGIFactory4*      factory,
                  int                 bufferCount,
                  int                 width,
                  int                 height)
{
    // Get the HWND from SDL
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(_window->getWindow(), &info);
    SDL_assert(info.subsystem == SDL_SYSWM_WINDOWS);
    assert(IsWindow(info.info.win.window));

    // Fill the swapchain description structure
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount          = bufferCount;
    swapChainDesc.BufferDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect           = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.OutputWindow         = info.info.win.window;
    swapChainDesc.SampleDesc.Count     = 1;
    swapChainDesc.Windowed             = TRUE;
    swapChainDesc.Flags                = 0;

    // Get the DXGI factory used to create the swap chain.
    IDXGIFactory2* dxgiFactory = nullptr;
    DX12_ASSERT(CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)&dxgiFactory));

    // Create the swap chain using the command queue, NOT using the device.  Thanks Killeak!
    DX12_ASSERT(
      dxgiFactory->CreateSwapChain(commandQueue, &swapChainDesc, (IDXGISwapChain**)_swapChain.GetAddressOf()));

    // increase max frame latency when required
    if (swapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT) {
        _swapChain->SetMaximumFrameLatency(bufferCount);
    }

    dxgiFactory->Release();
}

void
Swapchain::destroy()
{
    _swapChain.Reset();
}

IDXGISwapChain3*
Swapchain::getSwapchain() const
{
    return _swapChain.Get();
}

} // namespace dx12al
