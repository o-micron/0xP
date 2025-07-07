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

#include <Renderer/WebGPU/XPWGPURenderer.h>
#include <Renderer/WebGPU/XPWGPUUIImpl.h>
#include <Renderer/WebGPU/XPWGPUWindow.h>

#include <glfw/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#pragma clang diagnostic pop

XPWGPUUIImpl::XPWGPUUIImpl(XPIRenderer* const renderer, XPIUI* const ui)
  : XPUIImpl(renderer, ui)
  , _renderer(renderer)
  , _ui(ui)
{
}

XPWGPUUIImpl::~XPWGPUUIImpl() {}

void
XPWGPUUIImpl::initialize()
{
    XPWGPURenderer* const renderer = dynamic_cast<XPWGPURenderer*>(_renderer);

    // _renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    // WGPU_CHECK(_renderPassDescriptor, "Failed to get a valid wgpu render pass descriptor");

    _ui->initialize();

    ImGui_ImplGlfw_InitForOther(renderer->getWindow()->getWindow(), true);
    ImGui_ImplWGPU_Init(
      renderer->getDevice(), XPMaxBuffersInFlight, WGPUTextureFormat_BGRA8Unorm, WGPUTextureFormat_Undefined);
}

void
XPWGPUUIImpl::finalize()
{
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    _ui->finalize();
}

void
XPWGPUUIImpl::beginFrame()
{
    XPWGPURenderer* const renderer = dynamic_cast<XPWGPURenderer*>(_renderer);

    // MTL::ClearColor uiClearColor(XP_DEFAULT_CLEAR_COLOR);
    // _renderPassDescriptor->colorAttachments()->object(0)->setClearColor(uiClearColor);
    // _renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
    // _renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
    // _renderPassDescriptor->colorAttachments()->object(0)->setTexture(renderer->getDrawable()->texture());

    // _encoder = renderer->getRenderingCommandBuffer()->renderCommandEncoder(_renderPassDescriptor);
    // _encoder->setLabel(NS_STRING_FROM_CSTRING("UIRenderEncoder"));
    // _encoder->pushDebugGroup(NS_STRING_FROM_CSTRING("UIRenderEncoder"));

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void
XPWGPUUIImpl::endFrame()
{
    XPWGPURenderer* const renderer = dynamic_cast<XPWGPURenderer*>(_renderer);

    ImGui::EndFrame();
    ImGui::Render();

    WGPURenderPassColorAttachment color_attachments = {};
    color_attachments.loadOp                        = WGPULoadOp_Clear;
    color_attachments.storeOp                       = WGPUStoreOp_Store;
    color_attachments.clearValue                    = { XP_DEFAULT_CLEAR_COLOR };
    color_attachments.view                          = wgpuSwapChainGetCurrentTextureView(renderer->getSwapchain());
    WGPURenderPassDescriptor render_pass_desc       = {};
    render_pass_desc.colorAttachmentCount           = 1;
    render_pass_desc.colorAttachments               = &color_attachments;
    render_pass_desc.depthStencilAttachment         = nullptr;

    WGPUCommandEncoderDescriptor enc_desc = {};
    WGPUCommandEncoder           encoder  = wgpuDeviceCreateCommandEncoder(renderer->getDevice(), &enc_desc);

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor cmd_buffer_desc = {};
    WGPUCommandBuffer           cmd_buffer      = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
    WGPUQueue                   queue           = wgpuDeviceGetQueue(renderer->getDevice());
    wgpuQueueSubmit(queue, 1, &cmd_buffer);

    // _encoder->popDebugGroup();
    // _encoder->endEncoding();
}

XPIUI*
XPWGPUUIImpl::getUI() const
{
    return _ui;
}

void
XPWGPUUIImpl::invalidateDeviceObjects()
{
    ImGui_ImplWGPU_InvalidateDeviceObjects();
}

void
XPWGPUUIImpl::createDeviceObjects()
{
    ImGui_ImplWGPU_CreateDeviceObjects();
}

void
XPWGPUUIImpl::onEvent(SDL_Event* event)
{
    _ui->onEvent(event);
}

void
XPWGPUUIImpl::simulateCopy(const char* text)
{
    ImGui::SetClipboardText(text);
}

std::string
XPWGPUUIImpl::simulatePaste()
{
    return ImGui::GetClipboardText();
}
