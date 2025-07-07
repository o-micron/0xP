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

#include <Renderer/DX12/dx12al/ConstantBuffer.hpp>
#include <Renderer/DX12/dx12al/DescriptorHeap.hpp>
namespace dx12al {

ConstantBuffer::ConstantBuffer() {}

ConstantBuffer::~ConstantBuffer() {}

void
ConstantBuffer::create(ID3D12Device*    device,
                       DescriptorHeap*  descHeap,
                       UINT             descHeapOffset,
                       void*            data,
                       const size_t     numBytes,
                       D3D12_HEAP_FLAGS miscFlag)
{
    _numBytes = ((numBytes + 255) & ~255);
    for (SIZE_T i = 0; i < XP_DX12_BUFFER_COUNT; ++i) {
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.Type                  = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CreationNodeMask      = 1;
        heapProps.VisibleNodeMask       = 1;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Alignment           = 0;
        bufferDesc.Width               = numBytes;
        bufferDesc.Height              = 1;
        bufferDesc.DepthOrArraySize    = 1;
        bufferDesc.MipLevels           = 1;
        bufferDesc.Format              = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count    = 1;
        bufferDesc.SampleDesc.Quality  = 0;
        bufferDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

        // create buffer resource on it's own individual upload heap
        DX12_ASSERT(device->CreateCommittedResource(
          &heapProps, miscFlag, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_buffer[i])));

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation                  = _buffer[i]->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes                     = numBytes;
        device->CreateConstantBufferView(&cbvDesc, descHeap->getCpuHandle(i, descHeapOffset));

        // Map and initialize the constant buffer. We don't unmap this until the
        // app closes. Keeping things mapped for the lifetime of the resource is okay.
        DX12_ASSERT(_buffer[i]->Map(0, nullptr, (void**)&_data[i]));
        memset(_data[i], 0, numBytes);
        if (data != nullptr) { memcpy(_data[i], data, numBytes); }
    }
}
void
ConstantBuffer::destroy()
{
    for (SIZE_T i = 0; i < XP_DX12_BUFFER_COUNT; ++i) {
        _buffer[i]->Unmap(0, nullptr);
        _buffer[i].Reset();
    }
}

void
ConstantBuffer::copyToGPU(const SIZE_T frameIndex, void* ptr, size_t numBytes)
{
    assert(_numBytes >= numBytes);
    memcpy(_data[frameIndex], ptr, numBytes);
}

ID3D12Resource*
ConstantBuffer::getBuffer(const SIZE_T frameIndex) const
{
    return _buffer[frameIndex].Get();
}

} // namespace dx12al
