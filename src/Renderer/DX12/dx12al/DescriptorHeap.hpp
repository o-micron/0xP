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

#include <Renderer/DX12/XPDX12.h>

namespace dx12al {

class DescriptorHeap
{
  public:
    DescriptorHeap();
    ~DescriptorHeap();
    void                        create(ID3D12Device*              device,
                                       const wchar_t*             debugLabel,
                                       D3D12_DESCRIPTOR_HEAP_TYPE Type,
                                       UINT                       NumDescriptors,
                                       bool                       bShaderVisible = false);
    void                        destroy();
    ID3D12DescriptorHeap*       getHeap(UINT frameIndex) const;
    D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(UINT frameIndex, UINT offset);
    D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(UINT frameIndex, UINT offset);

  private:
    D3D12_DESCRIPTOR_HEAP_DESC                                                     _desc;
    std::array<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>, XP_DX12_BUFFER_COUNT> _heap;
    UINT                                                                           _incrementSize;
};

} // namespace dx12al
