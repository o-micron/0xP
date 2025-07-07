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
#include <Renderer/Vulkan/XPVulkan.h>
#include <Renderer/Vulkan/XPVulkanExt.h>
#include <Renderer/Vulkan/XPVulkanGPUData.h>
#include <Renderer/Vulkan/XPVulkanRenderer.h>
#include <Renderer/Vulkan/XPVulkanWindow.h>
#include <Renderer/Vulkan/val/Buffer.h>
#include <Renderer/Vulkan/val/CommandPool.hpp>
#include <Renderer/Vulkan/val/Defs.h>
#include <Renderer/Vulkan/val/DescriptorSet.h>
#include <Renderer/Vulkan/val/Device.hpp>
#include <Renderer/Vulkan/val/FrameBuffer.hpp>
#include <Renderer/Vulkan/val/FreeFunctions.hpp>
#include <Renderer/Vulkan/val/Instance.hpp>
#include <Renderer/Vulkan/val/PhysicalDevice.hpp>
#include <Renderer/Vulkan/val/PushConstantData.h>
#include <Renderer/Vulkan/val/Queue.hpp>
#include <Renderer/Vulkan/val/Swapchain.hpp>
#include <Renderer/Vulkan/val/Synchronization.hpp>
#include <Renderer/Vulkan/val/Texture.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <SceneDescriptor/XPSceneStore.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/ImGUI/XPImGUI.h>
#endif
#include <Utilities/XPCrossShaderCompiler.h>
#include <Utilities/XPFS.h>
#include <Utilities/XPFreeCameraSystem.h>
#include <Utilities/XPLogger.h>

#include <Renderer/Vulkan/val/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/val/Shader.hpp>

#include <cstring>
#include <filesystem>
#include <memory>
#include <numeric>
#include <optional>
#include <vector>

XPVulkanRenderer::XPVulkanRenderer(XPRegistry* const registry)
  : XPIRenderer(registry)
  , _registry(registry)
  , _window(XP_NEW XPVulkanWindow(this))
  , _instance(XP_NEW val::Instance(this))
  , _swapchain(XP_NEW val::Swapchain(this))
  , _graphicsPipelines(XP_NEW val::GraphicsPipelines())
  , _descriptorSets(XP_NEW val::DescriptorSets())
  , _synchronization(XP_NEW val::Synchronization())
  , _gpuData(XP_NEW XPVulkanGPUData())
  , _resolution(XP_INITIAL_WINDOW_WIDTH, XP_INITIAL_WINDOW_HEIGHT)
  , _currentFrame(0)
  , _isCapturingDebugFrames(false)
  , _isFramebufferResized(false)
  , _renderingGpuTime(1.0f)
  , _uiGpuTime(1.0f)
  , _computeGpuTime(1.0f)
{
}

XPVulkanRenderer::~XPVulkanRenderer() {}

void
XPVulkanRenderer::initialize()
{
    _window->initialize();
    _instance->createInstance();
    _instance->loadDynamicFunctions();
#if defined(XP_VULKAN_VALIDATION_LAYERS)
    _instance->createDebuggerUtilsMessenger();
#endif
    _instance->createSurface();
    _instance->getPhysicalDevice().selectPhysicalDevice();
    _instance->getPhysicalDevice().getDevice().createDevice();
    _synchronization->initialize(_instance->getPhysicalDevice().getDevice().device);

    XPDataPipelineStore* dataPipelineStore = _registry->getDataPipelineStore();
    dataPipelineStore->createFile(XPFS::buildShaderAssetsPath("GBuffer.surface.spv"), XPEFileResourceType::Shader);
    dataPipelineStore->createFile(XPFS::buildShaderAssetsPath("Line.surface.spv"), XPEFileResourceType::Shader);
    dataPipelineStore->createFile(XPFS::buildShaderAssetsPath("MBuffer.surface.spv"), XPEFileResourceType::Shader);

    createDeviceObjects();
}

void
XPVulkanRenderer::finalize()
{
    val::Device& device = _instance->getPhysicalDevice().getDevice();

    invalidateDeviceObjects();

    for (auto& vulkanMeshPair : _meshMap) {
        XPVulkanMesh& mesh = *vulkanMeshPair.second;
        device.destroyBuffer(*mesh.vertexBuffer);
        device.destroyBuffer(*mesh.normalBuffer);
        device.destroyBuffer(*mesh.uvBuffer);
        device.destroyBuffer(*mesh.indexBuffer);
    }
    _meshMap.clear();

    _meshObjectMap.clear();

    XP_DELETE _gpuData;

    // destroy all graphics pipelines
    _graphicsPipelines->pipelines.clear();
    XP_DELETE _graphicsPipelines;

    XP_DELETE _descriptorSets;

    _synchronization->finalize(device.device);
    XP_DELETE _synchronization;

    XP_DELETE _swapchain;

    device.destroyCommandPools();
    device.destroyDevice();
    _instance->destroySurface();
#if defined(XP_VULKAN_VALIDATION_LAYERS)
    _instance->destroyDebuggerUtilsMessenger();
#endif
    _instance->unloadDynamicFunctions();
    _instance->destroyInstance();
    XP_DELETE _instance;

    _window->finalize();
    XP_DELETE _window;
}

XPProfilable void
XPVulkanRenderer::update()
{
    float deltaTime = _window->getDeltaTimeSeconds();

    _window->pollEvents();
    _window->refresh();

    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    auto& scene = *_registry->getScene();

    vkWaitForFences(device.device, 1, &_synchronization->inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(device.device,
                                            _swapchain->getSwapchain(),
                                            UINT64_MAX,
                                            _synchronization->semaphores[_currentFrame].imageAvailable,
                                            VK_NULL_HANDLE,
                                            &_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // recreate swapchain
        invalidateDeviceObjects();
        createDeviceObjects();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to acquire swapchain image");
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags                    = 0;
    commandBufferBeginInfo.pInheritanceInfo         = nullptr;

    VkCommandBuffer& commandBuffer = device.commandPools->standard.buffers[_currentImageIndex];
    VULKAN_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "Failed to begin command buffer");
    {
        renderToTexture(commandBuffer, scene, deltaTime);
    }
    VULKAN_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");

    if (_synchronization->imagesInFlightFences[_currentImageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(
          device.device, 1, &_synchronization->imagesInFlightFences[_currentImageIndex], VK_TRUE, UINT64_MAX);
    }
    _synchronization->imagesInFlightFences[_currentImageIndex] = _synchronization->inFlightFences[_currentFrame];

    std::vector<VkSemaphore>          waitSemaphores   = { _synchronization->semaphores[_currentFrame].imageAvailable };
    std::vector<VkSemaphore>          signalSemaphores = { _synchronization->semaphores[_currentFrame].renderFinished };
    std::vector<VkPipelineStageFlags> waitStageMasks   = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::vector<VkSwapchainKHR>       swapchains       = { _swapchain->getSwapchain() };
    std::vector<uint32_t>             imageIndices     = { _currentImageIndex };

    VkSubmitInfo submitInfo         = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = (uint32_t)waitSemaphores.size();
    submitInfo.pWaitSemaphores      = waitSemaphores.data();
    submitInfo.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
    submitInfo.pSignalSemaphores    = signalSemaphores.data();
    submitInfo.pWaitDstStageMask    = waitStageMasks.data();
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &physicalDevice.getDevice().commandPools->standard.buffers[_currentImageIndex];

    vkResetFences(device.device, 1, &_synchronization->inFlightFences[_currentFrame]);

    VULKAN_ASSERT(
      vkQueueSubmit(
        physicalDevice.getQueues().presentation.queue, 1, &submitInfo, _synchronization->inFlightFences[_currentFrame]),
      "Failed to submit graphics family queue");

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = (uint32_t)signalSemaphores.size();
    presentInfo.pWaitSemaphores    = signalSemaphores.data();
    presentInfo.swapchainCount     = (uint32_t)swapchains.size();
    presentInfo.pSwapchains        = swapchains.data();
    presentInfo.pImageIndices      = imageIndices.data();
    presentInfo.pResults           = nullptr;

    result = vkQueuePresentKHR(physicalDevice.getQueues().presentation.queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _isFramebufferResized) {
        _isFramebufferResized = false;
        // recreate swapchain
        invalidateDeviceObjects();
        createDeviceObjects();
    } else if (result != VK_SUCCESS) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to present swapchain image");
    }

    _currentFrame = (_currentFrame + 1) % val::XPMaxBuffersInFlight;
}

void
XPVulkanRenderer::onSceneTraitsChanged()
{
}

void
XPVulkanRenderer::invalidateDeviceObjects()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    // wait for device to become idle
    vkDeviceWaitIdle(device.device);

    destroyDescriptorSets();
    destroyDescriptorPool();
    destroyDescriptorSetLayout();
    destroyUniformBuffers();
    destroyFramebuffers();

    for (size_t i = 0; i < _graphicsPipelines->pipelines.size(); ++i) { _graphicsPipelines->pipelines[i]->destroy(); }

    _swapchain->destroyDepthResources();
    device.destroyCommandPools();
    _swapchain->destroySwapchain();
}

void
XPVulkanRenderer::createDeviceObjects()
{
    val::Device& device = _instance->getPhysicalDevice().getDevice();

    _swapchain->createSwapchain();
    device.createCommandPools(_swapchain);
    _swapchain->createDepthResources();

    for (const auto& it : _registry->getDataPipelineStore()->getShaderAssets()) { uploadShaderAsset(it.second); }

    if (!createFramebuffers()) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't create frame buffers.");
        return;
    }

    if (!createUniformBuffers()) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't create uniform buffers.");
        return;
    }
    if (!createDescriptorSetLayout()) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't create a descriptor set layout.");
        return;
    }
    if (!createDescriptorPool()) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't create descriptor pool.");
        return;
    }
    if (!createDescriptorSets()) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't create descriptor sets.");
        return;
    }
}

void
XPVulkanRenderer::beginUploadMeshAssets()
{
}

void
XPVulkanRenderer::endUploadMeshAssets()
{
}

void
XPVulkanRenderer::beginUploadShaderAssets()
{
}

void
XPVulkanRenderer::endUploadShaderAssets()
{
}

void
XPVulkanRenderer::beginUploadTextureAssets()
{
}

void
XPVulkanRenderer::endUploadTextureAssets()
{
}

void
XPVulkanRenderer::beginReUploadMeshAssets()
{
}

void
XPVulkanRenderer::endReUploadMeshAssets()
{
}

void
XPVulkanRenderer::beginReUploadShaderAssets()
{
}

void
XPVulkanRenderer::endReUploadShaderAssets()
{
    _registry->getEngine()->scheduleRenderTask([this]() {
        val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
        val::Device&         device         = physicalDevice.getDevice();

        // wait for device to become idle
        vkDeviceWaitIdle(device.device);

        invalidateDeviceObjects();
        createDeviceObjects();
    });
}

void
XPVulkanRenderer::beginReUploadTextureAssets()
{
}

void
XPVulkanRenderer::endReUploadTextureAssets()
{
}

XPProfilable void
XPVulkanRenderer::uploadMeshAsset(XPMeshAsset* meshAsset)
{
    val::Device& device = _instance->getPhysicalDevice().getDevice();

    XPMeshBuffer*                 meshBuffer = meshAsset->getMeshBuffer();
    std::unique_ptr<XPVulkanMesh> vulkanMesh = std::make_unique<XPVulkanMesh>();

    size_t positionsCount = meshBuffer->getPositionsCount();
    size_t normalsCount   = meshBuffer->getNormalsCount();
    size_t texcoordsCount = meshBuffer->getTexcoordsCount();
    size_t indicesCount   = meshBuffer->getIndicesCount();

    const XPVec4<float>* positions = meshBuffer->getPositions();
    const XPVec4<float>* normals   = meshBuffer->getNormals();
    const XPVec4<float>* texcoords = meshBuffer->getTexcoords();
    const uint32_t*      indices   = meshBuffer->getIndices();

    uint64_t vertexBufferSize   = static_cast<uint64_t>(positionsCount * XPMeshBuffer::sizeofPositionsType());
    uint64_t normalBufferSize   = static_cast<uint64_t>(normalsCount * XPMeshBuffer::sizeofNormalsType());
    uint64_t texcoordBufferSize = static_cast<uint64_t>(texcoordsCount * XPMeshBuffer::sizeofTexcoordsType());
    uint64_t indexBufferSize    = static_cast<uint64_t>(indicesCount * XPMeshBuffer::sizeofIndicesType());
    vulkanMesh->vertexBuffer    = device.createVertexBuffer(vertexBufferSize, val::EResourceStorageModePrivate);
    vulkanMesh->normalBuffer    = device.createVertexBuffer(normalBufferSize, val::EResourceStorageModePrivate);
    vulkanMesh->uvBuffer        = device.createVertexBuffer(texcoordBufferSize, val::EResourceStorageModePrivate);
    vulkanMesh->indexBuffer     = device.createIndexBuffer(indexBufferSize, val::EResourceStorageModePrivate);

    device.writeBuffer(*vulkanMesh->vertexBuffer, (void*)positions, 0, vertexBufferSize);
    device.writeBuffer(*vulkanMesh->normalBuffer, (void*)normals, 0, normalBufferSize);
    device.writeBuffer(*vulkanMesh->uvBuffer, (void*)texcoords, 0, texcoordBufferSize);
    device.writeBuffer(*vulkanMesh->indexBuffer, (void*)indices, 0, indexBufferSize);

    auto vulkanMeshRef = vulkanMesh.get();
    meshAsset->setGPURef((void*)vulkanMeshRef);
    vulkanMesh->meshAsset = meshAsset;

    _meshMap.insert({ meshAsset->getFile()->getPath(), std::move(vulkanMesh) });
    for (size_t mboi = 0; mboi < meshBuffer->getObjectsCount(); ++mboi) {
        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(mboi);
        auto                vulkanMeshObject =
          std::make_unique<XPVulkanMeshObject>((XPVulkanMesh&)*vulkanMeshRef, meshBufferObject.boundingBox);
        vulkanMeshObject->name         = meshBufferObject.name;
        vulkanMeshObject->vertexOffset = meshBufferObject.vertexOffset;
        vulkanMeshObject->indexOffset  = meshBufferObject.indexOffset;
        vulkanMeshObject->numIndices   = meshBufferObject.numIndices;
        vulkanMeshRef->objects.push_back(*vulkanMeshObject.get());
        _meshObjectMap.insert({ vulkanMeshObject->name, std::move(vulkanMeshObject) });
    }
}

XPProfilable void
XPVulkanRenderer::uploadShaderAsset(XPShaderAsset* shaderAsset)
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    _graphicsPipelines->pipelines.resize(val::EGraphicsPipelines_Count);

    auto graphicsPipelineCreateInfo = std::make_unique<val::GraphicsPipelineCreateInfo>(
      device.device, physicalDevice, _swapchain->getExtent(), shaderAsset->getShaderBuffer());
    auto graphicsPipeline = std::make_unique<val::GraphicsPipeline>(std::move(graphicsPipelineCreateInfo));

    auto it = val::EGraphicsPipelinesMap.find(graphicsPipeline->getName());
    assert(it != val::EGraphicsPipelinesMap.end());
    _graphicsPipelines->pipelines[it->second] = std::move(graphicsPipeline);
}

XPProfilable void
XPVulkanRenderer::uploadTextureAsset(XPTextureAsset* textureAsset)
{
    //    XPTextureBuffer*                 textureBuffer = textureAsset->getTextureBuffer();
    //    std::unique_ptr<XPVulkanTexture> vulkanTexture = std::make_unique<XPVulkanTexture>();
    //    int                              width         = textureBuffer->getDimensions().x;
    //    int                              height        = textureBuffer->getDimensions().y;
    //    int                              channels      = 0;
    //
    //    vulkanTexture->textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
    //    vulkanTexture->textureDescriptor->setWidth(width);
    //    vulkanTexture->textureDescriptor->setHeight(height);
    //    switch (textureBuffer->getFormat()) {
    //        case XPETextureBufferFormat::R8: {
    //            vulkanTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatR8Unorm);
    //            channels = 1;
    //        } break;
    //        case XPETextureBufferFormat::R8_G8: {
    //            vulkanTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRG8Unorm);
    //            channels = 2;
    //        } break;
    //        case XPETextureBufferFormat::R8_G8_B8_A8: {
    //            vulkanTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
    //            channels = 4;
    //        } break;
    //        default: XP_LOG(XPLoggerSeverityFatal, "Couldn't find a corresponding texture format to this"); break;
    //    }
    //    vulkanTexture->textureDescriptor->setStorageMode(MTL::StorageModeShared);
    //    vulkanTexture->textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    //
    //    vulkanTexture->texture = NS::TransferPtr(_device->newTexture(vulkanTexture->textureDescriptor.get()));
    //
    //    // fill data
    //    MTL::Region  region      = MTL::Region(0, 0, 0, width, height, 1);
    //    NS::UInteger bytesPerRow = channels * width;
    //
    //    vulkanTexture->texture->replaceRegion(region, 0, textureBuffer->getPixels().data(), bytesPerRow);
    //
    //    textureAsset->setGPURef((void*)vulkanTexture->texture.get());
    //    vulkanTexture->textureAsset = textureAsset;
    //
    //    _textureMap.insert({ textureAsset->getFile()->getPath(), std::move(vulkanTexture) });
}

XPProfilable void
XPVulkanRenderer::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
    if (!meshAsset->getFile()->hasChanges()) { return; }

    _registry->getEngine()->scheduleRenderTask([this, meshAsset]() {
        val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
        val::Device&         device         = physicalDevice.getDevice();

        // wait for device to become idle
        vkDeviceWaitIdle(device.device);

        XPMeshBuffer* meshBuffer    = meshAsset->getMeshBuffer();
        XPVulkanMesh* vulkanMeshRef = _meshMap[meshAsset->getFile()->getPath()].get();

        size_t positionsCount = meshBuffer->getPositionsCount();
        size_t normalsCount   = meshBuffer->getNormalsCount();
        size_t texcoordsCount = meshBuffer->getTexcoordsCount();
        size_t indicesCount   = meshBuffer->getIndicesCount();

        const XPVec4<float>* positions = meshBuffer->getPositions();
        const XPVec4<float>* normals   = meshBuffer->getNormals();
        const XPVec4<float>* texcoords = meshBuffer->getTexcoords();
        const uint32_t*      indices   = meshBuffer->getIndices();

        auto vertexBufferSize   = static_cast<uint64_t>(positionsCount * XPMeshBuffer::sizeofPositionsType());
        auto normalBufferSize   = static_cast<uint64_t>(normalsCount * XPMeshBuffer::sizeofNormalsType());
        auto texcoordBufferSize = static_cast<uint64_t>(texcoordsCount * XPMeshBuffer::sizeofTexcoordsType());
        auto indexBufferSize    = static_cast<uint64_t>(indicesCount * XPMeshBuffer::sizeofIndicesType());

        if (vertexBufferSize != device.getBufferSizeInBytes(*vulkanMeshRef->vertexBuffer)) {
            device.destroyBuffer(*vulkanMeshRef->vertexBuffer);
            vulkanMeshRef->vertexBuffer = device.createVertexBuffer(vertexBufferSize, val::EResourceStorageModePrivate);
        }
        if (normalBufferSize != device.getBufferSizeInBytes(*vulkanMeshRef->normalBuffer)) {
            device.destroyBuffer(*vulkanMeshRef->normalBuffer);
            vulkanMeshRef->normalBuffer = device.createVertexBuffer(normalBufferSize, val::EResourceStorageModePrivate);
        }
        if (texcoordBufferSize != device.getBufferSizeInBytes(*vulkanMeshRef->uvBuffer)) {
            device.destroyBuffer(*vulkanMeshRef->uvBuffer);
            vulkanMeshRef->uvBuffer = device.createVertexBuffer(texcoordBufferSize, val::EResourceStorageModePrivate);
        }
        if (indexBufferSize != device.getBufferSizeInBytes(*vulkanMeshRef->indexBuffer)) {
            device.destroyBuffer(*vulkanMeshRef->indexBuffer);
            vulkanMeshRef->indexBuffer = device.createIndexBuffer(indexBufferSize, val::EResourceStorageModePrivate);
        }

        device.writeBuffer(*vulkanMeshRef->vertexBuffer, (void*)positions, 0, vertexBufferSize);
        device.writeBuffer(*vulkanMeshRef->normalBuffer, (void*)normals, 0, normalBufferSize);
        device.writeBuffer(*vulkanMeshRef->uvBuffer, (void*)texcoords, 0, texcoordBufferSize);
        device.writeBuffer(*vulkanMeshRef->indexBuffer, (void*)indices, 0, indexBufferSize);

        vulkanMeshRef->meshAsset = meshAsset;
        vulkanMeshRef->objects.clear();

        for (size_t mboi = 0; mboi < meshBuffer->getObjectsCount(); ++mboi) {
            XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(mboi);
            auto                vulkanMeshObject =
              std::make_unique<XPVulkanMeshObject>((XPVulkanMesh&)*vulkanMeshRef, meshBufferObject.boundingBox);
            vulkanMeshObject->name         = meshBufferObject.name;
            vulkanMeshObject->vertexOffset = meshBufferObject.vertexOffset;
            vulkanMeshObject->indexOffset  = meshBufferObject.indexOffset;
            vulkanMeshObject->numIndices   = meshBufferObject.numIndices;
            vulkanMeshRef->objects.push_back(*vulkanMeshObject.get());
            _meshObjectMap[vulkanMeshObject->name] = std::move(vulkanMeshObject);
        }

        _registry->getScene()->addAttachmentChanges(XPEInteractionHasMeshRendererChanges, false);
    });
}

XPProfilable void
XPVulkanRenderer::reUploadShaderAsset(XPShaderAsset* shaderAsset)
{
    if (!shaderAsset->getFile()->hasChanges()) { return; };

    _registry->getEngine()->scheduleRenderTask([this, shaderAsset]() {
        val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
        val::Device&         device         = physicalDevice.getDevice();

        // wait for device to become idle
        vkDeviceWaitIdle(device.device);

        std::filesystem::path p(shaderAsset->getFile()->getPath());
        std::string           shaderName = p.stem().string();
        // destroy
        for (size_t i = 0; i < _graphicsPipelines->pipelines.size(); ++i) {
            auto it = val::EGraphicsPipelinesMap.find(shaderName);
            if (it != val::EGraphicsPipelinesMap.end()) {
                _graphicsPipelines->pipelines[it->second]->recreate();
                break;
            }
        }

        _registry->getScene()->addAttachmentChanges(XPEInteractionHasMeshRendererChanges, false);
    });
}

XPProfilable void
XPVulkanRenderer::reUploadTextureAsset(XPTextureAsset* textureAsset)
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    // wait for device to become idle
    vkDeviceWaitIdle(device.device);
}

XPRegistry*
XPVulkanRenderer::getRegistry()
{
    return _registry;
}

// void
// XPVulkanRenderer::scheduleReadPixelFromTexture(MTL::Texture* texture, std::shared_ptr<PickBlitCommandInfo>
// blitCmdInfo)
//{
//     MTL::Origin readOrigin = MTL::Origin(static_cast<NS::UInteger>(blitCmdInfo->normalizedCoordinates.x),
//                                          static_cast<NS::UInteger>(blitCmdInfo->normalizedCoordinates.y),
//                                          0);
//     MTL::Size   readSize   = MTL::Size(1, 1, 1);
//
//     MTL::PixelFormat pixelFormat(texture->pixelFormat());
//     if (pixelFormat != MTL::PixelFormatRGBA32Float) {
//         XP_LOGV(XPLoggerSeverityFatal,
//                 "%s:%i %s",
//                 __FILE__,
//                 __LINE__,
//                 fmt::format("Unsupported pixel format: 0x{}.", (uint32_t)pixelFormat).c_str());
//         return;
//     }
//
//     // assuming we have a RGBA texture ...
//     NS::UInteger bytesPerPixel = 4 * sizeof(float);
//     NS::UInteger bytesPerRow   = 1 * bytesPerPixel;
//     NS::UInteger bytesPerImage = 1 * bytesPerRow;
//
//     MTL::Buffer* readBuffer = _device->newBuffer(bytesPerImage, MTL::ResourceStorageModeShared);
//
//     if (!readBuffer) {
//         XP_LOGV(XPLoggerSeverityFatal,
//                 "%s:%i %s",
//                 __FILE__,
//                 __LINE__,
//                 fmt::format("Failed to create buffer for {} bytes.", bytesPerImage).c_str());
//         return;
//     }
//
//     // wait for gpu to finish the ongoing rendering work
//     // note that we have to wait for all the in flight frames to finish this time
//     //    dispatch_semaphore_wait(_inFlightSemaphore, DISPATCH_TIME_FOREVER);
//
//     // Copy the pixel data of the selected region to a vulkan buffer with a shared
//     // storage mode, which makes the buffer accessible to the CPU.
//     MTL::CommandBuffer*      ioCommandBuffer = _ioCommandQueue->commandBuffer();
//     MTL::BlitCommandEncoder* blitEncoder     = ioCommandBuffer->blitCommandEncoder();
//
//     blitEncoder->copyFromTexture(texture, // source texture
//                                  0,       // source slice
//                                  0,       // source level
//                                  MTL::Origin(blitCmdInfo->normalizedCoordinates.x * texture->width(),
//                                              blitCmdInfo->normalizedCoordinates.y * texture->height(),
//                                              0),     // source origin
//                                  MTL::Size(1, 1, 1), // source size
//                                  readBuffer,         // dst buffer
//                                  0,                  // dst offset
//                                  bytesPerRow,        // dst bytes per row
//                                  bytesPerImage       // dst bytes per image
//     );
//
//     blitEncoder->endEncoding();
//     ioCommandBuffer->addCompletedHandler([this, readBuffer, blitCmdInfo](MTL::CommandBuffer*) {
//         std::array<float, 4>* pixels = (std::array<float, 4>*)readBuffer->contents();
//         uint32_t              nodeId = pixels->at(3);
//         readBuffer->release();
//
//         if (nodeId == 0) {
//             blitCmdInfo->cbfn(nullptr);
//             return;
//         }
//         auto optNode = _registry->getScene()->getSceneStore()->getNode(nodeId);
//         if (optNode.has_value()) {
//             blitCmdInfo->cbfn(optNode.value());
//             return;
//         }
//         blitCmdInfo->cbfn(nullptr);
//     });
//     ioCommandBuffer->commit();
//}

void
XPVulkanRenderer::getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn)
{
    //    scheduleReadPixelFromTexture(
    //      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexMetallicRoughnessAmbientObjectId]
    //        .texture.get(),
    //      std::make_shared<PickBlitCommandInfo>(coordinates, XPVec2<size_t>(1, 1), cbfn));
}

void*
XPVulkanRenderer::getMainTexture()
{
    return nullptr;
}

XPVec2<int>
XPVulkanRenderer::getWindowSize()
{
    return _window->getWindowSize();
}

XPVec2<int>
XPVulkanRenderer::getResolution()
{
    return _resolution;
}

XPVec2<float>
XPVulkanRenderer::getMouseLocation()
{
    return _window->getMouseLocation();
}

XPVec2<float>
XPVulkanRenderer::getNormalizedMouseLocation()
{
    return _window->getNormalizedMouseLocation();
}

bool
XPVulkanRenderer::isLeftMouseButtonPressed()
{
    return _window->isLeftMouseButtonPressed();
}

bool
XPVulkanRenderer::isMiddleMouseButtonPressed()
{
    return _window->isMiddleMouseButtonPressed();
}

bool
XPVulkanRenderer::isRightMouseButtonPressed()
{
    return _window->isRightMouseButtonPressed();
}

float
XPVulkanRenderer::getDeltaTime()
{
    return _window->getDeltaTimeSeconds();
}

uint32_t
XPVulkanRenderer::getNumDrawCallsVertices()
{
    return _gpuData->numDrawCallsVertices;
}

uint32_t
XPVulkanRenderer::getTotalNumDrawCallsVertices()
{
    return _gpuData->numTotalDrawCallsVertices;
}

uint32_t
XPVulkanRenderer::getNumDrawCalls()
{
    return _gpuData->numDrawCalls;
}

uint32_t
XPVulkanRenderer::getTotalNumDrawCalls()
{
    return _gpuData->numTotalDrawCalls;
}

bool
XPVulkanRenderer::isCapturingDebugFrames()
{
    return _isCapturingDebugFrames;
}

bool
XPVulkanRenderer::isFramebufferResized()
{
    return _isFramebufferResized;
}

float
XPVulkanRenderer::getRenderingGPUTime()
{
    return _renderingGpuTime;
}

float
XPVulkanRenderer::getUIGPUTime()
{
    return _uiGpuTime;
}

float
XPVulkanRenderer::getComputeGPUTime()
{
    return _computeGpuTime;
}

void
XPVulkanRenderer::captureDebugFrames()
{
    _isCapturingDebugFrames = true;
}

void
XPVulkanRenderer::setFramebufferResized()
{
    _isFramebufferResized = true;
}

void
XPVulkanRenderer::simulateCopy(const char* text)
{
#ifdef XP_EDITOR_MODE
    _registry->getUI()->simulateCopy(text);
#else
    _window->simulateCopy(text);
#endif
}

std::string
XPVulkanRenderer::simulatePaste()
{
#ifdef XP_EDITOR_MODE
    return _registry->getUI()->simulatePaste();
#else
    return _window->simulatePaste();
#endif
}

XPVulkanWindow&
XPVulkanRenderer::getWindow() const
{
    return *_window;
}

val::Instance&
XPVulkanRenderer::getInstance() const
{
    return *_instance;
}

val::Swapchain&
XPVulkanRenderer::getSwapchain() const
{
    return *_swapchain;
}

val::GraphicsPipelines&
XPVulkanRenderer::getGraphicsPipelines() const
{
    return *_graphicsPipelines;
}

#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
const val::FrameBuffer&
XPVulkanRenderer::getGBufferFB() const
{
    return _gbufferFB[_currentImageIndex];
}
const val::FrameBuffer&
XPVulkanRenderer::getCombineFB() const
{
    return _combineFB[_currentImageIndex];
}
#endif

XPVulkanGPUData&
XPVulkanRenderer::getGPUData() const
{
    return *_gpuData;
}

val::DescriptorSets&
XPVulkanRenderer::getDescriptorSets() const
{
    return *_descriptorSets;
}

uint32_t
XPVulkanRenderer::getCurrentFrame() const
{
    return _currentFrame;
}

uint32_t
XPVulkanRenderer::getCurrentImageIndex() const
{
    return _currentImageIndex;
}

bool
XPVulkanRenderer::createDescriptorSetLayout()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    {
        _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].layout =
          _graphicsPipelines->pipelines[val::EGraphicsPipelines_GBuffer]
            ->getCreateInfo()
            .getVertexFragmentShader()
            .getDescriptorSetLayouts()[0];
        //
        // VkDescriptorSetLayoutBinding matricesLayoutBinding = {};
        // matricesLayoutBinding.stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
        // matricesLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // matricesLayoutBinding.binding                      = 0;
        // matricesLayoutBinding.descriptorCount              = 1;
        // matricesLayoutBinding.pImmutableSamplers           = nullptr;
        //
        // std::vector<VkDescriptorSetLayoutBinding> bindings = { matricesLayoutBinding };
        //
        // VkDescriptorSetLayoutCreateInfo matricesLayoutCreateInfo = {};
        // matricesLayoutCreateInfo.sType                           =
        // VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; matricesLayoutCreateInfo.bindingCount =
        // (uint32_t)bindings.size(); matricesLayoutCreateInfo.pBindings                       = bindings.data();
        //
        // VULKAN_ASSERT(
        //   vkCreateDescriptorSetLayout(device.device,
        //                               &matricesLayoutCreateInfo,
        //                               nullptr,
        //                               &_descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].layout),
        //   "Failed to create matrices descriptor set layout");
    }

    {
        _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].layout =
          _graphicsPipelines->pipelines[val::EGraphicsPipelines_MBuffer]
            ->getCreateInfo()
            .getVertexFragmentShader()
            .getDescriptorSetLayouts()[0];
        //
        // std::array<VkDescriptorSetLayoutBinding, val::EGBufferAttachments_Count> layoutBindings = {};
        // layoutBindings[val::EGBufferAttachments_PositionUTexture].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        // layoutBindings[val::EGBufferAttachments_PositionUTexture].descriptorType =
        //   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // layoutBindings[val::EGBufferAttachments_PositionUTexture].binding =
        // val::EGBufferAttachments_PositionUTexture;
        // layoutBindings[val::EGBufferAttachments_PositionUTexture].descriptorCount    = 1;
        // layoutBindings[val::EGBufferAttachments_PositionUTexture].pImmutableSamplers = nullptr;
        //
        // layoutBindings[val::EGBufferAttachments_NormalVTexture].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        // layoutBindings[val::EGBufferAttachments_NormalVTexture].descriptorType =
        //   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // layoutBindings[val::EGBufferAttachments_NormalVTexture].binding = val::EGBufferAttachments_NormalVTexture;
        // layoutBindings[val::EGBufferAttachments_NormalVTexture].descriptorCount    = 1;
        // layoutBindings[val::EGBufferAttachments_NormalVTexture].pImmutableSamplers = nullptr;
        //
        // layoutBindings[val::EGBufferAttachments_AlbedoTexture].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        // layoutBindings[val::EGBufferAttachments_AlbedoTexture].descriptorType =
        //   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // layoutBindings[val::EGBufferAttachments_AlbedoTexture].binding         =
        // val::EGBufferAttachments_AlbedoTexture;
        // layoutBindings[val::EGBufferAttachments_AlbedoTexture].descriptorCount = 1;
        // layoutBindings[val::EGBufferAttachments_AlbedoTexture].pImmutableSamplers = nullptr;
        //
        // layoutBindings[val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture].stageFlags =
        //   VK_SHADER_STAGE_FRAGMENT_BIT;
        // layoutBindings[val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture].descriptorType =
        //   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // layoutBindings[val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture].binding =
        //   val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture;
        // layoutBindings[val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture].descriptorCount    = 1;
        // layoutBindings[val::EGBufferAttachments_MetallicRoughnessAmbientObjectIdTexture].pImmutableSamplers =
        // nullptr;
        //
        // VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        // layoutCreateInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        // layoutCreateInfo.bindingCount                    = (uint32_t)layoutBindings.size();
        // layoutCreateInfo.pBindings                       = layoutBindings.data();
        //
        // VULKAN_ASSERT(
        //   vkCreateDescriptorSetLayout(device.device,
        //                               &layoutCreateInfo,
        //                               nullptr,
        //                               &_descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].layout),
        //   "Failed to create gbuffer attachments descriptor set layout");
    }

    return true;
}

bool
XPVulkanRenderer::createUniformBuffers()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    VkDeviceSize bufferSize = sizeof(XPCameraUBO);
    _uniformBuffers.resize(_swapchain->getColorTextures().size());
    for (size_t i = 0; i < _uniformBuffers.size(); ++i) {
        _uniformBuffers[i] =
          device.createBuffer(bufferSize,
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    return true;
}

bool
XPVulkanRenderer::createDescriptorPool()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    std::array<VkDescriptorPoolSize, val::EDescriptorSetLayout_Count> poolSizes = {};
    poolSizes[val::EDescriptorSetLayout_CameraMatrices].type                    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[val::EDescriptorSetLayout_CameraMatrices].descriptorCount =
      (uint32_t)_swapchain->getColorTextures().size();

    poolSizes[val::EDescriptorSetLayout_GBufferAttachments].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[val::EDescriptorSetLayout_GBufferAttachments].descriptorCount =
      (uint32_t)_swapchain->getColorTextures().size() * val::EGBufferAttachments_Count;

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.poolSizeCount              = (uint32_t)poolSizes.size();
    poolCreateInfo.pPoolSizes                 = poolSizes.data();
    poolCreateInfo.maxSets                    = [&]() -> uint32_t {
        uint32_t counter = 0;
        for (const VkDescriptorPoolSize& poolSize : poolSizes) { counter += poolSize.descriptorCount; }
        return counter;
    }();

    VULKAN_ASSERT(vkCreateDescriptorPool(device.device, &poolCreateInfo, nullptr, &_descriptorSets->pool),
                  "Failed to create descriptor set pool");

    return true;
}

bool
XPVulkanRenderer::createDescriptorSets()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    // mapped with: GBuffer.surface
    {
        std::vector<VkDescriptorSetLayout> layouts(
          _swapchain->getColorTextures().size(),
          _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].layout);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool              = _descriptorSets->pool;
        allocInfo.descriptorSetCount          = (uint32_t)layouts.size();
        allocInfo.pSetLayouts                 = layouts.data();

        _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets.resize(
          _swapchain->getColorTextures().size());
        VULKAN_ASSERT(
          vkAllocateDescriptorSets(
            device.device, &allocInfo, _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets.data()),
          "Failed to allocate descriptor sets");

        for (size_t i = 0; i < _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets.size(); ++i) {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer                 = _uniformBuffers[i]->buffer;
            bufferInfo.offset                 = 0;
            bufferInfo.range                  = sizeof(XPCameraUBO);

            VkWriteDescriptorSet cameraMatricesUboWriteDescriptor = {};
            cameraMatricesUboWriteDescriptor.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraMatricesUboWriteDescriptor.dstSet =
              _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets[i];
            cameraMatricesUboWriteDescriptor.dstBinding      = 0;
            cameraMatricesUboWriteDescriptor.dstArrayElement = 0;
            cameraMatricesUboWriteDescriptor.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraMatricesUboWriteDescriptor.descriptorCount = 1;
            cameraMatricesUboWriteDescriptor.pBufferInfo     = &bufferInfo;

            vkUpdateDescriptorSets(device.device, 1, &cameraMatricesUboWriteDescriptor, 0, nullptr);
        }
    }

    // mapped with: MBuffer.surface
    {
        val::DescriptorSet& descriptorSet = _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments];
        std::vector<VkDescriptorSetLayout> layouts(_swapchain->getColorTextures().size(), descriptorSet.layout);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool              = _descriptorSets->pool;
        allocInfo.descriptorSetCount          = (uint32_t)layouts.size();
        allocInfo.pSetLayouts                 = layouts.data();

        descriptorSet.sets.resize(_swapchain->getColorTextures().size());
        VULKAN_ASSERT(vkAllocateDescriptorSets(device.device, &allocInfo, descriptorSet.sets.data()),
                      "Failed to allocate descriptor sets");

        for (uint32_t i = 0; i < descriptorSet.sets.size(); ++i) {
            VkDescriptorSet destSet = descriptorSet.sets[i];
            for (uint32_t cai = 0; cai < _gbufferFB[i].getNumColorAttachments(); ++cai) {
                VkDescriptorImageInfo colorMapInfo = {};
                colorMapInfo.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                colorMapInfo.imageView             = _gbufferFB[i].getColorAttachment(cai).imageView;
                colorMapInfo.sampler               = _gbufferFB[i].getColorAttachment(cai).sampler;

                VkWriteDescriptorSet textureWriteDescriptorSet = {};
                textureWriteDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                textureWriteDescriptorSet.dstSet               = destSet;
                textureWriteDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                textureWriteDescriptorSet.dstBinding           = cai;
                textureWriteDescriptorSet.dstArrayElement      = 0;
                textureWriteDescriptorSet.descriptorCount      = 1;
                textureWriteDescriptorSet.pImageInfo           = &colorMapInfo;

                vkUpdateDescriptorSets(device.device, 1, &textureWriteDescriptorSet, 0, nullptr);
            }
        }
    }
    return true;
}

bool
XPVulkanRenderer::createFramebuffers()
{
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
    _gbufferFB.reserve(_swapchain->getColorTextures().size());
    for (uint32_t i = 0; i < _swapchain->getColorTextures().size(); ++i) {
        _gbufferFB.emplace_back(this,
                                _graphicsPipelines->pipelines[val::EGraphicsPipelines_GBuffer]
                                  ->getCreateInfo()
                                  .getVertexFragmentShader()
                                  .getFSOutputFormats()
                                  .size());
        _gbufferFB[i].create(
          _swapchain->getExtent().width, _swapchain->getExtent().height, val::EGraphicsPipelines_GBuffer);
    }
    _combineFB.reserve(_swapchain->getColorTextures().size());
    for (uint32_t i = 0; i < _swapchain->getColorTextures().size(); ++i) {
        _combineFB.emplace_back(this,
                                _graphicsPipelines->pipelines[val::EGraphicsPipelines_MBuffer]
                                  ->getCreateInfo()
                                  .getVertexFragmentShader()
                                  .getFSOutputFormats()
                                  .size());
        _combineFB[i].create(
          _swapchain->getExtent().width, _swapchain->getExtent().height, val::EGraphicsPipelines_MBuffer);
    }
#endif

    return true;
}

void
XPVulkanRenderer::destroyDescriptorSetLayout()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    for (uint32_t i = 0; i < val::EDescriptorSetLayout_Count; ++i) {
        vkDestroyDescriptorSetLayout(device.device, _descriptorSets->layouts[i].layout, nullptr);
    }
}

void
XPVulkanRenderer::destroyUniformBuffers()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    for (size_t i = 0; i < _uniformBuffers.size(); ++i) {
        device.destroyBuffer(*_uniformBuffers[i]);
        _uniformBuffers[i].reset(nullptr);
    }
    _uniformBuffers.clear();
}

void
XPVulkanRenderer::destroyDescriptorPool()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    vkDestroyDescriptorPool(device.device, _descriptorSets->pool, nullptr);
}

void
XPVulkanRenderer::destroyDescriptorSets()
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    for (uint32_t i = 0; i < val::EDescriptorSetLayout_Count; ++i) {
        vkFreeDescriptorSets(device.device,
                             _descriptorSets->pool,
                             (uint32_t)_descriptorSets->layouts[i].sets.size(),
                             _descriptorSets->layouts[i].sets.data());
    }
}

void
XPVulkanRenderer::destroyFramebuffers()
{
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
    for (uint32_t i = 0; i < _gbufferFB.size(); ++i) { _gbufferFB[i].destroy(); }
    for (uint32_t i = 0; i < _combineFB.size(); ++i) { _combineFB[i].destroy(); }
    _gbufferFB.clear();
    _combineFB.clear();
#endif
}

void
XPVulkanRenderer::compileLoadScene(XPScene& scene)
{
    auto& meshNodes = scene.getNodes(MeshRendererAttachmentDescriptor | TransformAttachmentDescriptor);

    // if (scene.hasOnlyAttachmentChanges(XPEInteractionHasTransformChanges)) {
    //     if (_gpuData->numMeshNodes == meshNodes.size()) {
    //         for (const auto& meshNode : meshNodes) {
    //             Transform* transform  = meshNode->getTransform();
    //             auto       operations = static_cast<XPMat4<float>::ModelMatrixOperations>(
    //               XPMat4<float>::ModelMatrixOperation_Translation | XPMat4<float>::ModelMatrixOperation_Rotation |
    //               XPMat4<float>::ModelMatrixOperation_Scale);
    //             XPMat4<float>::buildModelMatrix(
    //               transform->modelMatrix, transform->location, transform->euler, transform->scale, operations);
    //             _gpuData->modelMatrices[_gpuData->numMeshNodes] = transform->modelMatrix;
    //         }
    //         return;
    //     }
    // }

    uint32_t numMeshNodes = 0;
    uint32_t numSubMeshes = 0;
    for (const auto& meshNode : meshNodes) {
        MeshRenderer* meshRenderer = meshNode->getMeshRenderer();
        numSubMeshes += meshRenderer->info.size();
        ++numMeshNodes;
    }

    size_t previousNumMeshNodes = _gpuData->numMeshNodes;
    size_t previousNumSubMeshes = _gpuData->numSubMeshes;

    // ------------------------------------------------------------------------------------------------------------------------
    // IMPORTANT: we need to trim the vectors to fit without releaseing reserved memory
    // Handle cases where the size is either reduced or increased compared to previous scene data
    // ------------------------------------------------------------------------------------------------------------------------
    if (numMeshNodes < previousNumMeshNodes) {
        for (size_t i = numMeshNodes; i < previousNumMeshNodes; ++i) {
            _gpuData->modelMatrices.pop_back();
            _gpuData->meshNodesIds.pop_back();
        }
    } else if (numMeshNodes > previousNumMeshNodes) {
        _gpuData->modelMatrices.resize(numMeshNodes);
        _gpuData->meshNodesIds.resize(numMeshNodes);
    }

    if (numSubMeshes < previousNumSubMeshes) {
        for (size_t i = numSubMeshes; i < previousNumSubMeshes; ++i) {
            _gpuData->meshObjects.pop_back();
            _gpuData->boundingBoxes.pop_back();
            _gpuData->perMeshNodeIndices.pop_back();
        }
    } else if (numSubMeshes > previousNumSubMeshes) {
        _gpuData->meshObjects.resize(numSubMeshes);
        _gpuData->boundingBoxes.resize(numSubMeshes);
        _gpuData->perMeshNodeIndices.resize(numSubMeshes);
    }
    // ------------------------------------------------------------------------------------------------------------------------

    _gpuData->numTotalDrawCallsVertices = 0;
    _gpuData->numDrawCallsVertices      = 0;
    _gpuData->numTotalDrawCalls         = 0;
    _gpuData->numDrawCalls              = 0;
    _gpuData->numMeshNodes              = 0;
    _gpuData->numSubMeshes              = 0;
    for (const auto& meshNode : meshNodes) {
        Transform* transform  = meshNode->getTransform();
        auto       operations = static_cast<XPMat4<float>::ModelMatrixOperations>(
          XPMat4<float>::ModelMatrixOperation_Translation | XPMat4<float>::ModelMatrixOperation_Rotation |
          XPMat4<float>::ModelMatrixOperation_Scale);
        XPMat4<float>::buildModelMatrix(
          transform->modelMatrix, transform->location, transform->euler, transform->scale, operations);
        _gpuData->modelMatrices[_gpuData->numMeshNodes] = transform->modelMatrix;
        _gpuData->meshNodesIds[_gpuData->numMeshNodes]  = meshNode->getId();
        MeshRenderer* meshRenderer                      = meshNode->getMeshRenderer();
        for (const auto& subMesh : meshRenderer->info) {
            XPVulkanMeshObject* meshObject                       = _meshObjectMap[subMesh.mesh.text].get();
            _gpuData->meshObjects[_gpuData->numSubMeshes]        = meshObject;
            _gpuData->boundingBoxes[_gpuData->numSubMeshes]      = meshObject->boundingBox;
            _gpuData->perMeshNodeIndices[_gpuData->numSubMeshes] = _gpuData->numMeshNodes;
            ++_gpuData->numSubMeshes;
            ++_gpuData->numTotalDrawCalls;
            _gpuData->numTotalDrawCallsVertices += meshObject->numIndices;
        }
        ++_gpuData->numMeshNodes;
    }
}

void
XPVulkanRenderer::updateUniformBuffer(XPScene& scene, FreeCamera& camera, uint32_t randomImageIndex)
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    auto& meshNodes = scene.getNodes(MeshRendererAttachmentDescriptor | TransformAttachmentDescriptor);

    size_t      modelCounter = 0;
    XPCameraUBO ubo          = {};

    ubo.viewProjectionMatrix        = camera.activeProperties.viewProjectionMatrix;
    ubo.inverseViewProjectionMatrix = camera.activeProperties.inverseViewProjectionMatrix;
    ubo.inverseViewMatrix           = camera.activeProperties.inverseViewMatrix;

    void* data;
    vkMapMemory(device.device, _uniformBuffers[randomImageIndex]->memory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device.device, _uniformBuffers[randomImageIndex]->memory);
}

void
XPVulkanRenderer::recordCommandBuffers(XPScene&        scene,
                                       FreeCamera&     camera,
                                       uint32_t        randomImageIndex,
                                       VkCommandBuffer commandBuffer)
{
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    auto& layers      = scene.getLayers();
    auto& cameraNodes = scene.getNodes(FreeCameraAttachmentDescriptor);

    if (!layers.empty() && !cameraNodes.empty()) {
        if (scene.hasAnyAttachmentChanges(XPEInteractionHasTransformChanges | XPEInteractionHasMeshRendererChanges)) {
            // RAII, block and wait for all frames to finish !
            {
                compileLoadScene(scene);
            }
            scene.removeAttachmentChanges(XPEInteractionHasTransformChanges | XPEInteractionHasMeshRendererChanges,
                                          true);
        }
    }

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _graphicsPipelines->pipelines[val::EGraphicsPipelines_GBuffer]->getPipeline());
    vkCmdBindDescriptorSets(
      commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      _graphicsPipelines->pipelines[val::EGraphicsPipelines_GBuffer]->getCreateInfo().getPipelineLayout(),
      0,
      1,
      &_descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets[randomImageIndex],
      0,
      nullptr);

    bool frustumCullingEnabled     = false;
    _gpuData->numDrawCallsVertices = 0;
    _gpuData->numDrawCalls         = 0;
    for (size_t subMeshIndex = 0; subMeshIndex < _gpuData->numSubMeshes; ++subMeshIndex) {
        const XPVulkanMeshObject& meshObject  = *_gpuData->meshObjects[subMeshIndex];
        XPMat4<float>&            modelMatrix = _gpuData->modelMatrices[_gpuData->perMeshNodeIndices[subMeshIndex]];
        if ((!frustumCullingEnabled ||
             (frustumCullingEnabled && _gpuData->meshObjects[subMeshIndex]->boundingBox.minPoint.w == 1))) {

            val::PushConstantData pcd = {};
            pcd.modelMatrix           = modelMatrix;
            pcd.objectId              = _gpuData->meshNodesIds[_gpuData->perMeshNodeIndices[subMeshIndex]];

            vkCmdPushConstants(
              commandBuffer,
              _graphicsPipelines->pipelines[val::EGraphicsPipelines_GBuffer]->getCreateInfo().getPipelineLayout(),
              VK_SHADER_STAGE_VERTEX_BIT,
              0,
              sizeof(val::PushConstantData),
              (const void*)&pcd);

            const VkBuffer     buffers[] = { meshObject.mesh.vertexBuffer->buffer,
                                             meshObject.mesh.normalBuffer->buffer,
                                             meshObject.mesh.uvBuffer->buffer };
            const VkDeviceSize offsets[] = { XPMeshBuffer::sizeofPositionsType() * meshObject.vertexOffset,
                                             XPMeshBuffer::sizeofNormalsType() * meshObject.vertexOffset,
                                             XPMeshBuffer::sizeofTexcoordsType() * meshObject.vertexOffset };
            vkCmdBindVertexBuffers(commandBuffer, 0, 3, buffers, offsets);

            const VkDeviceSize indexBufferOffset = XPMeshBuffer::sizeofIndicesType() * meshObject.indexOffset;
            vkCmdBindIndexBuffer(
              commandBuffer, meshObject.mesh.indexBuffer->buffer, indexBufferOffset, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, meshObject.numIndices, 1, 0, 0, 0);

            ++_gpuData->numDrawCalls;
            _gpuData->numDrawCallsVertices += meshObject.numIndices;
        }
    }
}

void
XPVulkanRenderer::renderToTexture(VkCommandBuffer commandBuffer, XPScene& scene, float deltaTime)
{
#ifdef TODO_ENABLE_RENDER_TO_TEXTURES
    val::PhysicalDevice& physicalDevice = _instance->getPhysicalDevice();
    val::Device&         device         = physicalDevice.getDevice();

    if (_registry->getEngine()->hasTasks()) {
        vkDeviceWaitIdle(device.device);

        _registry->getEngine()->runUITasks();
        _registry->getEngine()->runGameTasks();
        _registry->getEngine()->runComputeTasks();
        _registry->getEngine()->runPhysicsTasks();
        _registry->getEngine()->runRenderTasks();

        vkDeviceWaitIdle(device.device);
        return;
    }

    auto& layers      = scene.getLayers();
    auto& cameraNodes = scene.getNodes(FreeCameraAttachmentDescriptor);

    // gbuffer render pass 0
    _gbufferFB[_currentImageIndex].beginRender(
      commandBuffer,
      _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].layout,
      _descriptorSets->layouts[val::EDescriptorSetLayout_CameraMatrices].sets[_currentImageIndex]);
    {
        if (!cameraNodes.empty()) {
            FreeCamera& camera = *(*cameraNodes.begin())->getFreeCamera();
            CameraSystemUpdateAll(&camera, _registry, _window->getInput(), deltaTime);

            updateUniformBuffer(scene, camera, _currentImageIndex);

            recordCommandBuffers(scene, camera, _currentImageIndex, commandBuffer);
        }
    }
    _gbufferFB[_currentImageIndex].endRender(commandBuffer);

    // combine render pass 1
    _combineFB[_currentImageIndex].beginRender(
      commandBuffer,
      _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].layout,
      _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].sets[_currentImageIndex]);
    {
    }
    _combineFB[_currentImageIndex].endRender(commandBuffer);

    // render combine to swapchain
    _combineFB[_currentImageIndex].renderToScreen(
      commandBuffer,
      _swapchain->getColorTextures()[_currentImageIndex]->imageView,
      _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].layout,
      _descriptorSets->layouts[val::EDescriptorSetLayout_GBufferAttachments].sets[_currentImageIndex]);
#endif
}
