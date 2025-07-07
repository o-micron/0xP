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

#include "CommandPool.hpp"

#include "CommandBuffer.hpp"

namespace val {

std::unique_ptr<CommandBuffer>
CommandPool::createCommandBuffer(VkDevice device)
{
    auto commandBuffer = std::make_unique<CommandBuffer>();

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool                 = pool;
    allocInfo.commandBufferCount          = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer->commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer->commandBuffer, &beginInfo);

    return commandBuffer;
}

void
CommandPool::destroyCommandBuffer(VkDevice device, std::unique_ptr<CommandBuffer> commandBuffer)
{
    vkFreeCommandBuffers(device, pool, 1, &commandBuffer->commandBuffer);
}

} // namespace val
