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

#include <Renderer/Interface/XPIRenderer.h>
#include <Renderer/WebGPU/XPWGPU.h>
#include <Renderer/WebGPU/XPWGPUGPUData.h>
#include <Renderer/WebGPU/XPWGPUWindow.h>
#include <Renderer/WebGPU/wgpu_cpp.h>

class XPRegistry;
class XPIUI;
struct FreeCamera;

class XPWGPURenderer final : public XPIRenderer
{
  public:
    XPWGPURenderer(XPRegistry* const registry, XPIUI* const ui);
    ~XPWGPURenderer() final;
    void        initialize() final;
    void        finalize() final;
    void        update() final;
    void        onSceneTraitsChanged() final;
    void        invalidateDeviceObjects() final;
    void        createDeviceObjects() final;
    void        onEvent(SDL_Event* event) final;
    void        beginUploadMeshAssets() final;
    void        endUploadMeshAssets() final;
    void        beginUploadShaderAssets() final;
    void        endUploadShaderAssets() final;
    void        beginUploadTextureAssets() final;
    void        endUploadTextureAssets() final;
    void        beginReUploadMeshAssets() final;
    void        endReUploadMeshAssets() final;
    void        beginReUploadShaderAssets() final;
    void        endReUploadShaderAssets() final;
    void        beginReUploadTextureAssets() final;
    void        endReUploadTextureAssets() final;
    void        uploadMeshAsset(XPMeshAsset* meshAsset) final;
    void        uploadShaderAsset(XPShaderAsset* shaderAsset) final;
    void        uploadTextureAsset(XPTextureAsset* textureAsset) final;
    void        reUploadMeshAsset(XPMeshAsset* meshAsset) final;
    void        reUploadShaderAsset(XPShaderAsset* shaderAsset) final;
    void        reUploadTextureAsset(XPTextureAsset* textureAsset) final;
    XPRegistry* getRegistry() final;
    XPUIImpl*   getUIImpl() final;
    void*       getMainTexture() final;
    void        getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>&) final;
    float       getScale() final;
    XPVec2<int> getWindowSize() final;
    XPVec2<int> getResolution() final;
    uint32_t    getNumDrawCallsVertices() final;
    uint32_t    getTotalNumDrawCallsVertices() final;
    uint32_t    getNumDrawCalls() final;
    uint32_t    getTotalNumDrawCalls() final;
    bool        isCapturingDebugFrames() final;
    float       getRenderingGPUTime() final;
    float       getUIGPUTime() final;
    float       getComputeGPUTime() final;
    void        setScale(float scale) final;
    void        setWindowSize(XPVec2<int> size) final;
    void        captureDebugFrames() final;
    void        simulateCopy(const char* text) final;
    std::string simulatePaste() final;

    XPWGPUWindow* getWindow();
    WGPUDevice    getDevice();
    WGPUSwapChain getSwapchain();
    WGPUSurface   getSurface();

    WGPUQueue             getIOCommandQueue();
    WGPUQueue             getRenderingCommandQueue();
    WGPUQueue             getUICommandQueue();
    WGPUCommandBuffer     getRenderingCommandBuffer();
    WGPUCommandBuffer     getUICommandBuffer();
    WGPUCommandEncoder    getUICommandEncoder();
    WGPURenderPassEncoder getUIRenderPassEncoder();

    void setDevice(WGPUDevice device);
    void setSwapchain(WGPUSwapChain swapchain);
    void setSurface(WGPUSurface surface);
    void setUICommandQueue(WGPUQueue commandQueue);
    void setUICommandBuffer(WGPUCommandBuffer commandBuffer);
    void setUICommandEncoder(WGPUCommandEncoder commandEncoder);
    void setUIRenderPassEncoder(WGPURenderPassEncoder renderPassEncoder);

    void compileLoadScene(XPScene& scene);
    void loadBuiltinShaders();
    void updateGPUScene(XPScene& scene, FreeCamera& camera);
    void updateMatrices(XPScene& scene, FreeCamera& camera, float deltaTime);
    void renderOCBuffer(WGPUCommandBuffer*          commandBuffer,
                        XPScene&                    scene,
                        FreeCamera&                 camera,
                        float                       deltaTime,
                        const XPVec2<unsigned int>& dimensions,
                        const XPWGPUViewport&       viewport);
    void renderGBuffer(WGPUCommandBuffer*          commandBuffer,
                       XPScene&                    scene,
                       FreeCamera&                 camera,
                       float                       deltaTime,
                       const XPVec2<unsigned int>& dimensions,
                       const XPWGPUViewport&       viewport);
    void renderMBuffer(WGPUCommandBuffer*          commandBuffer,
                       XPScene&                    scene,
                       FreeCamera&                 camera,
                       float                       deltaTime,
                       const XPVec2<unsigned int>& dimensions,
                       const XPWGPUViewport&       viewport);
    void renderUI(XPScene& scene, float deltaTime);
    bool createComputePipeline(XPWGPUComputePipeline&                   computePipeline,
                               const XPWGPUComputePipelineCreationInfo& creationInfo);
    void destroyComputePipeline(XPWGPUComputePipeline& computePipeline);
    bool createRenderPipeline(XPWGPURenderPipeline&                   renderPipeline,
                              const XPWGPURenderPipelineCreationInfo& creationInfo);
    void destroyRenderPipeline(XPWGPURenderPipeline& renderPipeline);
    void createFramebufferTexture(XPWGPUFrameBuffer& framebuffer, const XPWGPURenderPipelineCreationInfo& createInfo);
    void destroyFramebufferTexture(XPWGPUFrameBuffer& framebuffer);
    void reCreateFramebufferTexture(XPWGPUFrameBuffer& framebuffer, const std::string& name);
    void recreateFramebufferTextures();

    std::optional<XPWGPURenderPipeline*> getRenderPipelines(const std::string& name) const;
    std::optional<XPWGPUMeshObject*>     getMeshObjectMap(const std::string& name) const;
    std::optional<XPWGPUMesh*>           getMeshMap(const std::string& name) const;
    std::optional<XPWGPUShader*>         getShaderMap(const std::string& name) const;
    std::optional<XPWGPUTexture*>        getTextureMap(const std::string& name) const;
    std::optional<XPWGPUMaterial*>       getMaterialMap(const std::string& name) const;

  private:
    XPRegistry* const                                                       _registry               = nullptr;
    std::unique_ptr<XPWGPUWindow>                                           _window                 = nullptr;
    XPUIImpl*                                                               _uiImpl                 = nullptr;
    WGPUDevice                                                              _device                 = nullptr;
    WGPUSwapChain                                                           _swapchain              = nullptr;
    WGPUSurface                                                             _surface                = nullptr;
    WGPUQueue                                                               _renderingCommandQueue  = nullptr;
    WGPUQueue                                                               _uiCommandQueue         = nullptr;
    WGPUQueue                                                               _ioCommandQueue         = nullptr;
    WGPUCommandBuffer                                                       _renderingCommandBuffer = nullptr;
    WGPUCommandBuffer                                                       _uiCommandBuffer        = nullptr;
    WGPUCommandEncoder                                                      _uiCommandEncoder       = nullptr;
    WGPURenderPassEncoder                                                   _uiRenderPassEncoder    = nullptr;
    std::unique_ptr<XPWGPUGPUData>                                          _gpuData;
    size_t                                                                  _frameDataIndex;
    bool                                                                    _isCapturingDebugFrames;
    float                                                                   _renderingGpuTime;
    float                                                                   _uiGpuTime;
    float                                                                   _computeGpuTime;
    std::unordered_map<std::string, std::unique_ptr<XPWGPURenderPipeline>>  _renderPipelines;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUComputePipeline>> _computePipelines;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUMeshObject>>      _meshObjectMap;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUMesh>>            _meshMap;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUShader>>          _shaderMap;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUTexture>>         _textureMap;
    std::unordered_map<std::string, std::unique_ptr<XPWGPUMaterial>>        _materialMap;
};
