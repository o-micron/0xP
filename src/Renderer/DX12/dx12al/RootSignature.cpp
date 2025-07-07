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

#include <Renderer/DX12/dx12al/RootSignature.hpp>

namespace dx12al {

RootSignature::RootSignature() {}

RootSignature::~RootSignature() {}

void
RootSignature::create(ID3D12Device* device)
{
    // D3D12_ROOT_PARAMETER rootParams[2];

    // // model matrices
    // rootParams[0].ParameterType            = D3D12_ROOT_PARAMETER_TYPE_CBV;
    // rootParams[0].ShaderVisibility         = D3D12_SHADER_VISIBILITY_ALL;
    // rootParams[0].                         = 0;
    // rootParams[0].Descriptor.RegisterSpace = 0;

    // // camera matrices
    // rootParams[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    // rootParams[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    // rootParams[1].Descriptor.ShaderRegister = 1;
    // rootParams[1].Descriptor.RegisterSpace  = 0;

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
    // will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if ((device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))) != S_OK) {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    D3D12_ROOT_PARAMETER   rootParams[2];
    D3D12_DESCRIPTOR_RANGE descriptorRanges[2];

    // table for CBV
    {
        descriptorRanges[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        descriptorRanges[0].NumDescriptors                    = 1;
        descriptorRanges[0].BaseShaderRegister                = 0;
        descriptorRanges[0].RegisterSpace                     = 0;
        descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        rootParams[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParams[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
        rootParams[0].DescriptorTable.pDescriptorRanges   = &descriptorRanges[0];
    }
    // 32-bit constants
    {
        descriptorRanges[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        descriptorRanges[1].NumDescriptors                    = 1;
        descriptorRanges[1].BaseShaderRegister                = 1;
        descriptorRanges[1].RegisterSpace                     = 0;
        descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        rootParams[1].ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        rootParams[1].ShaderVisibility         = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParams[1].Constants.Num32BitValues = 16;
        rootParams[1].Constants.ShaderRegister = 1;
        rootParams[1].Constants.RegisterSpace  = 0;
    }

    D3D12_ROOT_SIGNATURE_DESC descRootSignature = D3D12_ROOT_SIGNATURE_DESC();
    descRootSignature.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                              D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                              D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                              D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                              D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    descRootSignature.NumParameters     = 2;
    descRootSignature.pParameters       = &rootParams[0];
    descRootSignature.NumStaticSamplers = 0;
    descRootSignature.pStaticSamplers   = nullptr;

    DX12_ASSERT(D3D12SerializeRootSignature(
      &descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, _blob.GetAddressOf(), _errorBlob.GetAddressOf()));
    DX12_ASSERT(device->CreateRootSignature(0,
                                            _blob->GetBufferPointer(),
                                            _blob->GetBufferSize(),
                                            __uuidof(ID3D12RootSignature),
                                            (void**)_rootSignature.GetAddressOf()));
}

void
RootSignature::destroy()
{
    _errorBlob.Reset();
    _blob.Reset();
    _rootSignature.Reset();
}

ID3D12RootSignature*
RootSignature::getSignature() const
{
    return _rootSignature.Get();
}

ID3DBlob*
RootSignature::getBlob() const
{
    return _blob.Get();
}

ID3DBlob*
RootSignature::getErrorBlob() const
{
    return _errorBlob.Get();
}

} // namespace dx12al
