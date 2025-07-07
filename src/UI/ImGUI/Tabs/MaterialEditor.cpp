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

#include <UI/ImGUI/Tabs/MaterialEditor.h>

#include <imgui_node_editor/imgui_node_editor.h>
#include <imgui_node_editor/imgui_node_editor_internal.h>

XPMaterialEditorUITab::XPMaterialEditorUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
    _context = ax::NodeEditor::CreateEditor();
}

XPMaterialEditorUITab::~XPMaterialEditorUITab() { ax::NodeEditor::DestroyEditor(_context); }

void
XPMaterialEditorUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    ax::NodeEditor::SetCurrentEditor(_context);
    ax::NodeEditor::Begin("Material Editor");
    {
        int uniqueId = 1;
        // Start drawing nodes.
        ax::NodeEditor::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
        ImGui::Text("-> In");
        ax::NodeEditor::EndPin();
        ImGui::SameLine();
        ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
        ImGui::Text("Out ->");
        ax::NodeEditor::EndPin();
        ax::NodeEditor::EndNode();
    }
    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}

void
XPMaterialEditorUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPMaterialEditorUITab::getViewMask() const
{
    return XPUiViewMaskMaterialEditor;
}

ImGuiWindowFlags
XPMaterialEditorUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPMaterialEditorUITab::getTitle() const
{
    return XP_TAB_MATERIAL_EDITOR_TITLE;
}

ImVec2
XPMaterialEditorUITab::getWindowPadding() const
{
    return ImVec2(5.0f, 5.0f);
}
