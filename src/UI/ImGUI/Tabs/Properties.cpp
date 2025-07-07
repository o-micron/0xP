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

#include <UI/ImGUI/Tabs/Properties.h>

XPPropertiesUITab::XPPropertiesUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPPropertiesUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)
}

void
XPPropertiesUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPPropertiesUITab::getViewMask() const
{
    return XPUiViewMaskProperties;
}

ImGuiWindowFlags
XPPropertiesUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPPropertiesUITab::getTitle() const
{
    return XP_TAB_PROPERTIES_TITLE;
}

ImVec2
XPPropertiesUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}
