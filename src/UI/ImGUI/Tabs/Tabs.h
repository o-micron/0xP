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

#include <Controllers/XPInput.h>
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <IconsFontAwesome5Pro.h>
#include <imgui.h>
#include <imgui_helper.h>
#include <imgui_stdlib.h>
#include <imguizmo.h>
#include <text_editor/TextEditor.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#define XP_COLOR_BLACK        0.f, 0.f, 0.f
#define XP_COLOR_DARKESTGRAY  0.1f, 0.1f, 0.1f
#define XP_COLOR_DARKERGRAY   0.175f, 0.175f, 0.175f
#define XP_COLOR_DARKGRAY     0.3f, 0.3f, 0.3f
#define XP_COLOR_GRAY         0.5f, 0.5f, 0.5f
#define XP_COLOR_LIGHTGRAY    0.6f, 0.6f, 0.6f
#define XP_COLOR_LIGHTERGRAY  0.7f, 0.7f, 0.7f
#define XP_COLOR_LIGHTESTGRAY 0.9f, 0.9f, 0.9f
#define XP_COLOR_WHITE        1.0f, 1.0f, 1.0f
#define XP_COLOR_YELLOW       0.9882f, 0.6392f, 0.0666f
#define XP_COLOR_PLATINUM     0.898f, 0.898f, 0.898f
#define XP_COLOR_RED          0.949f, 0.372f, 0.360f // #F25F5C, (242, 95, 92)
#define XP_COLOR_GREEN        0.427f, 0.609f, 0.486f // #6D9B7C, (109, 155, 124)
#define XP_COLOR_BLUE         0.003f, 0.729f, 0.937f // #01BAEF, (1, 186, 239)

#define XP_TAB_ASSETS_TITLE          "Assets"
#define XP_TAB_CONSOLE_TITLE         "Console"
#define XP_TAB_EDITOR_VIEWPORT_TITLE "Editor Viewport"
#define XP_TAB_GAME_VIEWPORT_TITLE   "Game Viewport"
#define XP_TAB_EMULATOR_TITLE        "Emulator"
#define XP_TAB_HIERARCHY_TITLE       "Hierarchy"
#define XP_TAB_LOGS_TITLE            "Logs"
#define XP_TAB_MATERIAL_EDITOR_TITLE "Material Editor"
#define XP_TAB_PAINT_TITLE           "Paint"
#define XP_TAB_PROFILER_TITLE        "Profiler"
#define XP_TAB_PROPERTIES_TITLE      "Properties"
#define XP_TAB_SETTINGS_TITLE        "Settings"
#define XP_TAB_TEXT_EDITOR_TITLE     "Text Editor"
#define XP_TAB_TEXTURE_EDITOR_TITLE  "Texture Editor"
#define XP_TAB_TRAITS_TITLE          "Traits"

#define XP_MAKE_UI_VIEW_MASK_BIT(NAME, ID)                                                                             \
    static constexpr uint32_t XPUiViewMask##NAME##Bit = ID;                                                            \
    static constexpr uint32_t XPUiViewMask##NAME      = 1 << XPUiViewMask##NAME##Bit;

XP_MAKE_UI_VIEW_MASK_BIT(GameViewport, 0)
XP_MAKE_UI_VIEW_MASK_BIT(Emulator, 1)
XP_MAKE_UI_VIEW_MASK_BIT(Assets, 2)
XP_MAKE_UI_VIEW_MASK_BIT(Console, 3)
XP_MAKE_UI_VIEW_MASK_BIT(EditorViewport, 4)
XP_MAKE_UI_VIEW_MASK_BIT(Hierarchy, 5)
XP_MAKE_UI_VIEW_MASK_BIT(Logs, 6)
XP_MAKE_UI_VIEW_MASK_BIT(MaterialEditor, 7)
XP_MAKE_UI_VIEW_MASK_BIT(Paint, 8)
XP_MAKE_UI_VIEW_MASK_BIT(Profiler, 9)
XP_MAKE_UI_VIEW_MASK_BIT(Properties, 10)
XP_MAKE_UI_VIEW_MASK_BIT(Settings, 11)
XP_MAKE_UI_VIEW_MASK_BIT(TextEditor, 12)
XP_MAKE_UI_VIEW_MASK_BIT(TextureEditor, 13)
XP_MAKE_UI_VIEW_MASK_BIT(Traits, 14)

#define XP_UI_VIEW_MASK_TABS_COUNT 15

// ------
// Any tab defined goes up, anything else that is not a tab to be defined down there
// ------

XP_MAKE_UI_VIEW_MASK_BIT(Colliders, XP_UI_VIEW_MASK_TABS_COUNT)
XP_MAKE_UI_VIEW_MASK_BIT(Materials, XP_UI_VIEW_MASK_TABS_COUNT + 1)
XP_MAKE_UI_VIEW_MASK_BIT(Textures, XP_UI_VIEW_MASK_TABS_COUNT + 2)
XP_MAKE_UI_VIEW_MASK_BIT(Shaders, XP_UI_VIEW_MASK_TABS_COUNT + 3)
XP_MAKE_UI_VIEW_MASK_BIT(Meshes, XP_UI_VIEW_MASK_TABS_COUNT + 4)
XP_MAKE_UI_VIEW_MASK_BIT(Scenes, XP_UI_VIEW_MASK_TABS_COUNT + 5)
XP_MAKE_UI_VIEW_MASK_BIT(Framebuffers, XP_UI_VIEW_MASK_TABS_COUNT + 6)
XP_MAKE_UI_VIEW_MASK_BIT(Scripts, XP_UI_VIEW_MASK_TABS_COUNT + 7)
XP_MAKE_UI_VIEW_MASK_BIT(RiscvBinaries, XP_UI_VIEW_MASK_TABS_COUNT + 8)
XP_MAKE_UI_VIEW_MASK_BIT(Files, XP_UI_VIEW_MASK_TABS_COUNT + 9)

static constexpr uint32_t XPUiViewMaskAll = UINT32_MAX;

#undef XP_MAKE_UI_VIEW_MASK_BIT

class XPUITab
{
  public:
    explicit XPUITab(XPRegistry* registry)
      : _registry(registry)
      , _input(registry)
      , _isFocused(false)
    {
    }
    virtual ~XPUITab() = default;
    virtual void render(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
    {
        XP_UNUSED(scene)

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, getWindowPadding());
        {
            if (ImGui::Begin(getTitle(), (bool*)0, getWindowFlags())) {
                _isFocused = ImGui::IsWindowFocused();

                openViewsMask |= getViewMask();
                {
                    ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                    if (dockNode) {
                        dockNode->LocalFlags |=
                          ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                    }
                }

                ImVec2 vMin   = ImGui::GetWindowContentRegionMin();
                ImVec2 vMax   = ImGui::GetWindowContentRegionMax();
                _windowSize.x = vMax.x - vMin.x;
                _windowSize.y = vMax.y - vMin.y;

                if (_isFocused) {
                    _input.update(deltaTime);
                } else {
                    _input.releaseAllKeyboardKeys();
                }
                renderView(_registry->getScene(), openViewsMask, deltaTime);
            } else {
                _isFocused = false;
                openViewsMask &= ~getViewMask();
            }
            ImGui::End();
        }
        ImGui::PopStyleVar();
    }
    virtual void renderFullScreen(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
    {
        XP_UNUSED(scene)

        auto resolution = _registry->getRenderer()->getWindowSize();

        ImGui::SetNextWindowSize(ImVec2((float)resolution.x, (float)resolution.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, getWindowPadding());
        {
            if (ImGui::Begin(getTitle(), (bool*)nullptr, getWindowFlags() | ImGuiWindowFlags_NoTitleBar)) {
                _isFocused = ImGui::IsWindowFocused();

                openViewsMask |= getViewMask();
                {
                    ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
                    if (dockNode) {
                        dockNode->LocalFlags |=
                          ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                    }
                }

                if (_isFocused) {
                    _input.update(deltaTime);
                } else {
                    _input.releaseAllKeyboardKeys();
                }
                renderView(_registry->getScene(), openViewsMask, deltaTime);
            } else {
                _isFocused = false;
                openViewsMask &= ~getViewMask();
            }
            ImGui::End();
        }
        ImGui::PopStyleVar();
    }
    [[nodiscard]] virtual bool hasFocus() const { return _isFocused; }

    virtual void                           onEvent(XPScene* scene, SDL_Event* event) = 0;
    virtual XPInput*                       getInput() { return &_input; }
    [[nodiscard]] virtual uint32_t         getViewMask() const      = 0;
    [[nodiscard]] virtual ImGuiWindowFlags getWindowFlags() const   = 0;
    [[nodiscard]] virtual const char*      getTitle() const         = 0;
    [[nodiscard]] virtual ImVec2           getWindowPadding() const = 0;
    [[nodiscard]] virtual XPVec2<float>    getWindowSize() const { return _windowSize; }

  private:
    virtual void renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime) = 0;

  protected:
    XPRegistry* const _registry;
    XPInput           _input;
    bool              _isFocused;
    XPVec2<float>     _windowSize;
};
