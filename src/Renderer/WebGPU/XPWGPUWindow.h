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

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMacros.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

class XPWGPURenderer;

enum XPEWindowState : uint32_t
{
    XPEWindowStateOk         = 0U,
    XPEWindowStateLostFocus  = 1 << 0U,
    XPEWindowStateShouldQuit = 1 << 1U
};

class XPWGPUWindow
{
  public:
    XPWGPUWindow(XPWGPURenderer* const renderer);
    ~XPWGPUWindow();
    void initialize();
    void finalize();
    void pollEvents();
    void refresh();
    void onResizingEvents();

    XPWGPURenderer* getRenderer() const;
    GLFWwindow*     getWindow() const;
    uint32_t        getState() const;
    float           getRendererScale() const;
    XPVec2<int>     getWindowSize() const;
    XPVec2<int>     getRendererSize() const;
    XPVec2<float>   getMouseLocation() const;
    float           getDeltaTime() const;
    float           getTime() const;
    void            setRendererScale(float rendererScale);
    void            setWindowSize(XPVec2<int> size);
    void            simulateCopy(const char* text);
    std::string     simulatePaste();

    // called when mouse clicks events triggers
    void onMouseFn(int button, int action, int mods);

    // called when mouse cursor movement event triggers
    void onMouseMoveFn(double x, double y);

    // called when mouse scroll event triggers
    void onMouseScrollFn(double x, double y);

    // called when keyboard keys events trigger
    void onKeyboardFn(int key, int scancode, int action, int mods);

    // called when window resize event triggers
    void onWindowResizeFn(int width, int height);

    // called when drag and dropping files event triggers
    void onWindowDragDropFn(int numFiles, const char** names);

    // called when window iconify event triggers
    void onWindowIconifyFn();

    // called when window is restored from iconify event triggers
    void onWindowIconifyRestoreFn();

    // called when window maximize event triggers
    void onWindowMaximizeFn();

    // called when window is restored from maximize event triggers
    void onWindowMaximizeRestoreFn();

  private:
    XPWGPURenderer* const _renderer                = nullptr;
    GLFWwindow*           _window                  = nullptr;
    uint32_t              _state                   = XPEWindowStateOk;
    XPVec2<int>           _size                    = XPVec2<int>(1024, 720);
    float                 _rendererScale           = 1.0f;
    XPVec2<float>         _mouseLocation           = XPVec2<float>(0.0f, 0.0f);
    double                _timeInMilliseconds      = 0;
    double                _deltaTimeInMilliseconds = 0;
};
