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

#include "GameViewport.h"

#include <Engine/XPRegistry.h>
#include <SceneDescriptor/XPScene.h>

#include <Utilities/XPFreeCameraSystem.h>

XPGameViewportUITab::XPGameViewportUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
    _input.subscribeKeyForStreams(SDLK_w);
    _input.subscribeKeyForStreams(SDLK_s);
    _input.subscribeKeyForStreams(SDLK_d);
    _input.subscribeKeyForStreams(SDLK_a);
    _input.subscribeKeyForStreams(SDLK_i);
    _input.subscribeKeyForStreams(SDLK_k);
}

void
XPGameViewportUITab::onSceneViewClick(const XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn)
{
    _registry->getRenderer()->getSelectedNodeFromViewport(coordinates, cbfn);
}

void
XPGameViewportUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)
}

void
XPGameViewportUITab::renderFullScreen(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)

    // auto windowSize = _registry->getRenderer()->getWindowSize();

    // ImGui::SetNextWindowSize(ImVec2((float)windowSize.x, (float)windowSize.y));
    // ImGui::SetNextWindowPos(ImVec2(0, 0));

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, getWindowPadding());
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    // {
    //     if (ImGui::Begin(getTitle(), (bool*)nullptr, getWindowFlags() | ImGuiWindowFlags_NoTitleBar)) {
    //         _isFocused = ImGui::IsWindowFocused();
    //         if (_isFocused) {
    //             _input.update(deltaTime);
    //         } else {
    //             _input.releaseAllKeyboardKeys();
    //         }
    //         renderView(_registry->getScene(), openViewsMask, deltaTime);
    //     } else {
    //         _isFocused = false;
    //         openViewsMask &= ~getViewMask();
    //     }
    //     ImGui::End();
    // }
    // ImGui::PopStyleVar(2);
}

void
XPGameViewportUITab::onEvent(XPScene* scene, SDL_Event* event)
{
    // if (!scene) { return; }

    // if (event->type == SDL_MOUSEMOTION) {
    //     if (_isDragged) {
    //         auto freeCameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);
    //         // nothing to view without cameras ..
    //         if (!freeCameraNodes.empty()) {
    //             FreeCamera* freeCamera = (*freeCameraNodes.begin())->getFreeCamera();
    //             CameraSystemRotate(freeCamera, event->motion.xrel, event->motion.yrel);
    //         }
    //     }
    // }
    // if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
    //     ImGuiIO& io = ImGui::GetIO();
    //     if ((io.WantCaptureKeyboard && !_isFocused) || !_isHovered) { _input.releaseAllKeyboardKeys(); }
    //     if (_isHovered) { _input.onEvent(event); }
    // }
}

uint32_t
XPGameViewportUITab::getViewMask() const
{
    return XPUiViewMaskGameViewport;
}

ImGuiWindowFlags
XPGameViewportUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
           ImGuiWindowFlags_NoDecoration;
}

const char*
XPGameViewportUITab::getTitle() const
{
    return XP_TAB_GAME_VIEWPORT_TITLE;
}

ImVec2
XPGameViewportUITab::getWindowPadding() const
{
    return ImVec2(0.0f, 0.0f);
}
