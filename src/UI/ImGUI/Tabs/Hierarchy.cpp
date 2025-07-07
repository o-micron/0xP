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

#include <UI/ImGUI/Tabs/Hierarchy.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPRegistry.h>
#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/Attachments/XPMeshRenderer.h>
#include <SceneDescriptor/Attachments/XPRigidbody.h>
#include <SceneDescriptor/Attachments/XPTransform.h>
#include <SceneDescriptor/XPScene.h>
#include <Shortcuts/XPShortcuts.h>

XPHierarchyUITab::XPHierarchyUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPHierarchyUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    static const std::function<void(XPNode*)> fnNode = [&](XPNode* node) {
        ImGuiTreeNodeFlags nodeTreeNodeFlags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
          ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (node->getNodes().empty()) { nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Leaf; }
        if (node->isSelected()) { nodeTreeNodeFlags |= ImGuiTreeNodeFlags_Selected; }
        if (ImGui::TreeNodeEx(node->getName().c_str(), nodeTreeNodeFlags, "%s", node->getName().c_str())) {
            // node context menu
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                if (ImGui::BeginPopupContextItem(node->getName().c_str())) {
                    if (ImGui::Selectable("Delete")) {
                        scene->getRegistry()->getEngine()->scheduleUITask(
                          [&]() { node->getAbsoluteLayer()->destroyNode(node); });
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta();
                if (dragDelta.x == 0.0f && dragDelta.y == 0.0f) {
                    //                            if (_window->isKeyboardShiftKeyDown()) {
                    //                                scene->addSelectedNode(node);
                    //                            } else {
                    scene->setSelectedNode(node);
                    //                            }
                }
            }
            for (XPNode* child : node->getNodes()) { fnNode(child); }
            ImGui::TreePop();
        }
    };
    static const std::function<void(XPLayer*)> fnLayer = [&](XPLayer* layer) {
        if (ImGui::TreeNode(layer->getName().c_str())) {
            // layer context menu
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                if (ImGui::BeginPopupContextItem(layer->getName().c_str())) {
                    if (ImGui::Selectable("Add Node")) {
                        auto&       freeCameras = _registry->getScene()->getNodes(FreeCameraAttachmentDescriptor);
                        FreeCamera* camera      = (*freeCameras.begin())->getFreeCamera();
                        spawnCube(_registry->getScene(), camera);
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }
            for (XPNode* child : layer->getNodes()) { fnNode(child); }
            ImGui::TreePop();
        }
    };
    for (XPLayer* layer : scene->getLayers()) { fnLayer(layer); }
}

void
XPHierarchyUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPHierarchyUITab::getViewMask() const
{
    return XPUiViewMaskHierarchy;
}

ImGuiWindowFlags
XPHierarchyUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPHierarchyUITab::getTitle() const
{
    return XP_TAB_HIERARCHY_TITLE;
}

ImVec2
XPHierarchyUITab::getWindowPadding() const
{
    return ImVec2(1.0f, 1.0f);
}
