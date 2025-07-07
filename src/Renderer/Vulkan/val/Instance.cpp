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

#include "Instance.hpp"

#include "Defs.h"
#include "PhysicalDevice.hpp"

#include <Renderer/Vulkan/XPVulkanRenderer.h>
#include <Renderer/Vulkan/XPVulkanWindow.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

// DYNAMICALLY LOADED FUNCTIONS ---------------------------------------------------------------------------------------
namespace vk {
namespace dyn {
extern ::PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT  = nullptr;
extern ::PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
extern ::PFN_vkQueueBeginDebugUtilsLabelEXT  vkQueueBeginDebugUtilsLabelEXT  = nullptr;
extern ::PFN_vkCmdBeginDebugUtilsLabelEXT    vkCmdBeginDebugUtilsLabelEXT    = nullptr;
extern ::PFN_vkCmdBeginRenderingKHR          vkCmdBeginRenderingKHR          = nullptr;
extern ::PFN_vkCmdEndRenderingKHR            vkCmdEndRenderingKHR            = nullptr;
extern ::PFN_vkDebugMarkerSetObjectNameEXT   vkDebugMarkerSetObjectNameEXT   = nullptr;
extern ::PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT    = nullptr;
} // namespace dyn
} // namespace vk
// --------------------------------------------------------------------------------------------------------------------
namespace val {

static VKAPI_ATTR VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void*                                       pUserData);

Instance::Instance(XPVulkanRenderer* renderer)
  : renderer(renderer)
  , physicalDevice(std::make_unique<PhysicalDevice>(this))
{
}

Instance::~Instance() {}

void
Instance::createInstance()
{
    // platform extensions
    std::vector<const char*> platformExtensions;
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(renderer->getWindow().getWindow(), &extensionCount, nullptr);
        std::vector<const char*> extensionNames(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(renderer->getWindow().getWindow(), &extensionCount, extensionNames.data());
        platformExtensions = extensionNames;
        platformExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#if defined(XP_PLATFORM_MACOS)
        platformExtensions.push_back("VK_KHR_portability_enumeration");
#endif
    }

#if defined(XP_VULKAN_VALIDATION_LAYERS)
    // validation layers
    std::vector<VkLayerProperties> availableLayers;
    {
        uint32_t availableLayersCount;
        VULKAN_ASSERT(vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr),
                      "Failed to enumerate instance layer properties");
        availableLayers.resize(availableLayersCount);
        VULKAN_ASSERT(vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data()),
                      "Failed to enumerate instance layer properties");
    }

    // check available layers are available
    for (const auto& requiredValidationLayer : RequiredValidationLayers) {
        bool found = false;
        for (const auto& availableLayer : availableLayers) {
            if (strcmp(requiredValidationLayer, availableLayer.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) { XP_LOGV(XPLoggerSeverityFatal, "Missing required validation layer %s", requiredValidationLayer); }
    }
#endif

    VkApplicationInfo appInfo  = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext              = nullptr;
    appInfo.apiVersion         = VK_API_VERSION_1_3;
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pApplicationName   = "XPAPP";
    appInfo.pEngineName        = "XPENGINE";
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 0, 1);

    VkInstanceCreateInfo instanceCreateInfo    = {};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                   = nullptr;
    instanceCreateInfo.flags                   = 0;
    instanceCreateInfo.pApplicationInfo        = &appInfo;
    instanceCreateInfo.enabledExtensionCount   = (uint32_t)platformExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();
#if defined(XP_VULKAN_VALIDATION_LAYERS)
    instanceCreateInfo.enabledLayerCount   = (uint32_t)RequiredValidationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = RequiredValidationLayers.data();
#else
    instanceCreateInfo.enabledLayerCount   = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
#if defined(XP_PLATFORM_MACOS)
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    // Create instance
    VULKAN_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance), "Failed to create instance");
}

void
Instance::loadDynamicFunctions()
{
    vk::dyn::vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    assert(vk::dyn::vkCreateDebugUtilsMessengerEXT && "Failed to get address of vkCreateDebugUtilsMessengerEXT");

    vk::dyn::vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    assert(vk::dyn::vkDestroyDebugUtilsMessengerEXT && "Failed to get address of vkDestroyDebugUtilsMessengerEXT");

    vk::dyn::vkQueueBeginDebugUtilsLabelEXT =
      (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT");
    assert(vk::dyn::vkQueueBeginDebugUtilsLabelEXT && "Failed to get address of vkQueueBeginDebugUtilsLabelEXT");

    vk::dyn::vkCmdBeginDebugUtilsLabelEXT =
      (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
    assert(vk::dyn::vkCmdBeginDebugUtilsLabelEXT && "Failed to get address of vkCmdBeginDebugUtilsLabelEXT");

    vk::dyn::vkCmdBeginRenderingKHR =
      (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(instance, "vkCmdBeginRenderingKHR");
    assert(vk::dyn::vkCmdBeginRenderingKHR && "Failed to get address of vkCmdBeginRenderingKHR");

    vk::dyn::vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(instance, "vkCmdEndRenderingKHR");
    assert(vk::dyn::vkCmdEndRenderingKHR && "Failed to get address of vkCmdEndRenderingKHR");

    vk::dyn::vkDebugMarkerSetObjectNameEXT =
      (PFN_vkDebugMarkerSetObjectNameEXT)vkGetInstanceProcAddr(instance, "vkDebugMarkerSetObjectNameEXT");
    assert(vk::dyn::vkDebugMarkerSetObjectNameEXT && "Failed to get address of vkDebugMarkerSetObjectNameEXT");

    vk::dyn::vkSetDebugUtilsObjectNameEXT =
      (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
    assert(vk::dyn::vkSetDebugUtilsObjectNameEXT && "Failed to get address of vkSetDebugUtilsObjectNameEXT");
}

void
Instance::createDebuggerUtilsMessenger()
{
    // Create debugger
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.pNext                              = nullptr;
    debugCreateInfo.flags                              = 0;
    debugCreateInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData       = this;

    vk::dyn::vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger);
}

void
Instance::createSurface()
{
    renderer->getWindow().createSurface(instance, surface);
}

void
Instance::destroyInstance()
{
    vkDestroyInstance(instance, nullptr);
}

void
Instance::unloadDynamicFunctions()
{
    vk::dyn::vkCmdEndRenderingKHR            = nullptr;
    vk::dyn::vkCmdBeginRenderingKHR          = nullptr;
    vk::dyn::vkCmdBeginDebugUtilsLabelEXT    = nullptr;
    vk::dyn::vkQueueBeginDebugUtilsLabelEXT  = nullptr;
    vk::dyn::vkDestroyDebugUtilsMessengerEXT = nullptr;
    vk::dyn::vkCreateDebugUtilsMessengerEXT  = nullptr;
    vk::dyn::vkDebugMarkerSetObjectNameEXT   = nullptr;
    vk::dyn::vkSetDebugUtilsObjectNameEXT    = nullptr;
}

void
Instance::destroyDebuggerUtilsMessenger()
{
    if (debugMessenger) { vk::dyn::vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr); }
}

void
Instance::destroySurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

static VKAPI_ATTR VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void*                                       pUserData)
{
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            XP_LOGV(XPLoggerSeverityError, "%s", pCallbackData->pMessage)
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            XP_LOGV(XPLoggerSeverityWarning, "%s", pCallbackData->pMessage)
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            XP_LOGV(XPLoggerSeverityInfo, "%s", pCallbackData->pMessage)
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            XP_LOGV(XPLoggerSeverityTrace, "%s", pCallbackData->pMessage)
        } break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: {
            XP_LOG(XPLoggerSeverityFatal, "Unreachable()!")
        } break;
    }

    return VK_FALSE;
}

XPVulkanRenderer*
Instance::getRenderer() const
{
    return renderer;
}

VkInstance
Instance::getInstance() const
{
    return instance;
}

VkSurfaceKHR
Instance::getSurface() const
{
    return surface;
}

VkDebugUtilsMessengerEXT
Instance::getDebugMessenger() const
{
    return debugMessenger;
}

PhysicalDevice&
Instance::getPhysicalDevice() const
{
    return *physicalDevice;
}

} // namespace val
