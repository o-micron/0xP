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
#include <Renderer/DX12/XPDX12GPUData.h>
#include <Renderer/DX12/XPDX12Renderer.h>
#include <Renderer/DX12/XPDX12Window.h>
#include <Renderer/DX12/XPShaderTypes.h>
#include <Renderer/DX12/dx12al/CommandList.hpp>
#include <Renderer/DX12/dx12al/ConstantBuffer.hpp>
#include <Renderer/DX12/dx12al/DescriptorHeap.hpp>
#include <Renderer/DX12/dx12al/Device.hpp>
#include <Renderer/DX12/dx12al/GraphicsPipeline.hpp>
#include <Renderer/DX12/dx12al/PipelineStateObject.hpp>
#include <Renderer/DX12/dx12al/Queue.hpp>
#include <Renderer/DX12/dx12al/Shader.hpp>
#include <Renderer/DX12/dx12al/Swapchain.hpp>
#include <Renderer/DX12/dx12al/Synchronization.hpp>
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#pragma clang diagnostic pop

XPDX12Renderer::XPDX12Renderer(XPRegistry* const registry)
  : XPIRenderer(registry)
  , _registry(registry)
  , _window(XP_NEW XPDX12Window(this))
  , _gpuData(XP_NEW XPDX12GPUData())
  , _resolution(XP_INITIAL_WINDOW_WIDTH, XP_INITIAL_WINDOW_HEIGHT)
  , _isCapturingDebugFrames(false)
  , _isFramebufferResized(false)
  , _renderingGpuTime(1.0f)
  , _uiGpuTime(1.0f)
  , _computeGpuTime(1.0f)
{
}

XPDX12Renderer::~XPDX12Renderer()
{
    XP_DELETE _gpuData;
    XP_DELETE _window;
}

XPProfilable void
XPDX12Renderer::initialize()
{
    _window->initialize();
    int adapterIndex = -1, outputIndex = -1;
    _window->createSurface(&adapterIndex, &outputIndex);

    auto windowSize = _window->getWindowSize();

    _device            = XP_NEW            dx12al::Device(this);
    _graphicsQueue     = XP_NEW     dx12al::Queue();
    _swapchain         = XP_NEW         dx12al::Swapchain(_window);
    _synchronization   = XP_NEW   dx12al::Synchronization();
    _rtvDescriptorHeap = XP_NEW dx12al::DescriptorHeap();
    _dsvDescriptorHeap = XP_NEW dx12al::DescriptorHeap();
    _cameraMatricesCB  = XP_NEW  dx12al::ConstantBuffer();
    // _meshMatricesCB    = XP_NEW    dx12al::ConstantBuffer();
    _cbvDescriptorHeap = XP_NEW dx12al::DescriptorHeap();
    _graphicsPipeline  = XP_NEW  dx12al::GraphicsPipeline();
    _commandList       = XP_NEW       dx12al::CommandList();

    _device->create(adapterIndex, outputIndex);

#if defined(XP_BUILD_DEBUG)
    _device->getDevice()->QueryInterface(IID_PPV_ARGS(&_infoQueue));
    _infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
    _infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
    _infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
#endif

    _graphicsQueue->create(_device->getDevice(), L"GraphicsQueue");
    _swapchain->create(_device->getDevice(),
                       _graphicsQueue->getCommandQueue(),
                       _device->getFactory(),
                       XP_DX12_BUFFER_COUNT,
                       windowSize.x,
                       windowSize.y);
    _synchronization->createFence(_device->getDevice());
    _rtvDescriptorHeap->create(_device->getDevice(), L"rtvDescHeap", D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
    _dsvDescriptorHeap->create(_device->getDevice(),
                               L"dsvDescHeap",
                               D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                               1,
                               false); // +1 if want to read depth in shader

    // create the descriptor heap for the view and proj matrix CB views (and now a texture2d SRV view also)
    _cbvDescriptorHeap->create(_device->getDevice(), L"cbvDescHeap", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

    // creatae constant buffers
    _cameraMatricesCB->create(_device->getDevice(), _cbvDescriptorHeap, 0, nullptr, sizeof(CameraMatrices));
    static_assert(sizeof(CameraMatrices) <= 256);
    // _meshMatricesCB->create(_device->getDevice(), _cbvDescriptorHeap, 1, nullptr, sizeof(MeshMatrices));
    static_assert(sizeof(MeshMatrices) <= 256);

    const std::string vsfxc = XPFS::buildShaderAssetsPath("GBuffer.vs.fxc");
    const std::string psfxc = XPFS::buildShaderAssetsPath("GBuffer.ps.fxc");
    _graphicsPipeline->create(_device->getDevice(), vsfxc.c_str(), psfxc.c_str());

    _commandList->create(_device->getDevice(), _graphicsQueue->getCommandAllocator(_synchronization->getFrameIndex()));

    // Create frame resources.
    {
        // Create a RTV for each frame.
        for (UINT i = 0; i < XP_DX12_BUFFER_COUNT; ++i) {
            DX12_ASSERT(_swapchain->getSwapchain()->GetBuffer(i, IID_PPV_ARGS(&_renderTargets[i])));
            _device->getDevice()->CreateRenderTargetView(
              _renderTargets[i], nullptr, _rtvDescriptorHeap->getCpuHandle(i, 0));
            // Create the depth stencil.
            {
                D3D12_RESOURCE_DESC shadowTextureDesc = {};
                shadowTextureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                shadowTextureDesc.Alignment           = 0;
                shadowTextureDesc.Width               = static_cast<UINT>(_resolution.x);
                shadowTextureDesc.Height              = static_cast<UINT>(_resolution.y);
                shadowTextureDesc.DepthOrArraySize    = 1;
                shadowTextureDesc.MipLevels           = 1;
                shadowTextureDesc.Format              = DXGI_FORMAT_D32_FLOAT;
                shadowTextureDesc.SampleDesc.Count    = 1;
                shadowTextureDesc.SampleDesc.Quality  = 0;
                shadowTextureDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                shadowTextureDesc.Flags =
                  D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

                // Performance tip: Tell the runtime at resource creation the
                // desired clear value.
                D3D12_CLEAR_VALUE clearValue;
                clearValue.Format               = DXGI_FORMAT_D32_FLOAT;
                clearValue.DepthStencil.Depth   = 1.0f;
                clearValue.DepthStencil.Stencil = 0;

                D3D12_HEAP_PROPERTIES heapProperties = {};
                heapProperties.Type                  = D3D12_HEAP_TYPE_DEFAULT;
                heapProperties.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heapProperties.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
                heapProperties.CreationNodeMask      = 1;
                heapProperties.VisibleNodeMask       = 1;

                DX12_ASSERT(_device->getDevice()->CreateCommittedResource(&heapProperties,
                                                                          D3D12_HEAP_FLAG_NONE,
                                                                          &shadowTextureDesc,
                                                                          D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                                          &clearValue,
                                                                          IID_PPV_ARGS(&_depthStencil[i])));

                SET_NAME_INDEXED(_depthStencil[i], L"DepthStencil", i);

                // Create the depth stencil view.
                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                dsvDesc.Format                        = DXGI_FORMAT_D32_FLOAT;
                dsvDesc.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Flags                         = D3D12_DSV_FLAG_NONE;
                _device->getDevice()->CreateDepthStencilView(
                  _depthStencil[i], &dsvDesc, _dsvDescriptorHeap->getCpuHandle(i, 0));
            }
        }
    }
}

XPProfilable void
XPDX12Renderer::finalize()
{
    _synchronization->waitForGPU(_graphicsQueue->getCommandQueue());

    _meshMap.clear();
    _meshObjectMap.clear();

    _commandList->destroy();
    _graphicsPipeline->destroy();
    _cbvDescriptorHeap->destroy();
    _cameraMatricesCB->destroy();
    // _meshMatricesCB->destroy();
    _dsvDescriptorHeap->destroy();
    _rtvDescriptorHeap->destroy();
    _synchronization->destroyFence();
    _graphicsQueue->destroy();
    _swapchain->destroy();

#if defined(XP_BUILD_DEBUG)
    _infoQueue->Release();
#endif

    _device->destroy();

    XP_DELETE _commandList;
    XP_DELETE _graphicsPipeline;
    XP_DELETE _cbvDescriptorHeap;
    XP_DELETE _cameraMatricesCB;
    // XP_DELETE _meshMatricesCB;
    XP_DELETE _dsvDescriptorHeap;
    XP_DELETE _rtvDescriptorHeap;
    XP_DELETE _synchronization;
    XP_DELETE _swapchain;
    XP_DELETE _graphicsQueue;
    XP_DELETE _device;
}

XPProfilable void
XPDX12Renderer::update()
{
    float deltaTime = _window->getDeltaTimeSeconds();

    _window->pollEvents();
    _window->refresh();

    // update constant buffers

    auto& scene       = *_registry->getScene();
    auto& layers      = scene.getLayers();
    auto& cameraNodes = scene.getNodes(FreeCameraAttachmentDescriptor);

    if (!layers.empty() && !cameraNodes.empty()) {
        if (scene.hasAnyTraitsChanges()) {
            // RAII, block and wait for all frames to finish !
            compileLoadScene(scene);
            scene.setHasTraitsChanges(false, false, true);
        }
    }

    if (!layers.empty() && !cameraNodes.empty()) {
        FreeCamera& camera = *(*cameraNodes.begin())->getFreeCamera();
        CameraSystemUpdateAll(&camera, _registry, _window->getInput(), deltaTime);

        // update viewport
        {
            auto        console             = _registry->getEngine()->getConsole();
            float       scale               = console->getVariableValue<float>("r.scale");
            XPVec2<int> presentResolution   = getWindowSize();
            XPVec2<int> renderingResolution = getWindowSize();
            renderingResolution.x           = presentResolution.x * scale;
            renderingResolution.y           = presentResolution.y * scale;

            if (!(renderingResolution.x <= 0 || renderingResolution.y <= 0 || renderingResolution.x == _resolution.x ||
                  renderingResolution.y == _resolution.y)) {
                _resolution = renderingResolution;
                // recreateFramebufferTextures();
                for (auto cameraNode : _registry->getScene()->getNodes(FreeCameraAttachmentDescriptor)) {
                    FreeCamera* freeCamera              = cameraNode->getFreeCamera();
                    freeCamera->activeProperties.width  = _resolution.x;
                    freeCamera->activeProperties.height = _resolution.y;
                }
            }
        }

        // update matrices
        {
            XP_UNUSED(scene)
            XP_UNUSED(camera)
            XP_UNUSED(deltaTime)
            CameraMatrices& cameraMatrices = _cameraMatricesCBData[_synchronization->getFrameIndex()];
            XPConsole*      console        = _registry->getEngine()->getConsole();
            if (console->getVariableValue<bool>("r.freeze") == false) {
                memcpy(&cameraMatrices.viewProjectionMatrix,
                       &camera.activeProperties.viewProjectionMatrix._00,
                       sizeof(XPMat4<float>));
                memcpy(&cameraMatrices.inverseViewProjectionMatrix,
                       &camera.activeProperties.inverseViewProjectionMatrix._00,
                       sizeof(XPMat4<float>));
                memcpy(&cameraMatrices.inverseViewMatrix,
                       &camera.activeProperties.inverseViewMatrix._00,
                       sizeof(XPMat4<float>));
            } else {
                memcpy(&cameraMatrices.viewProjectionMatrix,
                       &camera.frozenProperties.viewProjectionMatrix._00,
                       sizeof(XPMat4<float>));
                memcpy(&cameraMatrices.inverseViewProjectionMatrix,
                       &camera.frozenProperties.inverseViewProjectionMatrix._00,
                       sizeof(XPMat4<float>));
                memcpy(&cameraMatrices.inverseViewMatrix,
                       &camera.frozenProperties.inverseViewMatrix._00,
                       sizeof(XPMat4<float>));
            }

            glm::mat4* mat = (glm::mat4*)(&cameraMatrices.viewProjectionMatrix[0]);
            *mat           = glm::transpose(*mat);

            mat  = (glm::mat4*)(&cameraMatrices.inverseViewProjectionMatrix[0]);
            *mat = glm::transpose(*mat);

            mat  = (glm::mat4*)(&cameraMatrices.inverseViewMatrix[0]);
            *mat = glm::transpose(*mat);

            _cameraMatricesCB->copyToGPU(
              _synchronization->getFrameIndex(), (void*)&cameraMatrices, sizeof(CameraMatrices));
        }

        // Record all the commands we need to render the scene into the command list.
        // PopulateCommandList
        {
            D3D12_VIEWPORT viewport = {};
            viewport.TopLeftX       = 0.0f;
            viewport.TopLeftY       = 0.0f;
            viewport.Width          = static_cast<float>(_resolution.x);
            viewport.Height         = static_cast<float>(_resolution.y);

            D3D12_RECT scissor = {};
            scissor.left       = 0;
            scissor.top        = 0;
            scissor.right      = static_cast<LONG>(_resolution.x);
            scissor.bottom     = static_cast<LONG>(_resolution.y);

            auto& rootSig = _graphicsPipeline->getRootSignature();
            auto& pso     = _graphicsPipeline->getPSO();
            auto& vs      = _graphicsPipeline->getVertexShader();
            auto& ps      = _graphicsPipeline->getPixelShader();

            // Command list allocators can only be reset when the associated
            // command lists have finished execution on the GPU; apps should use
            // fences to determine GPU execution progress.
            DX12_ASSERT(_graphicsQueue->getCommandAllocator(_synchronization->getFrameIndex())->Reset());
            // However, when ExecuteCommandList() is called on a particular command
            // list, that command list can then be reset at any time and must be before
            // re-recording.
            DX12_ASSERT(_commandList->getCommandList()->Reset(
              _graphicsQueue->getCommandAllocator(_synchronization->getFrameIndex()), pso.getPSO()));

            // Set necessary state.
            _commandList->getCommandList()->SetGraphicsRootSignature(rootSig.getSignature());

            ID3D12DescriptorHeap* ppHeaps[] = { _cbvDescriptorHeap->getHeap(_synchronization->getFrameIndex()) };
            _commandList->getCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            _commandList->getCommandList()->SetGraphicsRootDescriptorTable(
              0, _cbvDescriptorHeap->getHeap(_synchronization->getFrameIndex())->GetGPUDescriptorHandleForHeapStart());
            _commandList->getCommandList()->RSSetViewports(1, &viewport);
            _commandList->getCommandList()->RSSetScissorRects(1, &scissor);
            _commandList->getCommandList()->OMSetStencilRef(0);

            // Indicate that the back buffer will be used as a render target.
            D3D12_RESOURCE_BARRIER barrierPresentToRenderTarget = {};
            barrierPresentToRenderTarget.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrierPresentToRenderTarget.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrierPresentToRenderTarget.Transition.pResource   = _renderTargets[_synchronization->getFrameIndex()];
            barrierPresentToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrierPresentToRenderTarget.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrierPresentToRenderTarget.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            _commandList->getCommandList()->ResourceBarrier(1, &barrierPresentToRenderTarget);

            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
              _rtvDescriptorHeap->getCpuHandle(_synchronization->getFrameIndex(), 0);
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle =
              _dsvDescriptorHeap->getCpuHandle(_synchronization->getFrameIndex(), 0);
            _commandList->getCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

            // Record commands.
            const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            _commandList->getCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            _commandList->getCommandList()->ClearDepthStencilView(
              dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            recordCommandList(scene, camera, _synchronization->getFrameIndex(), _commandList->getCommandList());

            // Indicate that the back buffer will now be used to present.
            D3D12_RESOURCE_BARRIER barrierRenderTargetToPresent = {};
            barrierRenderTargetToPresent.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrierRenderTargetToPresent.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrierRenderTargetToPresent.Transition.pResource   = _renderTargets[_synchronization->getFrameIndex()];
            barrierRenderTargetToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrierRenderTargetToPresent.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
            barrierRenderTargetToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            _commandList->getCommandList()->ResourceBarrier(1, &barrierRenderTargetToPresent);

            DX12_ASSERT(_commandList->getCommandList()->Close());
        }

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = { _commandList->getCommandList() };
        _graphicsQueue->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Present the frame.
        DX12_ASSERT(_swapchain->getSwapchain()->Present(1, 0));

        _synchronization->moveToNextFrame(_graphicsQueue->getCommandQueue(), _swapchain->getSwapchain());
    }
}

void
XPDX12Renderer::onSceneTraitsChanged()
{
}

XPProfilable void
XPDX12Renderer::invalidateDeviceObjects()
{
    XP_UNIMPLEMENTED("XPDX12Renderer::invalidateDeviceObjects unimplememted");
}

XPProfilable void
XPDX12Renderer::createDeviceObjects()
{
    XP_UNIMPLEMENTED("XPDX12Renderer::createDeviceObjects unimplememted");
}

XPProfilable void
XPDX12Renderer::beginUploadMeshAssets()
{
}

XPProfilable void
XPDX12Renderer::endUploadMeshAssets()
{
}

void
XPDX12Renderer::beginUploadShaderAssets()
{
}

void
XPDX12Renderer::endUploadShaderAssets()
{
}

void
XPDX12Renderer::beginUploadTextureAssets()
{
}

void
XPDX12Renderer::endUploadTextureAssets()
{
}

void
XPDX12Renderer::beginReUploadMeshAssets()
{
}

void
XPDX12Renderer::endReUploadMeshAssets()
{
}

void
XPDX12Renderer::beginReUploadShaderAssets()
{
}

void
XPDX12Renderer::endReUploadShaderAssets()
{
}

void
XPDX12Renderer::beginReUploadTextureAssets()
{
}

void
XPDX12Renderer::endReUploadTextureAssets()
{
}

XPProfilable void
XPDX12Renderer::uploadMeshAsset(XPMeshAsset* meshAsset)
{
    XPMeshBuffer*               meshBuffer = meshAsset->getMeshBuffer();
    std::unique_ptr<XPDX12Mesh> dx12Mesh   = std::make_unique<XPDX12Mesh>();

    size_t positionsCount = meshBuffer->getPositionsCount();
    size_t normalsCount   = meshBuffer->getNormalsCount();
    size_t texcoordsCount = meshBuffer->getTexcoordsCount();
    size_t indicesCount   = meshBuffer->getIndicesCount();

    const XPVec4<float>* positions = meshBuffer->getPositions();
    const XPVec4<float>* normals   = meshBuffer->getNormals();
    const XPVec4<float>* texcoords = meshBuffer->getTexcoords();
    const uint32_t*      indices   = meshBuffer->getIndices();

    const uint64_t vertexBufferSize   = static_cast<uint64_t>(positionsCount * XPMeshBuffer::sizeofPositions());
    const uint64_t normalBufferSize   = static_cast<uint64_t>(normalsCount * XPMeshBuffer::sizeofNormals());
    const uint64_t texcoordBufferSize = static_cast<uint64_t>(texcoordsCount * XPMeshBuffer::sizeofTexcoords());
    const uint64_t indexBufferSize    = static_cast<uint64_t>(indicesCount * XPMeshBuffer::sizeofIndices());

    dx12Mesh->vertexBuffer =
      _device->createVertexBuffer(L"VertexBuffer", vertexBufferSize, val::EResourceStorageModePrivate);
    dx12Mesh->normalBuffer =
      _device->createVertexBuffer(L"normalBuffer", normalBufferSize, val::EResourceStorageModePrivate);
    dx12Mesh->uvBuffer = _device->createVertexBuffer(L"UvBuffer", texcoordBufferSize, val::EResourceStorageModePrivate);
    dx12Mesh->indexBuffer =
      _device->createIndexBuffer(L"IndexBuffer", indexBufferSize, val::EResourceStorageModePrivate);

    {
        auto intermediateBuffer =
          _device->createVertexBuffer(L"IntermediateVertexBuffer", vertexBufferSize, val::EResourceStorageModeShared);
        _device->copyBuffer(
          (void*)positions, 0, intermediateBuffer, val::EResourceStorageModeShared, 0, vertexBufferSize);
        _device->copyBuffer(intermediateBuffer,
                            val::EResourceStorageModeShared,
                            0,
                            dx12Mesh->vertexBuffer,
                            val::EResourceStorageModePrivate,
                            0,
                            vertexBufferSize,
                            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        // Initialize the vertex buffer view.
        dx12Mesh->vertexBufferView.BufferLocation = dx12Mesh->vertexBuffer->GetGPUVirtualAddress();
        dx12Mesh->vertexBufferView.StrideInBytes  = XPMeshBuffer::sizeofPositions();
        dx12Mesh->vertexBufferView.SizeInBytes    = vertexBufferSize;
    }
    {
        auto intermediateBuffer =
          _device->createVertexBuffer(L"IntermediateNormalBuffer", normalBufferSize, val::EResourceStorageModeShared);
        _device->copyBuffer(
          (void*)normals, 0, intermediateBuffer, val::EResourceStorageModeShared, 0, normalBufferSize);
        _device->copyBuffer(intermediateBuffer,
                            val::EResourceStorageModeShared,
                            0,
                            dx12Mesh->normalBuffer,
                            val::EResourceStorageModePrivate,
                            0,
                            normalBufferSize,
                            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        // Initialize the normal buffer view.
        dx12Mesh->normalBufferView.BufferLocation = dx12Mesh->normalBuffer->GetGPUVirtualAddress();
        dx12Mesh->normalBufferView.StrideInBytes  = XPMeshBuffer::sizeofNormals();
        dx12Mesh->normalBufferView.SizeInBytes    = normalBufferSize;
    }
    {
        auto intermediateBuffer =
          _device->createVertexBuffer(L"IntermediateUvBuffer", texcoordBufferSize, val::EResourceStorageModeShared);
        _device->copyBuffer(
          (void*)texcoords, 0, intermediateBuffer, val::EResourceStorageModeShared, 0, texcoordBufferSize);
        _device->copyBuffer(intermediateBuffer,
                            val::EResourceStorageModeShared,
                            0,
                            dx12Mesh->uvBuffer,
                            val::EResourceStorageModePrivate,
                            0,
                            texcoordBufferSize,
                            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        // Initialize the uv buffer view.
        dx12Mesh->uvBufferView.BufferLocation = dx12Mesh->uvBuffer->GetGPUVirtualAddress();
        dx12Mesh->uvBufferView.StrideInBytes  = XPMeshBuffer::sizeofTexcoords();
        dx12Mesh->uvBufferView.SizeInBytes    = texcoordBufferSize;
    }
    {
        auto intermediateBuffer =
          _device->createVertexBuffer(L"IntermediateIndexBuffer", indexBufferSize, val::EResourceStorageModeShared);
        _device->copyBuffer((void*)indices, 0, intermediateBuffer, val::EResourceStorageModeShared, 0, indexBufferSize);
        _device->copyBuffer(intermediateBuffer,
                            val::EResourceStorageModeShared,
                            0,
                            dx12Mesh->indexBuffer,
                            val::EResourceStorageModePrivate,
                            0,
                            indexBufferSize,
                            D3D12_RESOURCE_STATE_INDEX_BUFFER);

        // Initialize the vertex buffer view.
        dx12Mesh->indexBufferView.BufferLocation = dx12Mesh->indexBuffer->GetGPUVirtualAddress();
        dx12Mesh->indexBufferView.Format         = DXGI_FORMAT_R32_UINT;
        dx12Mesh->indexBufferView.SizeInBytes    = indexBufferSize;
    }

    auto dx12MeshRef = dx12Mesh.get();
    meshAsset->setGPURef((void*)dx12MeshRef);
    dx12Mesh->meshAsset = meshAsset;

    _meshMap.insert({ meshAsset->getFile()->getPath(), std::move(dx12Mesh) });
    for (size_t mboi = 0; mboi < meshBuffer->getObjectsCount(); ++mboi) {
        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(mboi);
        auto                dx12MeshObject =
          std::make_unique<XPDX12MeshObject>((XPDX12Mesh&)*dx12MeshRef, meshBufferObject.boundingBox);
        dx12MeshObject->name         = meshBufferObject.name;
        dx12MeshObject->vertexOffset = meshBufferObject.vertexOffset;
        dx12MeshObject->indexOffset  = meshBufferObject.indexOffset;
        dx12MeshObject->numIndices   = meshBufferObject.numIndices;
        dx12MeshRef->objects.push_back(*dx12MeshObject.get());
        _meshObjectMap.insert({ dx12MeshObject->name, std::move(dx12MeshObject) });
    }
}

XPProfilable void
XPDX12Renderer::uploadShaderAsset(XPShaderAsset* shaderAsset)
{
    XP_UNIMPLEMENTED("XPDX12Renderer::uploadShaderAsset unimplememted");
}

XPProfilable void
XPDX12Renderer::uploadTextureAsset(XPTextureAsset* textureAsset)
{
    XP_UNIMPLEMENTED("XPDX12Renderer::uploadTextureAsset unimplememted");
}

XPProfilable void
XPDX12Renderer::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
    XP_UNIMPLEMENTED("XPDX12Renderer::reUploadMeshAsset unimplememted");
}

XPProfilable void
XPDX12Renderer::reUploadShaderAsset(XPShaderAsset* shaderAsset)
{
    XP_UNIMPLEMENTED("XPDX12Renderer::reUploadShaderAsset unimplememted");
}

XPProfilable void
XPDX12Renderer::reUploadTextureAsset(XPTextureAsset* textureAsset)
{
    XP_UNIMPLEMENTED("XPDX12Renderer::reUploadTextureAsset unimplememted");
}

XPRegistry*
XPDX12Renderer::getRegistry()
{
    return _registry;
}

void
XPDX12Renderer::getSelectedNodeFromViewport(XPVec2<float> coordinates, const std::function<void(XPNode*)>&)
{
    // XP_UNIMPLEMENTED("XPDX12Renderer::getSelectedNodeFromViewport unimplememted");
}

XPVec2<int>
XPDX12Renderer::getWindowSize()
{
    return _window->getWindowSize();
}

XPVec2<int>
XPDX12Renderer::getResolution()
{
    return _resolution;
}

XPVec2<float>
XPDX12Renderer::getMouseLocation()
{
    return _window->getMouseLocation();
}

void*
XPDX12Renderer::getMainTexture()
{
    return nullptr;
}

XPVec2<float>
XPDX12Renderer::getNormalizedMouseLocation()
{
    return _window->getNormalizedMouseLocation();
}

bool
XPDX12Renderer::isLeftMouseButtonPressed()
{
    return _window->isLeftMouseButtonPressed();
}

bool
XPDX12Renderer::isMiddleMouseButtonPressed()
{
    return _window->isMiddleMouseButtonPressed();
}

bool
XPDX12Renderer::isRightMouseButtonPressed()
{
    return _window->isRightMouseButtonPressed();
}

float
XPDX12Renderer::getDeltaTime()
{
    return _window->getDeltaTimeSeconds();
}

uint32_t
XPDX12Renderer::getNumDrawCallsVertices()
{
    return _gpuData->numDrawCallsVertices;
}

uint32_t
XPDX12Renderer::getTotalNumDrawCallsVertices()
{
    return _gpuData->numTotalDrawCallsVertices;
}

uint32_t
XPDX12Renderer::getNumDrawCalls()
{
    return _gpuData->numDrawCalls;
}

uint32_t
XPDX12Renderer::getTotalNumDrawCalls()
{
    return _gpuData->numTotalDrawCalls;
}

bool
XPDX12Renderer::isCapturingDebugFrames()
{
    return _isCapturingDebugFrames;
}

bool
XPDX12Renderer::isFramebufferResized()
{
    return _isFramebufferResized;
}

float
XPDX12Renderer::getRenderingGPUTime()
{
    return _renderingGpuTime;
}

float
XPDX12Renderer::getUIGPUTime()
{
    return _uiGpuTime;
}

float
XPDX12Renderer::getComputeGPUTime()
{
    return _computeGpuTime;
}

void
XPDX12Renderer::captureDebugFrames()
{
    _isCapturingDebugFrames = true;
}

void
XPDX12Renderer::setFramebufferResized()
{
    _isFramebufferResized = true;
}

void
XPDX12Renderer::simulateCopy(const char* text)
{
    _window->simulateCopy(text);
}

std::string
XPDX12Renderer::simulatePaste()
{
    return _window->simulatePaste();
}

XPProfilable void
XPDX12Renderer::compileLoadScene(XPScene& scene)
{
    auto& meshNodes = scene.getNodes(MeshRendererAttachmentDescriptor | TransformAttachmentDescriptor);

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
            XPDX12MeshObject* meshObject                         = _meshObjectMap[subMesh.mesh.text].get();
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

XPProfilable void
XPDX12Renderer::recordCommandList(XPScene&                   scene,
                                  FreeCamera&                camera,
                                  uint32_t                   randomImageIndex,
                                  ID3D12GraphicsCommandList* commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    bool frustumCullingEnabled     = false;
    _gpuData->numDrawCallsVertices = 0;
    _gpuData->numDrawCalls         = 0;
    for (size_t subMeshIndex = 0; subMeshIndex < _gpuData->numSubMeshes; ++subMeshIndex) {
        const XPDX12MeshObject& meshObject = *_gpuData->meshObjects[subMeshIndex];
        if ((!frustumCullingEnabled || (frustumCullingEnabled && meshObject.boundingBox.minPoint.w == 1))) {
            XPMat4<float>& modelMatrix  = _gpuData->modelMatrices[_gpuData->perMeshNodeIndices[subMeshIndex]];
            MeshMatrices&  meshMatrices = _meshMatricesCBData[_synchronization->getFrameIndex()];
            memcpy(meshMatrices.modelMatrix.data(), modelMatrix.arr.data(), sizeof(float4x4));
            glm::mat4* mat = (glm::mat4*)(&meshMatrices.modelMatrix[0]);
            *mat           = glm::transpose(*mat);
            commandList->SetGraphicsRoot32BitConstants(1, 16, &meshMatrices.modelMatrix[0], 0);

            UINT64 vertexBufferOffset = (UINT64)XPMeshBuffer::sizeofPositions() * (UINT64)meshObject.vertexOffset;
            D3D12_VERTEX_BUFFER_VIEW vertexBufferView = meshObject.mesh.vertexBufferView;
            vertexBufferView.BufferLocation += vertexBufferOffset;
            vertexBufferView.SizeInBytes -= vertexBufferOffset;

            UINT64 normalBufferOffset = (UINT64)XPMeshBuffer::sizeofNormals() * (UINT64)meshObject.vertexOffset;
            D3D12_VERTEX_BUFFER_VIEW normalBufferView = meshObject.mesh.normalBufferView;
            normalBufferView.BufferLocation += normalBufferOffset;
            normalBufferView.SizeInBytes -= normalBufferOffset;

            UINT64 uvBufferOffset = (UINT64)XPMeshBuffer::sizeofTexcoords() * (UINT64)meshObject.vertexOffset;
            D3D12_VERTEX_BUFFER_VIEW uvBufferView = meshObject.mesh.uvBufferView;
            uvBufferView.BufferLocation += uvBufferOffset;
            uvBufferView.SizeInBytes -= uvBufferOffset;

            const D3D12_VERTEX_BUFFER_VIEW pViews[] = { vertexBufferView, normalBufferView, uvBufferView };
            commandList->IASetVertexBuffers(0, 3, pViews);

            UINT64 indexBufferOffset = (UINT64)XPMeshBuffer::sizeofIndices() * (UINT64)meshObject.indexOffset;
            D3D12_INDEX_BUFFER_VIEW indexBufferView = meshObject.mesh.indexBufferView;
            indexBufferView.BufferLocation += indexBufferOffset;
            indexBufferView.SizeInBytes -= indexBufferOffset;

            commandList->IASetIndexBuffer(&indexBufferView);
            commandList->DrawIndexedInstanced(meshObject.numIndices, 1, 0, 0, 0);

            ++_gpuData->numDrawCalls;
            _gpuData->numDrawCallsVertices += meshObject.numIndices;
        }
    }
}
