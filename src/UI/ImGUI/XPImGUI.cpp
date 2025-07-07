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

#include <UI/ImGUI/XPImGUI.h>

#include <UI/ImGUI/Tabs/Assets.h>
#include <UI/ImGUI/Tabs/Console.h>
#include <UI/ImGUI/Tabs/EditorViewport.h>
#include <UI/ImGUI/Tabs/Emulator.h>
#include <UI/ImGUI/Tabs/GameViewport.h>
#include <UI/ImGUI/Tabs/Hierarchy.h>
#include <UI/ImGUI/Tabs/Logs.h>
#include <UI/ImGUI/Tabs/MaterialEditor.h>
#include <UI/ImGUI/Tabs/Paint.h>
#include <UI/ImGUI/Tabs/Profiler.h>
#include <UI/ImGUI/Tabs/Properties.h>
#include <UI/ImGUI/Tabs/Settings.h>
#include <UI/ImGUI/Tabs/TextEditor.h>
#include <UI/ImGUI/Tabs/TextureEditor.h>
#include <UI/ImGUI/Tabs/Traits.h>

#include <DataPipeline/XPAssimpModelLoader.h>
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPIFileWatch.h>
#include <DataPipeline/XPLogic.h>
#include <DataPipeline/XPMaterialAsset.h>
#include <DataPipeline/XPMaterialBuffer.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>
#include <SceneDescriptor/Attachments/XPFreeCamera.h>
#include <SceneDescriptor/Attachments/XPLogic.h>
#include <SceneDescriptor/Attachments/XPMeshRenderer.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <Utilities/XPFS.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <IconsFontAwesome5Pro.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ImGuiFileDialog/ImGuiFileDialogConfig.h>
#include <imgui.h>
#include <imgui_helper.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <netimgui/client/NetImgui_Api.h>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <sstream>

XPImGuiUIImpl::XPImGuiUIImpl(XPRegistry* const registry)
  : XPIUI(registry)
  , _registry(registry)
{
}

XPImGuiUIImpl::~XPImGuiUIImpl() = default;

XPProfilable void
XPImGuiUIImpl::initialize()
{
    auto console = _registry->getRenderer()->getRegistry()->getEngine()->getConsole();
    auto size    = console->getVariableValue<XPVec2<int>>("r.res");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize             = ImVec2(size.x, size.y);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    (void)io;
    io.IniFilename = nullptr;
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad; // Enable NetImgui Gamepad support

    static std::function<ImFont*(const std::string&, float)> buildAwesomeFont = [](const std::string& fontFile,
                                                                                   float              size) -> ImFont* {
        ImGuiIO& io = ImGui::GetIO();
        ImFont*  font;

        // Load a first font
        {
            ImFontConfig icons_config;
            icons_config.MergeMode = false;
            font                   = io.Fonts->AddFontFromFileTTF(fontFile.c_str(), size, &icons_config);
        }

        // Add character ranges and merge into the previous font
        // The ranges array is not copied by the AddFont* functions and is used lazily
        // so ensure it is available at the time of building or calling GetTexDataAsRGBA32().
        {
            static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            ImFontConfig         icons_config;
            icons_config.MergeMode     = true;
            icons_config.PixelSnapH    = true;
            icons_config.GlyphOffset.y = 0.0f;
            io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 10.0f, &icons_config, icons_ranges);
            io.Fonts->Build();
        }

        return font;
    };

    // thin font
    setFontCode(buildAwesomeFont(XPFS::buildFontAssetsPath("JetBrainsMono-Bold.ttf"), 15));

    // extra-light font
    setFontExtraLight(buildAwesomeFont(XPFS::buildFontAssetsPath("FiraSans-ExtraLight.ttf"), 15));

    // light font
    setFontLight(buildAwesomeFont(XPFS::buildFontAssetsPath("FiraSans-Light.ttf"), 15));

    // regular font
    setFontRegular(buildAwesomeFont(XPFS::buildFontAssetsPath("FiraSans-Regular.ttf"), 15));

    // medium font
    setFontMedium(buildAwesomeFont(XPFS::buildFontAssetsPath("FiraSans-Medium.ttf"), 15));

    // semi-bold font
    setFontSemiBold(buildAwesomeFont(XPFS::buildFontAssetsPath("FiraSans-SemiBold.ttf"), 15));

    // Setup style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.AntiAliasedLines           = true;
    style.AntiAliasedLinesUseTex     = true;
    style.AntiAliasedFill            = true;
    style.ChildRounding              = 5.0f;
    style.CircleTessellationMaxError = 10.0f;
    style.CurveTessellationTol       = 10.0f;
    style.FrameRounding              = 0.0f;
    style.GrabRounding               = 5.0f;
    style.ScrollbarRounding          = 0.0f;
    style.ScrollbarSize              = 4.0f;
    style.TabBorderSize              = 0.0f;
    style.TabRounding                = 0.0f;
    style.WindowRounding             = 5.0f;
    style.WindowTitleAlign           = ImVec2(0.01f, 0.5f);
    style.PopupRounding              = 5.0f;

    style.Colors[ImGuiCol_Border]               = ImVec4(0.235f, 0.243f, 0.259f, 1.000f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.200f, 0.250f, 0.290f, 1.000f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.459f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.000f, 1.000f, 0.627f, 0.588f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.231f, 0.239f, 0.286f, 0.392f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.192f, 0.200f, 0.208f, 0.392f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.153f, 0.161f, 0.169f, 0.392f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.460f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(1.000f, 1.000f, 1.000f, 0.700f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.000f, 1.000f, 0.627f, 0.588f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.941f, 0.776f, 0.455f, 1.000f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.900f, 0.700f, 0.000f, 1.000f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.600f, 0.000f, 1.000f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.610f, 0.610f, 0.610f, 1.000f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.000f, 0.430f, 0.350f, 1.000f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.459f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.459f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.000f, 1.000f, 0.627f, 1.000f);
    style.Colors[ImGuiCol_Tab]                  = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_TabActive]            = ImVec4(0.459f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_TabHovered]           = ImVec4(0.000f, 1.000f, 0.627f, 0.392f);
    style.Colors[ImGuiCol_TabUnfocused]         = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);
    style.Colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.459f, 0.000f, 1.000f, 1.000f);
    style.Colors[ImGuiCol_Text]                 = ImVec4(0.950f, 0.960f, 0.980f, 1.000f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.360f, 0.420f, 0.470f, 1.000f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.506f, 0.635f, 0.745f, 1.000f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.114f, 0.122f, 0.129f, 1.000f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.157f, 0.165f, 0.180f, 1.000f);

    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { style.Colors[ImGuiCol_WindowBg].w = 1.0f; }

    // tabs
    {
        static_assert(XP_UI_VIEW_MASK_TABS_COUNT == XP_UI_VIEW_MASK_TABS_COUNT, "Make sure tabs count are identical.");

#define CHECK_VIEW_MASK_INDICES(NAME, INDEX)                                                                           \
    static_assert(XPUiViewMask##NAME##Bit == INDEX);                                                                   \
    _tabs[INDEX] = std::make_unique<XP##NAME##UITab>(_registry);

        CHECK_VIEW_MASK_INDICES(GameViewport, 0)
        CHECK_VIEW_MASK_INDICES(Emulator, 1)
        CHECK_VIEW_MASK_INDICES(Assets, 2)
        CHECK_VIEW_MASK_INDICES(Console, 3)
        CHECK_VIEW_MASK_INDICES(EditorViewport, 4)
        CHECK_VIEW_MASK_INDICES(Hierarchy, 5)
        CHECK_VIEW_MASK_INDICES(Logs, 6)
        CHECK_VIEW_MASK_INDICES(MaterialEditor, 7)
        CHECK_VIEW_MASK_INDICES(Paint, 8)
        CHECK_VIEW_MASK_INDICES(Profiler, 9)
        CHECK_VIEW_MASK_INDICES(Properties, 10)
        CHECK_VIEW_MASK_INDICES(Settings, 11)
        CHECK_VIEW_MASK_INDICES(TextEditor, 12)
        CHECK_VIEW_MASK_INDICES(TextureEditor, 13)
        CHECK_VIEW_MASK_INDICES(Traits, 14)
#undef CHECK_VIEW_MASK_INDICES

        assert(_tabs.size() == XP_UI_VIEW_MASK_TABS_COUNT);
    }

    // remote imgui
    if (!NetImgui::Startup()) { XP_LOG(XPLoggerSeverityFatal, "Couldn't start remote ui session"); }
}

XPProfilable void
XPImGuiUIImpl::finalize()
{
    NetImgui::Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

XPProfilable void
XPImGuiUIImpl::update(float deltaTime)
{
    enum eSampleState : uint8_t
    {
        Start,
        Disconnected,
        Connected,
    };
    static eSampleState sampleState = eSampleState::Start;
    // try connect to server ..
    {
        constexpr char zClientName[] = "XP[REMOTE]";
        if (sampleState == eSampleState::Start) {
            NetImgui::ConnectToApp(zClientName, "localhost");
            while (NetImgui::IsConnectionPending()) {}
            bool bSuccess = NetImgui::IsConnected();
            sampleState   = bSuccess ? eSampleState::Connected : eSampleState::Disconnected;
            if (!bSuccess) { NetImgui::ConnectFromApp(zClientName); }
        } else if (sampleState == eSampleState::Disconnected && NetImgui::IsConnected()) {
            sampleState = eSampleState::Connected;
        } else if (sampleState == eSampleState::Connected && !NetImgui::IsConnected()) {
            sampleState = eSampleState::Disconnected;
            NetImgui::Disconnect();
            NetImgui::ConnectFromApp(zClientName);
        }
    }
    bool success = NetImgui::IsConnected() && NetImgui::NewFrame(true);
    if (success) {
        if (_showEditor) {
            renderEditorUI(deltaTime);
        } else {
            renderGameUI(deltaTime);
        }
    }
    NetImgui::EndFrame();
}

XPProfilable void
XPImGuiUIImpl::onEvent(SDL_Event* event)
{
    if (event->type == SDL_KEYUP) {
        if (event->key.keysym.sym == SDLK_F9) {
            static std::vector<const char*> focusedWindows;
            _showEditor = !_showEditor;
        }
    }
    for (auto& tab : _tabs) { tab->onEvent(_registry->getScene(), event); }
}

XPRegistry*
XPImGuiUIImpl::getRegistry()
{
    return _registry;
}

XPProfilable XPEHeaderFlags
XPImGuiUIImpl::renderHeader(XPNode* owner, const char* name)
{
    XP_UNUSED(owner)

    ImGui::PushID(name);
    uint32_t               flags      = XPEHeaderFlagsNone;
    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;
    if (ImGuiHelper::BeginGroupPanel(name)) {
        ImGui::Indent(16.0f);
        flags |= XPEHeaderFlagsTreeOpen;
        if (ImGui::BeginTable(name, 2, tableFlags)) { flags |= XPEHeaderFlagsTableOpen; }
    }
    return static_cast<XPEHeaderFlags>(flags);
}

XPProfilable void
XPImGuiUIImpl::renderFooter(XPNode* owner, const char* name, XPEHeaderFlags flags)
{
    XP_UNUSED(owner)
    XP_UNUSED(name)

    if ((flags & XPEHeaderFlagsTableOpen) == XPEHeaderFlagsTableOpen) { ImGui::EndTable(); }
    if ((flags & XPEHeaderFlagsTreeOpen) == XPEHeaderFlagsTreeOpen) {
        ImGui::Unindent(16.0f);
        ImGuiHelper::EndGroupPanel();
    }
    ImGui::PopID();
}

XPProfilable XPEHeaderFlags
XPImGuiUIImpl::renderHeadless(XPNode* owner, const char* name)
{
    XP_UNUSED(owner)

    ImGui::PushID(name);
    uint32_t               flags      = XPEHeaderFlagsNone;
    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;

    if (ImGui::BeginTable(name, 2, tableFlags)) { flags |= XPEHeaderFlagsTableOpen; }
    return static_cast<XPEHeaderFlags>(flags);
}

XPProfilable void
XPImGuiUIImpl::renderFootless(XPNode* owner, const char* name, XPEHeaderFlags flags)
{
    XP_UNUSED(owner)
    XP_UNUSED(name)

    if ((flags & XPEHeaderFlagsTableOpen) == XPEHeaderFlagsTableOpen) { ImGui::EndTable(); }
    ImGui::PopID();
}

XPEHeaderFlags
XPImGuiUIImpl::beginRenderStructSecondary(XPNode* owner, const char* name)
{
    ImGui::PushID(name);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(name);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::Indent(16.0f);
    uint32_t               flags      = XPEHeaderFlagsNone;
    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoBordersInBody |
                                        ImGuiTableFlags_PreciseWidths;

    if (ImGui::BeginTable(name, 2, tableFlags)) { flags |= XPEHeaderFlagsTableOpen; }
    return static_cast<XPEHeaderFlags>(flags);
}

void
XPImGuiUIImpl::endRenderStructSecondary(XPNode* owner, const char* name, XPEHeaderFlags flags)
{
    if ((flags & XPEHeaderFlagsTableOpen) == XPEHeaderFlagsTableOpen) { ImGui::EndTable(); }
    ImGui::Unindent(16.0f);
    ImGui::PopID();
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, bool& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::Checkbox(name, &value)) { ret = 1; }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, int8_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_S8, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, int16_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_S16, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, int32_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_S32, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, int64_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_S64, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, uint8_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_U8, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, uint16_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_U16, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, uint32_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_U32, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, uint64_t& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_U64, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, float& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_Float, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, double& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputScalar(
              name, ImGuiDataType_Double, &value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, char* value, size_t valueSize)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText(name, value, valueSize, ImGuiInputTextFlags_EnterReturnsTrue)) { ret = 1; }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, std::string& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText(name, &value, ImGuiInputTextFlags_EnterReturnsTrue)) { ret = 1; }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPVec2<int>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 2.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputInt("##0", &value.x, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) { ret = 1; }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputInt("##1", &value.y, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) { ret = 2; }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPVec2<float>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 2.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##0", ImGuiDataType_Float, &value.x, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##1", ImGuiDataType_Float, &value.y, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 2;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPVec3<float>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 3.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##0", ImGuiDataType_Float, &value.x, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##1", ImGuiDataType_Float, &value.y, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 2;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##2", ImGuiDataType_Float, &value.z, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 3;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPVec4<float>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 4.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##0", ImGuiDataType_Float, &value.x, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##1", ImGuiDataType_Float, &value.y, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 2;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##2", ImGuiDataType_Float, &value.z, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 3;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar(
              "##3", ImGuiDataType_Float, &value.w, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 4;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPMat3<float>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 3.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##00",
                               ImGuiDataType_Float,
                               &value._00,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##01",
                               ImGuiDataType_Float,
                               &value._01,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 2;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##02",
                               ImGuiDataType_Float,
                               &value._02,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 3;
        }

        //
        cursorPosition = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##10",
                               ImGuiDataType_Float,
                               &value._10,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 4;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##11",
                               ImGuiDataType_Float,
                               &value._11,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 5;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##12",
                               ImGuiDataType_Float,
                               &value._12,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 6;
        }

        //
        cursorPosition = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##20",
                               ImGuiDataType_Float,
                               &value._20,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 7;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##21",
                               ImGuiDataType_Float,
                               &value._21,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 8;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##22",
                               ImGuiDataType_Float,
                               &value._22,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 9;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPMat4<float>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name + 2);
        ImGui::TableSetColumnIndex(1);

        constexpr float padding   = 3.0f;
        constexpr float numFields = 4.0f;

        float pieceSize      = (ImGui::GetContentRegionAvail().x - 2.0f * padding) / numFields;
        float cursorPosition = ImGui::GetCursorPosX();

        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##00",
                               ImGuiDataType_Float,
                               &value._00,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 1;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##01",
                               ImGuiDataType_Float,
                               &value._01,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 2;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##02",
                               ImGuiDataType_Float,
                               &value._02,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 3;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##03",
                               ImGuiDataType_Float,
                               &value._03,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 4;
        }

        //
        cursorPosition = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##10",
                               ImGuiDataType_Float,
                               &value._10,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 5;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##11",
                               ImGuiDataType_Float,
                               &value._11,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 6;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##12",
                               ImGuiDataType_Float,
                               &value._12,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 7;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##13",
                               ImGuiDataType_Float,
                               &value._13,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 8;
        }

        //
        cursorPosition = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##20",
                               ImGuiDataType_Float,
                               &value._20,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 9;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##21",
                               ImGuiDataType_Float,
                               &value._21,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 10;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##22",
                               ImGuiDataType_Float,
                               &value._22,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 11;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##23",
                               ImGuiDataType_Float,
                               &value._23,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 12;
        }

        //
        cursorPosition = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##30",
                               ImGuiDataType_Float,
                               &value._30,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 13;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##31",
                               ImGuiDataType_Float,
                               &value._31,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 14;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##32",
                               ImGuiDataType_Float,
                               &value._32,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 15;
        }

        ImGui::SameLine();
        cursorPosition += pieceSize + padding;
        ImGui::SetCursorPosX(cursorPosition);
        ImGui::SetNextItemWidth(pieceSize);
        if (ImGui::InputScalar("##33",
                               ImGuiDataType_Float,
                               &value._33,
                               nullptr,
                               nullptr,
                               nullptr,
                               ImGuiInputTextFlags_EnterReturnsTrue)) {
            ret = 16;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, std::list<XPLogicSource>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        uint8_t counter = 0;
        for (XPLogicSource& logicSource : value) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", logicSource.logic->getName().c_str());
            ImGui::TableSetColumnIndex(1);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::Button((char*)logicSource.logic->getName().c_str(),
                          ImVec2(ImGui::GetContentRegionAvail().x - 35.0f, 30.0f));
            ImGui::PopStyleVar();
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_LOGIC")) {
                    IM_ASSERT(payload->DataSize == sizeof(XPLogic*));
                    XPLogic* logic = *(XPLogic**)payload->Data;
                    logicSource.logic->getEndFn()(owner);
                    logicSource.logic = logic;
                    logicSource.logic->getStartFn()(owner);
                    ret = counter + 1;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();
            ImGui::PushID(logicSource.logic);
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                if (ImGui::Button("-", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) { ret = counter + 2; }
                ImGui::PopStyleVar();
            }
            ImGui::PopID();
            counter += 2;
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, std::vector<XPMeshRendererInfo>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        for (XPMeshRendererInfo& meshRendererInfo : value) {
            ImGui::PushID(&meshRendererInfo);
            {
                ret += renderField(owner, name, meshRendererInfo.mesh);
                ret += renderField(owner, name, meshRendererInfo.material);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Polygon mode");
                ImGui::TableSetColumnIndex(1);

                static const char* modes[XPEMeshRendererPolygonModeCount] = {
                    XPEMeshRendererPolygonModePointStr.data(),
                    XPEMeshRendererPolygonModeLineStr.data(),
                    XPEMeshRendererPolygonModeFillStr.data()
                };

                static int selector = 0;
                selector            = static_cast<int>(meshRendererInfo.polygonMode);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::Combo("##PolygonModes", &selector, modes, XPEMeshRendererPolygonModeCount)) {
                    meshRendererInfo.polygonMode = static_cast<XPEMeshRendererPolygonMode>(selector);
                    ++ret;
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::PopID();

    return ret;
}

uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, std::vector<XPColliderInfo>& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        for (XPColliderInfo& colliderInfo : value) {
            ImGui::PushID(&colliderInfo);
            {
                ret += renderField(owner, name, colliderInfo.shapeName);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Shape");
                ImGui::TableSetColumnIndex(1);

                static const char* modes[XPEColliderShapeCount] = {
                    XPEColliderShapePlaneStr.data(),       XPEColliderShapeBoxStr.data(),
                    XPEColliderShapeSphereStr.data(),      XPEColliderShapeCapsuleStr.data(),
                    XPEColliderShapeCylinderStr.data(),    XPEColliderShapeConvexMeshStr.data(),
                    XPEColliderShapeTriangleMeshStr.data()
                };

                static int selector = 0;
                selector            = static_cast<int>(colliderInfo.shape);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::Combo("##Shapes", &selector, modes, XPEColliderShapeCount)) {
                    colliderInfo.shape = static_cast<XPEColliderShape>(selector);
                    ++ret;
                }

                switch (colliderInfo.shape) {
                    case XPEColliderShapePlane: {
                        ret += renderField(owner, "##Width", colliderInfo.parameters.planeWidth);
                        ret += renderField(owner, "##Depth", colliderInfo.parameters.planeDepth);
                    } break;
                    case XPEColliderShapeBox: {
                        ret += renderField(owner, "##Width", colliderInfo.parameters.boxWidth);
                        ret += renderField(owner, "##Height", colliderInfo.parameters.boxHeight);
                        ret += renderField(owner, "##Depth", colliderInfo.parameters.boxDepth);
                    } break;
                    case XPEColliderShapeSphere: {
                        ret += renderField(owner, "##Radius", colliderInfo.parameters.sphereRadius);
                    } break;
                    case XPEColliderShapeCapsule: {
                        ret += renderField(owner, "##Radius", colliderInfo.parameters.capsuleRadius);
                        ret += renderField(owner, "##Height", colliderInfo.parameters.capsuleHeight);
                        ret += renderField(owner, "##Density", colliderInfo.parameters.capsuleDensity);
                    } break;
                    case XPEColliderShapeCylinder: {
                        ret += renderField(owner, "##Radius", colliderInfo.parameters.cylinderRadius);
                        ret += renderField(owner, "##Height", colliderInfo.parameters.cylinderHeight);
                    } break;
                    default: break;
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPColliderRefString& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(&value);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("shape");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        std::string popupID = fmt::format("{} menu context", name);
        const bool  is_input_text_enter_pressed =
          ImGui::InputText("%s", &value.inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool is_input_text_active    = ImGui::IsItemActive();
        const bool is_input_text_activated = ImGui::IsItemActivated();

        if (is_input_text_activated) {
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 100));
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            ImGui::OpenPopup(popupID.c_str());
        }

        if (ImGui::BeginPopup(popupID.c_str(),
                              ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_ChildWindow)) {
            for (auto& meshAsset : _registry->getDataPipelineStore()->getMeshAssets()) {
                switch (meshAsset.second->getFile()->getResourceType()) {
                    case XPEFileResourceType::Unknown:
                    case XPEFileResourceType::Shader:
                    case XPEFileResourceType::Texture:
                    case XPEFileResourceType::Plugin:
                    case XPEFileResourceType::RiscvBinary:
                    case XPEFileResourceType::Count: break;
                    case XPEFileResourceType::PreloadedMesh: {
                        auto& suggestion = meshAsset.second->getFile()->getPath();
                        if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                            if (ImGui::Selectable(suggestion.c_str())) {
                                ImGui::ClearActiveID();
                                value.inputBuffer = suggestion;
                                value.text        = value.inputBuffer;
                            }
                        }
                    } break;
                    case XPEFileResourceType::Mesh: {
                        auto& suggestion = meshAsset.second->getFile()->getPath();
                        if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                            if (ImGui::Selectable(suggestion.c_str())) {
                                ImGui::ClearActiveID();
                                value.inputBuffer = suggestion;
                                value.text        = value.inputBuffer;
                            }
                        }
                    } break;
                    case XPEFileResourceType::Scene: {
                        for (auto& sceneMeshAsset : meshAsset.second->getFile()->getMeshAssets()) {
                            for (size_t mboi = 0; mboi < sceneMeshAsset->getMeshBuffer()->getObjectsCount(); ++mboi) {
                                XPMeshBufferObject& sceneMeshAssetObject =
                                  sceneMeshAsset->getMeshBuffer()->objectAtIndex(mboi);
                                auto& suggestion = sceneMeshAssetObject.name;
                                if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                                    if (ImGui::Selectable(suggestion.c_str())) {
                                        ImGui::ClearActiveID();
                                        value.inputBuffer = suggestion;
                                        value.text        = value.inputBuffer;
                                    }
                                }
                            }
                        }
                    } break;
                }
            }

            if (is_input_text_enter_pressed || (!is_input_text_active && !ImGui::IsWindowFocused())) {
                ret = 1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPMeshRefString& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(&value);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("mesh");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        std::string popupID = fmt::format("{} menu context", name);
        const bool  is_input_text_enter_pressed =
          ImGui::InputText("%s", &value.inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool is_input_text_active    = ImGui::IsItemActive();
        const bool is_input_text_activated = ImGui::IsItemActivated();

        if (is_input_text_activated) {
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 100));
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            ImGui::OpenPopup(popupID.c_str());
        }

        if (ImGui::BeginPopup(popupID.c_str(),
                              ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_ChildWindow)) {
            for (auto& meshAsset : _registry->getDataPipelineStore()->getMeshAssets()) {
                switch (meshAsset.second->getFile()->getResourceType()) {
                    case XPEFileResourceType::Unknown:
                    case XPEFileResourceType::Shader:
                    case XPEFileResourceType::Texture:
                    case XPEFileResourceType::Plugin:
                    case XPEFileResourceType::RiscvBinary:
                    case XPEFileResourceType::Count: break;
                    case XPEFileResourceType::PreloadedMesh: {
                        auto& suggestion = meshAsset.second->getFile()->getPath();
                        if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                            if (ImGui::Selectable(suggestion.c_str())) {
                                ImGui::ClearActiveID();
                                value.inputBuffer = suggestion;
                                value.text        = value.inputBuffer;
                            }
                        }
                    } break;
                    case XPEFileResourceType::Mesh: {
                        auto& suggestion = meshAsset.second->getFile()->getPath();
                        if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                            if (ImGui::Selectable(suggestion.c_str())) {
                                ImGui::ClearActiveID();
                                value.inputBuffer = suggestion;
                                value.text        = value.inputBuffer;
                            }
                        }
                    } break;
                    case XPEFileResourceType::Scene: {
                        for (auto& sceneMeshAsset : meshAsset.second->getFile()->getMeshAssets()) {
                            for (size_t mboi = 0; mboi < sceneMeshAsset->getMeshBuffer()->getObjectsCount(); ++mboi) {
                                XPMeshBufferObject& sceneMeshAssetObject =
                                  sceneMeshAsset->getMeshBuffer()->objectAtIndex(mboi);
                                auto& suggestion = sceneMeshAssetObject.name;
                                if (strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                                    if (ImGui::Selectable(suggestion.c_str())) {
                                        ImGui::ClearActiveID();
                                        value.inputBuffer = suggestion;
                                        value.text        = value.inputBuffer;
                                    }
                                }
                            }
                        }
                    } break;
                }
            }

            if (is_input_text_enter_pressed || (!is_input_text_active && !ImGui::IsWindowFocused())) {
                ret = 1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, XPMaterialRefString& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(&value);
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("material");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        std::string popupID = fmt::format("{} menu context", name);
        const bool  is_input_text_enter_pressed =
          ImGui::InputText("%s", &value.inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool is_input_text_active    = ImGui::IsItemActive();
        const bool is_input_text_activated = ImGui::IsItemActivated();

        if (is_input_text_activated) { ImGui::OpenPopup(popupID.c_str()); }

        ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 50));
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        if (ImGui::BeginPopup(popupID.c_str(),
                              ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
            for (auto& materialAsset : _registry->getDataPipelineStore()->getMaterialAssets()) {
                auto& suggestion = materialAsset.second->getName();
                if (!suggestion.empty() && strstr(suggestion.c_str(), value.inputBuffer.c_str()) != nullptr) {
                    if (ImGui::Selectable(suggestion.c_str())) {
                        ImGui::ClearActiveID();
                        value.inputBuffer = suggestion;
                        value.text        = value.inputBuffer;
                    }
                }
            }

            if (is_input_text_enter_pressed || (!is_input_text_active && !ImGui::IsWindowFocused())) {
                ret = 1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    ImGui::PopID();

    return ret;
}

XPProfilable uint8_t
XPImGuiUIImpl::renderField(XPNode* owner, const char* name, CameraProperties& value)
{
    XP_UNUSED(owner)

    uint8_t ret = 0;

    ImGui::PushID(name);
    {
        ret += renderField(owner, "##Position", value.location);
        ret += renderField(owner, "##Rotation", value.euler);
        ret += renderField(owner, "##Fov", value.fov);
        ret += renderField(owner, "##ZNear", value.znear);
        ret += renderField(owner, "##ZFar", value.zfar);
    }
    ImGui::PopID();

    return ret;
}

void
XPImGuiUIImpl::setFontCode(ImFont* font)
{
    _fontCode = font;
}

void
XPImGuiUIImpl::setFontExtraLight(ImFont* font)
{
    _fontExtraLight = font;
}

void
XPImGuiUIImpl::setFontLight(ImFont* font)
{
    _fontLight = font;
}

void
XPImGuiUIImpl::setFontRegular(ImFont* font)
{
    _fontRegular = font;
}

void
XPImGuiUIImpl::setFontMedium(ImFont* font)
{
    _fontMedium = font;
}

void
XPImGuiUIImpl::setFontSemiBold(ImFont* font)
{
    _fontSemiBold = font;
}

ImFont*
XPImGuiUIImpl::getFontCode()
{
    return _fontCode;
}

ImFont*
XPImGuiUIImpl::getFontExtraLight()
{
    return _fontExtraLight;
}

ImFont*
XPImGuiUIImpl::getFontLight()
{
    return _fontLight;
}

ImFont*
XPImGuiUIImpl::getFontRegular()
{
    return _fontRegular;
}

ImFont*
XPImGuiUIImpl::getFontMedium()
{
    return _fontMedium;
}

ImFont*
XPImGuiUIImpl::getFontSemiBold()
{
    return _fontSemiBold;
}

bool
XPImGuiUIImpl::shouldRecalculateDockingView()
{
    return _shouldRecalculateDockingView;
}

void
XPImGuiUIImpl::setPhysicsPlaying(bool playing)
{
    _physicsPlaying = playing;
}

bool
XPImGuiUIImpl::isPhysicsPlaying()
{
    return _physicsPlaying;
}

std::optional<XPInput*>
XPImGuiUIImpl::getFocusedTabInput()
{
    for (auto& tab : _tabs) {
        if (tab->hasFocus()) { return { tab->getInput() }; }
    }
    return std::nullopt;
}

std::vector<XPUITab*>
XPImGuiUIImpl::getTabs()
{
    std::vector<XPUITab*> tabs(_tabs.size());
    for (size_t i = 0; i < _tabs.size(); ++i) { tabs[i] = _tabs[i].get(); }
    return tabs;
}

void
XPImGuiUIImpl::makeTabVisible(const char* tabTitle)
{
    ImGuiWindow* window = ImGui::FindWindowByName(tabTitle);
    if (window == NULL || window->DockNode == NULL || window->DockNode->TabBar == NULL) { return; }
    window->DockNode->TabBar->VisibleTabId      = window->ID;
    window->DockNode->TabBar->SelectedTabId     = window->ID;
    window->DockNode->TabBar->NextSelectedTabId = 0;
}

void
XPImGuiUIImpl::simulateCopy(const char* text)
{
    ImGui::SetClipboardText(text);
}

std::string
XPImGuiUIImpl::simulatePaste()
{
    return ImGui::GetClipboardText();
}

void
XPImGuiUIImpl::setFinalFrameTexture(void* textureId, unsigned char* pixels, uint16_t width, uint16_t height)
{
    NetImgui::SendDataTexture((ImTextureID)textureId, pixels, width, height, NetImgui::eTexFormat::kTexFmtRGBA8);
}

XPProfilable void
XPImGuiUIImpl::renderGameUI(float deltaTime)
{
    _openViewsMask |= XPUiViewMaskEditorViewport;
    uint32_t dummyOpenViewsMask = 0;
    _tabs[XPUiViewMaskGameViewportBit]->renderFullScreen(_registry->getScene(), dummyOpenViewsMask, deltaTime);
}

XPProfilable void
XPImGuiUIImpl::renderEditorUI(float deltaTime)
{
    ImGuizmo::BeginFrame();
    renderDockView(deltaTime);
    // For some reason, there's no ImGuizmo::EndFrame(); ...
}

XPProfilable void
XPImGuiUIImpl::renderDockView(float deltaTime)
{
    enum class DockingUILayouts
    {
        LayoutA,
        LayoutB,
        LayoutC,

        Count
    };
    static DockingUILayouts layout                                                  = DockingUILayouts::LayoutA;
    const char*             layoutsNames[static_cast<int>(DockingUILayouts::Count)] = { XP_STRINGIFY(LayoutA),
                                                                                        XP_STRINGIFY(LayoutB),
                                                                                        XP_STRINGIFY(LayoutC) };
    static int              layoutComboIndex = static_cast<int>(DockingUILayouts::LayoutA);

    ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    {
        if (ImGui::Begin("Main DockSpace", (bool*)0, mainWindowFlags)) {
            {
                if (ImGui::BeginChild("Toolbar",
                                      ImVec2(ImGui::GetContentRegionAvail().x, 35.0f),
                                      false,
                                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoScrollbar)) {
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    float barWidth  = ImGui::GetContentRegionAvail().x;
                    float leftPosX  = ImGui::GetCursorPosX() + 5.0f;
                    float rightPosX = barWidth - 5.0f;
                    float midPosX   = (rightPosX - leftPosX) / 2.0f;

                    // left side
                    {
                        ImGui::SetCursorPosX(leftPosX);
                        // open Dialog Simple
                        if (ImGui::Button("Open File Dialog")) {
                            // auto   resolution       = _registry->getRenderer()->getWindowSize();
                            // ImVec2 window_pos       = ImVec2(resolution.x / 2, resolution.y / 2);
                            // ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
                            // ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                            static const std::string available = [](
                                                                   const std::unordered_set<std::string>& formatsList) {
                                std::ostringstream oss;
                                std::copy(
                                  formatsList.begin(), formatsList.end(), std::ostream_iterator<std::string>(oss, ","));
                                return oss.str();
                            }(XPFile::getAvailableMeshFileFormats());
                            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey",
                                                                    "Choose File",
                                                                    available.c_str(),
                                                                    ".",
                                                                    1,
                                                                    nullptr,
                                                                    ImGuiFileDialogFlags_Default);
                        }
                        if (ImGuiFileDialog::Instance()->IsOpened("ChooseFileDlgKey")) {
                            ImGuiIO& io = ImGui::GetIO();
                            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5f),
                                                     ImGuiCond_Always);
                            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                                                    ImGuiCond_Always,
                                                    ImVec2(0.5f, 0.5f));
                        }
                        // display
                        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
                            // action if OK
                            if (ImGuiFileDialog::Instance()->IsOk()) {
                                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                                // action
                                auto optNewScene =
                                  _registry->getScene()->getSceneDescriptorStore()->getScene(filePathName);
                                if (optNewScene.has_value()) {
                                    if (optNewScene.has_value()) { _registry->setSceneBuffered(optNewScene.value()); }
                                } else {
                                    _registry->getDataPipelineStore()->createFile(filePathName,
                                                                                  XPEFileResourceType::Scene);
                                }
                            }

                            // close
                            ImGuiFileDialog::Instance()->Close();
                        }

                        // leftPosX += ImGui::CalcTextSize("Open Scene").x;
                    }

                    // mid side
                    {
                        midPosX -= 25.0f;
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(midPosX);
                        // TODO: isPlaying = physics::isplaying ?
                        if (ImGui::Button(_physicsPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY, ImVec2(25.0f, 25.0f))) {
                            _physicsPlaying = !_physicsPlaying;
                        }
                        midPosX += 25.0f;

                        ImGui::SameLine();
                        ImGui::SetCursorPosX(midPosX);
                        if (!_registry->getDataPipelineStore()->isHasFilesNeedsReload()) {
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                            ImGui::Button(ICON_FA_BOLT, ImVec2(25.0f, 25.0f));
                            ImGui::PopStyleVar();
                            ImGui::PopItemFlag();
                        } else {
                            int numMeshChanges    = 0;
                            int numShaderChanges  = 0;
                            int numTextureChanges = 0;
                            if (ImGui::Button(ICON_FA_BOLT, ImVec2(25.0f, 25.0f))) {
                                // commit changes
                                _registry->getDataPipelineStore()->clearFilesNeedReload();
                                for (const auto& pair : _registry->getDataPipelineStore()->getMeshAssets()) {
                                    if (pair.first->hasChanges()) {
                                        ++numMeshChanges;
                                        pair.first->commitChanges();
                                    }
                                }
                                for (const auto& pair : _registry->getDataPipelineStore()->getShaderAssets()) {
                                    if (pair.first->hasChanges()) {
                                        ++numShaderChanges;
                                        pair.first->commitChanges();
                                    }
                                }
                                for (const auto& pair : _registry->getDataPipelineStore()->getTextureAssets()) {
                                    if (pair.first->hasChanges()) {
                                        ++numTextureChanges;
                                        pair.first->commitChanges();
                                    }
                                }
                                // TODO: queue plugin instances instead of immediately commiting their changes
                                // PrototypePipelineQueue queue = {};
                                // for (const auto& pair : _registry->getDataPipelineStore()->getShaderBuffers()) {
                                //     auto cmd =
                                //       std::make_unique<PrototypePipelineCommand_shortcutEditorCommitReloadPlugin>();
                                //     cmd->pluginInstance = pair.second;
                                //     queue.record(std::move(cmd));
                                // }
                                // PrototypePipelines::shortcutsQueue.push_back(std::move(queue));

                                XP_LOG(XPLoggerSeverityInfo,
                                       fmt::format("Commiting file changes: {} meshes, {} shaders and {} textures.",
                                                   numMeshChanges,
                                                   numShaderChanges,
                                                   numTextureChanges)
                                         .c_str());
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(XP_COLOR_LIGHTERGRAY, 1.0f));
                                ImGui::SetTooltip("Reload Available");
                                ImGui::PopStyleColor();
                                ImGui::PopStyleVar();
                            }
                        }
                        midPosX += 25.0f;
                    }

                    // right side
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(rightPosX - 135.0f);
                        ImGui::SetNextItemWidth(135.0f);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(50.0f, 5.0f));
                        {
                            if (ImGui::Combo("##DockingLayouts",
                                             &layoutComboIndex,
                                             layoutsNames,
                                             static_cast<int>(DockingUILayouts::Count),
                                             ImGuiComboFlags_HeightSmall)) {}
                        }
                        ImGui::PopStyleVar();
                        rightPosX += 135.0f;
                    }
                }
                ImGui::EndChild();
            }

            if (ImGui::DockBuilderGetNode(ImGui::GetID("TheDockspace")) == nullptr || _shouldRecalculateDockingView ||
                layoutComboIndex != static_cast<int>(layout)) {

                layout              = static_cast<DockingUILayouts>(layoutComboIndex);
                ImGuiID dockspaceId = ImGui::GetID("TheDockspace");
                ImGui::DockBuilderRemoveNode(dockspaceId); // Clear out existing layout
                ImGui::DockBuilderAddNode(dockspaceId,
                                          ImGuiDockNodeFlags_DockSpace); // Add empty node

                ImGuiID dock_main_id = dockspaceId; // This variable will track the document node, however we are not
                                                    // using it here as we aren't docking anything into it.

                // Default Layout
                // ------------------------------------------------------
                // |        |                               |           |
                // |        |                               |           |
                // |        |                               |           |
                // |   E    |            G                  |           |
                // |        |                               |           |
                // |        |                               |     A     |
                // |        |                               |           |
                // |----------------------------------------|           |
                // |                  C                     |           |
                // |                                        |           |
                // ------------------------------------------------------

                // ------------------------------------------------------
                // |        |                               |           |
                // |        |                               |           |
                // |        |                               |           |
                // |        |            G                  |           |
                // |        |                               |           |
                // |   A    |                               |     H     |
                // |        |                               |           |
                // |        |-------------------------------------------|
                // |        |           |                               |
                // |        |    E      |             F                 |
                // ------------------------------------------------------

                // ------------------------------------------------------
                // |                                  |                 |
                // |                                  |                 |
                // |                                  |                 |
                // |                                  |                 |
                // |               A                  |                 |
                // |                                  |       C         |
                // |                                  |                 |
                // |                                  |-----------------|
                // |                                  |                 |
                // |                                  |       D         |
                // ------------------------------------------------------

                {
                    switch (layout) {
                        case DockingUILayouts::LayoutA: {
                            ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f, dock_id_g;
                            ImGui::DockBuilderSetNodeSize(dock_main_id, viewport->Size);
                            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, &dock_id_a, &dock_id_b);
                            ImGui::DockBuilderSetNodeSize(
                              dock_id_a, ImGui::DockBuilderGetNode(dock_main_id)->Size * ImVec2(0.2f, 0.2f));
                            ImGui::DockBuilderSetNodeSize(
                              dock_id_b, ImGui::DockBuilderGetNode(dock_main_id)->Size * ImVec2(0.7f, 0.7f));

                            ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.3f, &dock_id_c, &dock_id_d);
                            ImGui::DockBuilderSetNodeSize(dock_id_c, ImGui::DockBuilderGetNode(dock_id_b)->Size * 0.3f);
                            ImGui::DockBuilderSetNodeSize(dock_id_d, ImGui::DockBuilderGetNode(dock_id_b)->Size * 0.7f);

                            ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Left, 0.3f, &dock_id_e, &dock_id_f);
                            ImGui::DockBuilderSetNodeSize(dock_id_e, ImGui::DockBuilderGetNode(dock_id_d)->Size * 0.3f);
                            ImGui::DockBuilderSetNodeSize(dock_id_f, ImGui::DockBuilderGetNode(dock_id_d)->Size * 0.7f);

                            ImGui::DockBuilderSplitNode(dock_id_f, ImGuiDir_Right, 1.0f, nullptr, &dock_id_g);
                            ImGui::DockBuilderSetNodeSize(dock_id_g, ImGui::DockBuilderGetNode(dock_id_f)->Size);

                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_main_id);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_a);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_b);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_c);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_d);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_e);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_f);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_g);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            ImGui::DockBuilderDockWindow(XP_TAB_PAINT_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXTURE_EDITOR_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXT_EDITOR_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_MATERIAL_EDITOR_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_EMULATOR_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_EDITOR_VIEWPORT_TITLE, dock_id_g);
                            ImGui::DockBuilderDockWindow(XP_TAB_HIERARCHY_TITLE, dock_id_e);
                            ImGui::DockBuilderDockWindow(XP_TAB_TRAITS_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROPERTIES_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_SETTINGS_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROFILER_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_ASSETS_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_CONSOLE_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_LOGS_TITLE, dock_id_c);
                        } break;

                        case DockingUILayouts::LayoutB: {
                            ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f, dock_id_g,
                              dock_id_h, dock_id_i;
                            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, &dock_id_a, &dock_id_b);
                            ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.25f, &dock_id_c, &dock_id_d);
                            ImGui::DockBuilderSplitNode(dock_id_c, ImGuiDir_Left, 0.25f, &dock_id_e, &dock_id_f);
                            ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Right, 0.25f, &dock_id_h, &dock_id_g);
                            ImGui::DockBuilderSplitNode(dock_id_g, ImGuiDir_Right, 0.25f, nullptr, &dock_id_i);
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_main_id);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_a);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_b);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_c);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_d);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_e);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_f);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_g);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_h);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_i);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            ImGui::DockBuilderDockWindow(XP_TAB_PAINT_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXTURE_EDITOR_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXT_EDITOR_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_MATERIAL_EDITOR_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_EMULATOR_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_EDITOR_VIEWPORT_TITLE, dock_id_i);
                            ImGui::DockBuilderDockWindow(XP_TAB_HIERARCHY_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_TRAITS_TITLE, dock_id_h);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROPERTIES_TITLE, dock_id_h);
                            ImGui::DockBuilderDockWindow(XP_TAB_SETTINGS_TITLE, dock_id_h);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROFILER_TITLE, dock_id_e);
                            ImGui::DockBuilderDockWindow(XP_TAB_ASSETS_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_CONSOLE_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_LOGS_TITLE, dock_id_f);
                        } break;

                        case DockingUILayouts::LayoutC: {
                            ImGuiID dock_id_a, dock_id_b, dock_id_c, dock_id_d, dock_id_e, dock_id_f;
                            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.35f, &dock_id_a, &dock_id_b);
                            ImGui::DockBuilderSplitNode(dock_id_b, ImGuiDir_Down, 0.25f, &dock_id_c, &dock_id_d);
                            ImGui::DockBuilderSplitNode(dock_id_d, ImGuiDir_Up, 0.2f, &dock_id_e, &dock_id_f);
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_main_id);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_a);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_b);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_c);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_d);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            {
                                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_e);
                                node->LocalFlags |=
                                  ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
                            }
                            ImGui::DockBuilderDockWindow(XP_TAB_PAINT_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXTURE_EDITOR_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_EMULATOR_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_EDITOR_VIEWPORT_TITLE, dock_id_a);
                            ImGui::DockBuilderDockWindow(XP_TAB_TEXT_EDITOR_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_MATERIAL_EDITOR_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_HIERARCHY_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_TRAITS_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROPERTIES_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_SETTINGS_TITLE, dock_id_f);
                            ImGui::DockBuilderDockWindow(XP_TAB_PROFILER_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_ASSETS_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_CONSOLE_TITLE, dock_id_c);
                            ImGui::DockBuilderDockWindow(XP_TAB_LOGS_TITLE, dock_id_c);
                        } break;

                        case DockingUILayouts::Count: break;
                    }
                }

                ImGui::DockBuilderFinish(dockspaceId);
                _shouldRecalculateDockingView = false;
            }

            ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(XP_COLOR_DARKERGRAY, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGuiID dockspaceId = ImGui::GetID("TheDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), 0);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::End();
        }
    }
    ImGui::PopStyleVar(4);

    renderTabs(deltaTime);

    makeTabVisible(XP_TAB_EDITOR_VIEWPORT_TITLE);
}

XPProfilable void
XPImGuiUIImpl::renderTabs(float deltaTime)
{
    for (size_t i = 1; i < _tabs.size(); ++i) { _tabs[i]->render(_registry->getScene(), _openViewsMask, deltaTime); }
}
