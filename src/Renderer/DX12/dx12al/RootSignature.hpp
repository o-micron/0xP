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

class RootSignature
{
  public:
    RootSignature();
    ~RootSignature();
    void                 create(ID3D12Device* device);
    void                 destroy();
    ID3D12RootSignature* getSignature() const;
    ID3DBlob*            getBlob() const;
    ID3DBlob*            getErrorBlob() const;

  private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
    Microsoft::WRL::ComPtr<ID3DBlob>            _blob;
    Microsoft::WRL::ComPtr<ID3DBlob>            _errorBlob;
};

} // namespace dx12al
