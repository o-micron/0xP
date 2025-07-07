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

#include <Renderer/DX12/XPDX12.h>
#include <Renderer/DX12/XPShaderTypes.h>
#include <Renderer/DX12/dx12al/ConstantBuffer.hpp>
#include <Renderer/Interface/XPIRenderer.h>

#include <string>
#include <unordered_map>

class XPRegistry;
class XPIUI;
struct FreeCamera;
struct XPDX12GPUData;
class XPDX12Window;
class XPScene;
namespace dx12al {
class Device;
class Swapchain;
class Queue;
class Synchronization;
class DescriptorHeap;
class GraphicsPipeline;
class CommandList;
}

class XPDX12Renderer final : public XPIRenderer
{
  public:
    XPDX12Renderer(XPRegistry* const registry);
    ~XPDX12Renderer() final;
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

  private:
    void compileLoadScene(XPScene& scene);
    void recordCommandList(XPScene&                   scene,
                           FreeCamera&                camera,
                           uint32_t                   randomImageIndex,
                           ID3D12GraphicsCommandList* commandList);

    XPRegistry* const _registry = nullptr;
    XPDX12Window*     _window   = nullptr;
    dx12al::Device*   _device   = nullptr;
#if defined(XP_BUILD_DEBUG)
    ID3D12InfoQueue* _infoQueue = nullptr;
#endif
    dx12al::Swapchain*                                                 _swapchain         = nullptr;
    dx12al::Queue*                                                     _graphicsQueue     = nullptr;
    dx12al::Synchronization*                                           _synchronization   = nullptr;
    dx12al::DescriptorHeap*                                            _rtvDescriptorHeap = nullptr;
    dx12al::DescriptorHeap*                                            _dsvDescriptorHeap = nullptr;
    dx12al::DescriptorHeap*                                            _cbvDescriptorHeap = nullptr;
    dx12al::ConstantBuffer*                                            _cameraMatricesCB  = nullptr;
    dx12al::GraphicsPipeline*                                          _graphicsPipeline  = nullptr;
    dx12al::CommandList*                                               _commandList       = nullptr;
    ID3D12Resource*                                                    _renderTargets[XP_DX12_BUFFER_COUNT];
    ID3D12Resource*                                                    _depthStencil[XP_DX12_BUFFER_COUNT];
    XPDX12GPUData*                                                     _gpuData = nullptr;
    XPVec2<int>                                                        _resolution;
    bool                                                               _isCapturingDebugFrames;
    bool                                                               _isFramebufferResized;
    float                                                              _renderingGpuTime;
    float                                                              _uiGpuTime;
    float                                                              _computeGpuTime;
    std::unordered_map<std::string, std::unique_ptr<XPDX12MeshObject>> _meshObjectMap;
    std::unordered_map<std::string, std::unique_ptr<XPDX12Mesh>>       _meshMap;
    std::array<CameraMatrices, XP_DX12_BUFFER_COUNT>                   _cameraMatricesCBData;
    std::array<MeshMatrices, XP_DX12_BUFFER_COUNT>                     _meshMatricesCBData;
};
