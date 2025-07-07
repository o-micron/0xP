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

#include <UI/ImGUI/Tabs/Emulator.h>

#include <Emulator/XPEmulatorConfig.h>
#include <Emulator/XPEmulatorProcessor.h>
#include <SceneDescriptor/Attachments/XPScript.h>
#include <SceneDescriptor/XPNode.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

XPEmulatorUITab::XPEmulatorUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPEmulatorUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    XPNode* selectedNode = nullptr;
    if (!scene->getSelectedNodes().empty()) { selectedNode = *scene->getSelectedNodes().begin(); }
    if (selectedNode == nullptr || !selectedNode->hasScriptAttachment()) { return; }

    Script* script = selectedNode->getScript();

    if (!script->isLoaded) { return; }

    static MemoryEditor mem_edit;

    void*               mem_data          = script->processor->bus.memory.ram;
    size_t              mem_size          = XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE;
    size_t              base_display_addr = XP_EMULATOR_CONFIG_MEMORY_RAM_BASE;
    MemoryEditor::Sizes s;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("context");
    }
    mem_edit.DrawContents(mem_data, mem_size, base_display_addr);
}

void
XPEmulatorUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPEmulatorUITab::getViewMask() const
{
    return XPUiViewMaskEmulator;
}

ImGuiWindowFlags
XPEmulatorUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
}

const char*
XPEmulatorUITab::getTitle() const
{
    return XP_TAB_EMULATOR_TITLE;
}

ImVec2
XPEmulatorUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}
