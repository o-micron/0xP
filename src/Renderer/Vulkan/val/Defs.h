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

#include <assert.h>
#include <vector>

namespace val {

static const size_t XPMaxBuffersInFlight    = 3;
static const size_t XPNumPerFrameBuffers    = XPMaxBuffersInFlight + 1;
static const size_t XPMaxCopyBufferSize     = 10'000;
static const size_t XPMaxNumObjectsPerScene = 200'000;
static const size_t XPMaxNumMeshesPerScene  = 400'000;

static const std::vector<const char*> RequiredValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#if defined(XP_PLATFORM_MACOS)
static const std::vector<const char*> RequiredExtensions = { "VK_KHR_swapchain",
                                                             "VK_KHR_dynamic_rendering",
                                                             "VK_KHR_portability_subset",
                                                             "VK_KHR_synchronization2",
    #if defined(XP_VULKAN_DEBUG_UTILS)
                                                             "VK_EXT_debug_utils"
    #endif
};
#else
static const std::vector<const char*> RequiredExtensions = { "VK_KHR_swapchain",
                                                             "VK_KHR_dynamic_rendering",
                                                             "VK_KHR_synchronization2"
    #if defined(XP_VULKAN_DEBUG_UTILS)
                                                             "VK_EXT_debug_utils"
    #endif
};
#endif

#define XP_DEFAULT_CLEAR_COLOR 0.0, 0.0, 0.0, 0.0

#define VULKAN_ASSERT(result, MSG)                                                                                     \
    if (result != VK_SUCCESS) { XP_LOG(XPLoggerSeverityFatal, MSG) }

static inline void
VULKAN_CHECK(VkResult result)
{
    assert(result == VK_SUCCESS);
}

} // namespace val
