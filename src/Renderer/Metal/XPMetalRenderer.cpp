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
#include <Renderer/Metal/MSL/XPMSLParser.h>
#include <Renderer/Metal/XPMetal.h>
#include <Renderer/Metal/XPMetalRenderer.h>
#include <Renderer/Metal/XPMetalWindow.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <SceneDescriptor/XPSceneStore.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/ImGUI/Tabs/Tabs.h>
    #include <UI/ImGUI/XPImGUI.h>
#endif
#include <Utilities/XPCrossShaderCompiler.h>
#include <Utilities/XPFS.h>
#include <Utilities/XPFreeCameraSystem.h>
#include <Utilities/XPLogger.h>

#include <cstring>
#include <memory>
#include <optional>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#pragma clang diagnostic pop

XPMetalRenderer::XPMetalRenderer(XPRegistry* const registry)
  : XPIRenderer(registry)
  , _registry(registry)
  , _window(std::make_unique<XPMetalWindow>(this))
  , _resolution(XP_INITIAL_WINDOW_WIDTH, XP_INITIAL_WINDOW_HEIGHT)
  , _frameDataIndex(0)
  , _isCapturingDebugFrames(false)
  , _renderingGpuTime(1.0f)
  , _uiGpuTime(1.0f)
  , _computeGpuTime(1.0f)
  , _lineRenderer(std::make_unique<XPMetalLineRenderer>())
{
}

XPMetalRenderer::~XPMetalRenderer() {}

void
XPMetalRenderer::initialize()
{
    // // TODO: Find a way out for cross compiling from GLSL or whatever else I see possible ..
    // XPCrossShaderInputInfo inputInfo = {};
    // inputInfo.format                 = XPCrossShaderFormat_GLSL_CORE_410;
    // std::vector<std::filesystem::path> files;
    // XPFS::iterateFilesRecursivelyFromDirectory(_registry->getFileWatch()->getPrototypesPath().c_str(), files);
    // for (const auto& prototypeEntry : files) {
    //     if (!std::filesystem::is_regular_file(prototypeEntry)) { continue; }
    //     auto fullPath = prototypeEntry.string();
    //     if (XPFile::isShaderFile(fullPath) && prototypeEntry.extension() == ".glsl") {
    //         XPCrossShaderOutputInfo outputInfo;
    //         outputInfo.format = XPCrossShaderFormat_MSL;
    //         bool success      = false;
    //         if (prototypeEntry.stem() == "frag") {
    //             inputInfo.stage = XPCrossShaderStage_Fragment;
    //             if (XPFS::readFileText(fullPath.c_str(), inputInfo.source)) {
    //                 success = crossShaderCompile(inputInfo, outputInfo);
    //             }
    //         } else if (prototypeEntry.stem() == "vert") {
    //             inputInfo.stage = XPCrossShaderStage_Vertex;
    //             if (XPFS::readFileText(fullPath.c_str(), inputInfo.source)) {
    //                 success = crossShaderCompile(inputInfo, outputInfo);
    //             }
    //         } else {
    //             // assume it's a fragment shader
    //             inputInfo.stage = XPCrossShaderStage_Fragment;
    //             if (XPFS::readFileText(fullPath.c_str(), inputInfo.source)) {
    //                 success = crossShaderCompile(inputInfo, outputInfo);
    //             }
    //         }
    //         if (!success) {
    //             XP_LOGV(XPLoggerSeverityError,
    //                     "Failed to cross compile the prototype shader [%s]\n%s",
    //                     fullPath.c_str(),
    //                     outputInfo.errors.c_str());
    //         } else {
    //             std::filesystem::create_directories(XPFS::buildShaderAssetsPath(prototypeEntry.parent_path().stem()));
    //             std::string outputFilepath = XPFS::buildShaderAssetsPath(fmt::format(
    //               "{}/{}.metal", prototypeEntry.parent_path().stem().string().c_str(),
    //               prototypeEntry.stem().string()));
    //             XPFS::writeFileText(outputFilepath.c_str(), outputInfo.source);
    //         }
    //     }
    // }

    _autoReleasePool = NS::AutoreleasePool::alloc()->init();
    _window->initialize();

    _renderingCommandQueueSemaphore = dispatch_semaphore_create(XPMaxBuffersInFlight);

    _ioCommandQueue        = _device->newCommandQueue();
    _renderingCommandQueue = _device->newCommandQueue();
    _computeCommandQueue   = _device->newCommandQueue();

    // load builtin shaders for framebuffers
    {
        _framePipeline = std::make_unique<XPMetalFramePipeline>();
        // main framebuffer
        {
            XPMetalRenderPipelineCreationInfo mBufferCreationInfo(
              "MBuffer",
              mBufferVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction),
              { MTL::PixelFormatBGRA8Unorm },
              MTL::PixelFormatInvalid,
              MTL::PixelFormatInvalid,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/MBuffer.metal");
                auto renderPipeline = std::make_unique<XPMetalRenderPipeline>();
                createRenderPipeline(*renderPipeline, mBufferCreationInfo);
                _renderPipelines["MBuffer"] = std::move(renderPipeline);
            }
            {
                _framePipeline->mBuffer                              = std::make_unique<XPMetalMainBuffer>();
                _framePipeline->mBuffer->frameBuffer                 = std::make_unique<XPMetalFrameBuffer>();
                _framePipeline->mBuffer->frameBuffer->passDescriptor = MTL::RenderPassDescriptor::alloc()->init();
                METAL_CHECK(_framePipeline->mBuffer->frameBuffer->passDescriptor,
                            "Failed to get a valid metal render pass descriptor");
                createFramebufferTexture(*_framePipeline->mBuffer->frameBuffer.get(), mBufferCreationInfo);
                _framePipeline->mBuffer->frameBuffer->clearColor = MTL::ClearColor(XP_DEFAULT_CLEAR_COLOR);
                _framePipeline->mBuffer->renderPipeline          = _renderPipelines["MBuffer"].get();
            }
        }

        // GBuffer
        {
            XPMetalRenderPipelineCreationInfo gBufferCreationInfo(
              "GBuffer",
              gBufferVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction |
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_EnableDepthWrite),
              { MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatRGBA32Float },
              MTL::PixelFormatDepth32Float,
              MTL::PixelFormatStencil8,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                // -----------------
                // COLOR ATTACHMENTS
                // -----------------
                // float4 positionU;
                // float4 normalV;
                // float4 albedo;
                // float4 metallicRoughnessAmbientObjectId;
                // ------------------------------------------
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/GBuffer.metal");
                auto renderPipeline = std::make_unique<XPMetalRenderPipeline>();
                createRenderPipeline(*renderPipeline, gBufferCreationInfo);
                _renderPipelines["GBuffer"] = std::move(renderPipeline);
            }
            {
                _framePipeline->gBuffer                              = std::make_unique<XPMetalGBuffer>();
                _framePipeline->gBuffer->frameBuffer                 = std::make_unique<XPMetalFrameBuffer>();
                _framePipeline->gBuffer->frameBuffer->passDescriptor = MTL::RenderPassDescriptor::alloc()->init();
                METAL_CHECK(_framePipeline->gBuffer->frameBuffer->passDescriptor,
                            "Failed to get a valid metal render pass descriptor");
                createFramebufferTexture(*_framePipeline->gBuffer->frameBuffer.get(), gBufferCreationInfo);
                _framePipeline->gBuffer->frameBuffer->clearColor = MTL::ClearColor(XP_DEFAULT_CLEAR_COLOR);
                _framePipeline->gBuffer->renderPipeline          = _renderPipelines["GBuffer"].get();
            }
        }

        // line rendering
        {
            XPMetalRenderPipelineCreationInfo lineCreationInfo(
              "Line",
              lineVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction |
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_EnableDepthWrite),
              { MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatRGBA32Float },
              MTL::PixelFormatDepth32Float,
              MTL::PixelFormatStencil8,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/Line.metal");
                auto renderPipeline = std::make_unique<XPMetalRenderPipeline>();
                createRenderPipeline(*renderPipeline, lineCreationInfo);
                _renderPipelines["Line"] = std::move(renderPipeline);
            }
        }

        // main compute
        {
            XPMetalComputePipelineCreationInfo creationInfo(
              "MainCompute",
              { 0, 0, 0, 0 },
              static_cast<XPMetalComputePipelineCreationInfo::ComputePipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalComputePipelineCreationInfo::ComputePipelineFlags_CompileShaderFromSource |
#endif
                XPMetalComputePipelineCreationInfo::ComputePipelineFlags_None));
            {
                _computePipelines["MainCompute"] = std::make_unique<XPMetalComputePipeline>();
                createComputePipeline(*_computePipelines["MainCompute"].get(), creationInfo);
            }
        }
    }

    // for (auto& meshAssetPair : _registry->getDataPipelineStore()->getMeshAssets()) {
    //     uploadMeshAsset(meshAssetPair.second);
    // }

    _gpuData = std::make_unique<XPMetalGPUData>();

    //    _computeRenderingEvent = std::make_unique<XPMetalEvent>(_device);
    //    _renderingUIEvent = std::make_unique<XPMetalEvent>(_device);
}

void
XPMetalRenderer::finalize()
{
    // wait for gpu to finish the ongoing rendering work
    // note that we have to wait for all the in flight frames to finish this time
    for (size_t i = 0; i < XPMaxBuffersInFlight; ++i) {
        dispatch_semaphore_wait(_renderingCommandQueueSemaphore, DISPATCH_TIME_FOREVER);
    }

    //    _renderingUIEvent.reset();
    //    _computeRenderingEvent.reset();

    _gpuData.reset();

    _shaderMap.clear();

    _textureMap.clear();

    _meshMap.clear();
    _meshObjectMap.clear();

    for (auto& pair : _computePipelines) { destroyComputePipeline(*pair.second); }

    for (auto& pair : _renderPipelines) { destroyRenderPipeline(*pair.second); }

    destroyFramebufferTexture(*_framePipeline->mBuffer->frameBuffer);
    _framePipeline->mBuffer->frameBuffer->passDescriptor->release();

    destroyFramebufferTexture(*_framePipeline->gBuffer->frameBuffer);
    _framePipeline->gBuffer->frameBuffer->passDescriptor->release();

    _ioCommandQueue->release();
    _renderingCommandQueue->release();
    _computeCommandQueue->release();

    _window->finalize();

    _autoReleasePool->release();

    free(_renderingCommandQueueSemaphore);
}

XPProfilable void
XPMetalRenderer::update()
{
    NS::AutoreleasePool* localReleasePool = NS::AutoreleasePool::alloc()->init();
    _window->pollEvents();
    _window->refresh();

    if (_registry->getEngine()->hasTasks()) {
        dispatch_semaphore_wait(_renderingCommandQueueSemaphore, DISPATCH_TIME_FOREVER);

        _renderingCommandBuffer = _renderingCommandQueue->commandBuffer();

        _registry->getEngine()->runUITasks();
        _registry->getEngine()->runGameTasks();
        _registry->getEngine()->runPhysicsTasks();
        _registry->getEngine()->runRenderTasks();
        _registry->getEngine()->runComputeTasks();

        _renderingCommandBuffer->commit();

        _renderingCommandBuffer->waitUntilCompleted();

        {
            __block dispatch_semaphore_t block_sema = _renderingCommandQueueSemaphore;
            dispatch_semaphore_signal(block_sema);
        }
    }

    auto& scene       = *_registry->getScene();
    auto& layers      = scene.getLayers();
    auto& cameraNodes = scene.getNodes(FreeCameraAttachmentDescriptor);

    if (!layers.empty() && !cameraNodes.empty()) {
        if (scene.hasAnyAttachmentChanges(XPEInteractionHasTransformChanges | XPEInteractionHasMeshRendererChanges)) {
            // RAII, block and wait for all frames to finish !
            {
                XPMetalFramesInFlightLock lockAllFrames({ _renderingCommandQueueSemaphore });
                compileLoadScene(scene);
            }
            scene.removeAttachmentChanges(XPEInteractionHasTransformChanges | XPEInteractionHasMeshRendererChanges,
                                          true);
        }
    }

    // Prevent the app from rendering more than maximum buffers in flight frames at once.
    dispatch_semaphore_wait(_renderingCommandQueueSemaphore, DISPATCH_TIME_FOREVER);

    _frameDataIndex = (_frameDataIndex + 1) % XPMaxBuffersInFlight;

    _vertexShaderArgBuffer0[_frameDataIndex].viewportSize.x =
      static_cast<uint>(_framePipeline->mBuffer->frameBuffer->colorAttachments[0].texture->width());
    _vertexShaderArgBuffer0[_frameDataIndex].viewportSize.y =
      static_cast<uint>(_framePipeline->mBuffer->frameBuffer->colorAttachments[0].texture->height());

    MTL::Viewport viewport = {};
    viewport.originX       = 0.0;
    viewport.originY       = 0.0;
    viewport.width         = _vertexShaderArgBuffer0[_frameDataIndex].viewportSize.x;
    viewport.height        = _vertexShaderArgBuffer0[_frameDataIndex].viewportSize.y;
    viewport.znear         = 0.0;
    viewport.zfar          = 1.0;

    _renderingCommandBuffer = _renderingCommandQueue->commandBuffer();

    // events waiting
    //    _computeRenderingEvent->wait(_renderingCommandBuffer);
    //    _renderingUIEvent->wait(_uiCommandBuffer);

    float deltaTime = _window->getDeltaTimeSeconds();
    if (!layers.empty() && !cameraNodes.empty()) {
        FreeCamera& camera = *(*cameraNodes.begin())->getFreeCamera();
        CameraSystemUpdateAll(&camera, _registry, _window->getInput(), deltaTime);
        updateViewport(scene, camera, viewport);
        updateMatrices(scene, camera, deltaTime);
        updateGPUScene(scene, camera);
        renderGBuffer(_renderingCommandBuffer, scene, camera, deltaTime, viewport);
        renderMBuffer(_renderingCommandBuffer, scene, camera, deltaTime, viewport);
    }

    {
        __block dispatch_semaphore_t block_sema = _renderingCommandQueueSemaphore;

        _renderingCommandBuffer->addCompletedHandler(^(MTL::CommandBuffer* buffer) {
          // get gpu time from buffer
          _renderingGpuTime = (((double)buffer->GPUEndTime() - (double)buffer->GPUStartTime()) * 1000.0);
          // Allow the app to start another frame by dispatching the in-flight semaphore.
          dispatch_semaphore_signal(block_sema);
        });
    }

    // Schedule a present once the framebuffer is complete using the current drawable.
    _renderingCommandBuffer->presentDrawable(_drawable);
    // _uiCommandBuffer->presentDrawable(_drawable);

    // signal events
    //    _computeRenderingEvent->signal(_renderingCommandBuffer);
    //    _renderingUIEvent->signal(_uiCommandBuffer);

    // Finalize rendering here & push the command buffer to the GPU.
    _renderingCommandBuffer->commit();
    localReleasePool->release();
}

void
XPMetalRenderer::onSceneTraitsChanged()
{
}

void
XPMetalRenderer::invalidateDeviceObjects()
{
}

void
XPMetalRenderer::createDeviceObjects()
{
}

void
XPMetalRenderer::beginUploadMeshAssets()
{
}

void
XPMetalRenderer::endUploadMeshAssets()
{
}

void
XPMetalRenderer::beginUploadShaderAssets()
{
}

void
XPMetalRenderer::endUploadShaderAssets()
{
}

void
XPMetalRenderer::beginUploadTextureAssets()
{
}

void
XPMetalRenderer::endUploadTextureAssets()
{
}

void
XPMetalRenderer::beginReUploadMeshAssets()
{
}

void
XPMetalRenderer::endReUploadMeshAssets()
{
}

void
XPMetalRenderer::beginReUploadShaderAssets()
{
}

void
XPMetalRenderer::endReUploadShaderAssets()
{
}

void
XPMetalRenderer::beginReUploadTextureAssets()
{
}

void
XPMetalRenderer::endReUploadTextureAssets()
{
}

XPProfilable void
XPMetalRenderer::uploadMeshAsset(XPMeshAsset* meshAsset)
{
    XPMeshBuffer*                meshBuffer = meshAsset->getMeshBuffer();
    std::unique_ptr<XPMetalMesh> metalMesh  = std::make_unique<XPMetalMesh>();

    size_t positionsCount = meshBuffer->getPositionsCount();
    size_t normalsCount   = meshBuffer->getNormalsCount();
    size_t texcoordsCount = meshBuffer->getTexcoordsCount();
    size_t indicesCount   = meshBuffer->getIndicesCount();

    const XPVec4<float>* positions = meshBuffer->getPositions();
    const XPVec4<float>* normals   = meshBuffer->getNormals();
    const XPVec4<float>* texcoords = meshBuffer->getTexcoords();
    const uint32_t*      indices   = meshBuffer->getIndices();

    const uint64_t vertexBufferSize   = static_cast<uint64_t>(positionsCount * XPMeshBuffer::sizeofPositionsType());
    const uint64_t normalBufferSize   = static_cast<uint64_t>(normalsCount * XPMeshBuffer::sizeofNormalsType());
    const uint64_t texcoordBufferSize = static_cast<uint64_t>(texcoordsCount * XPMeshBuffer::sizeofTexcoordsType());
    const uint64_t indexBufferSize    = static_cast<uint64_t>(indicesCount * XPMeshBuffer::sizeofIndicesType());
    metalMesh->vertexBuffer = NS::TransferPtr(_device->newBuffer(vertexBufferSize, MTL::ResourceStorageModePrivate));
    metalMesh->normalBuffer = NS::TransferPtr(_device->newBuffer(normalBufferSize, MTL::ResourceStorageModePrivate));
    metalMesh->uvBuffer     = NS::TransferPtr(_device->newBuffer(texcoordBufferSize, MTL::ResourceStorageModePrivate));
    metalMesh->indexBuffer  = NS::TransferPtr(_device->newBuffer(indexBufferSize, MTL::ResourceStorageModePrivate));

    auto tmpVertexBuffer = _device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
    auto tmpNormalBuffer = _device->newBuffer(normalBufferSize, MTL::ResourceStorageModeShared);
    auto tmpUvBuffer     = _device->newBuffer(texcoordBufferSize, MTL::ResourceStorageModeShared);
    auto tmpIndexBuffer  = _device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);

    memcpy(tmpVertexBuffer->contents(), positions, vertexBufferSize);
    memcpy(tmpNormalBuffer->contents(), normals, normalBufferSize);
    memcpy(tmpUvBuffer->contents(), texcoords, texcoordBufferSize);
    memcpy(tmpIndexBuffer->contents(), indices, indexBufferSize);

    MTL::CommandBuffer*      commandBuffer      = _ioCommandQueue->commandBuffer();
    MTL::BlitCommandEncoder* blitCommandEncoder = commandBuffer->blitCommandEncoder();
    {
        // copy shared to private buffers
        blitCommandEncoder->copyFromBuffer(
          tmpVertexBuffer, 0, metalMesh->vertexBuffer.get(), 0, metalMesh->vertexBuffer->length());
        blitCommandEncoder->copyFromBuffer(
          tmpNormalBuffer, 0, metalMesh->normalBuffer.get(), 0, metalMesh->normalBuffer->length());
        blitCommandEncoder->copyFromBuffer(tmpUvBuffer, 0, metalMesh->uvBuffer.get(), 0, metalMesh->uvBuffer->length());
        blitCommandEncoder->copyFromBuffer(
          tmpIndexBuffer, 0, metalMesh->indexBuffer.get(), 0, metalMesh->indexBuffer->length());
    }
    blitCommandEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // release tmp buffers
    tmpVertexBuffer->release();
    tmpNormalBuffer->release();
    tmpUvBuffer->release();
    tmpIndexBuffer->release();

    auto metalMeshRef = metalMesh.get();
    meshAsset->setGPURef((void*)metalMeshRef);
    metalMesh->meshAsset = meshAsset;

    _meshMap.insert({ meshAsset->getFile()->getPath(), std::move(metalMesh) });
    for (size_t mboi = 0; mboi < meshBuffer->getObjectsCount(); ++mboi) {
        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(mboi);
        auto                metalMeshObject =
          std::make_unique<XPMetalMeshObject>((XPMetalMesh&)*metalMeshRef, meshBufferObject.boundingBox);
        metalMeshObject->name         = meshBufferObject.name;
        metalMeshObject->vertexOffset = meshBufferObject.vertexOffset;
        metalMeshObject->indexOffset  = meshBufferObject.indexOffset;
        metalMeshObject->numIndices   = meshBufferObject.numIndices;
        metalMeshRef->objects.push_back(*metalMeshObject.get());
        _meshObjectMap.insert({ metalMeshObject->name, std::move(metalMeshObject) });
    }
}

XPProfilable void
XPMetalRenderer::uploadShaderAsset(XPShaderAsset* shaderAsset){ XP_UNUSED(shaderAsset) }

XPProfilable void XPMetalRenderer::uploadTextureAsset(XPTextureAsset* textureAsset)
{
    XPTextureBuffer*                textureBuffer = textureAsset->getTextureBuffer();
    std::unique_ptr<XPMetalTexture> metalTexture  = std::make_unique<XPMetalTexture>();
    int                             width         = textureBuffer->getDimensions().x;
    int                             height        = textureBuffer->getDimensions().y;
    int                             channels      = 0;

    metalTexture->textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
    metalTexture->textureDescriptor->setWidth(width);
    metalTexture->textureDescriptor->setHeight(height);
    switch (textureBuffer->getFormat()) {
        case XPETextureBufferFormat::R8: {
            metalTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatR8Unorm);
            channels = 1;
        } break;
        case XPETextureBufferFormat::R8_G8: {
            metalTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRG8Unorm);
            channels = 2;
        } break;
        case XPETextureBufferFormat::R8_G8_B8_A8: {
            metalTexture->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
            channels = 4;
        } break;
        default: XP_LOG(XPLoggerSeverityFatal, "Couldn't find a corresponding texture format to this"); break;
    }
    metalTexture->textureDescriptor->setStorageMode(MTL::StorageModeShared);
    metalTexture->textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

    metalTexture->texture = NS::TransferPtr(_device->newTexture(metalTexture->textureDescriptor.get()));

    // fill data
    MTL::Region  region      = MTL::Region(0, 0, 0, width, height, 1);
    NS::UInteger bytesPerRow = channels * width;

    metalTexture->texture->replaceRegion(region, 0, textureBuffer->getPixels().data(), bytesPerRow);

    textureAsset->setGPURef((void*)metalTexture->texture.get());
    metalTexture->textureAsset = textureAsset;

    _textureMap.insert({ textureAsset->getFile()->getPath(), std::move(metalTexture) });
}

XPProfilable void
XPMetalRenderer::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
    if (!meshAsset->getFile()->hasChanges()) { return; }

    _registry->getEngine()->scheduleRenderTask([this, meshAsset]() {
        XPMetalFramesInFlightLock lock({ _renderingCommandQueueSemaphore });

        XPMeshBuffer* meshBuffer   = meshAsset->getMeshBuffer();
        XPMetalMesh*  metalMeshRef = _meshMap[meshAsset->getFile()->getPath()].get();

        size_t positionsCount = meshBuffer->getPositionsCount();
        size_t normalsCount   = meshBuffer->getNormalsCount();
        size_t texcoordsCount = meshBuffer->getTexcoordsCount();
        size_t indicesCount   = meshBuffer->getIndicesCount();

        const XPVec4<float>* positions = meshBuffer->getPositions();
        const XPVec4<float>* normals   = meshBuffer->getNormals();
        const XPVec4<float>* texcoords = meshBuffer->getTexcoords();
        const uint32_t*      indices   = meshBuffer->getIndices();

        const uint64_t vertexBufferSize   = static_cast<uint64_t>(positionsCount * XPMeshBuffer::sizeofPositionsType());
        const uint64_t normalBufferSize   = static_cast<uint64_t>(normalsCount * XPMeshBuffer::sizeofNormalsType());
        const uint64_t texcoordBufferSize = static_cast<uint64_t>(texcoordsCount * XPMeshBuffer::sizeofTexcoordsType());
        const uint64_t indexBufferSize    = static_cast<uint64_t>(indicesCount * XPMeshBuffer::sizeofIndicesType());

        auto tmpVertexBuffer = _device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
        auto tmpNormalBuffer = _device->newBuffer(normalBufferSize, MTL::ResourceStorageModeShared);
        auto tmpUvBuffer     = _device->newBuffer(texcoordBufferSize, MTL::ResourceStorageModeShared);
        auto tmpIndexBuffer  = _device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);

        memcpy(tmpVertexBuffer->contents(), positions, vertexBufferSize);
        memcpy(tmpNormalBuffer->contents(), normals, normalBufferSize);
        memcpy(tmpUvBuffer->contents(), texcoords, texcoordBufferSize);
        memcpy(tmpIndexBuffer->contents(), indices, indexBufferSize);

        if (vertexBufferSize != metalMeshRef->vertexBuffer->allocatedSize()) {
            metalMeshRef->vertexBuffer =
              NS::TransferPtr(_device->newBuffer(vertexBufferSize, MTL::ResourceStorageModePrivate));
        }
        if (normalBufferSize != metalMeshRef->normalBuffer->allocatedSize()) {
            metalMeshRef->normalBuffer =
              NS::TransferPtr(_device->newBuffer(normalBufferSize, MTL::ResourceStorageModePrivate));
        }
        if (texcoordBufferSize != metalMeshRef->uvBuffer->allocatedSize()) {
            metalMeshRef->uvBuffer =
              NS::TransferPtr(_device->newBuffer(texcoordBufferSize, MTL::ResourceStorageModePrivate));
        }
        if (indexBufferSize != metalMeshRef->indexBuffer->allocatedSize()) {
            metalMeshRef->indexBuffer =
              NS::TransferPtr(_device->newBuffer(indexBufferSize, MTL::ResourceStorageModePrivate));
        }

        MTL::CommandBuffer*      commandBuffer      = _ioCommandQueue->commandBuffer();
        MTL::BlitCommandEncoder* blitCommandEncoder = commandBuffer->blitCommandEncoder();
        {
            // copy shared to private buffers
            blitCommandEncoder->copyFromBuffer(
              tmpVertexBuffer, 0, metalMeshRef->vertexBuffer.get(), 0, vertexBufferSize);
            blitCommandEncoder->copyFromBuffer(
              tmpNormalBuffer, 0, metalMeshRef->normalBuffer.get(), 0, normalBufferSize);
            blitCommandEncoder->copyFromBuffer(tmpUvBuffer, 0, metalMeshRef->uvBuffer.get(), 0, texcoordBufferSize);
            blitCommandEncoder->copyFromBuffer(tmpIndexBuffer, 0, metalMeshRef->indexBuffer.get(), 0, indexBufferSize);
        }
        blitCommandEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();

        // release tmp buffers
        tmpVertexBuffer->release();
        tmpNormalBuffer->release();
        tmpUvBuffer->release();
        tmpIndexBuffer->release();

        metalMeshRef->meshAsset = meshAsset;
        metalMeshRef->objects.clear();

        for (size_t mboi = 0; mboi < meshBuffer->getObjectsCount(); ++mboi) {
            XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(mboi);
            auto                metalMeshObject =
              std::make_unique<XPMetalMeshObject>((XPMetalMesh&)*metalMeshRef, meshBufferObject.boundingBox);
            metalMeshObject->name         = meshBufferObject.name;
            metalMeshObject->vertexOffset = meshBufferObject.vertexOffset;
            metalMeshObject->indexOffset  = meshBufferObject.indexOffset;
            metalMeshObject->numIndices   = meshBufferObject.numIndices;
            metalMeshRef->objects.push_back(*metalMeshObject.get());
            _meshObjectMap[metalMeshObject->name] = std::move(metalMeshObject);
        }

        _registry->getScene()->addAttachmentChanges(XPEInteractionHasMeshRendererChanges, false);
    });
}

XPProfilable void
XPMetalRenderer::reUploadShaderAsset(XPShaderAsset* shaderAsset)
{
    if (!shaderAsset->getFile()->hasChanges()) { return; }

    _registry->getEngine()->scheduleRenderTask([this]() {
        XPMetalFramesInFlightLock lock({ _renderingCommandQueueSemaphore });

        destroyFramebufferTexture(*_framePipeline->mBuffer->frameBuffer.get());
        destroyFramebufferTexture(*_framePipeline->gBuffer->frameBuffer.get());
        destroyRenderPipeline(*_renderPipelines["MBuffer"].get());
        destroyRenderPipeline(*_renderPipelines["GBuffer"].get());
        destroyComputePipeline(*_computePipelines["MainCompute"].get());

        // main framebuffer
        {
            XPMetalRenderPipelineCreationInfo mBufferCreationInfo(
              "MBuffer",
              mBufferVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction |
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_EnableDepthWrite),
              { MTL::PixelFormatBGRA8Unorm_sRGB },
              MTL::PixelFormatDepth32Float,
              MTL::PixelFormatStencil8,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/MBuffer.metal");
                createRenderPipeline(*_renderPipelines["MBuffer"].get(), mBufferCreationInfo);
            }
            {
                createFramebufferTexture(*_framePipeline->mBuffer->frameBuffer.get(), mBufferCreationInfo);
                _framePipeline->mBuffer->frameBuffer->clearColor = MTL::ClearColor(XP_DEFAULT_CLEAR_COLOR);
                _framePipeline->mBuffer->renderPipeline          = _renderPipelines["MBuffer"].get();
            }
        }

        // GBuffer
        {
            XPMetalRenderPipelineCreationInfo gBufferCreationInfo(
              "GBuffer",
              gBufferVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction |
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_EnableDepthWrite),
              { MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatRGBA32Float },
              MTL::PixelFormatDepth32Float,
              MTL::PixelFormatStencil8,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                // -----------------
                // COLOR ATTACHMENTS
                // -----------------
                // float4 positionU;
                // float4 normalV;
                // float4 albedo;
                // float4 metallicRoughnessAmbientObjectId;
                // ------------------------------------------
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/GBuffer.metal");
                createRenderPipeline(*_renderPipelines["GBuffer"], gBufferCreationInfo);
            }
            {
                createFramebufferTexture(*_framePipeline->gBuffer->frameBuffer.get(), gBufferCreationInfo);
                _framePipeline->gBuffer->frameBuffer->clearColor = MTL::ClearColor(XP_DEFAULT_CLEAR_COLOR);
                _framePipeline->gBuffer->renderPipeline          = _renderPipelines["GBuffer"].get();
            }
        }

        // line rendering
        {
            XPMetalRenderPipelineCreationInfo lineCreationInfo(
              "Line",
              lineVertexDescriptorLayout(),
              static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_CompileShaderFromSource |
#endif
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_IncludeFragmentFunction |
                XPMetalRenderPipelineCreationInfo::RenderPipelineFlags_EnableDepthWrite),
              { MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatBGRA8Unorm_sRGB,
                MTL::PixelFormatRGBA32Float },
              MTL::PixelFormatDepth32Float,
              MTL::PixelFormatStencil8,
              MTL::CompareFunction::CompareFunctionLess,
              1);
            {
                auto shaderFilePath = XPFS::getExecutableDirectoryPath().append("shaders/Line.metal");
                auto renderPipeline = std::make_unique<XPMetalRenderPipeline>();
                createRenderPipeline(*renderPipeline, lineCreationInfo);
                _renderPipelines["Line"] = std::move(renderPipeline);
            }
        }

        // bounding box transform compute
        {
            XPMetalComputePipelineCreationInfo creationInfo(
              "MainCompute",
              { 0, 0, 0, 0 },
              static_cast<XPMetalComputePipelineCreationInfo::ComputePipelineFlags>(
#ifdef XP_METAL_RENDERER_USE_SHADERS_FROM_SOURCE
                XPMetalComputePipelineCreationInfo::ComputePipelineFlags_CompileShaderFromSource |
#endif
                XPMetalComputePipelineCreationInfo::ComputePipelineFlags_None));
            {
                _computePipelines["MainCompute"] = std::make_unique<XPMetalComputePipeline>();
                createComputePipeline(*_computePipelines["MainCompute"].get(), creationInfo);
            }
        }

        _registry->getScene()->addAttachmentChanges(XPEInteractionHasMeshRendererChanges, false);
    });
}

XPProfilable void
XPMetalRenderer::reUploadTextureAsset(XPTextureAsset* textureAsset)
{
    if (!textureAsset->getFile()->hasChanges()) { return; }

    _registry->getEngine()->scheduleRenderTask([this, textureAsset]() {
        XPMetalFramesInFlightLock lock({ _renderingCommandQueueSemaphore });

        XPTextureBuffer* textureBuffer   = textureAsset->getTextureBuffer();
        XPMetalTexture*  metalTextureRef = _textureMap[textureAsset->getFile()->getPath()].get();

        int width    = textureBuffer->getDimensions().x;
        int height   = textureBuffer->getDimensions().y;
        int channels = 0;

        metalTextureRef->textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
        metalTextureRef->textureDescriptor->setWidth(width);
        metalTextureRef->textureDescriptor->setHeight(height);
        switch (textureBuffer->getFormat()) {
            case XPETextureBufferFormat::R8: {
                metalTextureRef->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatR8Unorm);
                channels = 1;
            } break;
            case XPETextureBufferFormat::R8_G8: {
                metalTextureRef->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRG8Unorm);
                channels = 2;
            } break;
            case XPETextureBufferFormat::R8_G8_B8_A8: {
                metalTextureRef->textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
                channels = 4;
            } break;
            default: XP_LOG(XPLoggerSeverityFatal, "Couldn't find a corresponding texture format to this"); break;
        }
        metalTextureRef->textureDescriptor->setStorageMode(MTL::StorageModeShared);
        metalTextureRef->textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

        metalTextureRef->texture = NS::TransferPtr(_device->newTexture(metalTextureRef->textureDescriptor.get()));

        // fill data
        MTL::Region  region      = MTL::Region(0, 0, 0, width, height, 1);
        NS::UInteger bytesPerRow = channels * width;

        metalTextureRef->texture->replaceRegion(region, 0, textureBuffer->getPixels().data(), bytesPerRow);

        textureAsset->setGPURef((void*)metalTextureRef->texture.get());

        _registry->getScene()->addAttachmentChanges(XPEInteractionHasMeshRendererChanges, false);
    });
}

XPRegistry*
XPMetalRenderer::getRegistry()
{
    return _registry;
}

XPProfilable void
XPMetalRenderer::scheduleReadPixelFromTexture(MTL::Texture* texture, std::shared_ptr<PickBlitCommandInfo> blitCmdInfo)
{
    MTL::Origin readOrigin =
      MTL::Origin(static_cast<NS::UInteger>(blitCmdInfo->normalizedCoordinates.x * texture->width()),
                  static_cast<NS::UInteger>(blitCmdInfo->normalizedCoordinates.y * texture->height()),
                  0);
    MTL::Size readSize = MTL::Size(1, 1, 1);

    MTL::PixelFormat pixelFormat(texture->pixelFormat());
    if (pixelFormat != MTL::PixelFormatRGBA32Float) {
        XP_LOGV(XPLoggerSeverityFatal,
                "%s:%i %s",
                __FILE__,
                __LINE__,
                fmt::format("Unsupported pixel format: 0x{}.", (uint32_t)pixelFormat).c_str());
        return;
    }

    // assuming we have a RGBA texture ...
    NS::UInteger bytesPerPixel = 4 * sizeof(float);
    NS::UInteger bytesPerRow   = 1 * bytesPerPixel;
    NS::UInteger bytesPerImage = 1 * bytesPerRow;

    MTL::Buffer* readBuffer = _device->newBuffer(bytesPerImage, MTL::ResourceStorageModeShared);

    if (!readBuffer) {
        XP_LOGV(XPLoggerSeverityFatal,
                "%s:%i %s",
                __FILE__,
                __LINE__,
                fmt::format("Failed to create buffer for {} bytes.", bytesPerImage).c_str());
        return;
    }

    // wait for gpu to finish the ongoing rendering work
    // note that we have to wait for all the in flight frames to finish this time
    //    dispatch_semaphore_wait(_inFlightSemaphore, DISPATCH_TIME_FOREVER);

    // Copy the pixel data of the selected region to a Metal buffer with a shared
    // storage mode, which makes the buffer accessible to the CPU.
    MTL::CommandBuffer*      ioCommandBuffer = _ioCommandQueue->commandBuffer();
    MTL::BlitCommandEncoder* blitEncoder     = ioCommandBuffer->blitCommandEncoder();

    blitEncoder->copyFromTexture(texture,      // source texture
                                 0,            // source slice
                                 0,            // source level
                                 readOrigin,   // source origin
                                 readSize,     // source size
                                 readBuffer,   // dst buffer
                                 0,            // dst offset
                                 bytesPerRow,  // dst bytes per row
                                 bytesPerImage // dst bytes per image
    );

    blitEncoder->endEncoding();
    ioCommandBuffer->addCompletedHandler([this, readBuffer, blitCmdInfo](MTL::CommandBuffer*) {
        std::array<float, 4>* pixels = (std::array<float, 4>*)readBuffer->contents();
        uint32_t              nodeId = pixels->at(3);
        readBuffer->release();

        // note: we are doing nodeId-1 in the gbuffer shader to eliminate selection of none (skybox)
        // that's why we need to do nodeId - 1 and assume UINT32_MAX as clicking on nothing.
        if (nodeId == UINT32_MAX) {
            blitCmdInfo->cbfn(nullptr);
            return;
        }
        auto optNode = _registry->getScene()->getSceneStore()->getNode(nodeId - 1);
        if (optNode.has_value()) {
            blitCmdInfo->cbfn(optNode.value());
            return;
        }
        blitCmdInfo->cbfn(nullptr);
    });
    ioCommandBuffer->commit();
}

XPProfilable void
XPMetalRenderer::getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>& cbfn)
{
    scheduleReadPixelFromTexture(
      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexMetallicRoughnessAmbientObjectId]
        .texture.get(),
      std::make_shared<PickBlitCommandInfo>(coordinates, XPVec2<size_t>(1, 1), cbfn));
}

XPProfilable void*
XPMetalRenderer::getMainTexture()
{
    return _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexMetallicRoughnessAmbientObjectId]
      .texture.get();
}

XPVec2<int>
XPMetalRenderer::getWindowSize()
{
    return _window->getWindowSize();
}

XPVec2<int>
XPMetalRenderer::getResolution()
{
    return _resolution;
}

XPVec2<float>
XPMetalRenderer::getMouseLocation()
{
    return _window->getMouseLocation();
}

XPVec2<float>
XPMetalRenderer::getNormalizedMouseLocation()
{
    return _window->getNormalizedMouseLocation();
}

bool
XPMetalRenderer::isLeftMouseButtonPressed()
{
    return _window->isLeftMouseButtonPressed();
}

bool
XPMetalRenderer::isMiddleMouseButtonPressed()
{
    return _window->isMiddleMouseButtonPressed();
}

bool
XPMetalRenderer::isRightMouseButtonPressed()
{
    return _window->isRightMouseButtonPressed();
}

float
XPMetalRenderer::getDeltaTime()
{
    return _window->getDeltaTimeSeconds();
}

uint32_t
XPMetalRenderer::getNumDrawCallsVertices()
{
    return _gpuData->numDrawCallsVertices;
}

uint32_t
XPMetalRenderer::getTotalNumDrawCallsVertices()
{
    return _gpuData->numTotalDrawCallsVertices;
}

uint32_t
XPMetalRenderer::getNumDrawCalls()
{
    return _gpuData->numDrawCalls;
}

uint32_t
XPMetalRenderer::getTotalNumDrawCalls()
{
    return _gpuData->numTotalDrawCalls;
}

bool
XPMetalRenderer::isCapturingDebugFrames()
{
    return _isCapturingDebugFrames;
}

bool
XPMetalRenderer::isFramebufferResized()
{
    return false;
}

float
XPMetalRenderer::getRenderingGPUTime()
{
    return _renderingGpuTime;
}

float
XPMetalRenderer::getUIGPUTime()
{
    return _uiGpuTime;
}

float
XPMetalRenderer::getComputeGPUTime()
{
    return _computeGpuTime;
}

void
XPMetalRenderer::captureDebugFrames()
{
    _isCapturingDebugFrames = true;
}

void
XPMetalRenderer::setFramebufferResized()
{
}

void
XPMetalRenderer::simulateCopy(const char* text)
{
#ifdef XP_EDITOR_MODE
    _registry->getUI()->simulateCopy(text);
#else
    _window->simulateCopy(text);
#endif
}

std::string
XPMetalRenderer::simulatePaste()
{
#ifdef XP_EDITOR_MODE
    return _registry->getUI()->simulatePaste();
#else
    return _window->simulatePaste();
#endif
}

XPMetalWindow*
XPMetalRenderer::getWindow()
{
    return _window.get();
}

MTL::Device*
XPMetalRenderer::getDevice()
{
    return _device;
}

CA::MetalLayer*
XPMetalRenderer::getLayer()
{
    return _layer;
}

CA::MetalDrawable*
XPMetalRenderer::getDrawable()
{
    return _drawable;
}

MTL::CommandQueue*
XPMetalRenderer::getIOCommandQueue()
{
    return _ioCommandQueue;
}

MTL::CommandQueue*
XPMetalRenderer::getRenderingCommandQueue()
{
    return _renderingCommandQueue;
}

MTL::CommandBuffer*
XPMetalRenderer::getRenderingCommandBuffer()
{
    return _renderingCommandBuffer;
}

void
XPMetalRenderer::setDevice(MTL::Device* device)
{
    _device = device;
}

void
XPMetalRenderer::setLayer(CA::MetalLayer* layer)
{
    _layer = layer;
}

void
XPMetalRenderer::setDrawable(CA::MetalDrawable* drawable)
{
    _drawable = drawable;
}

XPProfilable NS::SharedPtr<MTL::VertexDescriptor>
             XPMetalRenderer::gBufferVertexDescriptorLayout()
{
    auto vertexDescriptor = NS::TransferPtr(MTL::VertexDescriptor::alloc()->init());
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(XPVertexInputIndexPositions);
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(XPVec4<float>));

    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(1)->setOffset(0);
    vertexDescriptor->attributes()->object(1)->setBufferIndex(XPVertexInputIndexNormals);
    vertexDescriptor->layouts()->object(1)->setStride(sizeof(XPVec4<float>));

    vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(2)->setOffset(0);
    vertexDescriptor->attributes()->object(2)->setBufferIndex(XPVertexInputIndexUvs);
    vertexDescriptor->layouts()->object(2)->setStride(sizeof(XPVec4<float>));

    return vertexDescriptor;
}

XPProfilable NS::SharedPtr<MTL::VertexDescriptor>
             XPMetalRenderer::lineVertexDescriptorLayout()
{
    auto vertexDescriptor = NS::TransferPtr(MTL::VertexDescriptor::alloc()->init());
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(XPVertexInputIndexPositions);
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(XPVec4<float>));

    return vertexDescriptor;
}

XPProfilable NS::SharedPtr<MTL::VertexDescriptor>
             XPMetalRenderer::mBufferVertexDescriptorLayout()
{
    auto vertexDescriptor = NS::TransferPtr(MTL::VertexDescriptor::alloc()->init());
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(XPVertexInputIndexPositions);
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(XPVec4<float>));

    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(1)->setOffset(0);
    vertexDescriptor->attributes()->object(1)->setBufferIndex(XPVertexInputIndexNormals);
    vertexDescriptor->layouts()->object(1)->setStride(sizeof(XPVec4<float>));

    vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormat::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(2)->setOffset(0);
    vertexDescriptor->attributes()->object(2)->setBufferIndex(XPVertexInputIndexUvs);
    vertexDescriptor->layouts()->object(2)->setStride(sizeof(XPVec4<float>));

    return vertexDescriptor;
}

XPProfilable void
XPMetalRenderer::compileLoadScene(XPScene& scene)
{
    auto& meshNodes = scene.getNodes(MeshRendererAttachmentDescriptor | TransformAttachmentDescriptor);

    if (scene.hasOnlyAttachmentChanges(XPEInteractionHasTransformChanges)) {
        if (_gpuData->numMeshNodes == meshNodes.size()) {
            for (const auto& meshNode : meshNodes) {
                Transform* transform  = meshNode->getTransform();
                auto       operations = static_cast<XPMat4<float>::ModelMatrixOperations>(
                  XPMat4<float>::ModelMatrixOperation_Translation | XPMat4<float>::ModelMatrixOperation_Rotation |
                  XPMat4<float>::ModelMatrixOperation_Scale);
                XPMat4<float>::buildModelMatrix(
                  transform->modelMatrix, transform->location, transform->euler, transform->scale, operations);
                _gpuData->modelMatrices[_gpuData->numMeshNodes] = transform->modelMatrix;
            }
            return;
        }
    }

    XPMetalComputePipeline& computePipeline = *_computePipelines["MainCompute"].get();

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
            XPMetalMeshObject* meshObject                        = _meshObjectMap[subMesh.mesh.text].get();
            _gpuData->meshObjects[_gpuData->numSubMeshes]        = meshObject;
            _gpuData->boundingBoxes[_gpuData->numSubMeshes]      = meshObject->boundingBox;
            _gpuData->perMeshNodeIndices[_gpuData->numSubMeshes] = _gpuData->numMeshNodes;
            ++_gpuData->numSubMeshes;
            ++_gpuData->numTotalDrawCalls;
            _gpuData->numTotalDrawCallsVertices += meshObject->numIndices;
        }
        ++_gpuData->numMeshNodes;
    }

    // compute dispatch related arguments
    _gpuData->gridSize           = MTL::Size(_gpuData->numSubMeshes, 1, 1);
    NS::UInteger threadGroupSize = computePipeline.state->maxTotalThreadsPerThreadgroup();
    if (threadGroupSize > _gpuData->numSubMeshes) { threadGroupSize = _gpuData->numSubMeshes; }
    _gpuData->threadsPerThreadgroup = MTL::Size(threadGroupSize, 1, 1);

    // ------------------------------------------------------------------------------------------------------------------------
    // compute related gpu buffers, resize them and make them ready for UpdateGPUScene
    // ------------------------------------------------------------------------------------------------------------------------
    if (!computePipeline.buffers[0]) {
        computePipeline.buffers[0] =
          _device->newBuffer(NS::UInteger(sizeof(float4x4) * _gpuData->numMeshNodes), MTL::ResourceStorageModeShared);
        computePipeline.buffers[0]->setLabel(NS_STRING_FROM_CSTRING("modelMatrices buffer"));
    }
    if (!computePipeline.buffers[1]) {
        computePipeline.buffers[1] = _device->newBuffer(NS::UInteger(sizeof(XPGPUBoundingBox) * _gpuData->numSubMeshes),
                                                        MTL::ResourceStorageModeShared);
        computePipeline.buffers[1]->setLabel(NS_STRING_FROM_CSTRING("boundingBoxes buffer"));
    }
    if (!computePipeline.buffers[2]) {
        computePipeline.buffers[2] =
          _device->newBuffer(NS::UInteger(sizeof(uint) * _gpuData->numSubMeshes), MTL::ResourceStorageModeShared);
        computePipeline.buffers[2]->setLabel(NS_STRING_FROM_CSTRING("modelMatricesIndices buffer"));
    }
    if (!computePipeline.buffers[3]) {
        computePipeline.buffers[3] =
          _device->newBuffer(NS::UInteger(sizeof(XPFrameDataPerObject)), MTL::ResourceStorageModeShared);
        computePipeline.buffers[3]->setLabel(NS_STRING_FROM_CSTRING("frameDataPerObject buffer"));
    }

    if (computePipeline.buffers[0]->length() != sizeof(float4x4) * _gpuData->numMeshNodes) {
        computePipeline.buffers[0]->release();
        computePipeline.buffers[0] =
          _device->newBuffer(NS::UInteger(sizeof(float4x4) * _gpuData->numMeshNodes), MTL::ResourceStorageModeShared);
    }
    if (computePipeline.buffers[1]->length() != sizeof(XPGPUBoundingBox) * _gpuData->numSubMeshes) {
        computePipeline.buffers[1]->release();
        computePipeline.buffers[1] = _device->newBuffer(NS::UInteger(sizeof(XPGPUBoundingBox) * _gpuData->numSubMeshes),
                                                        MTL::ResourceStorageModeShared);
    }
    if (computePipeline.buffers[2]->length() != sizeof(uint) * _gpuData->numSubMeshes) {
        computePipeline.buffers[2]->release();
        computePipeline.buffers[2] =
          _device->newBuffer(NS::UInteger(sizeof(uint) * _gpuData->numSubMeshes), MTL::ResourceStorageModeShared);
    }
    if (computePipeline.buffers[3]->length() != sizeof(XPFrameDataPerObject)) {
        computePipeline.buffers[3]->release();
        computePipeline.buffers[3] =
          _device->newBuffer(NS::UInteger(sizeof(XPFrameDataPerObject)), MTL::ResourceStorageModeShared);
    }

    memcpy(
      computePipeline.buffers[0]->contents(), _gpuData->modelMatrices.data(), computePipeline.buffers[0]->length());
    memcpy(
      computePipeline.buffers[1]->contents(), _gpuData->boundingBoxes.data(), computePipeline.buffers[1]->length());
    memcpy(computePipeline.buffers[2]->contents(),
           _gpuData->perMeshNodeIndices.data(),
           computePipeline.buffers[2]->length());
    memcpy(computePipeline.buffers[3]->contents(),
           &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject,
           computePipeline.buffers[3]->length());
    // ------------------------------------------------------------------------------------------------------------------------
}

XPProfilable void
XPMetalRenderer::loadBuiltinShaders()
{
}

XPProfilable void
XPMetalRenderer::updateGPUScene(XPScene& scene, FreeCamera& camera)
{
    XP_UNUSED(scene)
    XP_UNUSED(camera)

    constexpr size_t csCoordsSize           = 8;
    glm::vec4        csCoords[csCoordsSize] = {
        // clang-format off
        glm::vec4( -1.0f, -1.0f, -1.0f, 1.0f ),
        glm::vec4( -1.0f, 1.0f, -1.0f, 1.0f ),
        glm::vec4( 1.0f, 1.0f, -1.0f, 1.0f ),
        glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f ),

        glm::vec4( -1.0f, -1.0f, 1.0f, 1.0f ),
        glm::vec4( -1.0f, 1.0f, 1.0f, 1.0f ),
        glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
        glm::vec4( 1.0f, -1.0f, 1.0f, 1.0f)
        // clang-format on
    };

    for (size_t i = 0; i < csCoordsSize; ++i) {
        glm::vec4 corner =
          _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewProjectionMatrix.glm * csCoords[i];
        _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[i].x = corner.x / corner.w;
        _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[i].y = corner.y / corner.w;
        _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[i].z = corner.z / corner.w;
    }

    glm::mat4 m = glm::transpose(_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.viewProjectionMatrix.glm);
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Left]   = m[3] + m[0];
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Right]  = m[3] - m[0];
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Bottom] = m[3] + m[1];
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Top]    = m[3] - m[1];
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Near]   = m[3] + m[2];
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPlanes[Far]    = m[3] - m[2];

    XPMetalComputePipeline& computePipeline = *_computePipelines["MainCompute"].get();

    assert(computePipeline.buffers[3]->length() ==
             sizeof(_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject) &&
           computePipeline.buffers[3]->allocatedSize() ==
             sizeof(_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject));

    //    memcpy(
    //      computePipeline.buffers[1]->contents(), _gpuData->boundingBoxes.data(),
    //      computePipeline.buffers[1]->length());
    memcpy(computePipeline.buffers[3]->contents(),
           &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject,
           computePipeline.buffers[3]->length());

    _computeCommandBuffer = _computeCommandQueue->commandBuffer();

    MTL::ComputeCommandEncoder* encoder = _computeCommandBuffer->computeCommandEncoder();

    encoder->setComputePipelineState(computePipeline.state);

    // TODO: IMPORTANT: WE ALREADY UPLOADED THE DATA TO GPU.
    // WE DO THAT ONLY WHEN SCENE HAS CHANGES TO REDUCE LATENCY AND GET AROUND BUGS RELATED TO SYNCHRONIZATION
    // THIS IS THE CASE BECAUSE WE BASICALLY FEEDBACK THE RESULT TO THE SAME BUFFERS ANYWAYS ...

    encoder->setBuffer(computePipeline.buffers[0], NS::UInteger(0), NS::UInteger(0));
    encoder->setBuffer(computePipeline.buffers[1], NS::UInteger(0), NS::UInteger(1));
    encoder->setBuffer(computePipeline.buffers[2], NS::UInteger(0), NS::UInteger(2));
    encoder->setBuffer(computePipeline.buffers[3], NS::UInteger(0), NS::UInteger(3));

    encoder->dispatchThreads(_gpuData->gridSize, _gpuData->threadsPerThreadgroup);

    // Calculate the maximum threads per threadgroup based on the thread execution width.
    // NS::UInteger w                     = computePipeline.state->threadExecutionWidth();
    // NS::UInteger h                     = computePipeline.state->maxTotalThreadsPerThreadgroup() / w;
    // MTL::Size    threadsPerThreadgroup = MTL::Size(w, 1, 1);
    // MTL::Size    threadgroupsPerGrid   = MTL::Size((totalNumberOfSubMeshes + w - 1) / w, 1, 1);
    // encoder->dispatchThreadgroups(threadgroupsPerGrid, threadsPerThreadgroup);

    encoder->endEncoding();

    _computeCommandBuffer->commit();

    _computeCommandBuffer->waitUntilCompleted();

    _computeGpuTime =
      (((double)_computeCommandBuffer->GPUEndTime() - (double)_computeCommandBuffer->GPUStartTime()) * 1000.0);

    // copy calculated values from GPU to CPU
    //    memcpy(
    //      _gpuData->boundingBoxes.data(), computePipeline.buffers[1]->contents(),
    //      computePipeline.buffers[1]->length());

    memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject,
           computePipeline.buffers[3]->contents(),
           computePipeline.buffers[3]->length());

    // copy calculated values back to the meshObjects
    for (size_t i = 0; i < _gpuData->numSubMeshes; ++i) {
        memcpy(&_gpuData->meshObjects[i]->boundingBox,
               static_cast<char*>(computePipeline.buffers[1]->contents()) + i * sizeof(XPBoundingBox),
               sizeof(XPBoundingBox));
    }

    XPConsole* console = _registry->getEngine()->getConsole();
    if (console->getVariableValue<bool>("r.freeze") == true) {
        // make sure the active camera is being used now
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.viewProjectionMatrix._00,
               &camera.activeProperties.viewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewProjectionMatrix._00,
               &camera.activeProperties.inverseViewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewMatrix._00,
               &camera.activeProperties.inverseViewMatrix._00,
               sizeof(XPMat4<float>));
    }
}

XPProfilable void
XPMetalRenderer::updateViewport(XPScene& scene, FreeCamera& camera, MTL::Viewport& viewport)
{
    auto        console             = _registry->getEngine()->getConsole();
    float       scale               = console->getVariableValue<float>("r.scale");
    XPVec2<int> presentResolution   = getWindowSize();
    XPVec2<int> renderingResolution = getWindowSize();
    renderingResolution.x           = presentResolution.x * scale;
    renderingResolution.y           = presentResolution.y * scale;

    if (renderingResolution.x <= 0 || renderingResolution.y <= 0 || renderingResolution.x == _resolution.x ||
        renderingResolution.y == _resolution.y) {
        return;
    }

    _resolution = renderingResolution;
    recreateFramebufferTextures();
    for (auto cameraNode : _registry->getScene()->getNodes(FreeCameraAttachmentDescriptor)) {
        FreeCamera* freeCamera              = cameraNode->getFreeCamera();
        freeCamera->activeProperties.width  = _resolution.x;
        freeCamera->activeProperties.height = _resolution.y;
    }
}

XPProfilable void
XPMetalRenderer::updateMatrices(XPScene& scene, FreeCamera& camera, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(camera)
    XP_UNUSED(deltaTime)

    XPConsole* console = _registry->getEngine()->getConsole();
    if (console->getVariableValue<bool>("r.freeze") == false) {
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.viewProjectionMatrix._00,
               &camera.activeProperties.viewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewProjectionMatrix._00,
               &camera.activeProperties.inverseViewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewMatrix._00,
               &camera.activeProperties.inverseViewMatrix._00,
               sizeof(XPMat4<float>));
    } else {
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.viewProjectionMatrix._00,
               &camera.frozenProperties.viewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewProjectionMatrix._00,
               &camera.frozenProperties.inverseViewProjectionMatrix._00,
               sizeof(XPMat4<float>));
        memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.inverseViewMatrix._00,
               &camera.frozenProperties.inverseViewMatrix._00,
               sizeof(XPMat4<float>));
    }

    //    _frameDataPerObject[_frameDataIndex].time  = _window->getTime();
    _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.flags =
      static_cast<uint>(XPFragmentDataSourceFlags_None);
}

XPProfilable void
XPMetalRenderer::renderGBuffer(MTL::CommandBuffer*  commandBuffer,
                               XPScene&             scene,
                               FreeCamera&          camera,
                               float                deltaTime,
                               const MTL::Viewport& viewport)
{
    XP_UNUSED(scene)
    XP_UNUSED(deltaTime)

    commandBuffer->setLabel(NS_STRING_FROM_CSTRING("MainCommandBuffer"));

    for (size_t i = 0; i < _framePipeline->gBuffer->frameBuffer->colorAttachments.size(); ++i) {
        _framePipeline->gBuffer->frameBuffer->passDescriptor->colorAttachments()->object(i)->setClearColor(
          _framePipeline->gBuffer->frameBuffer->clearColor);
        _framePipeline->gBuffer->frameBuffer->passDescriptor->colorAttachments()->object(i)->setLoadAction(
          MTL::LoadActionClear);
        _framePipeline->gBuffer->frameBuffer->passDescriptor->colorAttachments()->object(i)->setStoreAction(
          MTL::StoreActionStore);
        _framePipeline->gBuffer->frameBuffer->passDescriptor->colorAttachments()->object(i)->setTexture(
          _framePipeline->gBuffer->frameBuffer->colorAttachments[i].texture.get());
    }
    // TODO: depth attachment might actually be not used or even created !
    _framePipeline->gBuffer->frameBuffer->passDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionDontCare);
    _framePipeline->gBuffer->frameBuffer->passDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
    _framePipeline->gBuffer->frameBuffer->passDescriptor->depthAttachment()->setTexture(
      _framePipeline->gBuffer->frameBuffer->depthAttachment.texture.get());

    auto encoder = commandBuffer->renderCommandEncoder(_framePipeline->gBuffer->frameBuffer->passDescriptor);
    encoder->setLabel(NS_STRING_FROM_CSTRING("GBuffer"));
    encoder->pushDebugGroup(NS_STRING_FROM_CSTRING("GBuffer"));
    encoder->setViewport(viewport);
    encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    encoder->setRenderPipelineState(_framePipeline->gBuffer->renderPipeline->renderPipelineState);
    encoder->setDepthStencilState(_framePipeline->gBuffer->renderPipeline->depthStencilState);

    auto console               = _registry->getEngine()->getConsole();
    bool frustumCullingEnabled = console->getVariableValue<bool>("r.frustumCulling");

    _gpuData->numDrawCallsVertices = 0;
    _gpuData->numDrawCalls         = 0;
    for (size_t subMeshIndex = 0; subMeshIndex < _gpuData->numSubMeshes; ++subMeshIndex) {
        XPMetalMeshObject& meshObject = *_gpuData->meshObjects[subMeshIndex];
        if ((!frustumCullingEnabled ||
             (frustumCullingEnabled && _gpuData->meshObjects[subMeshIndex]->boundingBox.minPoint.w == 1))
#if !defined(XP_EDITOR_MODE)
            || true
#endif
        ) {

            memcpy(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.modelMatrix._00,
                   &_gpuData->modelMatrices[_gpuData->perMeshNodeIndices[subMeshIndex]]._00,
                   sizeof(XPMat4<float>));

            glm::vec4& albedo =
              *reinterpret_cast<glm::vec4*>(&_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.albedo);
            albedo.x = 1.0;
            albedo.y = 1.0;
            albedo.z = 1.0;
            albedo.w = 1.0;

            _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.metallic  = 0.2;
            _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.roughness = 0.7;
            _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.flags =
              static_cast<uint>(XPFragmentDataSourceFlags_None);
            _vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.objectId =
              _gpuData->meshNodesIds[_gpuData->perMeshNodeIndices[subMeshIndex]];

            encoder->setVertexBytes(&_vertexShaderArgBuffer0[_frameDataIndex],
                                    sizeof(XPVertexShaderArgumentBuffer),
                                    GBufferVertexIn_VertexShaderArgumentBuffer0Index);

            encoder->setVertexBuffer(meshObject.mesh.vertexBuffer.get(),
                                     XPMeshBuffer::sizeofPositionsType() * meshObject.vertexOffset,
                                     XPVertexInputIndexPositions);
            encoder->setVertexBuffer(meshObject.mesh.normalBuffer.get(),
                                     XPMeshBuffer::sizeofNormalsType() * meshObject.vertexOffset,
                                     XPVertexInputIndexNormals);
            encoder->setVertexBuffer(meshObject.mesh.uvBuffer.get(),
                                     XPMeshBuffer::sizeofTexcoordsType() * meshObject.vertexOffset,
                                     XPVertexInputIndexUvs);
            encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                           NS::UInteger(meshObject.numIndices),
                                           MTL::IndexTypeUInt32,
                                           meshObject.mesh.indexBuffer.get(),
                                           NS::UInteger(XPMeshBuffer::sizeofIndicesType() * meshObject.indexOffset));
            ++_gpuData->numDrawCalls;
            _gpuData->numDrawCallsVertices += meshObject.numIndices;
        }
    }
    {
        _lineRenderer->lines.clear();
        {
            auto console = _registry->getEngine()->getConsole();
            // render bounding boxes
            if (console->getVariableValue<bool>("r.bb")) {
                _lineRenderer->lines.reserve(_gpuData->numSubMeshes * 24);
                for (size_t subMeshIndex = 0; subMeshIndex < _gpuData->numSubMeshes; ++subMeshIndex) {
                    for (size_t p = 0; p + 1 < 24; p += 2) {
                        auto& start = _gpuData->meshObjects[subMeshIndex]->boundingBox.worldCorners[p];
                        auto& end   = _gpuData->meshObjects[subMeshIndex]->boundingBox.worldCorners[p + 1];
                        _lineRenderer->lines.push_back(XPLine(start, end));
                    }
                }
            }
            // render frustum volume corners
            if (console->getVariableValue<bool>("r.freeze")) {
                std::array<XPVec4<float>, 24> frustumCorners;
                memcpy(&frustumCorners[0].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[1].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[1].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[0].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[2].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[0].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[3].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[3].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[4].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[3].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[5].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[2].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[6].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[2].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[7].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[1].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[8].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[5].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[9].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[4].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[10].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[4].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[11].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[7].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[12].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[7].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[13].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[6].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[14].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[6].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[15].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[5].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[16].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[1].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[17].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[5].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[18].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[0].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[19].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[4].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[20].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[2].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[21].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[6].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[22].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[3].x,
                       sizeof(XPVec4<float>));
                memcpy(&frustumCorners[23].x,
                       &_vertexShaderArgBuffer0[_frameDataIndex].frameDataPerObject.frustumPoints[7].x,
                       sizeof(XPVec4<float>));
                for (size_t p = 0; p + 1 < 24; p += 2) {
                    auto& start = frustumCorners[p];
                    auto& end   = frustumCorners[p + 1];
                    _lineRenderer->lines.push_back(
                      XPLine(XPVec4<float>(start.x, start.y, start.z, 1), XPVec4<float>(end.x, end.y, end.z, 1)));
                }
            }
        }

        if (_lineRenderer->lines.size() >= 2) {
            // load lines buffer
            {
                // a line is (start and end vector3) that's why we multiply by 2 here ..
                const uint64_t vertexBufferSize = static_cast<uint64_t>(sizeof(XPLine) * _lineRenderer->lines.size());
                if (_lineRenderer->vertexBuffer->length() != vertexBufferSize) {
                    _lineRenderer->vertexBuffer =
                      NS::TransferPtr(_device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared));
                }
                memcpy(_lineRenderer->vertexBuffer->contents(), _lineRenderer->lines.data(), vertexBufferSize);
            }

            encoder->setRenderPipelineState(_renderPipelines["Line"]->renderPipelineState);
            encoder->setVertexBytes(&_vertexShaderArgBuffer0[_frameDataIndex],
                                    sizeof(XPVertexShaderArgumentBuffer),
                                    LineVertexIn_VertexShaderArgumentBuffer0Index);
            encoder->setVertexBuffer(_lineRenderer->vertexBuffer.get(), NS::UInteger(0), XPVertexInputIndexPositions);
            encoder->drawPrimitives(
              MTL::PrimitiveTypeLine, NS::UInteger(0), NS::UInteger(_lineRenderer->lines.size() * 2));
        }
    }
    encoder->popDebugGroup();
    encoder->endEncoding();
}

XPProfilable void
XPMetalRenderer::renderMBuffer(MTL::CommandBuffer*  commandBuffer,
                               XPScene&             scene,
                               FreeCamera&          camera,
                               float                deltaTime,
                               const MTL::Viewport& viewport)
{
    XP_UNUSED(scene)
    XP_UNUSED(camera)
    XP_UNUSED(deltaTime)

    _framePipeline->mBuffer->frameBuffer->passDescriptor->colorAttachments()->object(0)->setClearColor(
      _framePipeline->mBuffer->frameBuffer->clearColor);
    _framePipeline->mBuffer->frameBuffer->passDescriptor->colorAttachments()->object(0)->setLoadAction(
      MTL::LoadActionClear);
    _framePipeline->mBuffer->frameBuffer->passDescriptor->colorAttachments()->object(0)->setStoreAction(
      MTL::StoreActionStore);
    _framePipeline->mBuffer->frameBuffer->passDescriptor->colorAttachments()->object(0)->setTexture(
      _drawable->texture());
    _framePipeline->mBuffer->frameBuffer->passDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionDontCare);
    _framePipeline->mBuffer->frameBuffer->passDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);

    auto encoder = commandBuffer->renderCommandEncoder(_framePipeline->mBuffer->frameBuffer->passDescriptor);
    encoder->setLabel(NS_STRING_FROM_CSTRING("MBuffer"));
    encoder->pushDebugGroup(NS_STRING_FROM_CSTRING("MBuffer"));
    encoder->setViewport(viewport);
    encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    encoder->setRenderPipelineState(_framePipeline->mBuffer->renderPipeline->renderPipelineState);
    XPMetalMeshObject& meshObject = *_meshObjectMap["QUAD"].get();
    encoder->setVertexBuffer(meshObject.mesh.vertexBuffer.get(),
                             XPMeshBuffer::sizeofPositionsType() * meshObject.vertexOffset,
                             XPVertexInputIndexPositions);
    encoder->setVertexBuffer(meshObject.mesh.normalBuffer.get(),
                             XPMeshBuffer::sizeofNormalsType() * meshObject.vertexOffset,
                             XPVertexInputIndexNormals);
    encoder->setVertexBuffer(meshObject.mesh.uvBuffer.get(),
                             XPMeshBuffer::sizeofTexcoordsType() * meshObject.vertexOffset,
                             XPVertexInputIndexUvs);
    encoder->setVertexBytes(&_vertexShaderArgBuffer0[_frameDataIndex],
                            sizeof(XPVertexShaderArgumentBuffer),
                            MBufferVertexIn_VertexShaderArgumentBuffer0Index);
    encoder->setFragmentTexture(
      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexPositionU].texture.get(),
      XPMBufferIndexPositionU);
    encoder->setFragmentTexture(
      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexNormalV].texture.get(),
      XPMBufferIndexNormalV);
    encoder->setFragmentTexture(
      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexAlbedo].texture.get(), XPMBufferIndexAlbedo);
    encoder->setFragmentTexture(
      _framePipeline->gBuffer->frameBuffer->colorAttachments[XPMBufferIndexMetallicRoughnessAmbientObjectId]
        .texture.get(),
      XPMBufferIndexMetallicRoughnessAmbientObjectId);
    encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                   NS::UInteger(meshObject.numIndices),
                                   MTL::IndexTypeUInt32,
                                   meshObject.mesh.indexBuffer.get(),
                                   NS::UInteger(XPMeshBuffer::sizeofIndicesType() * meshObject.indexOffset));
    encoder->popDebugGroup();
    encoder->endEncoding();
}

XPProfilable bool
XPMetalRenderer::createComputePipeline(XPMetalComputePipeline&                   computePipeline,
                                       const XPMetalComputePipelineCreationInfo& creationInfo)
{
    MTL::AutoreleasedComputePipelineReflection reflection = nullptr;
    if (creationInfo.isShaderCompiledFromBinary()) {
        auto       csMetalLibPath = XPFS::buildShaderAssetsPath(fmt::format("{}.metallib", creationInfo.name));
        auto       metalLibPath   = NS_STRING_FROM_CSTRING(csMetalLibPath.c_str());
        NS::Error* error;
        auto       defaultLibrary = NS::TransferPtr(_device->newLibrary(metalLibPath, &error));
        if (error) {
            XP_LOG(XPLoggerSeverityError, error->debugDescription()->cString(NS::StringEncoding::UTF8StringEncoding));
            XP_LOG(XPLoggerSeverityError, error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
        }
        METAL_CHECK(defaultLibrary.get(), "Failed to create new compute library from binary");

        auto computeFunction = NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("computeShader")));
        METAL_CHECK(computeFunction.get(), "Failed to create a compute function from binary");

        computePipeline.state = _device->newComputePipelineState(
          computeFunction.get(), MTL::PipelineOptionBufferTypeInfo, &reflection, &error);
        METAL_CHECK(computePipeline.state, "Failed to create a compute pipeline state from binary");
    } else {
        auto        csMetalLibPath = XPFS::buildShaderAssetsPath(fmt::format("{}.metal", creationInfo.name));
        std::string metalSourceCode;
        METAL_CHECK(XPFS::readFileBlock(csMetalLibPath.c_str(), metalSourceCode),
                    "Failed to read compute shader source code from file");
        auto       metalSourceCodeNSString = NS_STRING_FROM_CSTRING(metalSourceCode.c_str());
        NS::Error* error                   = nullptr;
        auto       compileOptions          = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
        compileOptions->setLibraryType(MTL::LibraryType::LibraryTypeExecutable);
        compileOptions->setLanguageVersion(MTL::LanguageVersion::LanguageVersion3_0);
        const auto key                = NS_STRING_FROM_CSTRING("XP_RUNTIME_COMPILER");
        const auto val                = NS_STRING_FROM_CSTRING("0");
        auto       preprocessorMacros = NS::TransferPtr(NS::Dictionary::alloc()->init(
          (const NS::Object* const*)&val, (const NS::Object* const*)&key, NS::UInteger(1)));
        compileOptions->setPreprocessorMacros(preprocessorMacros.get());
        {
            //        NSEnumerator *enumerator = [myDict keyEnumerator];
            //        id key;
            //        // extra parens to suppress warning about using = instead of ==
            //        while((key = [enumerator nextObject]))
            //            NSLog(@"key=%@ value=%@", key, [myDict objectForKey:key]);
            auto enumerator = compileOptions->preprocessorMacros()->keyEnumerator();
            while (auto key = enumerator->nextObject()) {
                NS::String* keyStr   = reinterpret_cast<NS::String*>(key);
                NS::String* valueStr = reinterpret_cast<NS::String*>(compileOptions->preprocessorMacros()->object(key));
                XP_LOGV(XPLoggerSeverityInfo,
                        "SHADER MACRO: <%s, %s>",
                        keyStr->cString(NS::UTF8StringEncoding),
                        valueStr->cString(NS::UTF8StringEncoding));
            }
        }
        auto defaultLibrary =
          NS::TransferPtr(_device->newLibrary(metalSourceCodeNSString, compileOptions.get(), &error));
        if (error) {
            XP_LOG(XPLoggerSeverityError, error->debugDescription()->cString(NS::StringEncoding::UTF8StringEncoding));
            XP_LOG(XPLoggerSeverityError, error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
        }
        METAL_CHECK(defaultLibrary.get(), "Failed to create new compute library from metal source code");
        auto computeFunction = NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("computeShader")));
        METAL_CHECK(computeFunction.get(), "Failed to create a compute function from metal source code");

        computePipeline.state = _device->newComputePipelineState(
          computeFunction.get(), MTL::PipelineOptionBufferTypeInfo, &reflection, &error);
        METAL_CHECK(computePipeline.state, "Failed to create a compute pipeline state from metal source code");
    }
    {
        NS::Array* bindings = reflection->bindings();
        XP_LOGV(XPLoggerSeverityInfo, "Compute Shader Bindings <%s>:", creationInfo.name.c_str());

        size_t buffersCounter = 0;
        for (size_t i = 0; i < bindings->count(); ++i) {
            MTL::Binding* binding = reinterpret_cast<MTL::Binding*>(bindings->object(i));
            const char*   access  = [binding]() -> const char* {
                switch (binding->access()) {
                    case MTL::ArgumentAccessReadOnly: return "read only";
                    case MTL::ArgumentAccessWriteOnly: return "write only";
                    case MTL::ArgumentAccessReadWrite: return "read write";
                }
            }();
            const char* type = [binding]() -> const char* {
                switch (binding->type()) {
                    case MTL::BindingTypeBuffer: return "buffer";
                    case MTL::BindingTypeImageblock: return "image block";
                    case MTL::BindingTypeImageblockData: return "image block data";
                    case MTL::BindingTypeInstanceAccelerationStructure: return "instance acceleration structure";
                    case MTL::BindingTypeIntersectionFunctionTable: return "intersection function table";
                    case MTL::BindingTypeObjectPayload: return "object payload";
                    case MTL::BindingTypePrimitiveAccelerationStructure: return "primitive acceleration structure";
                    case MTL::BindingTypeSampler: return "sampler";
                    case MTL::BindingTypeTexture: return "texture";
                    case MTL::BindingTypeThreadgroupMemory: return "thread group memory";
                    case MTL::BindingTypeVisibleFunctionTable: return "visible function table";
                }
            }();
            XP_LOGV(XPLoggerSeverityInfo,
                    "\tindex: %lu, access: %s, type: %s, name: %s",
                    binding->index(),
                    access,
                    type,
                    binding->name()->cString(NS::UTF8StringEncoding));

            if (binding->type() == MTL::BindingTypeBuffer) { ++buffersCounter; }
        }
        computePipeline.buffers.resize(buffersCounter);
        assert(creationInfo.bindings.size() == computePipeline.buffers.size());
    }
    return true;
}

XPProfilable void
XPMetalRenderer::destroyComputePipeline(XPMetalComputePipeline& computePipeline)
{
    for (auto& buffer : computePipeline.buffers) { buffer->release(); }
    computePipeline.state->release();
}

XPProfilable bool
XPMetalRenderer::createRenderPipeline(XPMetalRenderPipeline&                   renderPipeline,
                                      const XPMetalRenderPipelineCreationInfo& creationInfo)
{
    MTL::AutoreleasedRenderPipelineReflection reflection = nullptr;
    if (creationInfo.isShaderCompiledFromBinary()) {
        auto       vfsMetalLibPath = XPFS::buildShaderAssetsPath(fmt::format("{}.metallib", creationInfo.name));
        auto       metalLibPath    = NS_STRING_FROM_CSTRING(vfsMetalLibPath.c_str());
        NS::Error* error;
        auto       defaultLibrary = NS::TransferPtr(_device->newLibrary(metalLibPath, &error));
        if (error) {
            XP_LOG(XPLoggerSeverityError, error->debugDescription()->cString(NS::StringEncoding::UTF8StringEncoding));
            XP_LOG(XPLoggerSeverityError, error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
        }
        METAL_CHECK(defaultLibrary.get(), "Failed to create new render library from binary");

        auto vertexFunction = NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("vertexShader")));
        METAL_CHECK(vertexFunction.get(), "Failed to create a vertex function from binary");
        NS::SharedPtr<MTL::Function> fragmentFunction;
        if (creationInfo.hasFragmentFunction()) {
            fragmentFunction = NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("fragmentShader")));
        }
        if (!vertexFunction && !fragmentFunction) {
            // this seems to be a compute shader as it doesn't have vertex or fragment shader functions
            return false;
        }

        // Configure a pipeline descriptor that is used to create a pipeline state.
        renderPipeline.renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        renderPipeline.renderPipelineDescriptor->setLabel(NS_STRING_FROM_CSTRING(creationInfo.name.c_str()));
        renderPipeline.renderPipelineDescriptor->setVertexDescriptor(creationInfo.vertexDescriptor.get());
        renderPipeline.renderPipelineDescriptor->setVertexFunction(vertexFunction.get());
        if (creationInfo.hasFragmentFunction()) {
            renderPipeline.renderPipelineDescriptor->setFragmentFunction(fragmentFunction.get());
        } else {
            renderPipeline.renderPipelineDescriptor->setFragmentFunction(nullptr);
        }
        // color attachment pixel format
        const auto& colorPixelFormats = creationInfo.colorAttachmentPixelFormats;
        for (size_t ai = 0; ai < colorPixelFormats.size(); ++ai) {
            renderPipeline.renderPipelineDescriptor->colorAttachments()->object(ai)->setPixelFormat(
              colorPixelFormats[ai]);
        }

        // depth attachment pixel format
        if (creationInfo.isDepthWriteEnabled() && creationInfo.depthAttachmentPixelFormat.has_value()) {
            renderPipeline.renderPipelineDescriptor->setDepthAttachmentPixelFormat(
              creationInfo.depthAttachmentPixelFormat.value());
        } else {
            renderPipeline.renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatInvalid);
        }

        // stencil attachment pixel format
        if (creationInfo.isStencilWriteEnabled() && creationInfo.stencilPixelFormat.has_value()) {
            renderPipeline.renderPipelineDescriptor->setStencilAttachmentPixelFormat(
              creationInfo.stencilPixelFormat.value());
        } else {
            renderPipeline.renderPipelineDescriptor->setStencilAttachmentPixelFormat(MTL::PixelFormatInvalid);
        }

        renderPipeline.renderPipelineState = _device->newRenderPipelineState(
          renderPipeline.renderPipelineDescriptor, MTL::PipelineOptionBufferTypeInfo, &reflection, &error);
        METAL_CHECK(renderPipeline.renderPipelineState, "Failed to create a metal render pipeline state");

        renderPipeline.depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
        renderPipeline.depthStencilDescriptor->setDepthCompareFunction(creationInfo.depthComparisonFunction);
        renderPipeline.depthStencilDescriptor->setDepthWriteEnabled(creationInfo.isDepthWriteEnabled());

        renderPipeline.depthStencilState = _device->newDepthStencilState(renderPipeline.depthStencilDescriptor);
        METAL_CHECK(renderPipeline.depthStencilState, "Failed to create a metal depth stencil state");
    } else {
        auto        vfsMetalLibPath = XPFS::buildShaderAssetsPath(fmt::format("{}.metal", creationInfo.name));
        std::string metalSourceCode;
        METAL_CHECK(XPFS::readFileBlock(vfsMetalLibPath.c_str(), metalSourceCode),
                    "Failed to read render shader source code from file");
        auto       metalSourceCodeNSString = NS_STRING_FROM_CSTRING(metalSourceCode.c_str());
        NS::Error* error                   = nullptr;
        auto       compileOptions          = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
        compileOptions->setLibraryType(MTL::LibraryType::LibraryTypeExecutable);
        compileOptions->setLanguageVersion(MTL::LanguageVersion::LanguageVersion3_0);
        const auto key                = NS_STRING_FROM_CSTRING("XP_RUNTIME_COMPILER");
        const auto val                = NS_STRING_FROM_CSTRING("0");
        auto       preprocessorMacros = NS::TransferPtr(NS::Dictionary::alloc()->init(
          (const NS::Object* const*)&val, (const NS::Object* const*)&key, NS::UInteger(1)));
        compileOptions->setPreprocessorMacros(preprocessorMacros.get());
        {
            //        NSEnumerator *enumerator = [myDict keyEnumerator];
            //        id key;
            //        // extra parens to suppress warning about using = instead of ==
            //        while((key = [enumerator nextObject]))
            //            NSLog(@"key=%@ value=%@", key, [myDict objectForKey:key]);
            auto enumerator = compileOptions->preprocessorMacros()->keyEnumerator();
            while (auto key = enumerator->nextObject()) {
                NS::String* keyStr   = reinterpret_cast<NS::String*>(key);
                NS::String* valueStr = reinterpret_cast<NS::String*>(compileOptions->preprocessorMacros()->object(key));
                XP_LOGV(XPLoggerSeverityInfo,
                        "SHADER MACRO: <%s, %s>",
                        keyStr->cString(NS::UTF8StringEncoding),
                        valueStr->cString(NS::UTF8StringEncoding));
            }
        }
        auto defaultLibrary =
          NS::TransferPtr(_device->newLibrary(metalSourceCodeNSString, compileOptions.get(), &error));
        if (error) {
            XP_LOG(XPLoggerSeverityError, error->debugDescription()->cString(NS::StringEncoding::UTF8StringEncoding));
            XP_LOG(XPLoggerSeverityError, error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
        }
        METAL_CHECK(defaultLibrary.get(), "Failed to create new render library from metal source code");
        NS::SharedPtr<MTL::Function> vertexFunction =
          NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("vertexShader")));
        METAL_CHECK(vertexFunction.get(), "Failed to create a vertex function from metal source code");
        NS::SharedPtr<MTL::Function> fragmentFunction;
        if (creationInfo.hasFragmentFunction()) {
            fragmentFunction = NS::TransferPtr(defaultLibrary->newFunction(NS_STRING_FROM_CSTRING("fragmentShader")));
        }
        if (!vertexFunction && !fragmentFunction) {
            // this seems to be a compute shader as it doesn't have vertex or fragment shader functions
            return false;
        }

        XP_LOGV(XPLoggerSeverityInfo, "%s:", vfsMetalLibPath.c_str());
        for (size_t vai = 0; vai < vertexFunction->vertexAttributes()->count(); ++vai) {
            MTL::VertexAttribute* attrib = ((MTL::VertexAttribute*)vertexFunction->vertexAttributes()->object(vai));
            XP_LOGV(XPLoggerSeverityInfo,
                    "\t%s: %s",
                    typeToStr(attrib->attributeType()),
                    attrib->name()->cString(NS::UTF8StringEncoding));
        }
        XP_LOG(XPLoggerSeverityInfo, "\n");

        // Configure a pipeline descriptor that is used to create a pipeline state.
        renderPipeline.renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        renderPipeline.renderPipelineDescriptor->setLabel(NS_STRING_FROM_CSTRING(creationInfo.name.c_str()));
        renderPipeline.renderPipelineDescriptor->setVertexDescriptor(creationInfo.vertexDescriptor.get());
        renderPipeline.renderPipelineDescriptor->setVertexFunction(vertexFunction.get());
        if (creationInfo.hasFragmentFunction()) {
            renderPipeline.renderPipelineDescriptor->setFragmentFunction(fragmentFunction.get());
        } else {
            renderPipeline.renderPipelineDescriptor->setFragmentFunction(nullptr);
        }
        // color attachment pixel format
        auto& colorPixelFormats = creationInfo.colorAttachmentPixelFormats;
        for (size_t ai = 0; ai < colorPixelFormats.size(); ++ai) {
            renderPipeline.renderPipelineDescriptor->colorAttachments()->object(ai)->setPixelFormat(
              colorPixelFormats[ai]);
        }

        // depth attachment pixel format
        if (creationInfo.isDepthWriteEnabled() && creationInfo.depthAttachmentPixelFormat.has_value()) {
            renderPipeline.renderPipelineDescriptor->setDepthAttachmentPixelFormat(
              creationInfo.depthAttachmentPixelFormat.value());
        } else {
            renderPipeline.renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatInvalid);
        }

        // stencil attachment pixel format
        if (creationInfo.isStencilWriteEnabled() && creationInfo.stencilPixelFormat.has_value()) {
            renderPipeline.renderPipelineDescriptor->setStencilAttachmentPixelFormat(
              creationInfo.stencilPixelFormat.value());
        } else {
            renderPipeline.renderPipelineDescriptor->setStencilAttachmentPixelFormat(MTL::PixelFormatInvalid);
        }

        renderPipeline.renderPipelineState = _device->newRenderPipelineState(
          renderPipeline.renderPipelineDescriptor, MTL::PipelineOptionBufferTypeInfo, &reflection, &error);
        METAL_CHECK(renderPipeline.renderPipelineState, "Failed to create a metal render pipeline state");

        renderPipeline.depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
        renderPipeline.depthStencilDescriptor->setDepthCompareFunction(creationInfo.depthComparisonFunction);
        renderPipeline.depthStencilDescriptor->setDepthWriteEnabled(creationInfo.isDepthWriteEnabled());

        renderPipeline.depthStencilState = _device->newDepthStencilState(renderPipeline.depthStencilDescriptor);
        METAL_CHECK(renderPipeline.depthStencilState, "Failed to create a metal depth stencil state");
    }

    NS::Array* vertexShaderBindings   = reflection->vertexBindings();
    NS::Array* fragmentShaderBindings = reflection->fragmentBindings();
    parseMeshRenderingShader(creationInfo.name.c_str(), vertexShaderBindings, fragmentShaderBindings);

    return true;
}

XPProfilable void
XPMetalRenderer::destroyRenderPipeline(XPMetalRenderPipeline& renderPipeline)
{
    renderPipeline.depthStencilDescriptor->release();
    renderPipeline.depthStencilState->release();

    renderPipeline.renderPipelineDescriptor->release();
    renderPipeline.renderPipelineState->release();
}

XPProfilable void
XPMetalRenderer::createFramebufferTexture(XPMetalFrameBuffer&                      framebuffer,
                                          const XPMetalRenderPipelineCreationInfo& createInfo)
{
    auto rendererResolution = getResolution();
    framebuffer.colorAttachments.resize(createInfo.colorAttachmentPixelFormats.size());
    for (size_t i = 0; i < framebuffer.colorAttachments.size(); ++i) {
        auto  colorPixelFormat            = createInfo.colorAttachmentPixelFormats[i];
        auto& colorAttachment             = framebuffer.colorAttachments[i];
        colorAttachment.textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
        METAL_CHECK(colorAttachment.textureDescriptor, "Failed to create a texture descriptor");
        colorAttachment.textureDescriptor->setWidth(rendererResolution.x);
        colorAttachment.textureDescriptor->setHeight(rendererResolution.y);
        colorAttachment.textureDescriptor->setPixelFormat(colorPixelFormat);
        colorAttachment.textureDescriptor->setStorageMode(MTL::StorageModePrivate);
        colorAttachment.textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

        colorAttachment.texture = NS::TransferPtr(_device->newTexture(colorAttachment.textureDescriptor.get()));
        METAL_CHECK(colorAttachment.texture, "Failed to create a texture");
    }
    if (createInfo.isDepthWriteEnabled()) {
        auto depthPixelFormat                         = createInfo.depthAttachmentPixelFormat.value();
        framebuffer.depthAttachment.textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
        METAL_CHECK(framebuffer.depthAttachment.textureDescriptor, "Failed to create a depth texture descriptor");
        framebuffer.depthAttachment.textureDescriptor->setWidth(rendererResolution.x);
        framebuffer.depthAttachment.textureDescriptor->setHeight(rendererResolution.y);
        framebuffer.depthAttachment.textureDescriptor->setPixelFormat(depthPixelFormat);
        framebuffer.depthAttachment.textureDescriptor->setStorageMode(MTL::StorageModeMemoryless);
        framebuffer.depthAttachment.textureDescriptor->setUsage(MTL::TextureUsageRenderTarget |
                                                                MTL::TextureUsageShaderRead);

        framebuffer.depthAttachment.texture =
          NS::TransferPtr(_device->newTexture(framebuffer.depthAttachment.textureDescriptor.get()));
        METAL_CHECK(framebuffer.depthAttachment.texture, "Failed to create a depth texture");
    }
}

XPProfilable void
XPMetalRenderer::destroyFramebufferTexture(XPMetalFrameBuffer& framebuffer)
{
    framebuffer.depthAttachment.textureDescriptor.reset();
    framebuffer.depthAttachment.texture.reset();
    for (auto& colorAttachment : framebuffer.colorAttachments) {
        colorAttachment.textureDescriptor.reset();
        colorAttachment.texture.reset();
    }
}

XPProfilable void
XPMetalRenderer::reCreateFramebufferTexture(XPMetalFrameBuffer& framebuffer, const std::string& name)
{
    // try retrieving information about texture descriptors before destroying the textures
    XPMetalRenderPipelineCreationInfo createInfo(
      name,
      NS::SharedPtr<MTL::VertexDescriptor>(), // vertex desctiptor not needed here, just pass nullptr ...
      static_cast<XPMetalRenderPipelineCreationInfo::RenderPipelineFlags>(
        XPMetalRenderPipelineCreationInfo::RenderPipelineFlags::RenderPipelineFlags_IncludeFragmentFunction |
        (framebuffer.depthAttachment.texture.get() != nullptr
           ? XPMetalRenderPipelineCreationInfo::RenderPipelineFlags::RenderPipelineFlags_EnableDepthWrite
           : XPMetalRenderPipelineCreationInfo::RenderPipelineFlags::RenderPipelineFlags_None)),
      {},
      framebuffer.depthAttachment.texture.get() != nullptr
        ? std::optional<MTL::PixelFormat>{ MTL::PixelFormatDepth32Float }
        : std::nullopt,
      {},
      MTL::CompareFunction::CompareFunctionLess,
      1);
    createInfo.colorAttachmentPixelFormats.resize(framebuffer.colorAttachments.size());
    for (size_t i = 0; i < framebuffer.colorAttachments.size(); ++i) {
        createInfo.colorAttachmentPixelFormats[i] = framebuffer.colorAttachments[i].textureDescriptor->pixelFormat();
    }

    // destroy and create a new one
    destroyFramebufferTexture(framebuffer);
    createFramebufferTexture(framebuffer, createInfo);
}

XPProfilable void
XPMetalRenderer::recreateFramebufferTextures()
{
    reCreateFramebufferTexture(*_framePipeline->mBuffer->frameBuffer, "MBuffer");
    reCreateFramebufferTexture(*_framePipeline->gBuffer->frameBuffer, "GBuffer");
}

XPProfilable std::optional<XPMetalRenderPipeline*>
             XPMetalRenderer::getRenderPipelines(const std::string& name) const
{
    auto it = _renderPipelines.find(name);
    if (it != _renderPipelines.end()) { return { it->second.get() }; }
    return std::nullopt;
}

XPProfilable std::optional<XPMetalMeshObject*>
             XPMetalRenderer::getMeshObjectMap(const std::string& name) const
{
    auto it = _meshObjectMap.find(name);
    if (it != _meshObjectMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

XPProfilable std::optional<XPMetalMesh*>
             XPMetalRenderer::getMeshMap(const std::string& name) const
{
    auto it = _meshMap.find(name);
    if (it != _meshMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

XPProfilable std::optional<XPMetalShader*>
             XPMetalRenderer::getShaderMap(const std::string& name) const
{
    auto it = _shaderMap.find(name);
    if (it != _shaderMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

XPProfilable std::optional<XPMetalTexture*>
             XPMetalRenderer::getTextureMap(const std::string& name) const
{
    auto it = _textureMap.find(name);
    if (it != _textureMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}

XPProfilable std::optional<XPMetalMaterial*>
             XPMetalRenderer::getMaterialMap(const std::string& name) const
{
    auto it = _materialMap.find(name);
    if (it != _materialMap.end()) { return { it->second.get() }; }
    return std::nullopt;
}
