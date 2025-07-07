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

#include "FreeFunctions.hpp"

#include "Defs.h"
#include "PushConstantData.h"
#include "Swapchain.hpp"

#include <Renderer/Vulkan/XPVulkanExt.h>
#include <Utilities/XPLogger.h>
#include <Utilities/XPMaths.h>

static uint32_t
FormatSize(VkFormat format);

namespace val {
namespace ff {

VkDescriptorPoolCreateInfo
descriptorPoolCreateInfo(uint32_t poolSizeCount, VkDescriptorPoolSize* pPoolSizes, uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext         = nullptr;
    descriptorPoolInfo.flags         = 0;
    descriptorPoolInfo.poolSizeCount = poolSizeCount;
    descriptorPoolInfo.pPoolSizes    = pPoolSizes;
    descriptorPoolInfo.maxSets       = maxSets;
    return descriptorPoolInfo;
}

VkDescriptorPoolSize
descriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type            = type;
    descriptorPoolSize.descriptorCount = descriptorCount;
    return descriptorPoolSize;
}

VkDescriptorSetLayoutBinding
descriptorSetLayoutBinding(VkDescriptorType   type,
                           VkShaderStageFlags stageFlags,
                           uint32_t           binding,
                           uint32_t           descriptorCount)
{
    VkDescriptorSetLayoutBinding setLayoutBinding{};
    setLayoutBinding.descriptorType  = type;
    setLayoutBinding.stageFlags      = stageFlags;
    setLayoutBinding.binding         = binding;
    setLayoutBinding.descriptorCount = descriptorCount;
    return setLayoutBinding;
}

VkDescriptorSetLayoutCreateInfo
descriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* pBindings, uint32_t bindingCount)
{
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext        = nullptr;
    descriptorSetLayoutCreateInfo.flags        = 0;
    descriptorSetLayoutCreateInfo.pBindings    = pBindings;
    descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
    return descriptorSetLayoutCreateInfo;
}

VkDescriptorSetAllocateInfo
descriptorSetAllocateInfo(VkDescriptorPool             descriptorPool,
                          const VkDescriptorSetLayout* pSetLayouts,
                          uint32_t                     descriptorSetCount)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext              = nullptr;
    descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
    descriptorSetAllocateInfo.pSetLayouts        = pSetLayouts;
    descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
    return descriptorSetAllocateInfo;
}

VkDescriptorImageInfo
descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
{
    VkDescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.sampler     = sampler;
    descriptorImageInfo.imageView   = imageView;
    descriptorImageInfo.imageLayout = imageLayout;
    return descriptorImageInfo;
}

VkWriteDescriptorSet
writeDescriptorSet(VkDescriptorSet         dstSet,
                   VkDescriptorType        type,
                   uint32_t                binding,
                   VkDescriptorBufferInfo* bufferInfo,
                   uint32_t                descriptorCount)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext           = nullptr;
    writeDescriptorSet.dstSet          = dstSet;
    writeDescriptorSet.descriptorType  = type;
    writeDescriptorSet.dstBinding      = binding;
    writeDescriptorSet.pBufferInfo     = bufferInfo;
    writeDescriptorSet.descriptorCount = descriptorCount;
    return writeDescriptorSet;
}

VkWriteDescriptorSet
writeDescriptorSet(VkDescriptorSet        dstSet,
                   VkDescriptorType       type,
                   uint32_t               binding,
                   VkDescriptorImageInfo* imageInfo,
                   uint32_t               descriptorCount)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext           = nullptr;
    writeDescriptorSet.dstSet          = dstSet;
    writeDescriptorSet.descriptorType  = type;
    writeDescriptorSet.dstBinding      = binding;
    writeDescriptorSet.pImageInfo      = imageInfo;
    writeDescriptorSet.descriptorCount = descriptorCount;
    return writeDescriptorSet;
}

VkPipelineLayoutCreateInfo
pipelineLayoutCreateInfo(const VkDescriptorSetLayout* pSetLayouts,
                         uint32_t                     setLayoutCount,
                         const VkPushConstantRange*   pPushConstantRanges,
                         uint32_t                     pushConstantRangesCount)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.flags                  = 0;
    pipelineLayoutCreateInfo.setLayoutCount         = setLayoutCount;
    pipelineLayoutCreateInfo.pSetLayouts            = pSetLayouts;
    pipelineLayoutCreateInfo.pPushConstantRanges    = pPushConstantRanges;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangesCount;
    return pipelineLayoutCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo
pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology                     topology,
                                     VkPipelineInputAssemblyStateCreateFlags flags,
                                     VkBool32                                primitiveRestartEnable)
{
    VkPipelineInputAssemblyStateCreateInfo createInfo = {};
    createInfo.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.pNext                                  = nullptr;
    createInfo.flags                                  = flags;
    createInfo.topology                               = topology;
    createInfo.primitiveRestartEnable                 = primitiveRestartEnable;
    return createInfo;
}

VkPipelineRasterizationStateCreateInfo
pipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode)
{
    VkPipelineRasterizationStateCreateInfo createInfo = {};
    createInfo.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.pNext                                  = nullptr;
    createInfo.flags                                  = 0;
    createInfo.depthClampEnable                       = VK_FALSE;
    createInfo.rasterizerDiscardEnable                = VK_FALSE;
    createInfo.polygonMode                            = polygonMode;
    createInfo.lineWidth                              = 1.0f;
    createInfo.cullMode                               = cullMode;
    createInfo.frontFace                              = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    createInfo.depthBiasEnable                        = VK_FALSE;
    createInfo.depthBiasConstantFactor                = 0.0f;
    createInfo.depthBiasClamp                         = 0.0f;
    createInfo.depthBiasSlopeFactor                   = 0.0f;
    return createInfo;
}

VkPipelineColorBlendAttachmentState
pipelineColorBlendAttachmentState(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable)
{
    VkPipelineColorBlendAttachmentState state = {};
    state.colorWriteMask                      = colorWriteMask;
    state.blendEnable                         = blendEnable;
    state.srcColorBlendFactor                 = VK_BLEND_FACTOR_ONE;
    state.dstColorBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    state.colorBlendOp                        = VK_BLEND_OP_ADD;
    state.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;
    state.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    state.alphaBlendOp                        = VK_BLEND_OP_ADD;
    return state;
}

VkPipelineColorBlendStateCreateInfo
pipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments)
{
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.pNext                               = nullptr;
    createInfo.flags                               = 0;
    createInfo.logicOpEnable                       = VK_FALSE;
    createInfo.logicOp                             = VK_LOGIC_OP_COPY;
    createInfo.attachmentCount                     = attachmentCount;
    createInfo.pAttachments                        = pAttachments;
    createInfo.blendConstants[0]                   = 0.0f;
    createInfo.blendConstants[1]                   = 0.0f;
    createInfo.blendConstants[2]                   = 0.0f;
    createInfo.blendConstants[3]                   = 0.0f;
    return createInfo;
}

VkPipelineDepthStencilStateCreateInfo
pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp)
{
    VkPipelineDepthStencilStateCreateInfo createInfo = {};
    createInfo.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.pNext                                 = nullptr;
    createInfo.flags                                 = 0;
    createInfo.depthTestEnable                       = depthTestEnable;
    createInfo.depthWriteEnable                      = depthWriteEnable;
    createInfo.depthCompareOp                        = depthCompareOp;
    createInfo.depthBoundsTestEnable                 = VK_FALSE;
    createInfo.minDepthBounds                        = 0.0f;
    createInfo.maxDepthBounds                        = 1.0f;
    createInfo.stencilTestEnable                     = VK_FALSE;
    createInfo.front                                 = {};
    createInfo.back                                  = {};
    return createInfo;
}

VkPipelineViewportStateCreateInfo
pipelineViewportStateCreateInfo(const val::Swapchain& swapchain)
{
    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = (float)swapchain.getExtent().width;
    viewport.height     = (float)swapchain.getExtent().height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {};
    scissor.offset   = { 0, 0 };
    scissor.extent   = swapchain.getExtent();

    VkPipelineViewportStateCreateInfo createInfo = {};
    createInfo.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.pNext                             = nullptr;
    createInfo.flags                             = 0;
    createInfo.viewportCount                     = 1;
    createInfo.pViewports                        = &viewport;
    createInfo.scissorCount                      = 1;
    createInfo.pScissors                         = &scissor;

    return createInfo;
}

VkPipelineMultisampleStateCreateInfo
pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits                 rasterizationSamples,
                                   VkPipelineMultisampleStateCreateFlags flags)
{
    VkPipelineMultisampleStateCreateInfo createInfo = {};
    createInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.pNext                                = nullptr;
    createInfo.flags                                = flags;
    createInfo.rasterizationSamples                 = rasterizationSamples;
    return createInfo;
}

VkPipelineDynamicStateCreateInfo
pipelineDynamicStateCreateInfo(uint32_t                          dynamicStateCount,
                               const VkDynamicState*             pDynamicStates,
                               VkPipelineDynamicStateCreateFlags flags)
{
    VkPipelineDynamicStateCreateInfo createInfo = {};
    createInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo.pNext                            = nullptr;
    createInfo.flags                            = flags;
    createInfo.dynamicStateCount                = dynamicStateCount;
    createInfo.pDynamicStates                   = pDynamicStates;
    return createInfo;
}

VkPushConstantRange
pushConstantRange(VkShaderStageFlags stageFlags, uint32_t size, uint32_t offset)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = stageFlags;
    pushConstantRange.offset     = offset;
    pushConstantRange.size       = size;
    return pushConstantRange;
}

std::array<VkVertexInputBindingDescription, 3>
defaultVertexInputBindingDescriptions()
{
    VkVertexInputBindingDescription bindingDescription0 = {};
    bindingDescription0.binding                         = 0;
    bindingDescription0.stride                          = sizeof(XPVec4<float>);
    bindingDescription0.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription bindingDescription1 = {};
    bindingDescription1.binding                         = 1;
    bindingDescription1.stride                          = sizeof(XPVec4<float>);
    bindingDescription1.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription bindingDescription2 = {};
    bindingDescription2.binding                         = 2;
    bindingDescription2.stride                          = sizeof(XPVec4<float>);
    bindingDescription2.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    return { bindingDescription0, bindingDescription1, bindingDescription2 };
}

std::array<VkVertexInputAttributeDescription, 3>
defaultVertexAttributeDescriptions()
{
    VkVertexInputAttributeDescription attr0 = {};
    attr0.binding                           = 0;
    attr0.location                          = 0;
    attr0.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr0.offset                            = (uint32_t)0;

    VkVertexInputAttributeDescription attr1 = {};
    attr1.binding                           = 1;
    attr1.location                          = 1;
    attr1.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr1.offset                            = (uint32_t)0;

    VkVertexInputAttributeDescription attr2 = {};
    attr2.binding                           = 2;
    attr2.location                          = 2;
    attr2.format                            = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr2.offset                            = (uint32_t)0;

    return { attr0, attr1, attr2 };
}

VkVertexInputBindingDescription
vertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription vInputBindDescription{};
    vInputBindDescription.binding   = binding;
    vInputBindDescription.stride    = stride;
    vInputBindDescription.inputRate = inputRate;
    return vInputBindDescription;
}

VkVertexInputAttributeDescription
vertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
{
    VkVertexInputAttributeDescription vInputAttribDescription{};
    vInputAttribDescription.location = location;
    vInputAttribDescription.binding  = binding;
    vInputAttribDescription.format   = format;
    vInputAttribDescription.offset   = offset;
    return vInputAttribDescription;
}

VkPipelineVertexInputStateCreateInfo
pipelineVertexInputStateCreateInfo(uint32_t                                 bindingDescriptionCount,
                                   const VkVertexInputBindingDescription*   pBindingDescriptions,
                                   uint32_t                                 attributeDescriptionCount,
                                   const VkVertexInputAttributeDescription* pAttributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext                                = nullptr;
    createInfo.flags                                = 0;
    createInfo.vertexBindingDescriptionCount        = (uint32_t)bindingDescriptionCount;
    createInfo.pVertexBindingDescriptions           = pBindingDescriptions;
    createInfo.vertexAttributeDescriptionCount      = (uint32_t)attributeDescriptionCount;
    createInfo.pVertexAttributeDescriptions         = pAttributeDescriptions;
    return createInfo;
}

VkPipelineShaderStageCreateInfo
vertexShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pNext                           = nullptr;
    createInfo.flags                           = 0;
    createInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
    createInfo.module                          = shaderModule;
    createInfo.pName                           = "main";
    return createInfo;
}

VkPipelineShaderStageCreateInfo
fragmentShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pNext                           = nullptr;
    createInfo.flags                           = 0;
    createInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo.module                          = shaderModule;
    createInfo.pName                           = "main";
    return createInfo;
}

VkPipelineShaderStageCreateInfo
computeShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pNext                           = nullptr;
    createInfo.flags                           = 0;
    createInfo.stage                           = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.module                          = shaderModule;
    createInfo.pName                           = "main";
    return createInfo;
}

std::vector<VkPushConstantRange>
defaultPushConstantRanges()
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset              = 0;
    pushConstantRange.size                = sizeof(val::PushConstantData);
    return { pushConstantRange };
}

VkGraphicsPipelineCreateInfo
graphicsPipelineCreateInfo(VkPipelineLayout                              layout,
                           VkRenderPass                                  renderPass,
                           VkPipelineCreateFlags                         flags,
                           const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
                           const VkPipelineRasterizationStateCreateInfo& rasterizationState,
                           const VkPipelineColorBlendStateCreateInfo&    colorBlendState,
                           const VkPipelineDepthStencilStateCreateInfo&  depthStencilState,
                           const VkPipelineViewportStateCreateInfo&      viewportState,
                           const VkPipelineMultisampleStateCreateInfo&   multisampleState,
                           const VkPipelineDynamicStateCreateInfo&       dynamicState,
                           const VkPipelineShaderStageCreateInfo*        pShaderStages,
                           uint32_t                                      shaderStagesCount,
                           const VkPipelineVertexInputStateCreateInfo&   vertexInputState)
{
    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pNext                        = nullptr;
    createInfo.flags                        = 0;
    createInfo.layout                       = layout;
    createInfo.renderPass                   = renderPass;
    createInfo.flags                        = flags;
    createInfo.basePipelineIndex            = -1;
    createInfo.basePipelineHandle           = VK_NULL_HANDLE;
    createInfo.pInputAssemblyState          = &inputAssemblyState;
    createInfo.pRasterizationState          = &rasterizationState;
    createInfo.pColorBlendState             = &colorBlendState;
    createInfo.pMultisampleState            = &multisampleState;
    createInfo.pViewportState               = &viewportState;
    createInfo.pDepthStencilState           = &depthStencilState;
    createInfo.pDynamicState                = &dynamicState;
    createInfo.stageCount                   = shaderStagesCount;
    createInfo.pStages                      = pShaderStages;
    createInfo.subpass                      = 0;
    createInfo.pVertexInputState            = &vertexInputState;
    return createInfo;
}

VkSubmitInfo
submitInfo()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    return submitInfo;
}

VkSamplerCreateInfo
samplerCreateInfo()
{
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext         = nullptr;
    samplerCreateInfo.flags         = 0;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    return samplerCreateInfo;
}

VkImageMemoryBarrier
imageMemoryBarrier()
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext               = nullptr;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return imageMemoryBarrier;
}

VkFenceCreateInfo
fenceCreateInfo(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = flags;
    return fenceCreateInfo;
}

VkCommandBufferBeginInfo
commandBufferBeginInfo()
{
    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.pNext = nullptr;
    cmdBufferBeginInfo.flags = 0;
    return cmdBufferBeginInfo;
}

VkViewport
viewport(float width, float height, float minDepth, float maxDepth)
{
    VkViewport viewport{};
    viewport.x        = 0;
    viewport.y        = 0;
    viewport.width    = width;
    viewport.height   = height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    return viewport;
}

VkRect2D
rect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY)
{
    VkRect2D rect2D{};
    rect2D.extent.width  = width;
    rect2D.extent.height = height;
    rect2D.offset.x      = offsetX;
    rect2D.offset.y      = offsetY;
    return rect2D;
}

uint32_t
findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) { return i; }
        }
        typeFilter >>= 1;
    }

    XP_LOG(XPLoggerSeverityFatal, "Failed to find suitable memory type");
    return 0;
}

void
createImage(VkPhysicalDevice      physicalDevice,
            VkDevice              device,
            uint32_t              queueFamilyIndex,
            VkSharingMode         sharingMode,
            uint32_t              width,
            uint32_t              height,
            VkFormat              format,
            VkImageTiling         tiling,
            VkImageUsageFlags     usage,
            VkMemoryPropertyFlags properties,
            VkImage&              image,
            VkDeviceMemory&       memory,
            VkImageLayout         initialLayout)
{
    VkImageCreateInfo imageInfo     = {};
    imageInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext                 = nullptr;
    imageInfo.flags                 = 0;
    imageInfo.imageType             = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width          = width;
    imageInfo.extent.height         = height;
    imageInfo.extent.depth          = 1;
    imageInfo.mipLevels             = 1;
    imageInfo.arrayLayers           = 1;
    imageInfo.format                = format;
    imageInfo.tiling                = tiling;
    imageInfo.initialLayout         = initialLayout;
    imageInfo.usage                 = usage;
    imageInfo.samples               = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode           = sharingMode;
    imageInfo.queueFamilyIndexCount = 1;
    imageInfo.pQueueFamilyIndices   = &queueFamilyIndex;

    VULKAN_ASSERT(vkCreateImage(device, &imageInfo, nullptr, &image), "Failed to create image");

    VkMemoryRequirements memRequirements = {};
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext                = nullptr;
    allocInfo.allocationSize       = memRequirements.size;
    allocInfo.memoryTypeIndex      = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    VULKAN_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &memory), "Failed to allocate memory for image");

    VULKAN_ASSERT(vkBindImageMemory(device, image, memory, 0), "Failed to bind image memory");
}

void
createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView)
{
    VkImageViewCreateInfo viewInfo           = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext                           = nullptr;
    viewInfo.flags                           = 0;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;
    viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

    VULKAN_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &imageView), "Failed to create image view");
}

void
createSampler(VkDevice device, VkSampler& sampler)
{
    // TODO: find a better way to create samplers
    VkSamplerCreateInfo samplerInfo     = val::ff::samplerCreateInfo();
    samplerInfo.pNext                   = nullptr;
    samplerInfo.flags                   = 0;
    samplerInfo.magFilter               = VK_FILTER_LINEAR;
    samplerInfo.minFilter               = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.anisotropyEnable        = VK_FALSE;
    samplerInfo.maxAnisotropy           = 1.0f;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 1.0f;

    VULKAN_ASSERT(vkCreateSampler(device, &samplerInfo, nullptr, &sampler), "Failed to create a sampler");
}

void
destroyImage(VkDevice device, VkImage& image, VkDeviceMemory& memory)
{
    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, memory, nullptr);
}

void
destroyImageView(VkDevice device, VkImageView& imageView)
{
    vkDestroyImageView(device, imageView, nullptr);
}

void
destroySampler(VkDevice device, VkSampler& sampler)
{
    vkDestroySampler(device, sampler, nullptr);
}

void
annotateObject(VkDevice                   device,
               VkDebugReportObjectTypeEXT debugReportObjectType,
               VkObjectType               objectType,
               uint64_t                   object,
               const char*                name)
{
#if defined(XP_VULKAN_DEBUG_UTILS)
    VkDebugMarkerObjectNameInfoEXT debugMarkerImageNameInfo = {};
    debugMarkerImageNameInfo.sType                          = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
    debugMarkerImageNameInfo.pNext                          = NULL;
    debugMarkerImageNameInfo.objectType                     = debugReportObjectType;
    debugMarkerImageNameInfo.object                         = object;
    debugMarkerImageNameInfo.pObjectName                    = name;

    VkDebugUtilsObjectNameInfoEXT debugUtilsImageInfo = {};
    debugUtilsImageInfo.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    debugUtilsImageInfo.pNext                         = NULL;
    debugUtilsImageInfo.objectType                    = objectType;
    debugUtilsImageInfo.objectHandle                  = object;
    debugUtilsImageInfo.pObjectName                   = name;

    vk::dyn::vkDebugMarkerSetObjectNameEXT(device, &debugMarkerImageNameInfo);
    vk::dyn::vkSetDebugUtilsObjectNameEXT(device, &debugUtilsImageInfo);
#endif
};

} // namespace ff
} // namespace val

static uint32_t
FormatSize(VkFormat format)
{
    uint32_t result = 0;
    switch (format) {
        case VK_FORMAT_UNDEFINED: result = 0; break;
        case VK_FORMAT_R4G4_UNORM_PACK8: result = 1; break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16: result = 2; break;
        case VK_FORMAT_R8_UNORM: result = 1; break;
        case VK_FORMAT_R8_SNORM: result = 1; break;
        case VK_FORMAT_R8_USCALED: result = 1; break;
        case VK_FORMAT_R8_SSCALED: result = 1; break;
        case VK_FORMAT_R8_UINT: result = 1; break;
        case VK_FORMAT_R8_SINT: result = 1; break;
        case VK_FORMAT_R8_SRGB: result = 1; break;
        case VK_FORMAT_R8G8_UNORM: result = 2; break;
        case VK_FORMAT_R8G8_SNORM: result = 2; break;
        case VK_FORMAT_R8G8_USCALED: result = 2; break;
        case VK_FORMAT_R8G8_SSCALED: result = 2; break;
        case VK_FORMAT_R8G8_UINT: result = 2; break;
        case VK_FORMAT_R8G8_SINT: result = 2; break;
        case VK_FORMAT_R8G8_SRGB: result = 2; break;
        case VK_FORMAT_R8G8B8_UNORM: result = 3; break;
        case VK_FORMAT_R8G8B8_SNORM: result = 3; break;
        case VK_FORMAT_R8G8B8_USCALED: result = 3; break;
        case VK_FORMAT_R8G8B8_SSCALED: result = 3; break;
        case VK_FORMAT_R8G8B8_UINT: result = 3; break;
        case VK_FORMAT_R8G8B8_SINT: result = 3; break;
        case VK_FORMAT_R8G8B8_SRGB: result = 3; break;
        case VK_FORMAT_B8G8R8_UNORM: result = 3; break;
        case VK_FORMAT_B8G8R8_SNORM: result = 3; break;
        case VK_FORMAT_B8G8R8_USCALED: result = 3; break;
        case VK_FORMAT_B8G8R8_SSCALED: result = 3; break;
        case VK_FORMAT_B8G8R8_UINT: result = 3; break;
        case VK_FORMAT_B8G8R8_SINT: result = 3; break;
        case VK_FORMAT_B8G8R8_SRGB: result = 3; break;
        case VK_FORMAT_R8G8B8A8_UNORM: result = 4; break;
        case VK_FORMAT_R8G8B8A8_SNORM: result = 4; break;
        case VK_FORMAT_R8G8B8A8_USCALED: result = 4; break;
        case VK_FORMAT_R8G8B8A8_SSCALED: result = 4; break;
        case VK_FORMAT_R8G8B8A8_UINT: result = 4; break;
        case VK_FORMAT_R8G8B8A8_SINT: result = 4; break;
        case VK_FORMAT_R8G8B8A8_SRGB: result = 4; break;
        case VK_FORMAT_B8G8R8A8_UNORM: result = 4; break;
        case VK_FORMAT_B8G8R8A8_SNORM: result = 4; break;
        case VK_FORMAT_B8G8R8A8_USCALED: result = 4; break;
        case VK_FORMAT_B8G8R8A8_SSCALED: result = 4; break;
        case VK_FORMAT_B8G8R8A8_UINT: result = 4; break;
        case VK_FORMAT_B8G8R8A8_SINT: result = 4; break;
        case VK_FORMAT_B8G8R8A8_SRGB: result = 4; break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32: result = 4; break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32: result = 4; break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32: result = 4; break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32: result = 4; break;
        case VK_FORMAT_R16_UNORM: result = 2; break;
        case VK_FORMAT_R16_SNORM: result = 2; break;
        case VK_FORMAT_R16_USCALED: result = 2; break;
        case VK_FORMAT_R16_SSCALED: result = 2; break;
        case VK_FORMAT_R16_UINT: result = 2; break;
        case VK_FORMAT_R16_SINT: result = 2; break;
        case VK_FORMAT_R16_SFLOAT: result = 2; break;
        case VK_FORMAT_R16G16_UNORM: result = 4; break;
        case VK_FORMAT_R16G16_SNORM: result = 4; break;
        case VK_FORMAT_R16G16_USCALED: result = 4; break;
        case VK_FORMAT_R16G16_SSCALED: result = 4; break;
        case VK_FORMAT_R16G16_UINT: result = 4; break;
        case VK_FORMAT_R16G16_SINT: result = 4; break;
        case VK_FORMAT_R16G16_SFLOAT: result = 4; break;
        case VK_FORMAT_R16G16B16_UNORM: result = 6; break;
        case VK_FORMAT_R16G16B16_SNORM: result = 6; break;
        case VK_FORMAT_R16G16B16_USCALED: result = 6; break;
        case VK_FORMAT_R16G16B16_SSCALED: result = 6; break;
        case VK_FORMAT_R16G16B16_UINT: result = 6; break;
        case VK_FORMAT_R16G16B16_SINT: result = 6; break;
        case VK_FORMAT_R16G16B16_SFLOAT: result = 6; break;
        case VK_FORMAT_R16G16B16A16_UNORM: result = 8; break;
        case VK_FORMAT_R16G16B16A16_SNORM: result = 8; break;
        case VK_FORMAT_R16G16B16A16_USCALED: result = 8; break;
        case VK_FORMAT_R16G16B16A16_SSCALED: result = 8; break;
        case VK_FORMAT_R16G16B16A16_UINT: result = 8; break;
        case VK_FORMAT_R16G16B16A16_SINT: result = 8; break;
        case VK_FORMAT_R16G16B16A16_SFLOAT: result = 8; break;
        case VK_FORMAT_R32_UINT: result = 4; break;
        case VK_FORMAT_R32_SINT: result = 4; break;
        case VK_FORMAT_R32_SFLOAT: result = 4; break;
        case VK_FORMAT_R32G32_UINT: result = 8; break;
        case VK_FORMAT_R32G32_SINT: result = 8; break;
        case VK_FORMAT_R32G32_SFLOAT: result = 8; break;
        case VK_FORMAT_R32G32B32_UINT: result = 12; break;
        case VK_FORMAT_R32G32B32_SINT: result = 12; break;
        case VK_FORMAT_R32G32B32_SFLOAT: result = 12; break;
        case VK_FORMAT_R32G32B32A32_UINT: result = 16; break;
        case VK_FORMAT_R32G32B32A32_SINT: result = 16; break;
        case VK_FORMAT_R32G32B32A32_SFLOAT: result = 16; break;
        case VK_FORMAT_R64_UINT: result = 8; break;
        case VK_FORMAT_R64_SINT: result = 8; break;
        case VK_FORMAT_R64_SFLOAT: result = 8; break;
        case VK_FORMAT_R64G64_UINT: result = 16; break;
        case VK_FORMAT_R64G64_SINT: result = 16; break;
        case VK_FORMAT_R64G64_SFLOAT: result = 16; break;
        case VK_FORMAT_R64G64B64_UINT: result = 24; break;
        case VK_FORMAT_R64G64B64_SINT: result = 24; break;
        case VK_FORMAT_R64G64B64_SFLOAT: result = 24; break;
        case VK_FORMAT_R64G64B64A64_UINT: result = 32; break;
        case VK_FORMAT_R64G64B64A64_SINT: result = 32; break;
        case VK_FORMAT_R64G64B64A64_SFLOAT: result = 32; break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32: result = 4; break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: result = 4; break;

        default: break;
    }
    return result;
}
