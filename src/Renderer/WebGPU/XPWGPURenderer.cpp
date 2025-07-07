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

#include <Renderer/WebGPU/XPWGPURenderer.h>

#include <DataPipeline/XPAssimpModelLoader.h>
#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPIFileWatch.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Renderer/WebGPU/XPWGPUUIImpl.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneStore.h>
#include <SceneDescriptor/XPStore.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/ImGUI/XPImGUI.h>
    #include <UI/Interface/XPIUI.h>
#endif
#include <Utilities/XPCrossShaderCompiler.h>
#include <Utilities/XPFS.h>
#include <Utilities/XPFreeCameraSystem.h>
#include <Utilities/XPLogger.h>

#include <Engine/XPRegistry.h>

XPWGPURenderer::XPWGPURenderer(XPRegistry* const registry, XPIUI* const ui)
  : XPIRenderer(registry)
  , _registry(registry)
  , _window(std::make_unique<XPWGPUWindow>(this))
  , _uiImpl(XP_NEW XPWGPUUIImpl(this, ui))
  , _frameDataIndex(0)
  , _isCapturingDebugFrames(false)
  , _renderingGpuTime(1.0f)
  , _uiGpuTime(1.0f)
  , _computeGpuTime(1.0f)
{
}

XPWGPURenderer::~XPWGPURenderer() { delete _uiImpl; }

void
XPWGPURenderer::initialize()
{
    _window->initialize();

    _uiImpl->initialize();

    _gpuData = std::make_unique<XPWGPUGPUData>();
}

void
XPWGPURenderer::finalize()
{
    _gpuData.release();

    _uiImpl->finalize();

    _window->finalize();
}

void
XPWGPURenderer::update()
{
    float deltaTime = _window->getDeltaTime();

    _window->pollEvents();
    _window->refresh();

    auto& scene       = *_registry->getScene();
    auto& layers      = scene.getLayers();
    auto& cameraNodes = scene.getNodes(FreeCameraAttachmentDescriptor);

    if (!layers.empty() && !cameraNodes.empty()) {
        if (scene.hasAnyTraitsChanges()) {
            // // RAII, block and wait for all frames to finish !
            // {
            //     XPMetalFramesInFlightLock lockAllFrames({ _renderingCommandQueueSemaphore, _uiCommandQueueSemaphore
            //     }); compileLoadScene(scene);
            // }
        }
    }

    renderUI(scene, deltaTime);
}

void
XPWGPURenderer::onSceneTraitsChanged()
{
}

void
XPWGPURenderer::invalidateDeviceObjects()
{
    _uiImpl->invalidateDeviceObjects();
}

void
XPWGPURenderer::createDeviceObjects()
{
    _uiImpl->createDeviceObjects();
}

void
XPWGPURenderer::onEvent(SDL_Event* event)
{
}

void
XPWGPURenderer::beginUploadMeshAssets()
{
}

void
XPWGPURenderer::endUploadMeshAssets()
{
}

void
XPWGPURenderer::beginUploadShaderAssets()
{
}

void
XPWGPURenderer::endUploadShaderAssets()
{
}

void
XPWGPURenderer::beginUploadTextureAssets()
{
}

void
XPWGPURenderer::endUploadTextureAssets()
{
}

void
XPWGPURenderer::beginReUploadMeshAssets()
{
}

void
XPWGPURenderer::endReUploadMeshAssets()
{
}

void
XPWGPURenderer::beginReUploadShaderAssets()
{
}

void
XPWGPURenderer::endReUploadShaderAssets()
{
}

void
XPWGPURenderer::beginReUploadTextureAssets()
{
}

void
XPWGPURenderer::endReUploadTextureAssets()
{
}

void
XPWGPURenderer::uploadMeshAsset(XPMeshAsset* meshAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

void
XPWGPURenderer::uploadShaderAsset(XPShaderAsset* shaderAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

void
XPWGPURenderer::uploadTextureAsset(XPTextureAsset* textureAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

void
XPWGPURenderer::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

void
XPWGPURenderer::reUploadShaderAsset(XPShaderAsset* shaderAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

void
XPWGPURenderer::reUploadTextureAsset(XPTextureAsset* textureAsset)
{
    // TODO:
    // XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

XPRegistry*
XPWGPURenderer::getRegistry()
{
    return _registry;
}

XPUIImpl*
XPWGPURenderer::getUIImpl()
{
    return _uiImpl;
}

void*
XPWGPURenderer::getMainTexture()
{
    // TODO:
    XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
    return nullptr;
}

void
XPWGPURenderer::getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>&)
{
    // TODO:
    XP_UNIMPLEMENTED("NOT IMPLEMENTED YET FOR WGPU BACKEND");
}

float
XPWGPURenderer::getScale()
{
    return _window->getRendererScale();
}

XPVec2<int>
XPWGPURenderer::getWindowSize()
{
    return _window->getWindowSize();
}

XPVec2<int>
XPWGPURenderer::getResolution()
{
    return _window->getRendererSize();
}

uint32_t
XPWGPURenderer::getNumDrawCallsVertices()
{
    return _gpuData->numDrawCallsVertices;
}

uint32_t
XPWGPURenderer::getTotalNumDrawCallsVertices()
{
    return _gpuData->numTotalDrawCallsVertices;
}

uint32_t
XPWGPURenderer::getNumDrawCalls()
{
    return _gpuData->numDrawCalls;
}

uint32_t
XPWGPURenderer::getTotalNumDrawCalls()
{
    return _gpuData->numTotalDrawCalls;
}

bool
XPWGPURenderer::isCapturingDebugFrames()
{
    return _isCapturingDebugFrames;
}

float
XPWGPURenderer::getRenderingGPUTime()
{
    return _renderingGpuTime;
}

float
XPWGPURenderer::getUIGPUTime()
{
    return _uiGpuTime;
}

float
XPWGPURenderer::getComputeGPUTime()
{
    return _computeGpuTime;
}

void
XPWGPURenderer::setScale(float scale)
{
    // TODO: add proper resizable window
    // _window->setRendererScale(scale);
}

void
XPWGPURenderer::setWindowSize(XPVec2<int> size)
{
    _window->setWindowSize(size);
    _registry->getUI()->setShouldRecalculateDockingView();
    recreateFramebufferTextures();
    for (auto cameraNode : _registry->getScene()->getNodes(FreeCameraAttachmentDescriptor)) {
        FreeCamera* freeCamera = cameraNode->getFreeCamera();
        freeCamera->width      = size.x;
        freeCamera->height     = size.y;
    }
}

void
XPWGPURenderer::captureDebugFrames()
{
    _isCapturingDebugFrames = true;
}

void
XPWGPURenderer::simulateCopy(const char* text)
{
#ifdef XP_EDITOR_MODE
    _uiImpl->simulateCopy(text);
#else
    _window->simulateCopy(text);
#endif
}

std::string
XPWGPURenderer::simulatePaste()
{
#ifdef XP_EDITOR_MODE
    return _uiImpl->simulatePaste();
#else
    return _window->simulatePaste();
#endif
}

XPWGPUWindow*
XPWGPURenderer::getWindow()
{
    return _window.get();
}

WGPUDevice
XPWGPURenderer::getDevice()
{
    return _device;
}

WGPUSwapChain
XPWGPURenderer::getSwapchain()
{
    return _swapchain;
}

WGPUSurface
XPWGPURenderer::getSurface()
{
    return _surface;
}

WGPUQueue
XPWGPURenderer::getIOCommandQueue()
{
    return _ioCommandQueue;
}

WGPUQueue
XPWGPURenderer::getRenderingCommandQueue()
{
    return _renderingCommandQueue;
}

WGPUQueue
XPWGPURenderer::getUICommandQueue()
{
    return _uiCommandQueue;
}

WGPUCommandBuffer
XPWGPURenderer::getRenderingCommandBuffer()
{
    return _renderingCommandBuffer;
}

WGPUCommandBuffer
XPWGPURenderer::getUICommandBuffer()
{
    return _uiCommandBuffer;
}

WGPUCommandEncoder
XPWGPURenderer::getUICommandEncoder()
{
    return _uiCommandEncoder;
}

WGPURenderPassEncoder
XPWGPURenderer::getUIRenderPassEncoder()
{
    return _uiRenderPassEncoder;
}

void
XPWGPURenderer::setDevice(WGPUDevice device)
{
    _device = device;
}

void
XPWGPURenderer::setSwapchain(WGPUSwapChain swapchain)
{
    _swapchain = swapchain;
}

void
XPWGPURenderer::setSurface(WGPUSurface surface)
{
    _surface = surface;
}

void
XPWGPURenderer::setUICommandQueue(WGPUQueue commandQueue)
{
    _uiCommandQueue = commandQueue;
}

void
XPWGPURenderer::setUICommandBuffer(WGPUCommandBuffer commandBuffer)
{
    _uiCommandBuffer = commandBuffer;
}

void
XPWGPURenderer::setUICommandEncoder(WGPUCommandEncoder commandEncoder)
{
    _uiCommandEncoder = commandEncoder;
}

void
XPWGPURenderer::setUIRenderPassEncoder(WGPURenderPassEncoder renderPassEncoder)
{
    _uiRenderPassEncoder = renderPassEncoder;
}

void
XPWGPURenderer::compileLoadScene(XPScene& scene)
{
}

void
XPWGPURenderer::loadBuiltinShaders()
{
}

void
XPWGPURenderer::updateGPUScene(XPScene& scene, FreeCamera& camera)
{
}

void
XPWGPURenderer::updateMatrices(XPScene& scene, FreeCamera& camera, float deltaTime)
{
}

void
XPWGPURenderer::renderOCBuffer(WGPUCommandBuffer*          commandBuffer,
                               XPScene&                    scene,
                               FreeCamera&                 camera,
                               float                       deltaTime,
                               const XPVec2<unsigned int>& dimensions,
                               const XPWGPUViewport&       viewport)
{
}

void
XPWGPURenderer::renderGBuffer(WGPUCommandBuffer*          commandBuffer,
                              XPScene&                    scene,
                              FreeCamera&                 camera,
                              float                       deltaTime,
                              const XPVec2<unsigned int>& dimensions,
                              const XPWGPUViewport&       viewport)
{
}

void
XPWGPURenderer::renderMBuffer(WGPUCommandBuffer*          commandBuffer,
                              XPScene&                    scene,
                              FreeCamera&                 camera,
                              float                       deltaTime,
                              const XPVec2<unsigned int>& dimensions,
                              const XPWGPUViewport&       viewport)
{
}

void
XPWGPURenderer::renderUI(XPScene& scene, float deltaTime)
{
    XP_UNUSED(scene)

    _uiImpl->beginFrame();
    _uiImpl->getUI()->update(deltaTime);
    _uiImpl->endFrame();
}

bool
XPWGPURenderer::createComputePipeline(XPWGPUComputePipeline&                   computePipeline,
                                      const XPWGPUComputePipelineCreationInfo& creationInfo)
{
}

void
XPWGPURenderer::destroyComputePipeline(XPWGPUComputePipeline& computePipeline)
{
}

bool
XPWGPURenderer::createRenderPipeline(XPWGPURenderPipeline&                   renderPipeline,
                                     const XPWGPURenderPipelineCreationInfo& creationInfo)
{
}

void
XPWGPURenderer::destroyRenderPipeline(XPWGPURenderPipeline& renderPipeline)
{
}

void
XPWGPURenderer::createFramebufferTexture(XPWGPUFrameBuffer&                      framebuffer,
                                         const XPWGPURenderPipelineCreationInfo& createInfo)
{
}

void
XPWGPURenderer::destroyFramebufferTexture(XPWGPUFrameBuffer& framebuffer)
{
}

void
XPWGPURenderer::reCreateFramebufferTexture(XPWGPUFrameBuffer& framebuffer, const std::string& name)
{
}

void
XPWGPURenderer::recreateFramebufferTextures()
{
}

std::optional<XPWGPURenderPipeline*>
XPWGPURenderer::getRenderPipelines(const std::string& name) const
{
    auto it = _renderPipelines.find(name);
    if (it != _renderPipelines.end()) { return { it->second.get() }; }
    return std::nullopt;
}

std::optional<XPWGPUMeshObject*>
XPWGPURenderer::getMeshObjectMap(const std::string& name) const
{
    auto it = _meshObjectMap.find(name);
    if (it != _meshObjectMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

std::optional<XPWGPUMesh*>
XPWGPURenderer::getMeshMap(const std::string& name) const
{
    auto it = _meshMap.find(name);
    if (it != _meshMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

std::optional<XPWGPUShader*>
XPWGPURenderer::getShaderMap(const std::string& name) const
{
    auto it = _shaderMap.find(name);
    if (it != _shaderMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

std::optional<XPWGPUTexture*>
XPWGPURenderer::getTextureMap(const std::string& name) const
{
    auto it = _textureMap.find(name);
    if (it != _textureMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

std::optional<XPWGPUMaterial*>
XPWGPURenderer::getMaterialMap(const std::string& name) const
{
    auto it = _materialMap.find(name);
    if (it != _materialMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}
