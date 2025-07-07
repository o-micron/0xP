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

#include <Utilities/XPLogger.h>
#include <Utilities/XPMaths.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#if defined(__EMSCRIPTEN__)
    #include <SDL_events.h>
#else
    #include <SDL2/SDL_events.h>
#endif
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <cstddef>

class XPRegistry;
class XPUIImpl;
class XPMeshAsset;
class XPShaderAsset;
class XPTextureAsset;
class XPNode;

class XP_PURE_ABSTRACT XPIRenderer
{
  public:
    explicit XPIRenderer(XPRegistry* registry) { XP_UNUSED(registry) }
    virtual ~XPIRenderer()                                                                                   = default;
    virtual void                      initialize()                                                           = 0;
    virtual void                      finalize()                                                             = 0;
    virtual void                      update()                                                               = 0;
    virtual void                      onSceneTraitsChanged()                                                 = 0;
    virtual void                      invalidateDeviceObjects()                                              = 0;
    virtual void                      createDeviceObjects()                                                  = 0;
    virtual void                      beginUploadMeshAssets()                                                = 0;
    virtual void                      endUploadMeshAssets()                                                  = 0;
    virtual void                      beginUploadShaderAssets()                                              = 0;
    virtual void                      endUploadShaderAssets()                                                = 0;
    virtual void                      beginUploadTextureAssets()                                             = 0;
    virtual void                      endUploadTextureAssets()                                               = 0;
    virtual void                      beginReUploadMeshAssets()                                              = 0;
    virtual void                      endReUploadMeshAssets()                                                = 0;
    virtual void                      beginReUploadShaderAssets()                                            = 0;
    virtual void                      endReUploadShaderAssets()                                              = 0;
    virtual void                      beginReUploadTextureAssets()                                           = 0;
    virtual void                      endReUploadTextureAssets()                                             = 0;
    virtual void                      uploadMeshAsset(XPMeshAsset* meshAsset)                                = 0;
    virtual void                      uploadShaderAsset(XPShaderAsset* shaderAsset)                          = 0;
    virtual void                      uploadTextureAsset(XPTextureAsset* textureAsset)                       = 0;
    virtual void                      reUploadMeshAsset(XPMeshAsset* meshAsset)                              = 0;
    virtual void                      reUploadShaderAsset(XPShaderAsset* shaderAsset)                        = 0;
    virtual void                      reUploadTextureAsset(XPTextureAsset* textureAsset)                     = 0;
    [[nodiscard]] virtual XPRegistry* getRegistry()                                                          = 0;
    virtual void getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>&) = 0;
    [[nodiscard]] virtual void*         getMainTexture()                                                     = 0;
    [[nodiscard]] virtual XPVec2<int>   getWindowSize()                                                      = 0;
    [[nodiscard]] virtual XPVec2<int>   getResolution()                                                      = 0;
    [[nodiscard]] virtual XPVec2<float> getMouseLocation()                                                   = 0;
    [[nodiscard]] virtual XPVec2<float> getNormalizedMouseLocation()                                         = 0;
    [[nodiscard]] virtual bool          isLeftMouseButtonPressed()                                           = 0;
    [[nodiscard]] virtual bool          isMiddleMouseButtonPressed()                                         = 0;
    [[nodiscard]] virtual bool          isRightMouseButtonPressed()                                          = 0;
    [[nodiscard]] virtual float         getDeltaTime()                                                       = 0;
    [[nodiscard]] virtual uint32_t      getNumDrawCallsVertices()                                            = 0;
    [[nodiscard]] virtual uint32_t      getTotalNumDrawCallsVertices()                                       = 0;
    [[nodiscard]] virtual uint32_t      getNumDrawCalls()                                                    = 0;
    [[nodiscard]] virtual uint32_t      getTotalNumDrawCalls()                                               = 0;
    [[nodiscard]] virtual bool          isCapturingDebugFrames()                                             = 0;
    [[nodiscard]] virtual bool          isFramebufferResized()                                               = 0;
    [[nodiscard]] virtual float         getRenderingGPUTime()                                                = 0;
    [[nodiscard]] virtual float         getUIGPUTime()                                                       = 0;
    [[nodiscard]] virtual float         getComputeGPUTime()                                                  = 0;
    virtual void                        captureDebugFrames()                                                 = 0;
    virtual void                        setFramebufferResized()                                              = 0;
    virtual void                        simulateCopy(const char* text)                                       = 0;
    [[nodiscard]] virtual std::string   simulatePaste()                                                      = 0;
};
