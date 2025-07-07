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

#include <UI/ImGUI/Tabs/EditorViewport.h>

#include <Utilities/XPFreeCameraSystem.h>

XPEditorViewportUITab::XPEditorViewportUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
    _input.subscribeKeyForStreams(SDLK_w);
    _input.subscribeKeyForStreams(SDLK_s);
    _input.subscribeKeyForStreams(SDLK_d);
    _input.subscribeKeyForStreams(SDLK_a);
    _input.subscribeKeyForStreams(SDLK_i);
    _input.subscribeKeyForStreams(SDLK_k);

    // _input.subscribeKeyForStreams(SDLK_UP);
    // _input.subscribeKeyForStreams(SDLK_DOWN);
    // _input.subscribeKeyForStreams(SDLK_RIGHT);
    // _input.subscribeKeyForStreams(SDLK_LEFT);
}

void
XPEditorViewportUITab::onSceneViewClick(const XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn)
{
    _registry->getRenderer()->getSelectedNodeFromViewport(coordinates, cbfn);
}

void
XPEditorViewportUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(openViewsMask)

    auto freeCameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);
    // nothing to view without cameras ..
    if (freeCameraNodes.empty()) { return; }

    FreeCamera* freeCamera = (*freeCameraNodes.begin())->getFreeCamera();
    CameraSystemUpdateAll(freeCamera, _registry, _input, deltaTime);

    static ImGuizmo::OPERATION guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    static ImGuizmo::MODE      guizmoMode      = ImGuizmo::MODE::LOCAL;

    XPNode* selectedNode = nullptr;
    if (!scene->getSelectedNodes().empty()) { selectedNode = *scene->getSelectedNodes().begin(); }

    ImVec2 availableContentRegion = ImGui::GetContentRegionAvail();
    float  posX                   = ImGui::GetCursorPosX();
    float  posY                   = ImGui::GetCursorPosY();
    // ImGui::Image(scene->getRegistry()->getRenderer()->getMainTexture(), availableContentRegion);
    ImVec2 viewportImageSize          = ImGui::GetItemRectMax() - ImGui::GetItemRectMin();
    ImVec2 mouseNormalizedRelativePos = (ImGui::GetMousePos() - ImGui::GetItemRectMin()) / viewportImageSize;
    _isReceivingDrop                  = false;
    if (ImGui::BeginDragDropTarget()) {
        _isReceivingDrop = true;
        ImGui::EndDragDropTarget();
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
        if (!_isReceivingDrop && !_isUsingGuizmo && _isHovered) {
            if (!_isDragged && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                static std::function<void(XPNode * node)> cbfn;
                cbfn = [scene](XPNode* node) {
                    scene->clearSelectedNodes();
                    if (node) { scene->setSelectedNode(node); }
                };
                // map from viewport coordinates to whole window size coordinates
                onSceneViewClick(XPVec2<float>(mouseNormalizedRelativePos.x, mouseNormalizedRelativePos.y), cbfn);
            }
            _isDragged = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
        }
        _isHovered = true;
    } else {
        _isHovered = false;
    }
    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Tab)) {
        guizmoOperation = static_cast<ImGuizmo::OPERATION>((static_cast<int>(guizmoOperation) + 1) %
                                                           static_cast<int>(ImGuizmo::OPERATION::BOUNDS));
    }

    _isUsingGuizmo = false;
    if (selectedNode) {
        if (selectedNode->hasTransformAttachment()) {
            ImGuizmo::SetDrawlist();
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 siz = ImGui::GetWindowSize();
            ImGuizmo::SetRect(pos.x, pos.y, siz.x, siz.y);
            Transform*           tr = selectedNode->getTransform();
            static XPMat4<float> guizmoModelMatrix;
            auto                 operations = static_cast<XPMat4<float>::ModelMatrixOperations>(
              XPMat4<float>::ModelMatrixOperation_Translation | XPMat4<float>::ModelMatrixOperation_Rotation |
              XPMat4<float>::ModelMatrixOperation_Scale);
            XPMat4<float>::buildModelMatrix(tr->modelMatrix, tr->location, tr->euler, tr->scale, operations);
            memcpy(&guizmoModelMatrix._00, &tr->modelMatrix._00, sizeof(XPMat4<float>));
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::Manipulate(&freeCamera->activeProperties.viewMatrix._00,
                                 &freeCamera->activeProperties.projectionMatrix._00,
                                 guizmoOperation,
                                 guizmoMode,
                                 &guizmoModelMatrix._00);
            if (ImGuizmo::IsUsingAny()) {
                _isUsingGuizmo = true;
                XPVec3<float> location, rotation, scale;
                XPMat4<float>::decomposeModelMatrix(guizmoModelMatrix, location, rotation, scale);
                if (guizmoOperation == ImGuizmo::OPERATION::TRANSLATE) {
                    tr->location = location;
                } else if (guizmoOperation == ImGuizmo::OPERATION::ROTATE) {
                    tr->euler = rotation;
                } else if (guizmoOperation == ImGuizmo::OPERATION::SCALE) {
                    tr->scale = scale;
                }
                tr->owner->addAttachmentChanges(XPEInteractionHasTransformChanges, true, false);
            }
        }
    }
    auto renderer = _registry->getRenderer();
    auto console  = _registry->getEngine()->getConsole();
    if (console->getVariableValue<bool>("stats")) {
        ImGui::SetCursorPos(ImVec2(posX + 5.0f, posY + 5.0f));
        ImGui::Text("FPS: %.1f\nCPU: %.4f ms", ImGui::GetIO().Framerate, deltaTime);
        posY = ImGui::GetCursorPosY();
    }
    if (console->getVariableValue<bool>("stats_gpu")) {
        ImGui::SetCursorPos(ImVec2(posX + 5.0f, posY));
        ImGui::Text("RENDERING GPU: %.4f ms\nUI GPU: %.4f ms\nCOMPUTE GPU: %.4f ms",
                    renderer->getRenderingGPUTime(),
                    renderer->getUIGPUTime(),
                    renderer->getComputeGPUTime());
        posY = ImGui::GetCursorPosY();
    }
    ImGui::SetCursorPos(ImVec2(posX + 5.0f, posY));
    ImGui::Text("VERTICES: %u (%u occluded)\nDRAW CALLS: %u (%u occluded)",
                renderer->getNumDrawCallsVertices(),
                renderer->getTotalNumDrawCallsVertices() - renderer->getNumDrawCallsVertices(),
                renderer->getNumDrawCalls(),
                renderer->getTotalNumDrawCalls() - renderer->getNumDrawCalls());
}

void
XPEditorViewportUITab::onEvent(XPScene* scene, SDL_Event* event)
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
XPEditorViewportUITab::getViewMask() const
{
    return XPUiViewMaskEditorViewport;
}

ImGuiWindowFlags
XPEditorViewportUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPEditorViewportUITab::getTitle() const
{
    return XP_TAB_EDITOR_VIEWPORT_TITLE;
}

ImVec2
XPEditorViewportUITab::getWindowPadding() const
{
    return ImVec2(0.0f, 0.0f);
}
