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

class DescriptorHeap;

class ConstantBuffer
{
  public:
    ConstantBuffer();
    ~ConstantBuffer();
    void            create(ID3D12Device*    device,
                           DescriptorHeap*  descHeap,
                           UINT             descHeapOffset,
                           void*            data,
                           const size_t     numBytes,
                           D3D12_HEAP_FLAGS miscFlag = D3D12_HEAP_FLAG_NONE);
    void            destroy();
    void            copyToGPU(const SIZE_T frameIndex, void* ptr, size_t numBytes);
    ID3D12Resource* getBuffer(const SIZE_T frameIndex) const;
    UINT64          getConstDataSizeAligned() const;

  private:
    Microsoft::WRL::ComPtr<ID3D12Resource> _buffer[XP_DX12_BUFFER_COUNT];
    uint8_t*                               _data[XP_DX12_BUFFER_COUNT];
    UINT64                                 _numBytes;
};

} // namespace dx12al
