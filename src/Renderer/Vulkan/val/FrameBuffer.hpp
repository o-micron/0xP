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

#include <Renderer/Vulkan/val/Enums.h>

#include <vulkan/vulkan.h>

#include <vector>

class XPVulkanRenderer;
namespace val {

class GraphicsPipeline;
class VertexFragmentShader;
struct Texture;
class FrameBuffer
{
  public:
    FrameBuffer(XPVulkanRenderer* renderer, uint32_t numColorAttachments);
    ~FrameBuffer();
    void create(int32_t width, int32_t height, val::EGraphicsPipelines graphicsPipelineIndex);
    void reload();
    void destroy();

    void copyColorAttachmentToHostBuffer(uint32_t index, unsigned char** dstData, uint32_t& width, uint32_t& height);
    void transitionImagesToBeRenderedTo(VkCommandBuffer commandBuffer);
    void transitionImagesToBeReadByFragmentShaders(VkCommandBuffer commandBuffer);
    void transitionImagesToBePresented(VkCommandBuffer commandBuffer);
    void beginRender(VkCommandBuffer       commandBuffer,
                     VkDescriptorSetLayout descriptorSetLayout,
                     VkDescriptorSet       descriptorSet);
    void endRender(VkCommandBuffer commandBuffer);
    void renderToScreen(VkCommandBuffer       commandBuffer,
                        VkImageView           swapchainImageView,
                        VkDescriptorSetLayout descriptorSetLayout,
                        VkDescriptorSet       descriptorSet);

    int32_t               getWidth() const;
    int32_t               getHeight() const;
    VkDescriptorImageInfo getDescriptor() const;
    const size_t          getNumColorAttachments() const;
    const Texture&        getColorAttachment(uint32_t index) const;
    const Texture&        getDepthAttachment() const;

  private:
    void createAttachments(VkPhysicalDevice physicalDevice,
                           VkDevice         device,
                           VkFormat         depthAttachmentFormat,
                           uint32_t         graphicsQueueFamilyIndex);
    void destroyAttachments(VkDevice device);
    void createGraphicsPipeline(VkDevice                         device,
                                const val::GraphicsPipeline*     pipeline,
                                const val::VertexFragmentShader* shader);
    void destroyGraphicsPipeline(VkDevice device);

    XPVulkanRenderer*       _renderer;
    int32_t                 _width;
    int32_t                 _height;
    VkPipelineLayout        _pipelineLayout;
    VkPipeline              _pipeline;
    std::vector<VkFormat>   _colorAttachmentFormats;
    std::vector<Texture*>   _colorAttachments;
    Texture*                _depthAttachment;
    val::EGraphicsPipelines _graphicsPipelineIndex;
};

} // namespace val
