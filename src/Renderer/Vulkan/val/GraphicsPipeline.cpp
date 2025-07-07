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

#include <Renderer/Vulkan/val/GraphicsPipeline.hpp>

#include <Renderer/Vulkan/val/Defs.h>
#include <Renderer/Vulkan/val/DescriptorSet.h>
#include <Renderer/Vulkan/val/Enums.h>
#include <Renderer/Vulkan/val/PhysicalDevice.hpp>
#include <Renderer/Vulkan/val/PushConstantData.h>
#include <Renderer/Vulkan/val/Shader.hpp>

#include <DataPipeline/XPShaderBuffer.h>
#include <Utilities/XPLogger.h>
#include <Utilities/XPMaths.h>

namespace val {

GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo(VkDevice                   device,
                                                       const val::PhysicalDevice& physicalDevice,
                                                       VkExtent2D                 swapchainExtent,
                                                       const XPShaderBuffer*      shaderBuffer)
  : device(device)
  , physicalDevice(physicalDevice)
  , swapchainExtent(swapchainExtent)
  , layout(nullptr)
{
    shader = std::make_unique<val::VertexFragmentShader>(device, *shaderBuffer);

    create();
}

GraphicsPipelineCreateInfo::~GraphicsPipelineCreateInfo() { destroy(); }

void
GraphicsPipelineCreateInfo::create()
{
    if (layout != nullptr) {
        XP_UNIMPLEMENTED("Unreachable");
        return;
    }

    colorAttachmentFormats.clear();
    const auto& fsOutputFormats = shader->getFSOutputFormats();
    assert(fsOutputFormats.size() >= 1 && "At least there should be a single color attachment output format");
    for (const auto& it : fsOutputFormats) { colorAttachmentFormats.push_back(it.second); }

    pipelineRenderingCreateInfo                         = {};
    pipelineRenderingCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipelineRenderingCreateInfo.pNext                   = nullptr;
    pipelineRenderingCreateInfo.colorAttachmentCount    = static_cast<uint32_t>(colorAttachmentFormats.size());
    pipelineRenderingCreateInfo.pColorAttachmentFormats = (VkFormat*)colorAttachmentFormats.data();
    pipelineRenderingCreateInfo.depthAttachmentFormat   = physicalDevice.findSupportedDepthFormat();

    inputAssemblyStateCreateInfo                        = {};
    inputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext                  = nullptr;
    inputAssemblyStateCreateInfo.flags                  = 0;
    inputAssemblyStateCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    rasterizationStateCreateInfo                         = {};
    rasterizationStateCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext                   = nullptr;
    rasterizationStateCreateInfo.flags                   = 0;
    rasterizationStateCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth               = 1.0f;
    rasterizationStateCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp          = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor    = 0.0f;

    colorBlendAttachmentState.clear();
    colorBlendAttachmentState.resize(shader->getFSOutputFormats().size());
    for (uint32_t i = 0; i < colorBlendAttachmentState.size(); ++i) {
        colorBlendAttachmentState[i].colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState[i].blendEnable         = VK_FALSE;
        colorBlendAttachmentState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[i].colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachmentState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[i].alphaBlendOp        = VK_BLEND_OP_ADD;
    }

    colorBlendStateCreateInfo                   = {};
    colorBlendStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext             = nullptr;
    colorBlendStateCreateInfo.flags             = 0;
    colorBlendStateCreateInfo.logicOpEnable     = VK_FALSE;
    colorBlendStateCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount   = (uint32_t)colorBlendAttachmentState.size();
    colorBlendStateCreateInfo.pAttachments      = colorBlendAttachmentState.data();
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    depthStencilStateCreateInfo                       = {};
    depthStencilStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext                 = nullptr;
    depthStencilStateCreateInfo.flags                 = 0;
    depthStencilStateCreateInfo.depthTestEnable       = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable      = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds        = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds        = 1.0f;
    depthStencilStateCreateInfo.stencilTestEnable     = VK_FALSE;
    depthStencilStateCreateInfo.front                 = {};
    depthStencilStateCreateInfo.back                  = {};

    viewport          = {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)swapchainExtent.width;
    viewport.height   = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor        = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchainExtent;

    viewportStateCreateInfo               = {};
    viewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext         = nullptr;
    viewportStateCreateInfo.flags         = 0;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports    = &viewport;
    viewportStateCreateInfo.scissorCount  = 1;
    viewportStateCreateInfo.pScissors     = &scissor;

    multisampleStateCreateInfo                       = {};
    multisampleStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext                 = nullptr;
    multisampleStateCreateInfo.flags                 = 0;
    multisampleStateCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.alphaToOneEnable      = VK_FALSE;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.sampleShadingEnable   = VK_FALSE;

    dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    dynamicStateCreateInfo                   = {};
    dynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext             = nullptr;
    dynamicStateCreateInfo.flags             = 0;
    dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates    = dynamicStates.data();

    // shader ---------------------------------------------------------------------------------------------------------
    // shader modules
    shaderStageCreateInfos[0]        = {};
    shaderStageCreateInfos[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfos[0].pNext  = nullptr;
    shaderStageCreateInfos[0].flags  = 0;
    shaderStageCreateInfos[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfos[0].module = shader->getVertexShaderModule();
    shaderStageCreateInfos[0].pName  = "main";

    shaderStageCreateInfos[1]        = {};
    shaderStageCreateInfos[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfos[1].pNext  = nullptr;
    shaderStageCreateInfos[1].flags  = 0;
    shaderStageCreateInfos[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfos[1].module = shader->getFragmentShaderModule();
    shaderStageCreateInfos[1].pName  = "main";

    // vertex binding
    // vertexInputBindingDescriptions.resize(3);
    // vertexInputBindingDescriptions[0]           = {};
    // vertexInputBindingDescriptions[0].binding   = 0;
    // vertexInputBindingDescriptions[0].stride    = sizeof(XPVec4<float>);
    // vertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    // vertexInputBindingDescriptions[1]           = {};
    // vertexInputBindingDescriptions[1].binding   = 1;
    // vertexInputBindingDescriptions[1].stride    = sizeof(XPVec4<float>);
    // vertexInputBindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    // vertexInputBindingDescriptions[2]           = {};
    // vertexInputBindingDescriptions[2].binding   = 2;
    // vertexInputBindingDescriptions[2].stride    = sizeof(XPVec4<float>);
    // vertexInputBindingDescriptions[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // // vertex attributes
    // vertexAttributeDescriptions.resize(3);
    // vertexAttributeDescriptions[0]          = {};
    // vertexAttributeDescriptions[0].binding  = 0;
    // vertexAttributeDescriptions[0].location = 0;
    // vertexAttributeDescriptions[0].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    // vertexAttributeDescriptions[0].offset   = (uint32_t)0;
    // vertexAttributeDescriptions[1]          = {};
    // vertexAttributeDescriptions[1].binding  = 1;
    // vertexAttributeDescriptions[1].location = 1;
    // vertexAttributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    // vertexAttributeDescriptions[1].offset   = (uint32_t)0;
    // vertexAttributeDescriptions[2]          = {};
    // vertexAttributeDescriptions[2].binding  = 2;
    // vertexAttributeDescriptions[2].location = 2;
    // vertexAttributeDescriptions[2].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    // vertexAttributeDescriptions[2].offset   = (uint32_t)0;

    vertexAttributes.clear();
    vertexAttributes.reserve(shader->getVSInputAttributes().size());
    for (const auto& pair : shader->getVSInputAttributes()) { vertexAttributes.push_back(pair.second); }

    vertexBindings.clear();
    vertexBindings.reserve(shader->getVSInputBindings().size());
    for (const auto& pair : shader->getVSInputBindings()) { vertexBindings.push_back(pair.second); }

    vertexInputStateCreateInfo       = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount   = (uint32_t)vertexBindings.size();
    vertexInputStateCreateInfo.pVertexBindingDescriptions      = vertexBindings.data();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions    = vertexAttributes.data();

    pushConstantRanges.clear();
    pushConstantRanges.reserve(shader->getPushConstantRanges().size());
    for (const auto& pair : shader->getPushConstantRanges()) { pushConstantRanges.push_back(pair.second); }

    // layout create info
    layoutCreateInfo                        = {};
    layoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext                  = nullptr;
    layoutCreateInfo.flags                  = 0;
    layoutCreateInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
    layoutCreateInfo.pPushConstantRanges    = pushConstantRanges.data();
    layoutCreateInfo.setLayoutCount         = (uint32_t)shader->getDescriptorSetLayouts().size();
    layoutCreateInfo.pSetLayouts            = shader->getDescriptorSetLayouts().data();

    VULKAN_ASSERT(vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &layout),
                  "Failed to create graphics pipeline layout");

    // graphics pipeline createe info
    graphicsPipelineCreateInfo                     = {};
    graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext               = &pipelineRenderingCreateInfo;
    graphicsPipelineCreateInfo.flags               = 0;
    graphicsPipelineCreateInfo.layout              = layout;
    graphicsPipelineCreateInfo.renderPass          = nullptr;
    graphicsPipelineCreateInfo.basePipelineIndex   = -1;
    graphicsPipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState    = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState   = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState      = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState  = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState       = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.stageCount          = (uint32_t)shaderStageCreateInfos.size();
    graphicsPipelineCreateInfo.pStages             = shaderStageCreateInfos.data();
    graphicsPipelineCreateInfo.subpass             = 0;
    graphicsPipelineCreateInfo.pVertexInputState   = &vertexInputStateCreateInfo;
    // ----------------------------------------------------------------------------------------------------------------
}

void
GraphicsPipelineCreateInfo::recreate()
{
    destroy();
    shader->reload();
    create();
}

void
GraphicsPipelineCreateInfo::destroy()
{
    vkDestroyPipelineLayout(device, layout, nullptr);
    layout = nullptr;
}

VkDevice
GraphicsPipelineCreateInfo::getDevice() const
{
    return device;
}

const VertexFragmentShader&
GraphicsPipelineCreateInfo::getVertexFragmentShader() const
{
    return *shader;
}

const VkGraphicsPipelineCreateInfo&
GraphicsPipelineCreateInfo::getGraphicsPipelineCreateInfo() const
{
    return graphicsPipelineCreateInfo;
}

const VkPipelineLayout&
GraphicsPipelineCreateInfo::getPipelineLayout() const
{
    return layout;
}

const std::string&
GraphicsPipelineCreateInfo::getName() const
{
    return shader->getName();
}

GraphicsPipeline::GraphicsPipeline(std::unique_ptr<GraphicsPipelineCreateInfo> createInfo)
{
    this->createInfo = std::move(createInfo);

    create();
}

GraphicsPipeline::~GraphicsPipeline()
{
    destroy();
    createInfo.reset(nullptr);
}

void
GraphicsPipeline::recreate()
{
    destroy();
    createInfo->recreate();
    create();
}

void
GraphicsPipeline::create()
{
    VkDevice                            device = createInfo->getDevice();
    const VkGraphicsPipelineCreateInfo& info   = createInfo->getGraphicsPipelineCreateInfo();
    VULKAN_ASSERT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline),
                  "Failed to create graphics pipeline");
}

void
GraphicsPipeline::destroy()
{
    vkDestroyPipeline(createInfo->getDevice(), pipeline, nullptr);
    pipeline = nullptr;
}

const GraphicsPipelineCreateInfo&
GraphicsPipeline::getCreateInfo() const
{
    return *createInfo;
}

const VkPipeline&
GraphicsPipeline::getPipeline() const
{
    return pipeline;
}

const std::string&
GraphicsPipeline::getName() const
{
    return createInfo->getName();
}

} // namespace val
