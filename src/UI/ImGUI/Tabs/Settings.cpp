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

#include <UI/ImGUI/Tabs/Settings.h>

XPSettingsUITab::XPSettingsUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPSettingsUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    static char searchBuffer[128];
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputTextWithHint(
          "##settings search input text", ICON_FA_FILTER " Filter settings ...", searchBuffer, sizeof(searchBuffer))) {}
    ImGui::NewLine();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;

    if (ImGuiHelper::BeginGroupPanel("Theme")) {
        ImGui::Indent(16.0f);
        if (ImGui::BeginTable("##settings color content table", 2, tableFlags)) {
            ImGuiStyle& style = ImGui::GetStyle();

            // clang-format off
            static const std::map<std::string, std::pair<std::string, bool*>> boolProperties = {
                { "Anti-aliased lines", { "##UI [Bool] Anti-aliased lines", &style.AntiAliasedLines } },
                { "Anti-aliased lines use tex", { "##UI [Bool] Anti-aliased lines use tex", &style.AntiAliasedLinesUseTex } },
                { "Anti-aliased fill", { "##UI [Bool] Anti-aliased fill", &style.AntiAliasedFill } }
            };

            static const std::map<std::string, std::tuple<std::string, float*, float, float, float>> floatProperties = {
                { "Curve tessellation tol", { "##UI [Float] Curve tessellation tol", &style.CurveTessellationTol, 0.1f, 0.001f, 40.0f } },
                { "Circle segment max error", { "##UI [Float] Circle segment max error", &style.CircleTessellationMaxError, 0.1f, 0.001f, 40.0f } },
                { "Child rounding", { "##UI [Float] Child rounding", &style.ChildRounding, 0.1f, 0.0f, 40.0f } },
                { "Window rounding", { "##UI [Float] Window rounding", &style.WindowRounding, 0.1f, 0.0f, 40.0f } },
                { "Grab rounding", { "##UI [Float] Grab rounding", &style.GrabRounding, 0.1f, 0.0f, 40.0f } },
                { "Scrollbar rounding", { "##UI [Float] Scrollbar rounding", &style.ScrollbarRounding, 0.1f, 0.0f, 40.0f } },
                { "Scrollbar size", { "##UI [Float] Scrollbar size", &style.ScrollbarSize, 0.1f, 0.001f, 40.0f } },
                { "Frame rounding", { "##UI [Float] Frame rounding", &style.FrameRounding, 0.1f, 0.0f, 40.0f } },
                { "Tab rounding", { "##UI [Float] Tab rounding", &style.TabRounding, 0.1f, 0.0f, 40.0f } },
                { "Tab border size", { "##UI [Float] Tab border size", &style.TabBorderSize, 0.1f, 0.0f, 40.0f } }
            };

            static const std::map<std::string, std::tuple<std::string, float*, float, float, float>> vec2Properties = {
                { "Window title align", { "##UI [Vec2] Window title align", &style.WindowTitleAlign.x, 0.01f, 0.0f, 1.0f } }
            };

            static const std::map<std::string, std::pair<std::string, float*>> color4Properties = {
                { "Button", { "##UI [Vec4] Button", &style.Colors[ImGuiCol_Button].x } },
                { "Button hovered", { "##UI [Vec4] Button hovered", &style.Colors[ImGuiCol_ButtonHovered].x } },
                { "Button active", { "##UI [Vec4] Button active", &style.Colors[ImGuiCol_ButtonActive].x } },
                { "Text", { "##UI [Vec4] Text", &style.Colors[ImGuiCol_Text].x } },
                { "Text selected bg", { "##UI [Vec4] Text selected bg", &style.Colors[ImGuiCol_TextSelectedBg].x } },
                { "Text disabled", { "##UI [Vec4] Text disabled", &style.Colors[ImGuiCol_TextDisabled].x } },
                { "Window bg", { "##UI [Vec4] Window bg", &style.Colors[ImGuiCol_WindowBg].x } },
                { "Child bg", { "##UI [Vec4] Child bg", &style.Colors[ImGuiCol_ChildBg].x } },
                { "Popup bg", { "##UI [Vec4] Popup bg", &style.Colors[ImGuiCol_PopupBg].x } },
                { "Border", { "##UI [Vec4] Border", &style.Colors[ImGuiCol_Border].x } },
                { "Border shadow", { "##UI [Vec4] Border shadow", &style.Colors[ImGuiCol_BorderShadow].x } },
                { "Frame bg", { "##UI [Vec4] Frame bg", &style.Colors[ImGuiCol_FrameBg].x } },
                { "Frame bg hovered", { "##UI [Vec4] Frame bg hovered", &style.Colors[ImGuiCol_FrameBgHovered].x } },
                { "Frame bg active", { "##UI [Vec4] Frame bg active", &style.Colors[ImGuiCol_FrameBgActive].x } },
                { "Title bg", { "##UI [Vec4] Title bg", &style.Colors[ImGuiCol_TitleBg].x } },
                { "Title bg collapsed", { "##UI [Vec4] Title bg collapsed", &style.Colors[ImGuiCol_TitleBgCollapsed].x } },
                { "Title bg active", { "##UI [Vec4] Title bg active", &style.Colors[ImGuiCol_TitleBgActive].x } },
                { "Menubar bg", { "##UI [Vec4] Menubar bg", &style.Colors[ImGuiCol_MenuBarBg].x } },
                { "Tab", { "##UI [Vec4] Tab", &style.Colors[ImGuiCol_Tab].x } },
                { "Tab unfocused", { "##UI [Vec4] Tab unfocused", &style.Colors[ImGuiCol_TabUnfocused].x } },
                { "Tab hovered", { "##UI [Vec4] Tab hovered", &style.Colors[ImGuiCol_TabHovered].x } },
                { "Tab active", { "##UI [Vec4] Tab active", &style.Colors[ImGuiCol_TabActive].x } },
                { "Tab unfocused active", { "##UI [Vec4] Tab unfocused active", &style.Colors[ImGuiCol_TabUnfocusedActive].x } },
                { "Scrollbar bg", { "##UI [Vec4] Scrollbar bg", &style.Colors[ImGuiCol_ScrollbarBg].x } },
                { "Scrollbar grab", { "##UI [Vec4] Scrollbar grab", &style.Colors[ImGuiCol_ScrollbarGrab].x } },
                { "Scrollbar grab hovered", { "##UI [Vec4] Scrollbar grab hovered", &style.Colors[ImGuiCol_ScrollbarGrabHovered].x } },
                { "Scrollbar grab active", { "##UI [Vec4] Scrollbar grab active", &style.Colors[ImGuiCol_ScrollbarGrabActive].x } },
                { "Header", { "##UI [Vec4] Header", &style.Colors[ImGuiCol_Header].x } },
                { "Header hovered", { "##UI [Vec4] Header hovered", &style.Colors[ImGuiCol_HeaderHovered].x } },
                { "Header active", { "##UI [Vec4] Header active", &style.Colors[ImGuiCol_HeaderActive].x } },
                { "Plot lines", { "##UI [Vec4] Plot lines", &style.Colors[ImGuiCol_PlotLines].x } },
                { "Plot lines hovered", { "##UI [Vec4] Plot lines hovered", &style.Colors[ImGuiCol_PlotLinesHovered].x } },
                { "Plot histogram", { "##UI [Vec4] Plot histogram", &style.Colors[ImGuiCol_PlotHistogram].x } },
                { "Plot histogram hovered", { "##UI [Vec4] Plot histogram hovered", &style.Colors[ImGuiCol_PlotHistogramHovered].x } },
                { "Resize grip", { "##UI [Vec4] Resize grip", &style.Colors[ImGuiCol_ResizeGrip].x } },
                { "Resize grip hovered", { "##UI [Vec4] Resize grip hovered", &style.Colors[ImGuiCol_ResizeGripHovered].x } },
                { "Resize grip active", { "##UI [Vec4] Resize grip active", &style.Colors[ImGuiCol_ResizeGripActive].x } }
            };
            // clang-format on

            for (const auto& pair : boolProperties) {
                if (pair.first.find(searchBuffer) != std::string::npos) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(pair.first.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::Checkbox(pair.second.first.c_str(), pair.second.second);
                }
            }

            for (const auto& pair : floatProperties) {
                if (pair.first.find(searchBuffer) != std::string::npos) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(pair.first.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::DragFloat(std::get<0>(pair.second).c_str(),
                                     std::get<1>(pair.second),
                                     std::get<2>(pair.second),
                                     std::get<3>(pair.second),
                                     std::get<4>(pair.second));
                }
            }

            for (const auto& pair : vec2Properties) {
                if (pair.first.find(searchBuffer) != std::string::npos) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(pair.first.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::DragFloat2(std::get<0>(pair.second).c_str(),
                                      std::get<1>(pair.second),
                                      std::get<2>(pair.second),
                                      std::get<3>(pair.second),
                                      std::get<4>(pair.second));
                }
            }

            for (const auto& pair : color4Properties) {
                if (pair.first.find(searchBuffer) != std::string::npos) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(pair.first.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::ColorEdit4(pair.second.first.c_str(),
                                      pair.second.second,
                                      ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
                                        ImGuiColorEditFlags_DisplayRGB);
                }
            }

            ImGui::EndTable();
        }
        ImGui::Unindent(16.0f);
        ImGuiHelper::EndGroupPanel();
    }
}

void
XPSettingsUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPSettingsUITab::getViewMask() const
{
    return XPUiViewMaskSettings;
}

ImGuiWindowFlags
XPSettingsUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPSettingsUITab::getTitle() const
{
    return XP_TAB_SETTINGS_TITLE;
}

ImVec2
XPSettingsUITab::getWindowPadding() const
{
    return ImVec2(4.0f, 4.0f);
}
