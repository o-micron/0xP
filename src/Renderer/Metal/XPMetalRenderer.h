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
#include <Renderer/Metal/XPMetal.h>
#include <Renderer/Metal/XPMetalCulling.h>
#include <Renderer/Metal/XPMetalEvent.h>
#include <Renderer/Metal/XPMetalGPUData.h>
#include <Renderer/Metal/XPMetalWindow.h>
#include <Renderer/Metal/metal_cpp.h>

#include <memory>
#include <vector>

class XPRegistry;
class XPIUI;
struct FreeCamera;

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

class XPMetalRenderer final : public XPIRenderer
{
  public:
    XPMetalRenderer(XPRegistry* const registry);
    ~XPMetalRenderer() final;
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

    XPMetalWindow*      getWindow();
    MTL::Device*        getDevice();
    CA::MetalLayer*     getLayer();
    CA::MetalDrawable*  getDrawable();
    MTL::CommandQueue*  getIOCommandQueue();
    MTL::CommandQueue*  getRenderingCommandQueue();
    MTL::CommandBuffer* getRenderingCommandBuffer();

    void setDevice(MTL::Device* device);
    void setLayer(CA::MetalLayer* layer);
    void setDrawable(CA::MetalDrawable* drawable);

    NS::SharedPtr<MTL::VertexDescriptor> gBufferVertexDescriptorLayout();
    NS::SharedPtr<MTL::VertexDescriptor> lineVertexDescriptorLayout();
    NS::SharedPtr<MTL::VertexDescriptor> mBufferVertexDescriptorLayout();

    void compileLoadScene(XPScene& scene);
    void loadBuiltinShaders();
    void updateViewport(XPScene& scene, FreeCamera& camera, MTL::Viewport& viewport);
    void updateMatrices(XPScene& scene, FreeCamera& camera, float deltaTime);
    void updateGPUScene(XPScene& scene, FreeCamera& camera);
    void renderGBuffer(MTL::CommandBuffer*  commandBuffer,
                       XPScene&             scene,
                       FreeCamera&          camera,
                       float                deltaTime,
                       const MTL::Viewport& viewport);
    void renderMBuffer(MTL::CommandBuffer*  commandBuffer,
                       XPScene&             scene,
                       FreeCamera&          camera,
                       float                deltaTime,
                       const MTL::Viewport& viewport);
    bool createComputePipeline(XPMetalComputePipeline&                   computePipeline,
                               const XPMetalComputePipelineCreationInfo& creationInfo);
    void destroyComputePipeline(XPMetalComputePipeline& computePipeline);
    bool createRenderPipeline(XPMetalRenderPipeline&                   renderPipeline,
                              const XPMetalRenderPipelineCreationInfo& creationInfo);
    void destroyRenderPipeline(XPMetalRenderPipeline& renderPipeline);
    void createFramebufferTexture(XPMetalFrameBuffer& framebuffer, const XPMetalRenderPipelineCreationInfo& createInfo);
    void destroyFramebufferTexture(XPMetalFrameBuffer& framebuffer);
    void reCreateFramebufferTexture(XPMetalFrameBuffer& framebuffer, const std::string& name);
    void recreateFramebufferTextures();

    std::optional<XPMetalRenderPipeline*> getRenderPipelines(const std::string& name) const;
    std::optional<XPMetalMeshObject*>     getMeshObjectMap(const std::string& name) const;
    std::optional<XPMetalMesh*>           getMeshMap(const std::string& name) const;
    std::optional<XPMetalShader*>         getShaderMap(const std::string& name) const;
    std::optional<XPMetalTexture*>        getTextureMap(const std::string& name) const;
    std::optional<XPMetalMaterial*>       getMaterialMap(const std::string& name) const;

  private:
    void scheduleReadPixelFromTexture(MTL::Texture* texture, std::shared_ptr<PickBlitCommandInfo> blitCmdInfo);

    XPRegistry* const                                                        _registry               = nullptr;
    std::unique_ptr<XPMetalWindow>                                           _window                 = nullptr;
    NS::AutoreleasePool*                                                     _autoReleasePool        = nullptr;
    MTL::Device*                                                             _device                 = nullptr;
    CA::MetalLayer*                                                          _layer                  = nullptr;
    CA::MetalDrawable*                                                       _drawable               = nullptr;
    std::unique_ptr<XPMetalFramePipeline>                                    _framePipeline          = nullptr;
    MTL::CommandQueue*                                                       _ioCommandQueue         = nullptr;
    MTL::CommandQueue*                                                       _renderingCommandQueue  = nullptr;
    MTL::CommandQueue*                                                       _computeCommandQueue    = nullptr;
    MTL::CommandBuffer*                                                      _renderingCommandBuffer = nullptr;
    MTL::CommandBuffer*                                                      _computeCommandBuffer   = nullptr;
    dispatch_semaphore_t                                                     _renderingCommandQueueSemaphore;
    std::array<XPVertexShaderArgumentBuffer, XPNumPerFrameBuffers>           _vertexShaderArgBuffer0;
    std::unique_ptr<XPMetalGPUData>                                          _gpuData;
    XPVec2<int>                                                              _resolution;
    size_t                                                                   _frameDataIndex;
    bool                                                                     _isCapturingDebugFrames;
    float                                                                    _renderingGpuTime;
    float                                                                    _uiGpuTime;
    float                                                                    _computeGpuTime;
    std::unordered_map<std::string, std::unique_ptr<XPMetalRenderPipeline>>  _renderPipelines;
    std::unordered_map<std::string, std::unique_ptr<XPMetalComputePipeline>> _computePipelines;
    std::unordered_map<std::string, std::unique_ptr<XPMetalMeshObject>>      _meshObjectMap;
    std::unordered_map<std::string, std::unique_ptr<XPMetalMesh>>            _meshMap;
    std::unordered_map<std::string, std::unique_ptr<XPMetalShader>>          _shaderMap;
    std::unordered_map<std::string, std::unique_ptr<XPMetalTexture>>         _textureMap;
    std::unordered_map<std::string, std::unique_ptr<XPMetalMaterial>>        _materialMap;
    std::unique_ptr<XPMetalLineRenderer>                                     _lineRenderer;
};
