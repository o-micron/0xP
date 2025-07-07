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

#include <Utilities/XPPlatforms.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace val {

class Swapchain;

namespace ff {

VkDescriptorPoolCreateInfo
descriptorPoolCreateInfo(uint32_t poolSizeCount, VkDescriptorPoolSize* pPoolSizes, uint32_t maxSets);

VkDescriptorPoolSize
descriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

VkDescriptorSetLayoutBinding
descriptorSetLayoutBinding(VkDescriptorType   type,
                           VkShaderStageFlags stageFlags,
                           uint32_t           binding,
                           uint32_t           descriptorCount);

VkDescriptorSetLayoutCreateInfo
descriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* pBindings, uint32_t bindingCount);

VkDescriptorSetAllocateInfo
descriptorSetAllocateInfo(VkDescriptorPool             descriptorPool,
                          const VkDescriptorSetLayout* pSetLayouts,
                          uint32_t                     descriptorSetCount);

VkDescriptorImageInfo
descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);

VkWriteDescriptorSet
writeDescriptorSet(VkDescriptorSet         dstSet,
                   VkDescriptorType        type,
                   uint32_t                binding,
                   VkDescriptorBufferInfo* bufferInfo,
                   uint32_t                descriptorCount);

VkWriteDescriptorSet
writeDescriptorSet(VkDescriptorSet        dstSet,
                   VkDescriptorType       type,
                   uint32_t               binding,
                   VkDescriptorImageInfo* imageInfo,
                   uint32_t               descriptorCount);

VkPipelineLayoutCreateInfo
pipelineLayoutCreateInfo(const VkDescriptorSetLayout* pSetLayouts,
                         uint32_t                     setLayoutCount,
                         const VkPushConstantRange*   pPushConstantRanges,
                         uint32_t                     pushConstantRangesCount);

VkPipelineInputAssemblyStateCreateInfo
pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology                     topology,
                                     VkPipelineInputAssemblyStateCreateFlags flags,
                                     VkBool32                                primitiveRestartEnable);

VkPipelineRasterizationStateCreateInfo
pipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode);

VkPipelineColorBlendAttachmentState
pipelineColorBlendAttachmentState(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable);

VkPipelineColorBlendStateCreateInfo
pipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments);

VkPipelineDepthStencilStateCreateInfo
pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);

VkPipelineViewportStateCreateInfo
pipelineViewportStateCreateInfo(const Swapchain& swapchain);

VkPipelineMultisampleStateCreateInfo
pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits                 rasterizationSamples,
                                   VkPipelineMultisampleStateCreateFlags flags);

VkPipelineDynamicStateCreateInfo
pipelineDynamicStateCreateInfo(uint32_t                          dynamicStateCount,
                               const VkDynamicState*             pDynamicStates,
                               VkPipelineDynamicStateCreateFlags flags);

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
                           const VkPipelineVertexInputStateCreateInfo&   vertexInputState);

VkPushConstantRange
pushConstantRange(VkShaderStageFlags stageFlags, uint32_t size, uint32_t offset);

std::array<VkVertexInputBindingDescription, 3>
defaultVertexInputBindingDescriptions();

std::array<VkVertexInputAttributeDescription, 3>
defaultVertexAttributeDescriptions();

VkVertexInputBindingDescription
vertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

VkVertexInputAttributeDescription
vertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

VkPipelineVertexInputStateCreateInfo
pipelineVertexInputStateCreateInfo(uint32_t                                 bindingDescriptionCount,
                                   const VkVertexInputBindingDescription*   pBindingDescriptions,
                                   uint32_t                                 attributeDescriptionCount,
                                   const VkVertexInputAttributeDescription* pAttributeDescriptions);

VkPipelineShaderStageCreateInfo
vertexShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule);

VkPipelineShaderStageCreateInfo
fragmentShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule);

VkPipelineShaderStageCreateInfo
computeShaderPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule);

std::vector<VkPushConstantRange>
defaultPushConstantRanges();

VkSubmitInfo
submitInfo();

VkSamplerCreateInfo
samplerCreateInfo();

VkImageMemoryBarrier
imageMemoryBarrier();

VkFenceCreateInfo
fenceCreateInfo(VkFenceCreateFlags flags);

VkCommandBufferBeginInfo
commandBufferBeginInfo();

VkViewport
viewport(float width, float height, float minDepth, float maxDepth);

VkRect2D
rect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY);

uint32_t
findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

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
            VkImageLayout         initialLayout);

void
createImageView(VkDevice           device,
                VkImage            image,
                VkFormat           format,
                VkImageAspectFlags aspectFlags,
                VkImageView&       imageView);

void
createSampler(VkDevice device, VkSampler& sampler);

void
destroyImage(VkDevice device, VkImage& image, VkDeviceMemory& memory);

void
destroyImageView(VkDevice device, VkImageView& imageView);

void
destroySampler(VkDevice device, VkSampler& sampler);

void
annotateObject(VkDevice                   device,
               VkDebugReportObjectTypeEXT debugReportObjectType,
               VkObjectType               objectType,
               uint64_t                   object,
               const char*                name);

} // namespace ff
} // namespace val
