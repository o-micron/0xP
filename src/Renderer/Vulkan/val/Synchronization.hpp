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

#include "Defs.h"

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace val {

struct Semaphore
{
    VkSemaphore imageAvailable;
    VkSemaphore renderFinished;
};

struct Synchronization
{
    void initialize(VkDevice device);
    void finalize(VkDevice device);

    std::array<Semaphore, XPMaxBuffersInFlight> semaphores;
    std::array<VkFence, XPMaxBuffersInFlight>   inFlightFences;
    std::array<VkFence, XPMaxBuffersInFlight>   imagesInFlightFences;

  private:
    void createSemaphores(VkDevice device);
    void createFences(VkDevice device);
    void destroySemaphores(VkDevice device);
    void destroyFences(VkDevice device);
};

} // namespace val
