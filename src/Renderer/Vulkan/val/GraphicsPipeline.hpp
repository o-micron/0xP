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

#include <array>
#include <memory>
#include <string>
#include <vector>

class XPShaderBuffer;

namespace val {

class PhysicalDevice;
struct DescriptorSets;
class VertexFragmentShader;

class GraphicsPipelineCreateInfo
{
  public:
    GraphicsPipelineCreateInfo(VkDevice                   device,
                               const val::PhysicalDevice& physicalDevice,
                               VkExtent2D                 swapchainExtent,
                               const XPShaderBuffer*      shaderBuffer);
    virtual ~GraphicsPipelineCreateInfo();
    void                                create();
    void                                recreate();
    void                                destroy();
    VkDevice                            getDevice() const;
    const VertexFragmentShader&         getVertexFragmentShader() const;
    const VkGraphicsPipelineCreateInfo& getGraphicsPipelineCreateInfo() const;
    const VkPipelineLayout&             getPipelineLayout() const;
    const std::string&                  getName() const;

  protected:
    const val::PhysicalDevice&  physicalDevice;
    std::vector<VkDynamicState> dynamicStates;
    // std::vector<VkVertexInputBindingDescription>     vertexInputBindingDescriptions;
    // std::vector<VkVertexInputAttributeDescription>   vertexAttributeDescriptions;
    // std::vector<VkPushConstantRange>                 pushConstantRanges;
    // std::vector<VkDescriptorSetLayout>               descriptorSetLayouts;
    std::array<VkPipelineShaderStageCreateInfo, 2>   shaderStageCreateInfos;
    std::unique_ptr<VertexFragmentShader>            shader;
    VkPipelineInputAssemblyStateCreateInfo           inputAssemblyStateCreateInfo;
    VkPipelineRasterizationStateCreateInfo           rasterizationStateCreateInfo;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentState;
    VkPipelineColorBlendStateCreateInfo              colorBlendStateCreateInfo;
    VkPipelineDepthStencilStateCreateInfo            depthStencilStateCreateInfo;
    VkViewport                                       viewport;
    VkRect2D                                         scissor;
    VkExtent2D                                       swapchainExtent;
    VkPipelineViewportStateCreateInfo                viewportStateCreateInfo;
    VkPipelineMultisampleStateCreateInfo             multisampleStateCreateInfo;
    VkPipelineDynamicStateCreateInfo                 dynamicStateCreateInfo;
    VkPipelineVertexInputStateCreateInfo             vertexInputStateCreateInfo;
    VkPipelineLayoutCreateInfo                       layoutCreateInfo;
    VkGraphicsPipelineCreateInfo                     graphicsPipelineCreateInfo;
    VkPipelineLayout                                 layout;
    VkDevice                                         device;
    VkPipelineRenderingCreateInfoKHR                 pipelineRenderingCreateInfo;
    std::vector<VkFormat>                            colorAttachmentFormats;
    std::vector<VkVertexInputAttributeDescription>   vertexAttributes;
    std::vector<VkVertexInputBindingDescription>     vertexBindings;
    std::vector<VkPushConstantRange>                 pushConstantRanges;
};

class GraphicsPipeline
{
  public:
    GraphicsPipeline(std::unique_ptr<GraphicsPipelineCreateInfo> createInfo);
    virtual ~GraphicsPipeline();
    void                              create();
    void                              recreate();
    void                              destroy();
    const GraphicsPipelineCreateInfo& getCreateInfo() const;
    const VkPipeline&                 getPipeline() const;
    const std::string&                getName() const;

  protected:
    std::unique_ptr<GraphicsPipelineCreateInfo> createInfo;
    VkPipeline                                  pipeline;
};

struct GraphicsPipelines
{
    std::vector<std::unique_ptr<GraphicsPipeline>> pipelines;
};

} // namespace val
