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

#include <Controllers/XPInput.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <Renderer/DX12/XPDX12Renderer.h>
#include <Renderer/DX12/XPDX12Window.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPLogger.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#include <SDL2/SDL.h>
#pragma clang diagnostic pop

XPDX12Window::XPDX12Window(XPDX12Renderer* renderer)
  : _renderer(renderer)
  , _input(XP_NEW XPInput(renderer->getRegistry()))
{
    _input->subscribeKeyForStreams(SDLK_w);
    _input->subscribeKeyForStreams(SDLK_s);
    _input->subscribeKeyForStreams(SDLK_d);
    _input->subscribeKeyForStreams(SDLK_a);
    _input->subscribeKeyForStreams(SDLK_i);
    _input->subscribeKeyForStreams(SDLK_k);
}

XPDX12Window::~XPDX12Window() {}

XPProfilable void
XPDX12Window::initialize()
{
    _state = 0;

    auto console = _renderer->getRegistry()->getEngine()->getConsole();
    auto size    = console->getVariableValue<XPVec2<int>>("r.res");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to initialize window, %s", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, XP_FORCE_VSYNC ? "1" : "0");

    _window = SDL_CreateWindow("XPENGINE [" XP_PLATFORM_NAME "][" XP_CONFIG_NAME "][DX12]",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               size.x,
                               size.y,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!_window) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to create a window, %s", SDL_GetError());
        return;
    }

    _timeInMilliseconds = SDL_GetTicks64();
}

XPProfilable void
XPDX12Window::finalize()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

XPProfilable void
XPDX12Window::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            _state |= XPEWindowStateShouldQuit;
            _renderer->getRegistry()->getEngine()->quit();
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            // TODO: _state |= XPEWindowStateLostFocus;
            // TODO: shouldSleep = true;
            // SDL_Event e;
            // while (true) {
            //     if (SDL_WaitEvent(&e) == 1) {
            //         if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) { break; }
            //     }
            // }
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            // TODO: _state &= ~XPEWindowStateLostFocus;
            // TODO: shouldSleep = false;
        }
        if (event.type == SDL_WINDOWEVENT &&
            (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
             event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
             event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED)) {}

        if (event.type == SDL_MOUSEMOTION) {
            auto windowSize = getWindowSize();
            int  x, y;
            SDL_GetMouseState(&x, &y);
            _mouseLocation.x           = (float)x;
            _mouseLocation.y           = (float)y;
            _normalizedMouseLocation.x = (float)x / (float)windowSize.x;
            _normalizedMouseLocation.y = (float)y / (float)windowSize.y;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) { _mousePresses[0] = true; }
            if (event.button.button == SDL_BUTTON_MIDDLE) { _mousePresses[1] = true; }
            if (event.button.button == SDL_BUTTON_RIGHT) { _mousePresses[2] = true; }
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) { _mousePresses[0] = false; }
            if (event.button.button == SDL_BUTTON_MIDDLE) { _mousePresses[1] = false; }
            if (event.button.button == SDL_BUTTON_RIGHT) { _mousePresses[2] = false; }
        }

        _input->onEvent(&event);
    }
}

XPProfilable void
XPDX12Window::refresh()
{
    _deltaTimeInMilliseconds = SDL_GetTicks64() - _timeInMilliseconds;
    _deltaTimeInMilliseconds = _deltaTimeInMilliseconds <= 0 ? 1 : _deltaTimeInMilliseconds;
    _timeInMilliseconds      = SDL_GetTicks64();
    int width, height;
    SDL_GetRendererOutputSize(SDL_GetRenderer(_window), &width, &height);
    _input->update(_deltaTimeInMilliseconds);
}

XPDX12Renderer*
XPDX12Window::getRenderer() const
{
    return _renderer;
}

SDL_Window*
XPDX12Window::getWindow() const
{
    return _window;
}

SDL_Renderer*
XPDX12Window::getWindowRenderer() const
{
    return SDL_GetRenderer(_window);
}

XPInput&
XPDX12Window::getInput() const
{
    return *_input;
}

uint32_t
XPDX12Window::getState() const
{
    return _state;
}

XPVec2<int>
XPDX12Window::getWindowSize() const
{
    XPVec2<int> s;
    SDL_GetWindowSize(_window, &s.x, &s.y);
    return s;
}

XPVec2<float>
XPDX12Window::getMouseLocation() const
{
    return _mouseLocation;
}

XPVec2<float>
XPDX12Window::getNormalizedMouseLocation() const
{
    return _normalizedMouseLocation;
}

bool
XPDX12Window::isLeftMouseButtonPressed() const
{
    return _mousePresses.x;
}

bool
XPDX12Window::isMiddleMouseButtonPressed() const
{
    return _mousePresses.y;
}

bool
XPDX12Window::isRightMouseButtonPressed() const
{
    return _mousePresses.z;
}

float
XPDX12Window::getDeltaTimeSeconds() const
{
    return _deltaTimeInMilliseconds / 1000.0f;
}

uint64_t
XPDX12Window::getTimeMilliseconds() const
{
    return _timeInMilliseconds;
}

void
XPDX12Window::simulateCopy(const char* text)
{
    SDL_SetClipboardText(text);
}

std::string
XPDX12Window::simulatePaste()
{
    return { SDL_GetClipboardText() };
}

XPProfilable void
XPDX12Window::createSurface(int* adapterIndex, int* outputIndex)
{
    bool status = SDL_DXGIGetOutputInfo(SDL_GetWindowDisplayIndex(_window), adapterIndex, outputIndex);
    assert(status && "Failed to get DXGI adater and output indices for the specified display index");
}