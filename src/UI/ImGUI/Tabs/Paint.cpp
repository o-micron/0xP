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

#include <UI/ImGUI/Tabs/Paint.h>

#include <Utilities/XPFreeCameraSystem.h>

XPPaintUITab::XPPaintUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPPaintUITab::onSceneViewClick(const XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn)
{
    _registry->getRenderer()->getSelectedNodeFromViewport(coordinates, cbfn);
}

void
XPPaintUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(openViewsMask)

    // auto freeCameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);
    // // nothing to view without cameras ..
    // if (freeCameraNodes.empty()) { return; }

    // FreeCamera* freeCamera = (*freeCameraNodes.begin())->getFreeCamera();
    // CameraSystemUpdateAll(freeCamera, _registry, _input, deltaTime);

    // XPNode* selectedNode = nullptr;
    // if (!scene->getSelectedNodes().empty()) { selectedNode = *scene->getSelectedNodes().begin(); }
    // if (!selectedNode) { return; }

    // ImVec2 availableContentRegion = ImGui::GetContentRegionAvail();
    // ImGui::Image(scene->getRegistry()->getRenderer()->getMainTexture(), availableContentRegion);
    // ImVec2 viewportImageSize = ImGui::GetItemRectMax() - ImGui::GetItemRectMin();
}

void
XPPaintUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPPaintUITab::getViewMask() const
{
    return XPUiViewMaskPaint;
}

ImGuiWindowFlags
XPPaintUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPPaintUITab::getTitle() const
{
    return XP_TAB_PAINT_TITLE;
}

ImVec2
XPPaintUITab::getWindowPadding() const
{
    return ImVec2(0.0f, 0.0f);
}
