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

#include <Renderer/Metal/XPMetalEvent.h>

XPMetalEvent::XPMetalEvent(MTL::Device* device)
{
    _device  = device;
    _event   = NS::TransferPtr(_device->newEvent());
    _counter = 0;
}

void
XPMetalEvent::wait(MTL::CommandBuffer* commandBuffer)
{
    // Wait for the event to be signaled
    commandBuffer->encodeWait(_event.get(), _counter);
}

void
XPMetalEvent::signal(MTL::CommandBuffer* commandBuffer)
{
    // Increase the signal counter
    ++_counter;
    // Signal the event
    commandBuffer->encodeSignalEvent(_event.get(), _counter);
}
