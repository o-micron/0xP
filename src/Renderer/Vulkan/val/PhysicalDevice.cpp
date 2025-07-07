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

#include "PhysicalDevice.hpp"

#include "Defs.h"
#include "Device.hpp"
#include "Instance.hpp"
#include "Queue.hpp"

#include <Utilities/XPLogger.h>

namespace val {

void
PhysicalDevice::detectQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, Queues& queues)
{
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    std::vector<int32_t> graphicsQueueIndicesStack;
    std::vector<int32_t> presentationQueueIndicesStack;

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {

        VkBool32 supportsPresentation = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresentation);
        if (supportsPresentation == VK_TRUE && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queues.graphics.index     = i;
            queues.presentation.index = i;
        }

        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) { queues.compute.index = i; }
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) { queues.transfer.index = i; }
    }
}

bool
PhysicalDevice::meetsRequirements(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const Queues& queues)
{
    SwapchainSupportDetails swapchainDetails = querySwapchainSupportDetails(physicalDevice, surface);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

#if defined(XP_PLATFORM_WINDOWS)
    if (properties.deviceType != VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { return false; }
#endif

    bool supportsRequiredQueueFamilies = true;
    supportsRequiredQueueFamilies &= queues.graphics.index != -1;
    supportsRequiredQueueFamilies &= queues.compute.index != -1;
    supportsRequiredQueueFamilies &= queues.transfer.index != -1;
    supportsRequiredQueueFamilies &= queues.presentation.index != -1;
    if (!supportsRequiredQueueFamilies) { return false; }

    // device extension support
    uint32_t extensionCount;
    VULKAN_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr),
                  "Failed to enumerate device extension properties");
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VULKAN_ASSERT(
      vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()),
      "Failed to enumerate available device extension properties");

    for (const auto& requiredExtension : RequiredExtensions) {
        bool found = false;
        for (const auto& availableExtension : availableExtensions) {
            if (strcmp(requiredExtension, availableExtension.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) { XP_LOGV(XPLoggerSeverityFatal, "Missing required extension %s", requiredExtension); }
    }

    bool swapchainMeetsRequirements = true;
    swapchainMeetsRequirements &= !swapchainDetails.formats.empty();
    swapchainMeetsRequirements &= !swapchainDetails.presentationModes.empty();
    if (!swapchainMeetsRequirements) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to meet swapchain requirements");
        return false;
    }

    if (!features.samplerAnisotropy) {
        XP_LOG(XPLoggerSeverityFatal, "Failed to find feature <samplerAnisotropy>");
        return false;
    }

    return true;
}

SwapchainSupportDetails
PhysicalDevice::querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    SwapchainSupportDetails swapchainSupportDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportDetails.capabilities);
    // surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    swapchainSupportDetails.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupportDetails.formats.data());

    // presentation modes
    uint32_t presentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
    swapchainSupportDetails.presentationModes.resize(presentationModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentationModeCount, swapchainSupportDetails.presentationModes.data());

    return swapchainSupportDetails;
}

PhysicalDevice::PhysicalDevice(Instance* instance)
  : instance(instance)
  , queues(std::make_unique<Queues>())
  , device(std::make_unique<Device>(this))
{
}

PhysicalDevice::~PhysicalDevice() {}

void
PhysicalDevice::selectPhysicalDevice()
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    if (deviceCount <= 0) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't find any supported physical devices");
        return;
    }
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, physicalDevices.data());
    for (uint32_t i = 0; i < deviceCount; ++i) {
        detectQueueFamilyIndices(physicalDevices[i], instance->getSurface(), *queues);
        if (meetsRequirements(physicalDevices[i], instance->getSurface(), *queues)) {
            physicalDevice = physicalDevices[i];
            break;
        }
    }
    if (physicalDevice == nullptr) {
        XP_LOG(XPLoggerSeverityFatal, "Couldn't find a matching physical device");
        return;
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    // _window->setPhysicalDeviceStr(std::string(physicalDeviceProperties.deviceName,
    //                                           physicalDeviceProperties.deviceName +
    //                                           VK_MAX_PHYSICAL_DEVICE_NAME_SIZE));
    // _window->refreshTitleBar();
}

VkFormat
PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates,
                                    VkImageTiling                tiling,
                                    VkFormatFeatureFlags         features) const
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    XP_LOG(XPLoggerSeverityFatal, "Failed to find supported format");
    return candidates.back();
}

VkFormat
PhysicalDevice::findSupportedDepthFormat() const
{
    return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

Instance*
PhysicalDevice::getInstance() const
{
    return instance;
}

VkPhysicalDevice
PhysicalDevice::getPhysicalDevice() const
{
    return physicalDevice;
}

Queues&
PhysicalDevice::getQueues() const
{
    return *queues;
}

Device&
PhysicalDevice::getDevice() const
{
    return *device;
}

} // namespace val
