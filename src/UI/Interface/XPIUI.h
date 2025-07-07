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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <Controllers/XPInput.h>
#include <SceneDescriptor/XPEnums.h>
#include <SceneDescriptor/XPTypes.h>
#include <Utilities/XPMaths.h>

#include <list>
#include <optional>

class XPIRenderer;
class XPRegistry;
struct XPLogicSource;
struct XPMeshRendererInfo;
struct XPColliderInfo;
class XPNode;
struct ImFont;
class XPIUI;
struct CameraProperties;
class XPUITab;

// class XP_PURE_ABSTRACT XPUIImpl
// {
//   public:
//     XPUIImpl(XPIRenderer* const renderer, XPIUI* const ui)
//     {
//         XP_UNUSED(renderer)
//         XP_UNUSED(ui)
//     }
//     virtual ~XPUIImpl() {}
//     virtual void        initialize()                   = 0;
//     virtual void        finalize()                     = 0;
//     virtual void        beginFrame()                   = 0;
//     virtual void        endFrame()                     = 0;
//     virtual XPIUI*      getUI() const                  = 0;
//     virtual void        invalidateDeviceObjects()      = 0;
//     virtual void        createDeviceObjects()          = 0;
//     virtual void        onEvent(SDL_Event* event)      = 0;
//     virtual void        simulateCopy(const char* text) = 0;
//     virtual std::string simulatePaste()                = 0;
// };

class XP_PURE_ABSTRACT XPIUI
{
  public:
    XPIUI(XPRegistry* const registry) { XP_UNUSED(registry) }
    virtual ~XPIUI() {}
    virtual void           initialize()                                                                         = 0;
    virtual void           finalize()                                                                           = 0;
    virtual void           update(float deltaTime)                                                              = 0;
    virtual XPRegistry*    getRegistry()                                                                        = 0;
    virtual void           onEvent(SDL_Event* event)                                                            = 0;
    virtual XPEHeaderFlags renderHeader(XPNode* owner, const char* name)                                        = 0;
    virtual void           renderFooter(XPNode* owner, const char* name, XPEHeaderFlags flags)                  = 0;
    virtual XPEHeaderFlags renderHeadless(XPNode* owner, const char* name)                                      = 0;
    virtual void           renderFootless(XPNode* owner, const char* name, XPEHeaderFlags flags)                = 0;
    virtual XPEHeaderFlags beginRenderStructSecondary(XPNode* owner, const char* name)                          = 0;
    virtual void           endRenderStructSecondary(XPNode* owner, const char* name, XPEHeaderFlags flags)      = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, bool& value)                            = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, int8_t& value)                          = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, int16_t& value)                         = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, int32_t& value)                         = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, int64_t& value)                         = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, uint8_t& value)                         = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, uint16_t& value)                        = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, uint32_t& value)                        = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, uint64_t& value)                        = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, float& value)                           = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, double& value)                          = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, char* value, size_t valueSize)          = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, std::string& value)                     = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPVec2<int>& value)                     = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPVec2<float>& value)                   = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPVec3<float>& value)                   = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPVec4<float>& value)                   = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPMat3<float>& value)                   = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPMat4<float>& value)                   = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, std::list<XPLogicSource>& value)        = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, std::vector<XPMeshRendererInfo>& value) = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, std::vector<XPColliderInfo>& value)     = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPColliderRefString& value)             = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPMeshRefString& value)                 = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, XPMaterialRefString& value)             = 0;
    virtual uint8_t        renderField(XPNode* owner, const char* name, CameraProperties& value)                = 0;
    virtual void           setFontCode(ImFont* font)                                                            = 0;
    virtual void           setFontExtraLight(ImFont* font)                                                      = 0;
    virtual void           setFontLight(ImFont* font)                                                           = 0;
    virtual void           setFontRegular(ImFont* font)                                                         = 0;
    virtual void           setFontMedium(ImFont* font)                                                          = 0;
    virtual void           setFontSemiBold(ImFont* font)                                                        = 0;
    virtual ImFont*        getFontCode()                                                                        = 0;
    virtual ImFont*        getFontExtraLight()                                                                  = 0;
    virtual ImFont*        getFontLight()                                                                       = 0;
    virtual ImFont*        getFontRegular()                                                                     = 0;
    virtual ImFont*        getFontMedium()                                                                      = 0;
    virtual ImFont*        getFontSemiBold()                                                                    = 0;
    virtual bool           shouldRecalculateDockingView()                                                       = 0;
    virtual void           setPhysicsPlaying(bool playing)                                                      = 0;
    virtual bool           isPhysicsPlaying()                                                                   = 0;
    virtual std::optional<XPInput*> getFocusedTabInput()                                                        = 0;
    virtual std::vector<XPUITab*>   getTabs()                                                                   = 0;
    virtual void                    makeTabVisible(const char* tabTitle)                                        = 0;
    virtual void                    simulateCopy(const char* text)                                              = 0;
    virtual std::string             simulatePaste()                                                             = 0;
    virtual void setFinalFrameTexture(void* textureId, unsigned char* pixels, uint16_t width, uint16_t height)  = 0;
};
