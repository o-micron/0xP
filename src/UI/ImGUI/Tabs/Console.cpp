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

#include <UI/ImGUI/Tabs/Console.h>

XPConsoleUITab::XPConsoleUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPConsoleUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    static std::list<std::string> output;
    static std::string            buff = "";
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("%s", &buff, 0);
    scene->getRegistry()->getEngine()->getConsole()->renderVariablesUI(buff.c_str());
}

void
XPConsoleUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPConsoleUITab::getViewMask() const
{
    return XPUiViewMaskConsole;
}

ImGuiWindowFlags
XPConsoleUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPConsoleUITab::getTitle() const
{
    return XP_TAB_CONSOLE_TITLE;
}

ImVec2
XPConsoleUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}
