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

#include <memory>

class XPVulkanRenderer;

namespace val {

class PhysicalDevice;

class Instance
{
  public:
    Instance(XPVulkanRenderer* renderer);
    ~Instance();
    void createInstance();
    void loadDynamicFunctions();
    void createDebuggerUtilsMessenger();
    void createSurface();
    void destroyInstance();
    void unloadDynamicFunctions();
    void destroyDebuggerUtilsMessenger();
    void destroySurface();

    XPVulkanRenderer*        getRenderer() const;
    VkInstance               getInstance() const;
    VkSurfaceKHR             getSurface() const;
    VkDebugUtilsMessengerEXT getDebugMessenger() const;
    PhysicalDevice&          getPhysicalDevice() const;

  private:
    XPVulkanRenderer*               renderer;
    VkInstance                      instance;
    VkSurfaceKHR                    surface;
    VkDebugUtilsMessengerEXT        debugMessenger;
    std::unique_ptr<PhysicalDevice> physicalDevice;
};

} // namespace val
