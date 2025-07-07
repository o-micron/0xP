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

class XPVulkanRenderer;

namespace val {

struct Texture;

class Swapchain
{
  public:
    Swapchain(XPVulkanRenderer* renderer);
    ~Swapchain();
    void createSwapchain();
    void createDepthResources();
    void destroySwapchain();
    void destroyDepthResources();

    XPVulkanRenderer*                      getRenderer() const;
    VkSurfaceFormatKHR                     getFormat() const;
    VkPresentModeKHR                       getPresentMode() const;
    VkExtent2D                             getExtent() const;
    VkSwapchainKHR                         getSwapchain() const;
    std::vector<std::unique_ptr<Texture>>& getColorTextures();
    std::vector<std::unique_ptr<Texture>>& getDepthTextures();

  private:
    VkSurfaceFormatKHR selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    VkPresentModeKHR   selectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D         selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    XPVulkanRenderer*                     renderer;
    VkSurfaceFormatKHR                    format;
    VkPresentModeKHR                      presentMode;
    VkExtent2D                            extent;
    VkSwapchainKHR                        swapchain;
    std::vector<std::unique_ptr<Texture>> colorTextures;
    std::vector<std::unique_ptr<Texture>> depthTextures;
};

} // namespace val
