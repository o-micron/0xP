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

#include <Controllers/XPInput.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

typedef struct SDL_Window   SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
class XPVulkanRenderer;

enum XPEWindowState : uint32_t
{
    XPEWindowStateOk         = 0U,
    XPEWindowStateLostFocus  = 1 << 0U,
    XPEWindowStateShouldQuit = 1 << 1U
};

class XPVulkanWindow
{
  public:
    explicit XPVulkanWindow(XPVulkanRenderer* renderer);
    ~XPVulkanWindow();
    void initialize();
    void finalize();
    void pollEvents();
    void refresh();

    [[nodiscard]] XPVulkanRenderer* getRenderer() const;
    [[nodiscard]] SDL_Window*       getWindow() const;
    [[nodiscard]] SDL_Renderer*     getWindowRenderer() const;
    [[nodiscard]] XPInput&          getInput() const;
    [[nodiscard]] uint32_t          getState() const;
    [[nodiscard]] XPVec2<int>       getWindowSize() const;
    [[nodiscard]] XPVec2<float>     getMouseLocation() const;
    [[nodiscard]] XPVec2<float>     getNormalizedMouseLocation() const;
    [[nodiscard]] bool              isLeftMouseButtonPressed() const;
    [[nodiscard]] bool              isMiddleMouseButtonPressed() const;
    [[nodiscard]] bool              isRightMouseButtonPressed() const;
    [[nodiscard]] float             getDeltaTimeSeconds() const;
    [[nodiscard]] uint64_t          getTimeMilliseconds() const;
    static void                     simulateCopy(const char* text);
    static std::string              simulatePaste();

    void createSurface(VkInstance instance, VkSurfaceKHR& surface);

  private:
    XPVulkanRenderer* const _renderer                = nullptr;
    SDL_Window*             _window                  = nullptr;
    uint32_t                _state                   = XPEWindowStateOk;
    XPVec2<float>           _mouseLocation           = XPVec2<float>(0.0f, 0.0f);
    XPVec2<float>           _normalizedMouseLocation = XPVec2<float>(0.0f, 0.0f);
    XPVec3<bool>            _mousePresses            = XPVec3<bool>(false, false, false);
    uint64_t                _timeInMilliseconds      = 0;
    uint64_t                _deltaTimeInMilliseconds = 0;
    XPInput*                _input                   = 0;
};
