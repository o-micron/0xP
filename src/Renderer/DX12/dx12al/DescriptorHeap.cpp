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

#include <Renderer/DX12/dx12al/DescriptorHeap.hpp>

namespace dx12al {

DescriptorHeap::DescriptorHeap() {}

DescriptorHeap::~DescriptorHeap() {}

void
DescriptorHeap::create(ID3D12Device*              device,
                       const wchar_t*             debugLabel,
                       D3D12_DESCRIPTOR_HEAP_TYPE Type,
                       UINT                       NumDescriptors,
                       bool                       bShaderVisible)
{
    _desc                = {};
    _desc.Type           = Type;
    _desc.NumDescriptors = NumDescriptors;
    _desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    for (UINT i = 0; i < XP_DX12_BUFFER_COUNT; ++i) {
        DX12_ASSERT(device->CreateDescriptorHeap(&_desc, IID_PPV_ARGS(&_heap[i])));
        NAME_D3D12_OBJECT_INDEXED(_heap, debugLabel, i);
    }

    _incrementSize = device->GetDescriptorHandleIncrementSize(Type);
}

void
DescriptorHeap::destroy()
{
    for (UINT i = 0; i < XP_DX12_BUFFER_COUNT; ++i) { _heap[i].Reset(); }
}

ID3D12DescriptorHeap*
DescriptorHeap::getHeap(UINT frameIndex) const
{
    return _heap[frameIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE
DescriptorHeap::getCpuHandle(UINT frameIndex, UINT offset)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _heap[frameIndex]->GetCPUDescriptorHandleForHeapStart();
    handle.ptr                         = SIZE_T(INT64(handle.ptr) + INT64(offset) * INT64(_incrementSize));
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE
DescriptorHeap::getGpuHandle(UINT frameIndex, UINT offset)
{
    assert(_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    D3D12_GPU_DESCRIPTOR_HANDLE handle = _heap[frameIndex]->GetGPUDescriptorHandleForHeapStart();
    handle.ptr                         = SIZE_T(INT64(handle.ptr) + INT64(offset) * INT64(_incrementSize));
    return handle;
}

} // namespace dx12al
