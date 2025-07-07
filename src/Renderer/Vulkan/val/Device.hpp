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

#include "Enums.h"

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <stdint.h>

namespace val {

class PhysicalDevice;
struct CommandPools;
class Swapchain;
struct Buffer;
struct Texture;

class Device
{
  public:
    Device(PhysicalDevice* physicalDevice);
    ~Device();
    void                    createDevice();
    void                    createCommandPools(Swapchain* swapchain);
    void                    destroyDevice();
    void                    destroyCommandPools();
    void                    freeCommandPools();
    VkDeviceSize            getBufferSizeInBytes(const Buffer& buffer);
    void                    readBuffer(const Buffer& buffer, void* data, uint64_t offset, uint64_t& numBytes);
    void                    writeBuffer(Buffer& buffer, void* data, uint64_t offset, uint64_t& numBytes);
    void                    copyBuffer(Buffer&       dstBuffer,
                                       const Buffer& srcBuffer,
                                       uint64_t      dstOffset,
                                       uint64_t      srcOffset,
                                       uint64_t      numBytes);
    std::unique_ptr<Buffer> createBuffer(uint64_t              numBytes,
                                         VkBufferUsageFlags    usageFlags,
                                         VkMemoryPropertyFlags propertyFlags);

    std::unique_ptr<Buffer> createTransferBuffer(uint64_t numBytes);
    std::unique_ptr<Buffer> createVertexBuffer(uint64_t numBytes, EResourceStorageMode storageMode);
    std::unique_ptr<Buffer> createIndexBuffer(uint64_t numBytes, EResourceStorageMode storageMode);
    void                    destroyBuffer(Buffer& buffer);
    VkResult                mapBuffer(Buffer& buffer, uint64_t numBytes, uint64_t offset);
    VkResult                mapBuffer(Buffer& buffer);
    void                    unmapBuffer(Buffer& buffer);
    VkResult                flushBuffer(Buffer& buffer, uint64_t numBytes, uint64_t offset);
    VkResult                flushBuffer(Buffer& buffer);

    void destroyImage(Texture& texture);
    void destroyImageView(Texture& texture);
    void destroySampler(Texture& texture);

    void flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free);
    void executeSingleTimeCommand(VkQueue queue, VkCommandPool pool, std::function<void(VkCommandBuffer)>&& cbfn);

    void makeImageBarrier(VkCommandBuffer      commandBuffer,
                          VkImage              image,
                          VkPipelineStageFlags srcStageMask,
                          VkAccessFlags        srcAccessMask,
                          VkPipelineStageFlags dstStageMask,
                          VkAccessFlags        dstAccessMask,
                          VkImageLayout        oldLayout,
                          VkImageLayout        newLayout,
                          VkImageAspectFlags   aspectMask,
                          VkFormat             format);

    PhysicalDevice*               physicalDevice;
    VkDevice                      device;
    std::unique_ptr<CommandPools> commandPools;

  private:
    VkMemoryPropertyFlags findMemoryPropertiesFromStorageMode(EResourceStorageMode storageMode) const;
    uint32_t              findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;
};

} // namespace val
