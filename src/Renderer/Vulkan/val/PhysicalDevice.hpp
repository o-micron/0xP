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
#include <vector>

namespace val {

class Queue;
class Instance;
class Device;
struct Queues;

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentationModes;
};

class PhysicalDevice
{
  public:
    static void detectQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, Queues& queues);
    static bool meetsRequirements(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const Queues& queues);
    static SwapchainSupportDetails querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    PhysicalDevice(Instance* instance);
    ~PhysicalDevice();
    void     selectPhysicalDevice();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                 VkImageTiling                tiling,
                                 VkFormatFeatureFlags         features) const;
    VkFormat findSupportedDepthFormat() const;

    Instance*        getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;
    Queues&          getQueues() const;
    Device&          getDevice() const;

  private:
    Instance*               instance;
    VkPhysicalDevice        physicalDevice;
    std::unique_ptr<Queues> queues;
    std::unique_ptr<Device> device;
};

} // namespace val
