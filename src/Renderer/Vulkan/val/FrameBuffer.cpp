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

#include <Renderer/Vulkan/val/FrameBuffer.hpp>

#include <Renderer/Vulkan/XPVulkanExt.h>
#include <Renderer/Vulkan/XPVulkanRenderer.h>
#include <Renderer/Vulkan/val/Defs.h>
#include <Renderer/Vulkan/val/Device.hpp>
#include <Renderer/Vulkan/val/Enums.h>
#include <Renderer/Vulkan/val/FreeFunctions.hpp>
#include <Renderer/Vulkan/val/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/val/Instance.hpp>
#include <Renderer/Vulkan/val/PhysicalDevice.hpp>
#include <Renderer/Vulkan/val/Queue.hpp>
#include <Renderer/Vulkan/val/Shader.hpp>
#include <Renderer/Vulkan/val/Swapchain.hpp>
#include <Renderer/Vulkan/val/Texture.h>
#include <Utilities/XPLogger.h>

namespace val {

FrameBuffer::FrameBuffer(XPVulkanRenderer* renderer, uint32_t numColorAttachments)
  : _renderer(renderer)
  , _width(XP_INITIAL_WINDOW_WIDTH)
  , _height(XP_INITIAL_WINDOW_HEIGHT)
{
    _colorAttachments.resize(numColorAttachments);
    for (size_t i = 0; i < _colorAttachments.size(); ++i) { _colorAttachments[i] = XP_NEW Texture(); }

    _depthAttachment = XP_NEW Texture();
}

FrameBuffer::~FrameBuffer()
{
    for (size_t i = 0; i < _colorAttachments.size(); ++i) { XP_DELETE _colorAttachments[i]; }
    _colorAttachments.clear();

    XP_DELETE _depthAttachment;
}

void
FrameBuffer::create(int32_t width, int32_t height, val::EGraphicsPipelines graphicsPipelineIndex)
{
    _width                 = width;
    _height                = height;
    _graphicsPipelineIndex = graphicsPipelineIndex;

    Swapchain&      swapchain      = _renderer->getSwapchain();
    Instance&       instance       = _renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    const val::GraphicsPipeline*     pipeline          = nullptr;
    const val::VertexFragmentShader* shader            = nullptr;
    auto&                            graphicsPipelines = _renderer->getGraphicsPipelines();
    for (auto& it : val::EGraphicsPipelinesMap) {
        if (it.second == graphicsPipelineIndex) {
            pipeline = graphicsPipelines.pipelines[it.second].get();
            shader   = &pipeline->getCreateInfo().getVertexFragmentShader();
        }
    }
    _colorAttachmentFormats.resize(shader->getFSOutputFormats().size());
    for (size_t i = 0; i < _colorAttachmentFormats.size(); ++i) {
        _colorAttachmentFormats[i] = shader->getFSOutputFormats().at(i);
    }

    // color attachment ---------------------------------------------------------------------------------------------
    VkFormat depthFormat = physicalDevice.findSupportedDepthFormat();
    createAttachments(
      physicalDevice.getPhysicalDevice(), device.device, depthFormat, physicalDevice.getQueues().graphics.index);
    // --------------------------------------------------------------------------------------------------------------

    // create builtin quad shader and graphics pipeline -------------------------------------------------------------
    createGraphicsPipeline(device.device, pipeline, shader);
    // --------------------------------------------------------------------------------------------------------------
}

void
FrameBuffer::reload()
{
    destroy();
    create(_width, _height, _graphicsPipelineIndex);
}

void
FrameBuffer::destroy()
{
    Swapchain&      swapchain      = _renderer->getSwapchain();
    Instance&       instance       = _renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    destroyGraphicsPipeline(device.device);
    destroyAttachments(device.device);
}

void
FrameBuffer::copyColorAttachmentToHostBuffer(uint32_t index, unsigned char** dstData, uint32_t& width, uint32_t& height)
{
    // create temporary host visible buffer (or use one that is already created or cached ..)
    // transition color attachment image layout to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    // copy image to temporary (staging) buffer
    // map the staging buffer to memcpy to the dstData pointer
    // transition color attachment image layout back to where it was
    // destroy the temporary host visible (staging) buffer if it was created here
}

void
FrameBuffer::transitionImagesToBeRenderedTo(VkCommandBuffer commandBuffer)
{
    // transition images to prepare to be written to them as render targets
    {
        Instance&       instance       = _renderer->getInstance();
        PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
        Device&         device         = physicalDevice.getDevice();

        // transition color images to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        for (size_t i = 0; i < _colorAttachments.size(); ++i) {
            device.makeImageBarrier(commandBuffer,
                                    _colorAttachments[i]->image,
                                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                    VK_ACCESS_NONE,
                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    _colorAttachmentFormats[i]); // VK_FORMAT_B8G8R8A8_SRGB
        }

        // transition depth images to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        VkFormat depthFormat = physicalDevice.findSupportedDepthFormat();
        device.makeImageBarrier(commandBuffer,
                                _depthAttachment->image,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_ACCESS_NONE,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                depthFormat);
    }
}

void
FrameBuffer::transitionImagesToBeReadByFragmentShaders(VkCommandBuffer commandBuffer)
{
    // transition images back to be used as shader attachments
    {
        Instance&       instance       = _renderer->getInstance();
        PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
        Device&         device         = physicalDevice.getDevice();

        // transition color images to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        for (size_t i = 0; i < _colorAttachments.size(); ++i) {
            device.makeImageBarrier(commandBuffer,
                                    _colorAttachments[i]->image,
                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                    VK_ACCESS_SHADER_READ_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    _colorAttachmentFormats[i]); // VK_FORMAT_B8G8R8A8_SRGB
        }

        // transition depth images to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        VkFormat depthFormat = physicalDevice.findSupportedDepthFormat();
        device.makeImageBarrier(commandBuffer,
                                _depthAttachment->image,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                VK_ACCESS_SHADER_READ_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                depthFormat);
    }
}

void
FrameBuffer::transitionImagesToBePresented(VkCommandBuffer commandBuffer)
{
    {
        Instance&       instance       = _renderer->getInstance();
        PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
        Device&         device         = physicalDevice.getDevice();

        // transition color images to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        Swapchain& swapchain = _renderer->getSwapchain();
        for (size_t i = 0; i < _colorAttachments.size(); ++i) {
            device.makeImageBarrier(commandBuffer,
                                    _colorAttachments[i]->image,
                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                    VK_ACCESS_NONE,
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    _colorAttachmentFormats[i]);
        }

        // transition depth images to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        VkFormat depthFormat = physicalDevice.findSupportedDepthFormat();
        device.makeImageBarrier(commandBuffer,
                                _depthAttachment->image,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                VK_ACCESS_NONE,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                depthFormat);
    }
}

void
FrameBuffer::beginRender(VkCommandBuffer       commandBuffer,
                         VkDescriptorSetLayout descriptorSetLayout,
                         VkDescriptorSet       descriptorSet)
{
    transitionImagesToBeRenderedTo(commandBuffer);

    std::vector<VkRenderingAttachmentInfoKHR> colorAttachmentsInfo;
    colorAttachmentsInfo.resize(_colorAttachments.size());

    for (size_t i = 0; i < _colorAttachments.size(); ++i) {
        colorAttachmentsInfo[i].sType                           = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachmentsInfo[i].pNext                           = nullptr;
        colorAttachmentsInfo[i].imageView                       = _colorAttachments[i]->imageView;
        colorAttachmentsInfo[i].imageLayout                     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentsInfo[i].loadOp                          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentsInfo[i].storeOp                         = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentsInfo[i].clearValue.color                = { XP_DEFAULT_CLEAR_COLOR };
        colorAttachmentsInfo[i].clearValue.depthStencil.depth   = 1.0f;
        colorAttachmentsInfo[i].clearValue.depthStencil.stencil = 0.0f;
    }

    VkRenderingAttachmentInfoKHR depthAttachmentInfo    = {};
    depthAttachmentInfo.sType                           = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depthAttachmentInfo.pNext                           = nullptr;
    depthAttachmentInfo.imageView                       = _depthAttachment->imageView;
    depthAttachmentInfo.imageLayout                     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.loadOp                          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp                         = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentInfo.clearValue.color                = { XP_DEFAULT_CLEAR_COLOR };
    depthAttachmentInfo.clearValue.depthStencil.depth   = 1.0f;
    depthAttachmentInfo.clearValue.depthStencil.stencil = 0.0f;

    // VkRenderingAttachmentInfoKHR stencilAttachmentInfo    = {};
    // stencilAttachmentInfo.sType                           = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    // stencilAttachmentInfo.pNext                           = nullptr;
    // stencilAttachmentInfo.imageView                       = _depthAttachment->imageView;
    // stencilAttachmentInfo.imageLayout                     = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR;
    // stencilAttachmentInfo.loadOp                          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // stencilAttachmentInfo.storeOp                         = VK_ATTACHMENT_STORE_OP_STORE;
    // stencilAttachmentInfo.clearValue.color                = { XP_DEFAULT_CLEAR_COLOR };
    // stencilAttachmentInfo.clearValue.depthStencil.depth   = 1.0f;
    // stencilAttachmentInfo.clearValue.depthStencil.stencil = 0.0f;

    VkRenderingInfoKHR renderInfo       = {};
    renderInfo.sType                    = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderInfo.pNext                    = nullptr;
    renderInfo.renderArea.offset        = { 0, 0 };
    renderInfo.renderArea.extent.width  = _width;
    renderInfo.renderArea.extent.height = _height;
    renderInfo.layerCount               = 1;
    renderInfo.colorAttachmentCount     = (uint32_t)colorAttachmentsInfo.size();
    renderInfo.pColorAttachments        = colorAttachmentsInfo.data();
    renderInfo.pDepthAttachment         = &depthAttachmentInfo;
    renderInfo.pStencilAttachment       = VK_NULL_HANDLE;

    vk::dyn::vkCmdBeginRenderingKHR(commandBuffer, &renderInfo);

    VkViewport viewport = {};
    viewport.width      = _width;
    viewport.height     = _height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor      = {};
    scissor.extent.width  = _width;
    scissor.extent.height = _height;
    scissor.offset.x      = 0;
    scissor.offset.y      = 0;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void
FrameBuffer::endRender(VkCommandBuffer commandBuffer)
{
    vk::dyn::vkCmdEndRenderingKHR(commandBuffer);

    transitionImagesToBeReadByFragmentShaders(commandBuffer);
}

void
FrameBuffer::renderToScreen(VkCommandBuffer       commandBuffer,
                            VkImageView           swapchainImageView,
                            VkDescriptorSetLayout descriptorSetLayout,
                            VkDescriptorSet       descriptorSet)
{
    Swapchain&      swapchain      = _renderer->getSwapchain();
    Instance&       instance       = _renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    // since we're rendering to screen, we only need single color attachment
    // no depth or stencil attachments needed
    VkRenderingAttachmentInfoKHR colorAttachmentInfo = {};
    colorAttachmentInfo.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachmentInfo.pNext                        = nullptr;
    colorAttachmentInfo.imageView                    = swapchainImageView;
    colorAttachmentInfo.imageLayout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue.color             = { XP_DEFAULT_CLEAR_COLOR };

    VkRenderingInfoKHR renderInfo       = {};
    renderInfo.sType                    = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderInfo.pNext                    = nullptr;
    renderInfo.renderArea.offset        = { 0, 0 };
    renderInfo.renderArea.extent.width  = swapchain.getExtent().width;
    renderInfo.renderArea.extent.height = swapchain.getExtent().height;
    renderInfo.layerCount               = 1;
    renderInfo.colorAttachmentCount     = 1;
    renderInfo.pColorAttachments        = &colorAttachmentInfo;
    renderInfo.pDepthAttachment         = VK_NULL_HANDLE;
    renderInfo.pStencilAttachment       = VK_NULL_HANDLE;

    transitionImagesToBePresented(commandBuffer);

    vk::dyn::vkCmdBeginRenderingKHR(commandBuffer, &renderInfo);
    {
        VkViewport viewport = {};
        viewport.width      = swapchain.getExtent().width;
        viewport.height     = swapchain.getExtent().height;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor      = {};
        scissor.extent.width  = swapchain.getExtent().width;
        scissor.extent.height = swapchain.getExtent().height;
        scissor.offset.x      = 0;
        scissor.offset.y      = 0;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // Display the offscreen render target
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
        vkCmdBindDescriptorSets(
          commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }
    vk::dyn::vkCmdEndRenderingKHR(commandBuffer);
}

int32_t
FrameBuffer::getWidth() const
{
    return _width;
}

int32_t
FrameBuffer::getHeight() const
{
    return _height;
}

const size_t
FrameBuffer::getNumColorAttachments() const
{
    return _colorAttachments.size();
}

const Texture&
FrameBuffer::getColorAttachment(uint32_t index) const
{
    return *_colorAttachments[index];
}

const Texture&
FrameBuffer::getDepthAttachment() const
{
    return *_depthAttachment;
}

void
FrameBuffer::createAttachments(VkPhysicalDevice physicalDevice,
                               VkDevice         device,
                               VkFormat         depthAttachmentFormat,
                               uint32_t         graphicsQueueFamilyIndex)
{
    for (size_t i = 0; i < _colorAttachments.size(); ++i) {
        ff::createImage(physicalDevice,
                        device,
                        graphicsQueueFamilyIndex,
                        VK_SHARING_MODE_EXCLUSIVE,
                        _width,
                        _height,
                        _colorAttachmentFormats[i],
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        _colorAttachments[i]->image,
                        _colorAttachments[i]->memory,
                        VK_IMAGE_LAYOUT_PREINITIALIZED);
        ff::createImageView(device,
                            _colorAttachments[i]->image,
                            _colorAttachmentFormats[i],
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            _colorAttachments[i]->imageView);
        ff::createSampler(device, _colorAttachments[i]->sampler);
        ff::annotateObject(device,
                           VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT,
                           VK_OBJECT_TYPE_IMAGE,
                           (uint64_t)_colorAttachments[i]->image,
                           "Framebuffer::ColorAttachment::i");
    }
    // --------------------------------------------------------------------------------------------------------------

    // get depth format ---------------------------------------------------------------------------------------------
    ff::createImage(physicalDevice,
                    device,
                    graphicsQueueFamilyIndex,
                    VK_SHARING_MODE_EXCLUSIVE,
                    _width,
                    _height,
                    depthAttachmentFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    _depthAttachment->image,
                    _depthAttachment->memory,
                    VK_IMAGE_LAYOUT_PREINITIALIZED);
    ff::createImageView(
      device, _depthAttachment->image, depthAttachmentFormat, VK_IMAGE_ASPECT_DEPTH_BIT, _depthAttachment->imageView);
    ff::annotateObject(device,
                       VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT,
                       VK_OBJECT_TYPE_IMAGE,
                       (uint64_t)_depthAttachment->image,
                       "Framebuffer::DepthAttachment");
}

void
FrameBuffer::destroyAttachments(VkDevice device)
{
    for (size_t i = 0; i < _colorAttachments.size(); ++i) {
        ff::destroySampler(device, _colorAttachments[i]->sampler);
        ff::destroyImageView(device, _colorAttachments[i]->imageView);
        ff::destroyImage(device, _colorAttachments[i]->image, _colorAttachments[i]->memory);
    }

    ff::destroyImageView(device, _depthAttachment->imageView);
    ff::destroyImage(device, _depthAttachment->image, _depthAttachment->memory);
}

void
FrameBuffer::createGraphicsPipeline(VkDevice                         device,
                                    const val::GraphicsPipeline*     pipeline,
                                    const val::VertexFragmentShader* shader)
{
    _pipeline       = pipeline->getPipeline();
    _pipelineLayout = pipeline->getCreateInfo().getPipelineLayout();

    std::vector<VkPushConstantRange> pushConstantRanges;
    pushConstantRanges.reserve(shader->getPushConstantRanges().size());
    for (const auto& pair : shader->getPushConstantRanges()) { pushConstantRanges.push_back(pair.second); }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount             = shader->getDescriptorSetLayouts().size();
    pipelineLayoutCreateInfo.pSetLayouts                = shader->getDescriptorSetLayouts().data();
    pipelineLayoutCreateInfo.pushConstantRangeCount     = (uint32_t)pushConstantRanges.size();
    pipelineLayoutCreateInfo.pPushConstantRanges        = pushConstantRanges.data();

    VULKAN_ASSERT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout),
                  "Failed to create framebuffer pipeline layout");
    // ----------------------------------------------------------------------------------------------------------------
    // DEFAULT
    // ----------------------------------------------------------------------------------------------------------------
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext                  = nullptr;
    inputAssemblyStateCreateInfo.flags                  = 0;
    inputAssemblyStateCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext                   = nullptr;
    rasterizationStateCreateInfo.flags                   = 0;
    rasterizationStateCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth               = 1.0f;
    rasterizationStateCreateInfo.cullMode                = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp          = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor    = 0.0f;

    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = (float)_width;
    viewport.height     = (float)_height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor      = {};
    scissor.offset        = { 0, 0 };
    scissor.extent.width  = _width;
    scissor.extent.height = _height;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext                             = nullptr;
    viewportStateCreateInfo.flags                             = 0;
    viewportStateCreateInfo.viewportCount                     = 1;
    viewportStateCreateInfo.pViewports                        = &viewport;
    viewportStateCreateInfo.scissorCount                      = 1;
    viewportStateCreateInfo.pScissors                         = &scissor;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext                 = nullptr;
    depthStencilStateCreateInfo.flags                 = 0;
    depthStencilStateCreateInfo.depthTestEnable       = VK_TRUE; // TODO: Not all framebuffers would have depth
    depthStencilStateCreateInfo.depthWriteEnable      = VK_TRUE; // TODO: Not all framebuffers would have depth
    depthStencilStateCreateInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds        = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds        = 1.0f;
    depthStencilStateCreateInfo.stencilTestEnable     = VK_FALSE;
    depthStencilStateCreateInfo.front                 = {};
    depthStencilStateCreateInfo.back                  = {};

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext                 = nullptr;
    multisampleStateCreateInfo.flags                 = 0;
    multisampleStateCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.alphaToOneEnable      = VK_FALSE;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.sampleShadingEnable   = VK_FALSE;

    std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext                            = nullptr;
    dynamicStateCreateInfo.flags                            = 0;
    dynamicStateCreateInfo.dynamicStateCount                = (uint32_t)dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates                   = dynamicStates.data();

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
    colorBlendAttachmentStates.resize(shader->getFSOutputFormats().size());
    for (size_t i = 0; i < colorBlendAttachmentStates.size(); ++i) {
        colorBlendAttachmentStates[i].colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentStates[i].blendEnable         = VK_FALSE;
        colorBlendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentStates[i].colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachmentStates[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentStates[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentStates[i].alphaBlendOp        = VK_BLEND_OP_ADD;
    }
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext             = nullptr;
    colorBlendStateCreateInfo.flags             = 0;
    colorBlendStateCreateInfo.logicOpEnable     = VK_FALSE;
    colorBlendStateCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount   = (uint32_t)colorBlendAttachmentStates.size();
    colorBlendStateCreateInfo.pAttachments      = colorBlendAttachmentStates.data();
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    std::vector<VkFormat> colorAttachmentFormats;
    colorAttachmentFormats.resize(shader->getFSOutputFormats().size());
    const auto& shaderOutputFormats = shader->getFSOutputFormats();
    for (uint32_t i = 0; i < colorAttachmentFormats.size(); ++i) {
        colorAttachmentFormats[i] = shaderOutputFormats.at(i);
    }
    VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
    pipelineRenderingCreateInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipelineRenderingCreateInfo.colorAttachmentCount             = (uint32_t)colorAttachmentFormats.size();
    pipelineRenderingCreateInfo.pColorAttachmentFormats          = colorAttachmentFormats.data();

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfos = {
        shader->getVertexShaderStageCreateInfo(), shader->getFragmentShaderStageCreateInfo()
    };

    std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    vertexAttributes.reserve(shader->getVSInputAttributes().size());
    for (const auto& pair : shader->getVSInputAttributes()) { vertexAttributes.push_back(pair.second); }

    std::vector<VkVertexInputBindingDescription> vertexBindings;
    vertexBindings.reserve(shader->getVSInputBindings().size());
    for (const auto& pair : shader->getVSInputBindings()) { vertexBindings.push_back(pair.second); }

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount   = (uint32_t)vertexBindings.size();
    vertexInputStateCreateInfo.pVertexBindingDescriptions      = vertexBindings.data();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions    = vertexAttributes.data();
    // ----------------------------------------------------------------------------------------------------------------

    // graphics pipeline create info
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext                        = &pipelineRenderingCreateInfo;
    graphicsPipelineCreateInfo.flags                        = 0;
    graphicsPipelineCreateInfo.layout                       = _pipelineLayout;
    graphicsPipelineCreateInfo.renderPass                   = nullptr;
    graphicsPipelineCreateInfo.basePipelineIndex            = -1;
    graphicsPipelineCreateInfo.basePipelineHandle           = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.pInputAssemblyState          = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState          = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState             = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState            = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState               = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState           = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState                = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.stageCount                   = (uint32_t)shaderStageCreateInfos.size();
    graphicsPipelineCreateInfo.pStages                      = shaderStageCreateInfos.data();
    graphicsPipelineCreateInfo.subpass                      = 0;
    graphicsPipelineCreateInfo.pVertexInputState            = &vertexInputStateCreateInfo;

    vkCreateGraphicsPipelines(device, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &_pipeline);
    // ----------------------------------------------------------------------------------------------------------------
}

void
FrameBuffer::destroyGraphicsPipeline(VkDevice device)
{
}

} // namespace val
