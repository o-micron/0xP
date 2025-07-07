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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <stdint.h>
#include <unordered_map>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
// TODO: try to remove those SDL dependencies from this header
// convenient for now to have keycodes there etc ...
#if defined(__EMSCRIPTEN__)
    #include <SDL_events.h>
    #include <SDL_keyboard.h>
    #include <SDL_keycode.h>
    #include <SDL_mouse.h>
#else
    #include <SDL2/SDL_events.h>
    #include <SDL2/SDL_keyboard.h>
    #include <SDL2/SDL_keycode.h>
    #include <SDL2/SDL_mouse.h>
#endif
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

class XPRegistry;

enum XPInputFlags : uint32_t
{
    XPInputFlags_None = 0,

    XPInputFlags_ShiftDown = (1U << 0),
    XPInputFlags_CtrlDown  = (1U << 1),
    XPInputFlags_AltDown   = (1U << 2),
    XPInputFlags_CmdDown   = (1U << 3),
    XPInputFlags_FnDown    = (1U << 4),

    XPInputFlags_LeftMouseDown   = (1U << 5),
    XPInputFlags_MiddleMouseDown = (1U << 6),
    XPInputFlags_RightMouseDown  = (1U << 7),
};

enum XPInputAction : uint8_t
{
    XPInputAction_Pressed  = (1U << 0),
    XPInputAction_Repeat   = (1U << 1),
    XPInputAction_Released = (1U << 2)
};

class XPInput
{
  public:
    XPInput(XPRegistry* const registry);
    ~XPInput();
    void        update(float deltaTime);
    void        releaseAllKeyboardKeys();
    void        onEvent(SDL_Event* event);
    void        onKeyboard(int key, int scancode, XPInputAction action, int mods);
    void        onMouse(int button, int action, int mods);
    void        subscribeKeyForStreams(int key);
    void        unsubscribeKeyForStreams(int key);
    float       fetchKeyNormalizedValue(int key) const;
    bool        fetchKeyPressed(int key) const;
    bool        isShiftDown() const;
    bool        isCtrlDown() const;
    bool        isAltDown() const;
    bool        isCmdDown() const;
    bool        isFnDown() const;
    bool        isLeftMouseDown() const;
    bool        isMiddleMouseDown() const;
    bool        isRightMouseDown() const;
    XPRegistry* getRegistry() const;

  private:
    XPRegistry* const                               _registry = nullptr;
    std::unordered_map<int, std::pair<bool, float>> _keyValues;
    XPInputFlags                                    _flags;
};
