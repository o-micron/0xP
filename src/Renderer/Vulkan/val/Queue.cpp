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

#include "Queue.hpp"

#include "CommandBuffer.hpp"

#include <Renderer/Vulkan/XPVulkan.h>
#include <Renderer/Vulkan/XPVulkanExt.h>

namespace val {

void
Queue::setLabel(VkInstance instance, const char* name)
{
    VkDebugUtilsLabelEXT labelCreateInfo = {};
    labelCreateInfo.sType                = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelCreateInfo.pNext                = nullptr;
    labelCreateInfo.pLabelName           = name;

    vk::dyn::vkQueueBeginDebugUtilsLabelEXT(queue, &labelCreateInfo);
}

void
Queue::submit(CommandBuffer& commandBuffer)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext              = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer.commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
}

} // namespace val
