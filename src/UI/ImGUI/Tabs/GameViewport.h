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

#include <UI/ImGUI/Tabs/Tabs.h>

class XPGameViewportUITab final : public XPUITab
{
  public:
    XPGameViewportUITab(XPRegistry* const registry);
    void             onSceneViewClick(const XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn);
    void             renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime) final;
    void             renderFullScreen(XPScene* scene, uint32_t& openViewsMask, float deltaTime) override;
    void             onEvent(XPScene* scene, SDL_Event* event) final;
    uint32_t         getViewMask() const final;
    ImGuiWindowFlags getWindowFlags() const final;
    const char*      getTitle() const final;
    ImVec2           getWindowPadding() const final;

  private:
    bool _isHovered       = false;
    bool _isDragged       = false;
    bool _isReceivingDrop = false;
    bool _isUsingGuizmo   = false;
};
