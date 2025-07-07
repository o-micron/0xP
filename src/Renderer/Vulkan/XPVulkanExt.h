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

#include <vulkan/vulkan.h>

// DYNAMICALLY LOADED FUNCTIONS ---------------------------------------------------------------------------------------
namespace vk {
namespace dyn {
extern ::PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT;
extern ::PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
extern ::PFN_vkQueueBeginDebugUtilsLabelEXT  vkQueueBeginDebugUtilsLabelEXT;
extern ::PFN_vkCmdBeginDebugUtilsLabelEXT    vkCmdBeginDebugUtilsLabelEXT;
extern ::PFN_vkCmdBeginRenderingKHR          vkCmdBeginRenderingKHR;
extern ::PFN_vkCmdEndRenderingKHR            vkCmdEndRenderingKHR;
extern ::PFN_vkDebugMarkerSetObjectNameEXT   vkDebugMarkerSetObjectNameEXT;
extern ::PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT;
} // namespace dyn
} // namespace vk
// --------------------------------------------------------------------------------------------------------------------