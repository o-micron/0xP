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

#include "Device.hpp"

#include "Buffer.h"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Defs.h"
#include "FreeFunctions.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Queue.hpp"
#include "Swapchain.hpp"
#include "Texture.h"

#include <Utilities/XPLogger.h>

#include <set>

namespace val {

Device::Device(PhysicalDevice* physicalDevice)
  : physicalDevice(physicalDevice)
  , commandPools(std::make_unique<CommandPools>())
{
}

Device::~Device() {}

void
Device::createDevice()
{
    std::set<uint32_t>       uniqueQueueFamilies = { (uint32_t)physicalDevice->getQueues().graphics.index,
                                                     (uint32_t)physicalDevice->getQueues().compute.index,
                                                     (uint32_t)physicalDevice->getQueues().transfer.index,
                                                     (uint32_t)physicalDevice->getQueues().presentation.index };
    float                    queuePriorities     = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures      = {};
    deviceFeatures.samplerAnisotropy             = VK_TRUE;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t uniqueQueueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext            = nullptr;
        queueCreateInfo.flags            = 0;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriorities;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = {};
    dynamicRenderingFeature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.pNext            = nullptr;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo      = {};
    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                   = &dynamicRenderingFeature;
    deviceCreateInfo.flags                   = 0;
    deviceCreateInfo.queueCreateInfoCount    = (uint32_t)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount   = (uint32_t)RequiredExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();

#if defined(PROTOTYPE_ENABLE_VULKAN_VALIDATION_LAYERS)
    deviceCreateInfo.enabledLayerCount   = (uint32_t)requiredValidationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
    deviceCreateInfo.enabledLayerCount   = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
#endif
    VULKAN_ASSERT(
      vkCreateDevice(
        physicalDevice->getPhysicalDevice(), &deviceCreateInfo, nullptr, &physicalDevice->getDevice().device),
      "Failed to create device");

    vkGetDeviceQueue(physicalDevice->getDevice().device,
                     physicalDevice->getQueues().graphics.index,
                     0,
                     &physicalDevice->getQueues().graphics.queue);
    vkGetDeviceQueue(physicalDevice->getDevice().device,
                     physicalDevice->getQueues().compute.index,
                     0,
                     &physicalDevice->getQueues().compute.queue);
    vkGetDeviceQueue(physicalDevice->getDevice().device,
                     physicalDevice->getQueues().transfer.index,
                     0,
                     &physicalDevice->getQueues().transfer.queue);
    vkGetDeviceQueue(physicalDevice->getDevice().device,
                     physicalDevice->getQueues().presentation.index,
                     0,
                     &physicalDevice->getQueues().presentation.queue);
}

void
Device::createCommandPools(Swapchain* swapchain)
{
    // --------------------------------------------------------------------------------------------
    // GRAPHICS
    // --------------------------------------------------------------------------------------------
    {
        VkCommandPoolCreateInfo standardCommandPoolCreateInfo = {};
        standardCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        standardCommandPoolCreateInfo.pNext                   = nullptr;
        standardCommandPoolCreateInfo.flags =
          VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        standardCommandPoolCreateInfo.queueFamilyIndex = (uint32_t)physicalDevice->getQueues().graphics.index;
        VULKAN_ASSERT(
          vkCreateCommandPool(
            physicalDevice->getDevice().device, &standardCommandPoolCreateInfo, nullptr, &commandPools->standard.pool),
          "Failed to create a graphics standard command pool");

        commandPools->standard.buffers.resize(swapchain->getColorTextures().size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.pNext                       = nullptr;
        allocCreateInfo.commandPool                 = commandPools->standard.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (uint32_t)commandPools->standard.buffers.size();
        VULKAN_ASSERT(vkAllocateCommandBuffers(device, &allocCreateInfo, commandPools->standard.buffers.data()),
                      "Failed to allocate graphics standard command buffer");
    }
    {
        VkCommandPoolCreateInfo transientCommandPoolCreateInfo = {};
        transientCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transientCommandPoolCreateInfo.pNext                   = nullptr;
        transientCommandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        transientCommandPoolCreateInfo.queueFamilyIndex        = (uint32_t)physicalDevice->getQueues().graphics.index;
        VULKAN_ASSERT(vkCreateCommandPool(physicalDevice->getDevice().device,
                                          &transientCommandPoolCreateInfo,
                                          nullptr,
                                          &commandPools->transient.pool),
                      "Failed to create a graphics transient command pool");

        commandPools->transient.buffers.resize(swapchain->getColorTextures().size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.pNext                       = nullptr;
        allocCreateInfo.commandPool                 = commandPools->transient.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (uint32_t)commandPools->transient.buffers.size();
        VULKAN_ASSERT(vkAllocateCommandBuffers(device, &allocCreateInfo, commandPools->transient.buffers.data()),
                      "Failed to allocate graphics transient command buffer");
    }
    {
        VkCommandPoolCreateInfo resetCommandPoolCreateInfo = {};
        resetCommandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        resetCommandPoolCreateInfo.pNext                   = nullptr;
        resetCommandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        resetCommandPoolCreateInfo.queueFamilyIndex        = (uint32_t)physicalDevice->getQueues().graphics.index;
        VULKAN_ASSERT(
          vkCreateCommandPool(
            physicalDevice->getDevice().device, &resetCommandPoolCreateInfo, nullptr, &commandPools->reset.pool),
          "Failed to create a graphics reset command pool");

        commandPools->reset.buffers.resize(swapchain->getColorTextures().size());
        VkCommandBufferAllocateInfo allocCreateInfo = {};
        allocCreateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocCreateInfo.pNext                       = nullptr;
        allocCreateInfo.commandPool                 = commandPools->reset.pool;
        allocCreateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocCreateInfo.commandBufferCount          = (uint32_t)commandPools->reset.buffers.size();
        VULKAN_ASSERT(vkAllocateCommandBuffers(device, &allocCreateInfo, commandPools->reset.buffers.data()),
                      "Failed to allocate graphics reset command buffer");
    }
    // --------------------------------------------------------------------------------------------
}

void
Device::destroyDevice()
{
    vkDestroyDevice(device, nullptr);
}

void
Device::destroyCommandPools()
{
    freeCommandPools();

    // --------------------------------------------------------------------------------------------
    // GRAPHICS
    // --------------------------------------------------------------------------------------------
    vkDestroyCommandPool(device, commandPools->reset.pool, nullptr);
    commandPools->reset.pool = nullptr;

    vkDestroyCommandPool(device, commandPools->transient.pool, nullptr);
    commandPools->transient.pool = nullptr;

    vkDestroyCommandPool(device, commandPools->standard.pool, nullptr);
    commandPools->standard.pool = nullptr;
    // --------------------------------------------------------------------------------------------
}

void
Device::freeCommandPools()
{
    // --------------------------------------------------------------------------------------------
    // GRAPHICS
    // --------------------------------------------------------------------------------------------
    if (!commandPools->reset.buffers.empty()) {
        vkFreeCommandBuffers(device,
                             commandPools->reset.pool,
                             (uint32_t)commandPools->reset.buffers.size(),
                             commandPools->reset.buffers.data());
        commandPools->reset.buffers.clear();
    }
    if (!commandPools->transient.buffers.empty()) {
        vkFreeCommandBuffers(device,
                             commandPools->transient.pool,
                             (uint32_t)commandPools->transient.buffers.size(),
                             commandPools->transient.buffers.data());
        commandPools->transient.buffers.clear();
    }
    if (!commandPools->standard.buffers.empty()) {
        vkFreeCommandBuffers(device,
                             commandPools->standard.pool,
                             (uint32_t)commandPools->standard.buffers.size(),
                             commandPools->standard.buffers.data());
        commandPools->standard.buffers.clear();
    }
    // --------------------------------------------------------------------------------------------
}

VkDeviceSize
Device::getBufferSizeInBytes(const Buffer& buffer)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);
    return memRequirements.size;
}

void
Device::readBuffer(const Buffer& buffer, void* data, uint64_t srcOffset, uint64_t& numBytes)
{
    if (data == nullptr) {
        VkDeviceSize size = getBufferSizeInBytes(buffer);
        numBytes          = size;
        return;
    }
    void* bufferPtr;
    vkMapMemory(device, buffer.memory, srcOffset, numBytes, 0, &bufferPtr);
    memcpy(data, bufferPtr, numBytes);
    vkUnmapMemory(device, buffer.memory);
}

void
Device::writeBuffer(Buffer& buffer, void* data, uint64_t dstOffset, uint64_t& numBytes)
{
    if (data == nullptr) {
        VkDeviceSize size = getBufferSizeInBytes(buffer);
        numBytes          = size;
        return;
    }

    switch (buffer.storageMode) {
        case EResourceStorageModePrivate: {
            auto transferBuffer = createTransferBuffer(numBytes);

            // map transfer buffer memory
            void* transferBufferPtr;
            vkMapMemory(device, transferBuffer->memory, 0, numBytes, 0, &transferBufferPtr);
            memcpy(transferBufferPtr, data, numBytes);
            vkUnmapMemory(device, transferBuffer->memory);

            // copy from device to device memory
            copyBuffer(buffer, *transferBuffer, dstOffset, 0, numBytes);

            // destroy the staging transfer buffer
            destroyBuffer(*transferBuffer);
        } break;
        case EResourceStorageModeShared: {
            void* bufferPtr;
            vkMapMemory(device, buffer.memory, dstOffset, numBytes, 0, &bufferPtr);
            memcpy(bufferPtr, data, numBytes);
            vkUnmapMemory(device, buffer.memory);
        } break;

        default: XP_UNIMPLEMENTED("Unimplemented");
    };
}

void
Device::copyBuffer(Buffer&       dstBuffer,
                   const Buffer& srcBuffer,
                   uint64_t      dstOffset,
                   uint64_t      srcOffset,
                   uint64_t      numBytes)
{
    auto commandBuffer = commandPools->reset.createCommandBuffer(device);
    {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset    = srcOffset;
        copyRegion.dstOffset    = dstOffset;
        copyRegion.size         = numBytes;

        vkCmdCopyBuffer(commandBuffer->commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);
    }
    vkEndCommandBuffer(commandBuffer->commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer->commandBuffer;

    vkQueueSubmit(physicalDevice->getQueues().graphics.queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(physicalDevice->getQueues().graphics.queue);

    commandPools->reset.destroyCommandBuffer(device, std::move(commandBuffer));
}

std::unique_ptr<Buffer>
Device::createBuffer(uint64_t numBytes, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
{
    auto buffer = std::make_unique<Buffer>();

    buffer->mapped = nullptr;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.flags              = 0;
    bufferCreateInfo.size               = numBytes;
    bufferCreateInfo.usage              = usageFlags;
    bufferCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_ASSERT(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer->buffer), "Failed to create buffer");

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext                = nullptr;
    allocInfo.allocationSize       = memoryRequirements.size;
    allocInfo.memoryTypeIndex      = findMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);

    VULKAN_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &buffer->memory), "Failed to allocate memory");

    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);

    buffer->storageMode = EResourceStorageModePrivate;

    return buffer;
}

std::unique_ptr<Buffer>
Device::createTransferBuffer(uint64_t numBytes)
{
    auto buffer = createBuffer(numBytes,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer->storageMode = EResourceStorageModeShared;
    return buffer;
}

std::unique_ptr<Buffer>
Device::createVertexBuffer(uint64_t numBytes, EResourceStorageMode storageMode)
{
    auto buffer         = createBuffer(numBytes,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               findMemoryPropertiesFromStorageMode(storageMode));
    buffer->storageMode = storageMode;
    return buffer;
}

std::unique_ptr<Buffer>
Device::createIndexBuffer(uint64_t numBytes, EResourceStorageMode storageMode)
{
    auto buffer         = createBuffer(numBytes,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                               findMemoryPropertiesFromStorageMode(storageMode));
    buffer->storageMode = storageMode;
    return buffer;
}

void
Device::destroyBuffer(Buffer& buffer)
{
    vkDestroyBuffer(device, buffer.buffer, nullptr);
    vkFreeMemory(device, buffer.memory, nullptr);
}

VkResult
Device::mapBuffer(Buffer& buffer, uint64_t numBytes, uint64_t offset)
{
    return vkMapMemory(device, buffer.memory, offset, numBytes, 0, &buffer.mapped);
}

VkResult
Device::mapBuffer(Buffer& buffer)
{
    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);
    return mapBuffer(buffer, memoryRequirements.size, 0);
}

void
Device::unmapBuffer(Buffer& buffer)
{
    if (buffer.mapped) {
        vkUnmapMemory(device, buffer.memory);
        buffer.mapped = nullptr;
    }
}

VkResult
Device::flushBuffer(Buffer& buffer, uint64_t numBytes, uint64_t offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory              = buffer.memory;
    mappedRange.offset              = offset;
    mappedRange.size                = numBytes;
    return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
}

VkResult
Device::flushBuffer(Buffer& buffer)
{
    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);
    return flushBuffer(buffer, memoryRequirements.size, 0);
}

void
Device::destroyImage(Texture& texture)
{
    vkDestroyImage(device, texture.image, nullptr);
    vkFreeMemory(device, texture.memory, nullptr);
}

void
Device::destroyImageView(Texture& texture)
{
    vkDestroyImageView(device, texture.imageView, nullptr);
}

void
Device::destroySampler(Texture& texture)
{
    vkDestroySampler(device, texture.sampler, nullptr);
}

void
Device::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free)
{
    if (commandBuffer == VK_NULL_HANDLE) { return; }

    VULKAN_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");

    VkSubmitInfo submitInfo       = ff::submitInfo();
    submitInfo.pNext              = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;
    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo = ff::fenceCreateInfo(0);
    VkFence           fence;
    VULKAN_ASSERT(vkCreateFence(device, &fenceInfo, nullptr, &fence), "Failed to create a fence");
    // Submit to the queue
    VULKAN_ASSERT(vkQueueSubmit(queue, 1, &submitInfo, fence), "Failed to submit to queue");
    // Wait for the fence to signal that command buffer has finished executing
    VULKAN_ASSERT(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fences");
    vkDestroyFence(device, fence, nullptr);
    if (free) { vkFreeCommandBuffers(device, pool, 1, &commandBuffer); }
}

void
Device::executeSingleTimeCommand(VkQueue queue, VkCommandPool pool, std::function<void(VkCommandBuffer)>&& cbfn)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool                 = pool;
    allocInfo.commandBufferCount          = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    cbfn(commandBuffer);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
}

// void
// Device::makeImageBarrier(VkCommandBuffer      commandBuffer,
//                          VkImage              image,
//                          VkImageLayout        oldLayout,
//                          VkImageLayout        newLayout,
//                          VkAccessFlags        srcAccessMask,
//                          VkAccessFlags        dstAccessMask,
//                          VkPipelineStageFlags srcStage,
//                          VkPipelineStageFlags dstStage)
// {
//     VkImageMemoryBarrier imageMemoryBarrier            = {};
//     imageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//     imageMemoryBarrier.srcAccessMask                   = srcAccessMask;
//     imageMemoryBarrier.dstAccessMask                   = dstAccessMask;
//     imageMemoryBarrier.oldLayout                       = oldLayout;
//     imageMemoryBarrier.newLayout                       = newLayout;
//     imageMemoryBarrier.image                           = image;
//     imageMemoryBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
//     imageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
//     imageMemoryBarrier.subresourceRange.levelCount     = 1;
//     imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
//     imageMemoryBarrier.subresourceRange.layerCount     = 1;

//     vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
// }

void
Device::makeImageBarrier(VkCommandBuffer      commandBuffer,
                         VkImage              image,
                         VkPipelineStageFlags srcStageMask,
                         VkAccessFlags        srcAccessMask,
                         VkPipelineStageFlags dstStageMask,
                         VkAccessFlags        dstAccessMask,
                         VkImageLayout        oldLayout,
                         VkImageLayout        newLayout,
                         VkImageAspectFlags   aspectMask,
                         VkFormat             format)
{
    VkImageMemoryBarrier imageMemoryBarrier            = {};
    imageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask                   = srcAccessMask;
    imageMemoryBarrier.dstAccessMask                   = dstAccessMask;
    imageMemoryBarrier.oldLayout                       = oldLayout;
    imageMemoryBarrier.newLayout                       = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED; // ignored for exclusive ownership
    imageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED; // ignored for exclusive ownership
    imageMemoryBarrier.image                           = image;
    imageMemoryBarrier.subresourceRange.aspectMask     = aspectMask; // typically VK_IMAGE_ASPECT_COLOR_BIT
    imageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
    imageMemoryBarrier.subresourceRange.levelCount     = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

// void
// Device::makeImageBarrier2(VkCommandBuffer       commandBuffer,
//                           VkImage               image,
//                           VkPipelineStageFlags2 srcStageMask,
//                           VkAccessFlags2        srcAccessMask,
//                           VkPipelineStageFlags2 dstStageMask,
//                           VkAccessFlags2        dstAccessMask,
//                           VkImageLayout         oldLayout,
//                           VkImageLayout         newLayout,
//                           VkImageAspectFlags    aspectMask,
//                           VkFormat              format)
// {
//     VkImageMemoryBarrier2 imageMemoryBarrier           = {};
//     imageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//     imageMemoryBarrier.srcStageMask                    = srcStageMask;
//     imageMemoryBarrier.srcAccessMask                   = srcAccessMask;
//     imageMemoryBarrier.dstStageMask                    = dstStageMask;
//     imageMemoryBarrier.dstAccessMask                   = dstAccessMask;
//     imageMemoryBarrier.oldLayout                       = oldLayout;
//     imageMemoryBarrier.newLayout                       = newLayout;
//     imageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED; // ignored for exclusive ownership
//     imageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED; // ignored for exclusive ownership
//     imageMemoryBarrier.image                           = image;
//     imageMemoryBarrier.subresourceRange.aspectMask     = aspectMask; // typically VK_IMAGE_ASPECT_COLOR_BIT
//     imageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
//     imageMemoryBarrier.subresourceRange.levelCount     = 1;
//     imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
//     imageMemoryBarrier.subresourceRange.layerCount     = 1;

//     VkDependencyInfo dependencyInfo        = {};
//     dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
//     dependencyInfo.pNext                   = nullptr;
//     dependencyInfo.imageMemoryBarrierCount = 1;
//     dependencyInfo.pImageMemoryBarriers    = &imageMemoryBarrier;

//     vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
// }

VkMemoryPropertyFlags
Device::findMemoryPropertiesFromStorageMode(EResourceStorageMode storageMode) const
{
    switch (storageMode) {
        case EResourceStorageModePrivate: return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        case EResourceStorageModeShared:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        default: XP_UNIMPLEMENTED("Unimplemented");
    };

    return 0;
}

uint32_t
Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice->getPhysicalDevice(), &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    XP_LOG(XPLoggerSeverityFatal, "Failed to find suitable memory type");
    return 0;
}

} // namespace val
