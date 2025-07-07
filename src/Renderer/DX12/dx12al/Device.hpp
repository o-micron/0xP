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
#include <Renderer/DX12/dx12al/Enums.h>

class XPDX12Renderer;

namespace dx12al {

class Device
{
  public:
    Device(XPDX12Renderer* renderer);
    ~Device();
    void create(int adapterIndex, int outputIndex);
    void destroy();

    IDXGIFactory4* getFactory() const;
    ID3D12Device*  getDevice() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> createVertexBuffer(const wchar_t*            label,
                                                              size_t                    numBytes,
                                                              val::EResourceStorageMode storageMode);
    Microsoft::WRL::ComPtr<ID3D12Resource> createIndexBuffer(const wchar_t*            label,
                                                             size_t                    numBytes,
                                                             val::EResourceStorageMode storageMode);
    void                                   copyBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> srcBuffer,
                                                      val::EResourceStorageMode              srcStorageMode,
                                                      size_t                                 srcOffset,
                                                      Microsoft::WRL::ComPtr<ID3D12Resource> dstBuffer,
                                                      val::EResourceStorageMode              dstStorageMode,
                                                      size_t                                 dstOffset,
                                                      size_t                                 numBytes,
                                                      D3D12_RESOURCE_STATES                  statesAfter);
    void                                   copyBuffer(void*                                  srcData,
                                                      size_t                                 srcOffset,
                                                      Microsoft::WRL::ComPtr<ID3D12Resource> dstBuffer,
                                                      val::EResourceStorageMode              dstStorageMode,
                                                      size_t                                 dstOffset,
                                                      size_t                                 numBytes);

  private:
    Microsoft::WRL::ComPtr<ID3D12Resource> createBuffer(const wchar_t*            label,
                                                        size_t                    numBytes,
                                                        val::EResourceStorageMode storageMode);
    XPDX12Renderer*                        _renderer;
    Microsoft::WRL::ComPtr<IDXGIFactory4>  _factory4;
    Microsoft::WRL::ComPtr<ID3D12Device>   _device;
};

} // namespace dx12al