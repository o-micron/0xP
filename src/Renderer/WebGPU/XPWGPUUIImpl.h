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

#include <Utilities/XPPlatforms.h>

#include <Renderer/WebGPU/wgpu_cpp.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/Interface/XPIUI.h>
#endif

class XPWGPUUIImpl final : public XPUIImpl
{
  public:
    XPWGPUUIImpl(XPIRenderer* const renderer, XPIUI* const ui);
    ~XPWGPUUIImpl() final;
    void        initialize() final;
    void        finalize() final;
    void        beginFrame() final;
    void        endFrame() final;
    XPIUI*      getUI() const final;
    void        invalidateDeviceObjects() final;
    void        createDeviceObjects() final;
    void        onEvent(SDL_Event* event) final;
    void        simulateCopy(const char* text) final;
    std::string simulatePaste() final;

  private:
    XPIRenderer* const    _renderer          = nullptr;
    XPIUI* const          _ui                = nullptr;
    WGPURenderPassEncoder _renderPassEncoder = nullptr;
    WGPUCommandEncoder    _commandEncoder    = nullptr;
};
