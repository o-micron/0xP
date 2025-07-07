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

#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <Renderer/Interface/XPIRenderer.h>
#include <Renderer/Vulkan/XPVulkan.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class XPRegistry;
class XPIUI;
struct FreeCamera;
struct XPVulkanGPUData;
class XPVulkanWindow;

namespace val {
struct Buffer;
struct GraphicsPipelines;
class Instance;
class Swapchain;
struct DescriptorSets;
struct Synchronization;
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
class FrameBuffer;
#endif
}

struct PickBlitCommandInfo
{
    PickBlitCommandInfo(XPVec2<float>                normalizedCoordinates,
                        XPVec2<size_t>               numPixels,
                        std::function<void(XPNode*)> cbfn)
      : normalizedCoordinates(normalizedCoordinates)
      , numPixels(numPixels)
      , cbfn(cbfn)
    {
    }

    XPVec2<float>                normalizedCoordinates;
    XPVec2<size_t>               numPixels;
    std::function<void(XPNode*)> cbfn;
};

class XPVulkanRenderer final : public XPIRenderer
{
  public:
    XPVulkanRenderer(XPRegistry* const registry);
    ~XPVulkanRenderer() final;
    void                      initialize() final;
    void                      finalize() final;
    void                      update() final;
    void                      onSceneTraitsChanged() final;
    void                      invalidateDeviceObjects() final;
    void                      createDeviceObjects() final;
    void                      beginUploadMeshAssets() final;
    void                      endUploadMeshAssets() final;
    void                      beginUploadShaderAssets() final;
    void                      endUploadShaderAssets() final;
    void                      beginUploadTextureAssets() final;
    void                      endUploadTextureAssets() final;
    void                      beginReUploadMeshAssets() final;
    void                      endReUploadMeshAssets() final;
    void                      beginReUploadShaderAssets() final;
    void                      endReUploadShaderAssets() final;
    void                      beginReUploadTextureAssets() final;
    void                      endReUploadTextureAssets() final;
    void                      uploadMeshAsset(XPMeshAsset* meshAsset) final;
    void                      uploadShaderAsset(XPShaderAsset* shaderAsset) final;
    void                      uploadTextureAsset(XPTextureAsset* textureAsset) final;
    void                      reUploadMeshAsset(XPMeshAsset* meshAsset) final;
    void                      reUploadShaderAsset(XPShaderAsset* shaderAsset) final;
    void                      reUploadTextureAsset(XPTextureAsset* textureAsset) final;
    [[nodiscard]] XPRegistry* getRegistry() final;
    void getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>&) final;
    [[nodiscard]] void*         getMainTexture() final;
    [[nodiscard]] XPVec2<int>   getWindowSize() final;
    [[nodiscard]] XPVec2<int>   getResolution() final;
    [[nodiscard]] XPVec2<float> getMouseLocation() final;
    [[nodiscard]] XPVec2<float> getNormalizedMouseLocation() final;
    [[nodiscard]] bool          isLeftMouseButtonPressed() final;
    [[nodiscard]] bool          isMiddleMouseButtonPressed() final;
    [[nodiscard]] bool          isRightMouseButtonPressed() final;
    [[nodiscard]] float         getDeltaTime() final;
    [[nodiscard]] uint32_t      getNumDrawCallsVertices() final;
    [[nodiscard]] uint32_t      getTotalNumDrawCallsVertices() final;
    [[nodiscard]] uint32_t      getNumDrawCalls() final;
    [[nodiscard]] uint32_t      getTotalNumDrawCalls() final;
    [[nodiscard]] bool          isCapturingDebugFrames() final;
    [[nodiscard]] bool          isFramebufferResized() final;
    [[nodiscard]] float         getRenderingGPUTime() final;
    [[nodiscard]] float         getUIGPUTime() final;
    [[nodiscard]] float         getComputeGPUTime() final;
    void                        captureDebugFrames() final;
    void                        setFramebufferResized() final;
    void                        simulateCopy(const char* text) final;
    [[nodiscard]] std::string   simulatePaste() final;

    XPVulkanWindow&         getWindow() const;
    val::Instance&          getInstance() const;
    val::Swapchain&         getSwapchain() const;
    val::GraphicsPipelines& getGraphicsPipelines() const;
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
    const val::FrameBuffer& getGBufferFB() const;
    const val::FrameBuffer& getCombineFB() const;
#endif
    XPVulkanGPUData&     getGPUData() const;
    val::DescriptorSets& getDescriptorSets() const;
    uint32_t             getCurrentFrame() const;
    uint32_t             getCurrentImageIndex() const;

  private:
    bool createDescriptorSetLayout();
    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();
    bool createFramebuffers();

    void destroyDescriptorSetLayout();
    void destroyUniformBuffers();
    void destroyDescriptorPool();
    void destroyDescriptorSets();
    void destroyFramebuffers();

    void compileLoadScene(XPScene& scene);
    void updateUniformBuffer(XPScene& scene, FreeCamera& camera, uint32_t randomImageIndex);
    void recordCommandBuffers(XPScene&        scene,
                              FreeCamera&     camera,
                              uint32_t        randomImageIndex,
                              VkCommandBuffer commandBuffer);
    void renderToTexture(VkCommandBuffer commandBuffer, XPScene& scene, float deltaTime);
    // void renderToSwapchain(VkCommandBuffer commandBuffer, XPScene& scene, float deltaTime);

    XPRegistry* const       _registry          = nullptr;
    XPVulkanWindow*         _window            = nullptr;
    val::Instance*          _instance          = nullptr;
    val::Swapchain*         _swapchain         = nullptr;
    val::GraphicsPipelines* _graphicsPipelines = nullptr;
    val::DescriptorSets*    _descriptorSets    = nullptr;
    val::Synchronization*   _synchronization   = nullptr;
    XPVulkanGPUData*        _gpuData           = nullptr;
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
    std::vector<val::FrameBuffer> _gbufferFB;
    std::vector<val::FrameBuffer> _combineFB;
#endif
    std::vector<std::unique_ptr<val::Buffer>>                            _uniformBuffers;
    XPVec2<int>                                                          _resolution;
    uint32_t                                                             _currentFrame;
    uint32_t                                                             _currentImageIndex;
    bool                                                                 _isCapturingDebugFrames;
    bool                                                                 _isFramebufferResized;
    float                                                                _renderingGpuTime;
    float                                                                _uiGpuTime;
    float                                                                _computeGpuTime;
    std::unordered_map<std::string, std::unique_ptr<XPVulkanMeshObject>> _meshObjectMap;
    std::unordered_map<std::string, std::unique_ptr<XPVulkanMesh>>       _meshMap;
};
