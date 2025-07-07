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

#include "Synchronization.hpp"

#include "Defs.h"

#include <Utilities/XPLogger.h>

namespace val {

void
Synchronization::initialize(VkDevice device)
{
    createSemaphores(device);
    createFences(device);
}

void
Synchronization::finalize(VkDevice device)
{
    destroyFences(device);
    destroySemaphores(device);
}

void
Synchronization::createSemaphores(VkDevice device)
{
    for (size_t i = 0; i < semaphores.size(); ++i) {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext                 = nullptr;
        semaphoreCreateInfo.flags                 = 0;
        VULKAN_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores[i].imageAvailable),
                      "Failed to create image avaiable semaphore");
        VULKAN_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores[i].renderFinished),
                      "Failed to create render finished semaphore");
    }
}

void
Synchronization::createFences(VkDevice device)
{
    for (size_t i = 0; i < inFlightFences.size(); ++i) {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext             = nullptr;
        fenceCreateInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
        VULKAN_ASSERT(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]), "Failed to create fence");
    }
    for (size_t i = 0; i < imagesInFlightFences.size(); ++i) { imagesInFlightFences[i] = VK_NULL_HANDLE; }
}

void
Synchronization::destroySemaphores(VkDevice device)
{
    for (size_t i = 0; i < semaphores.size(); ++i) {
        vkDestroySemaphore(device, semaphores[i].renderFinished, nullptr);
        vkDestroySemaphore(device, semaphores[i].imageAvailable, nullptr);
    }
}

void
Synchronization::destroyFences(VkDevice device)
{
    for (size_t i = 0; i < inFlightFences.size(); ++i) { vkDestroyFence(device, inFlightFences[i], nullptr); }
}

} // namespace val
