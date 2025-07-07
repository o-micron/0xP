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

#include <Renderer/DX12/dx12al/Device.hpp>

#include <Renderer/DX12/XPDX12Renderer.h>

#include <D3Dcompiler.h>
#include <Windows.h>
#include <dxgi1_6.h>

namespace dx12al {

Device::Device(XPDX12Renderer* renderer)
  : _renderer(renderer)
{
}

Device::~Device() {}

void
Device::create(int adapterIndex, int outputIndex)
{
    bool requestHighPerformanceAdapter = true;
    UINT dxgiFactoryFlags              = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ID3D12Debug* debugController;
        if (DX12_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
    DX12_ASSERT(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_factory4)));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
    if (DX12_CHECK(_factory4->QueryInterface(IID_PPV_ARGS(&factory6)))) {
        if (DX12_CHECK(factory6->EnumAdapterByGpuPreference(adapterIndex,
                                                            requestHighPerformanceAdapter == true
                                                              ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                                                              : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                                                            IID_PPV_ARGS(&adapter)))) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            DX12_ASSERT(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)));
        }
    }
}

void
Device::destroy()
{
    _device.Reset();
}

IDXGIFactory4*
Device::getFactory() const
{
    return _factory4.Get();
}

ID3D12Device*
Device::getDevice() const
{
    return _device.Get();
}

Microsoft::WRL::ComPtr<ID3D12Resource>
Device::createBuffer(const wchar_t* label, size_t numBytes, val::EResourceStorageMode storageMode)
{
    // for now only handle cases where private or shared buffer only
    assert(storageMode == val::EResourceStorageModeShared || storageMode == val::EResourceStorageModePrivate);

    Microsoft::WRL::ComPtr<ID3D12Resource> resource;

    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type =
      storageMode == val::EResourceStorageModePrivate ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD;
    properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    properties.CreationNodeMask     = 1;
    properties.VisibleNodeMask      = 1;

    D3D12_RESOURCE_DESC desc = {};
    desc.Width               = numBytes;
    desc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment           = 0;
    desc.Height              = 1;
    desc.DepthOrArraySize    = 1;
    desc.MipLevels           = 1;
    desc.Format              = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count    = 1;
    desc.SampleDesc.Quality  = 0;
    desc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    DX12_ASSERT(_device->CreateCommittedResource(
      &properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource)));
    NAME_D3D12_OBJECT(resource, label);

    return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource>
Device::createVertexBuffer(const wchar_t* label, size_t numBytes, val::EResourceStorageMode storageMode)
{
    return createBuffer(label, numBytes, storageMode);
}

Microsoft::WRL::ComPtr<ID3D12Resource>
Device::createIndexBuffer(const wchar_t* label, size_t numBytes, val::EResourceStorageMode storageMode)
{
    return createBuffer(label, numBytes, storageMode);
}

void
Device::copyBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> srcBuffer,
                   val::EResourceStorageMode              srcStorageMode,
                   size_t                                 srcOffset,
                   Microsoft::WRL::ComPtr<ID3D12Resource> dstBuffer,
                   val::EResourceStorageMode              dstStorageMode,
                   size_t                                 dstOffset,
                   size_t                                 numBytes,
                   D3D12_RESOURCE_STATES                  statesAfter)
{
    // for now, handle only case of copy from shared to private buffers
    assert(srcStorageMode == val::EResourceStorageModeShared && dstStorageMode == val::EResourceStorageModePrivate);

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    intermediateCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> intermediateCommandList;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>        intermediateCommandQueue;

    // create intermediate data
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc;
        ZeroMemory(&queueDesc, sizeof(queueDesc));
        queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;

        DX12_ASSERT(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&intermediateCommandQueue)));
        NAME_D3D12_OBJECT(intermediateCommandQueue, L"IntermediateCommandQueue");

        DX12_ASSERT(
          _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&intermediateCommandAllocator)));
        NAME_D3D12_OBJECT(intermediateCommandAllocator, L"IntermediateCommandAllocator");

        DX12_ASSERT(_device->CreateCommandList(0,
                                               D3D12_COMMAND_LIST_TYPE_DIRECT,
                                               intermediateCommandAllocator.Get(),
                                               nullptr,
                                               IID_PPV_ARGS(&intermediateCommandList)));
        NAME_D3D12_OBJECT(intermediateCommandList, L"IntermediateCommandList");
    }

    intermediateCommandList->CopyBufferRegion(dstBuffer.Get(), dstOffset, srcBuffer.Get(), srcOffset, numBytes);

    D3D12_RESOURCE_BARRIER barrierDesc;
    ZeroMemory(&barrierDesc, sizeof(barrierDesc));
    barrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.pResource   = dstBuffer.Get();
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrierDesc.Transition.StateAfter  = statesAfter;
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    intermediateCommandList->ResourceBarrier(1, &barrierDesc);

    intermediateCommandList->Close();

    std::vector<ID3D12CommandList*> ppCommandLists{ intermediateCommandList.Get() };
    intermediateCommandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());

    UINT64                              initialValue{ 0 };
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    DX12_ASSERT(_device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    HANDLE fenceEventHandle{ CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS) };
    DX12_ASSERT(intermediateCommandQueue->Signal(fence.Get(), 1));
    DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEventHandle));
    WaitForSingleObject(fenceEventHandle, INFINITE);
    CloseHandle(fenceEventHandle);
}

void
Device::copyBuffer(void*                                  srcData,
                   size_t                                 srcOffset,
                   Microsoft::WRL::ComPtr<ID3D12Resource> dstBuffer,
                   val::EResourceStorageMode              dstStorageMode,
                   size_t                                 dstOffset,
                   size_t                                 numBytes)
{
    assert(dstStorageMode == val::EResourceStorageModeShared);

    void* pData;
    DX12_ASSERT(dstBuffer->Map(0, NULL, &pData));
    memcpy(pData, (void*)((char*)srcData + srcOffset), numBytes);
    dstBuffer->Unmap(0, NULL);
}

} // namespace dx12al