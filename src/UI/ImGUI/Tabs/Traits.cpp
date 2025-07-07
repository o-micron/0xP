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

#include <UI/ImGUI/Tabs/Traits.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

XPTraitsUITab::XPTraitsUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPTraitsUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    XPNode* selectedNode = nullptr;
    if (!scene->getSelectedNodes().empty()) { selectedNode = *scene->getSelectedNodes().begin(); }
    if (selectedNode == nullptr) { return; }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    if (ImGui::BeginPopupContextWindow(selectedNode->getName().c_str())) {
        for (uint32_t attachmenti = 0; attachmenti < AttachmentsTable.size(); ++attachmenti) {
            if (ImGui::Selectable(fmt::format("{} {}",
                                              selectedNode->hasAttachments(1U << attachmenti) ? "x " : "  ",
                                              AttachmentsTable[attachmenti].first)
                                    .c_str())) {
                if (selectedNode->hasAttachments(1U << attachmenti)) {
                    selectedNode->detach(1U << attachmenti);
                } else {
                    selectedNode->attach(1U << attachmenti);
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    selectedNode->renderAll(scene->getRegistry()->getUI());
}

void
XPTraitsUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPTraitsUITab::getViewMask() const
{
    return XPUiViewMaskTraits;
}

ImGuiWindowFlags
XPTraitsUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPTraitsUITab::getTitle() const
{
    return XP_TAB_TRAITS_TITLE;
}

ImVec2
XPTraitsUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}
