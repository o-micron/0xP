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

#include "Swapchain.hpp"

#include "Defs.h"
#include "Device.hpp"
#include "FreeFunctions.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Queue.hpp"
#include "Texture.h"

#include <Renderer/Vulkan/XPVulkanRenderer.h>

#include <Utilities/XPLogger.h>

namespace val {

// void
// Swapchain::createImageView(VkDevice           device,
//                            VkImage            image,
//                            VkFormat           format,
//                            VkImageAspectFlags aspectFlags,
//                            VkImageView&       imageView)
//{
//     VkImageViewCreateInfo viewInfo           = {};
//     viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//     viewInfo.image                           = image;
//     viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
//     viewInfo.format                          = format;
//     viewInfo.subresourceRange.aspectMask     = aspectFlags;
//     viewInfo.subresourceRange.baseMipLevel   = 0;
//     viewInfo.subresourceRange.levelCount     = 1;
//     viewInfo.subresourceRange.baseArrayLayer = 0;
//     viewInfo.subresourceRange.layerCount     = 1;
//     viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
//     viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
//     viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
//     viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
//
//     VULKAN_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &imageView), "Failed to create image view");
// }
//
// void
// Swapchain::destroyImageView(VkDevice device, VkImageView imageView)
//{
//     vkDestroyImageView(device, imageView, nullptr);
// }

Swapchain::Swapchain(XPVulkanRenderer* renderer)
  : renderer(renderer)
{
}

Swapchain::~Swapchain() {}

void
Swapchain::createSwapchain()
{
    // auto cameraObjects =
    // PrototypeEngineInternalApplication::scene->fetchObjectsByTraits(PrototypeTraitTypeMaskCamera);

    // auto    cameraObject = *cameraObjects.begin();
    // Camera* cam          = cameraObject->getCameraTrait();
    // CameraSystemSetResolution(cam, _window->_resolution.x, _window->_resolution.y);

    Instance&       instance       = renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    SwapchainSupportDetails swapchainDetails =
      physicalDevice.querySwapchainSupportDetails(physicalDevice.getPhysicalDevice(), instance.getSurface());
    format      = selectSwapSurfaceFormat(swapchainDetails.formats);
    presentMode = selectSwapPresentMode(swapchainDetails.presentationModes);
    extent      = selectSwapExtent(swapchainDetails.capabilities);

    uint32_t imageCount = swapchainDetails.capabilities.minImageCount + 1;
    if (swapchainDetails.capabilities.maxImageCount > 0 && imageCount > swapchainDetails.capabilities.maxImageCount) {
        imageCount = swapchainDetails.capabilities.maxImageCount;
    }

    // create swapchain
    std::vector<uint32_t> queueFamilyIndices = { (uint32_t)physicalDevice.getQueues().graphics.index,
                                                 (uint32_t)physicalDevice.getQueues().presentation.index };

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext                    = nullptr;
    swapchainCreateInfo.flags                    = 0;
    swapchainCreateInfo.surface                  = instance.getSurface();
    swapchainCreateInfo.minImageCount            = imageCount;
    swapchainCreateInfo.imageFormat              = format.format;
    swapchainCreateInfo.imageColorSpace          = format.colorSpace;
    swapchainCreateInfo.imageExtent              = extent;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (physicalDevice.getQueues().graphics.index != physicalDevice.getQueues().presentation.index) {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
        swapchainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
    } else {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
    }
    swapchainCreateInfo.preTransform   = swapchainDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode    = presentMode;
    swapchainCreateInfo.clipped        = VK_TRUE;
    swapchainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

    VULKAN_ASSERT(vkCreateSwapchainKHR(physicalDevice.getDevice().device, &swapchainCreateInfo, nullptr, &swapchain),
                  "Failed to create swapchain");

    uint32_t swapchainImagesCount;
    vkGetSwapchainImagesKHR(physicalDevice.getDevice().device, swapchain, &swapchainImagesCount, nullptr);
    std::vector<VkImage> images;
    images.resize(swapchainImagesCount);
    vkGetSwapchainImagesKHR(physicalDevice.getDevice().device, swapchain, &swapchainImagesCount, images.data());
    colorTextures.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        colorTextures[i]        = std::make_unique<Texture>();
        colorTextures[i]->image = images[i];
        ff::createImageView(device.device,
                            colorTextures[i]->image,
                            format.format,
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            colorTextures[i]->imageView);
        ff::createSampler(device.device, colorTextures[i]->sampler);

        // TODO: make sure you don't leave the memory requirement nulled out, find a way to get device memory from
        // swapchain images VkMemoryRequirements memRequirments;
        // vkGetImageMemoryRequirements(physicalDevice.device.device, colorTextures[i].image, &memRequirments);
    }
}

void
Swapchain::createDepthResources()
{
    Instance&       instance       = renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    VkFormat depthFormat = physicalDevice.findSupportedDepthFormat();

    uint32_t swapchainImagesCount;
    vkGetSwapchainImagesKHR(physicalDevice.getDevice().device, swapchain, &swapchainImagesCount, nullptr);
    depthTextures.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        depthTextures[i] = std::make_unique<Texture>();
        ff::createImage(physicalDevice.getPhysicalDevice(),
                        device.device,
                        physicalDevice.getQueues().graphics.index,
                        VK_SHARING_MODE_EXCLUSIVE,
                        extent.width,
                        extent.height,
                        depthFormat,
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        depthTextures[i]->image,
                        depthTextures[i]->memory,
                        VK_IMAGE_LAYOUT_PREINITIALIZED);
        ff::createImageView(
          device.device, depthTextures[i]->image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthTextures[i]->imageView);
    }
}

void
Swapchain::destroySwapchain()
{
    Instance&       instance       = renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    for (std::unique_ptr<val::Texture>& texture : colorTextures) {
        device.destroySampler(*texture);
        device.destroyImageView(*texture);
    }
    colorTextures.clear();
    vkDestroySwapchainKHR(device.device, swapchain, nullptr);
}

void
Swapchain::destroyDepthResources()
{
    Instance&       instance       = renderer->getInstance();
    PhysicalDevice& physicalDevice = instance.getPhysicalDevice();
    Device&         device         = physicalDevice.getDevice();

    for (std::unique_ptr<val::Texture>& texture : depthTextures) {
        device.destroyImageView(*texture);
        device.destroyImage(*texture);
    }
    depthTextures.clear();
}

XPVulkanRenderer*
Swapchain::getRenderer() const
{
    return renderer;
}

VkSurfaceFormatKHR
Swapchain::getFormat() const
{
    return format;
}

VkPresentModeKHR
Swapchain::getPresentMode() const
{
    return presentMode;
}

VkExtent2D
Swapchain::getExtent() const
{
    return extent;
}

VkSwapchainKHR
Swapchain::getSwapchain() const
{
    return swapchain;
}

std::vector<std::unique_ptr<Texture>>&
Swapchain::getColorTextures()
{
    return colorTextures;
}

std::vector<std::unique_ptr<Texture>>&
Swapchain::getDepthTextures()
{
    return depthTextures;
}

VkSurfaceFormatKHR
Swapchain::selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (const auto& availableFormat : availableSurfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableSurfaceFormats[0];
}

VkPresentModeKHR
Swapchain::selectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { return availablePresentMode; }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
Swapchain::selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        auto       windowSize   = renderer->getWindowSize();
        VkExtent2D actualExtent = { static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y) };
        actualExtent.width =
          std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

} // namespace val
