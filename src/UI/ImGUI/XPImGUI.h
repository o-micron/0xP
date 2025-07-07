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

#include <Utilities/XPPlatforms.h>

#include <UI/ImGUI/Tabs/Tabs.h>
#include <UI/Interface/XPIUI.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <list>
#include <memory>
#include <vector>

class XPUITab;
class XPImGuiUIImpl final : public XPIUI
{
  public:
    XPImGuiUIImpl(XPRegistry* const registry);
    ~XPImGuiUIImpl() final;
    void                    initialize() final;
    void                    finalize() final;
    void                    update(float deltaTime) final;
    void                    onEvent(SDL_Event* event) final;
    XPRegistry*             getRegistry() final;
    XPEHeaderFlags          renderHeader(XPNode* owner, const char* name) final;
    void                    renderFooter(XPNode* owner, const char* name, XPEHeaderFlags flags) final;
    XPEHeaderFlags          renderHeadless(XPNode* owner, const char* name) final;
    void                    renderFootless(XPNode* owner, const char* name, XPEHeaderFlags flags) final;
    XPEHeaderFlags          beginRenderStructSecondary(XPNode* owner, const char* name) final;
    void                    endRenderStructSecondary(XPNode* owner, const char* name, XPEHeaderFlags flags) final;
    uint8_t                 renderField(XPNode* owner, const char* name, bool& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, int8_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, int16_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, int32_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, int64_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, uint8_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, uint16_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, uint32_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, uint64_t& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, float& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, double& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, char* value, size_t valueSize) final;
    uint8_t                 renderField(XPNode* owner, const char* name, std::string& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPVec2<int>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPVec2<float>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPVec3<float>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPVec4<float>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPMat3<float>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPMat4<float>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, std::list<XPLogicSource>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, std::vector<XPMeshRendererInfo>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, std::vector<XPColliderInfo>& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPColliderRefString& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPMeshRefString& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, XPMaterialRefString& value) final;
    uint8_t                 renderField(XPNode* owner, const char* name, CameraProperties& value) final;
    void                    setFontCode(ImFont* font) final;
    void                    setFontExtraLight(ImFont* font) final;
    void                    setFontLight(ImFont* font) final;
    void                    setFontRegular(ImFont* font) final;
    void                    setFontMedium(ImFont* font) final;
    void                    setFontSemiBold(ImFont* font) final;
    ImFont*                 getFontCode() final;
    ImFont*                 getFontExtraLight() final;
    ImFont*                 getFontLight() final;
    ImFont*                 getFontRegular() final;
    ImFont*                 getFontMedium() final;
    ImFont*                 getFontSemiBold() final;
    bool                    shouldRecalculateDockingView() final;
    void                    setPhysicsPlaying(bool playing) final;
    bool                    isPhysicsPlaying() final;
    std::optional<XPInput*> getFocusedTabInput() final;
    std::vector<XPUITab*>   getTabs() final;
    void                    makeTabVisible(const char* tabTitle) final;
    void                    simulateCopy(const char* text) final;
    std::string             simulatePaste() final;
    void setFinalFrameTexture(void* textureId, unsigned char* pixels, uint16_t width, uint16_t height) final;

  private:
    void renderGameUI(float deltaTime);
    void renderEditorUI(float deltaTime);
    void renderDockView(float deltaTime);
    void renderTabs(float deltaTime);

    std::array<std::unique_ptr<XPUITab>, XP_UI_VIEW_MASK_TABS_COUNT> _tabs;
    XPRegistry* const                                                _registry                     = nullptr;
    ImFont*                                                          _fontCode                     = nullptr;
    ImFont*                                                          _fontExtraLight               = nullptr;
    ImFont*                                                          _fontLight                    = nullptr;
    ImFont*                                                          _fontRegular                  = nullptr;
    ImFont*                                                          _fontMedium                   = nullptr;
    ImFont*                                                          _fontSemiBold                 = nullptr;
    uint32_t                                                         _openViewsMask                = 0;
    bool                                                             _shouldRecalculateDockingView = false;
    bool                                                             _physicsPlaying               = false;
    bool                                                             _showEditor                   = true;
};
