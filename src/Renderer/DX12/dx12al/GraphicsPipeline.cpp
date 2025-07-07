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

#include <Renderer/DX12/dx12al/GraphicsPipeline.hpp>

namespace dx12al {

GraphicsPipeline::GraphicsPipeline() {}

GraphicsPipeline::~GraphicsPipeline() {}

void
GraphicsPipeline::create(ID3D12Device* device, const char* vsfilename, const char* psfilename)
{
    _gBufferRootSignature.create(device);
    _gBufferVS.create(vsfilename);
    _gBufferPS.create(psfilename);

    static D3D12_INPUT_ELEMENT_DESC elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    static D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.NumElements        = sizeof(elements) / sizeof(elements[0]);
    inputLayoutDesc.pInputElementDescs = elements;

    ID3DBlob*                          vsBlob  = _gBufferVS.getBlob();
    ID3DBlob*                          psBlob  = _gBufferPS.getBlob();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout                        = inputLayoutDesc;
    psoDesc.pRootSignature                     = _gBufferRootSignature.getSignature();
    psoDesc.VS = { reinterpret_cast<BYTE*>(vsBlob->GetBufferPointer()), vsBlob->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(psBlob->GetBufferPointer()), psBlob->GetBufferSize() };

    psoDesc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
    psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
    psoDesc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
    psoDesc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    psoDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    psoDesc.RasterizerState.DepthClipEnable       = TRUE;
    psoDesc.RasterizerState.MultisampleEnable     = FALSE;
    psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
    psoDesc.RasterizerState.ForcedSampleCount     = 0;
    psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    psoDesc.BlendState.AlphaToCoverageEnable                          = FALSE;
    psoDesc.BlendState.IndependentBlendEnable                         = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
        FALSE,
        FALSE,
        D3D12_BLEND_ONE,
        D3D12_BLEND_ZERO,
        D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE,
        D3D12_BLEND_ZERO,
        D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;
    }

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable              = TRUE;
    depthStencilDesc.DepthWriteMask           = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc                = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable            = FALSE;

    psoDesc.SampleMask            = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.DepthStencilState     = depthStencilDesc;
    psoDesc.SampleDesc.Count      = 1;
    psoDesc.NumRenderTargets      = 1;
    psoDesc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat             = DXGI_FORMAT_D32_FLOAT;

    _gBufferPSO.create(device, psoDesc);
}

void
GraphicsPipeline::destroy()
{
    _gBufferPSO.destroy();
    _gBufferPS.destroy();
    _gBufferVS.destroy();
    _gBufferRootSignature.destroy();
}

const RootSignature&
GraphicsPipeline::getRootSignature() const
{
    return _gBufferRootSignature;
}

const PipelineStateObject&
GraphicsPipeline::getPSO() const
{
    return _gBufferPSO;
}

const Shader&
GraphicsPipeline::getVertexShader() const
{
    return _gBufferVS;
}

const Shader&
GraphicsPipeline::getPixelShader() const
{
    return _gBufferPS;
}

} // namespace dx12al
