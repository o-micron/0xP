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

#include <Renderer/WebGPU/XPShaderTypes.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <Renderer/WebGPU/wgpu_cpp.h>

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------------------------------------
// IMPORTANT: There is a bit of a difference it seems between dawn, rust wgpu and emscripten ..
// ---------------------------------------------------------------------------------------------------------
// If using Dawn
#define WEBGPU_BACKEND_DAWN

// If using wgpu-native
#define WEBGPU_BACKEND_WGPU

// If using emscripten
#define WEBGPU_BACKEND_EMSCRIPTEN
// ---------------------------------------------------------------------------------------------------------

static const size_t XPMaxBuffersInFlight               = 3;
static const size_t XPNumPerFrameBuffers               = XPMaxBuffersInFlight + 1;
static const size_t XPMaxCopyBufferSize                = 10'000;
static const size_t XPMaxNumObjectsPerScene            = 200'000;
static const size_t XPMaxNumMeshesPerScene             = 400'000;

#define XP_DEFAULT_CLEAR_COLOR 0.0, 0.0, 0.0, 1.0

#define NS_STRING_FROM_CSTRING(STR) NS::String::string(STR, NS::UTF8StringEncoding)

#define WGPU_CHECK(PTR, MSG)                                                                                           \
    if (!PTR) { XP_LOG(XPLoggerSeverityFatal, MSG) }

class XPScene;
class XPMeshBuffer;
class XPShaderBuffer;
class XPTextureBuffer;
class XPMeshAsset;

struct XPWGPURenderPipelineCreationInfo
{
    enum RenderPipelineFlags : uint8_t
    {
        RenderPipelineFlags_None                    = 0,
        RenderPipelineFlags_CompileShaderFromSource = 1,
        RenderPipelineFlags_IncludeFragmentFunction = 2,
        RenderPipelineFlags_EnableDepthWrite        = 4,
        RenderPipelineFlags_EnableStencilWrite      = 8
    };

    XPWGPURenderPipelineCreationInfo(
      std::string name,
      //   NS::SharedPtr<MTL::VertexDescriptor> vertexDescriptor,
      RenderPipelineFlags flags,
      //   std::vector<MTL::PixelFormat>        colorAttachmentPixelFormats = {},
      //   std::optional<MTL::PixelFormat>      depthAttachmentPixelFormat  = {},
      //   std::optional<MTL::PixelFormat>      stencilPixelFormat          = {},
      //   MTL::CompareFunction                 depthComparisonFunction     = MTL::CompareFunction::CompareFunctionLess,
      int sampleCount = 1)
      : name(name)
      //   , vertexDescriptor(vertexDescriptor)
      , sampleCount(sampleCount)
      , flags(flags)
    //   , depthComparisonFunction(depthComparisonFunction)
    //   , colorAttachmentPixelFormats(colorAttachmentPixelFormats)
    //   , depthAttachmentPixelFormat(depthAttachmentPixelFormat)
    //   , stencilPixelFormat(stencilPixelFormat)
    {
    }

    bool isShaderCompiledFromBinary() const
    {
        return !((flags & RenderPipelineFlags_CompileShaderFromSource) == RenderPipelineFlags_CompileShaderFromSource);
    }

    bool hasFragmentFunction() const
    {
        return (flags & RenderPipelineFlags_IncludeFragmentFunction) == RenderPipelineFlags_IncludeFragmentFunction;
    }

    bool isDepthWriteEnabled() const
    {
        return (flags & RenderPipelineFlags_EnableDepthWrite) == RenderPipelineFlags_EnableDepthWrite;
    }

    bool isStencilWriteEnabled() const
    {
        return (flags & RenderPipelineFlags_EnableStencilWrite) == RenderPipelineFlags_EnableStencilWrite;
    }

    std::string name;
    // NS::SharedPtr<MTL::VertexDescriptor> vertexDescriptor;
    int                 sampleCount;
    RenderPipelineFlags flags;
    // MTL::CompareFunction                 depthComparisonFunction;
    // std::vector<MTL::PixelFormat>        colorAttachmentPixelFormats;
    // std::optional<MTL::PixelFormat>      depthAttachmentPixelFormat;
    // std::optional<MTL::PixelFormat>      stencilPixelFormat;
};

struct XPWGPURenderPipeline
{
    WGPURenderPipelineDescriptor renderPipelineDescriptor;
    WGPURenderPipeline           renderPipelineState = nullptr;
    WGPURenderPipelineDescriptor depthStencilDescriptor;
    WGPURenderPipeline           depthStencilState = nullptr;
};

struct XPWGPUComputePipelineCreationInfo
{
    enum ComputePipelineFlags : uint8_t
    {
        ComputePipelineFlags_None                    = 0,
        ComputePipelineFlags_CompileShaderFromSource = 1
    };

    XPWGPUComputePipelineCreationInfo(std::string name, std::vector<size_t> bindings, ComputePipelineFlags flags)
      : name(name)
      , bindings(bindings)
      , flags(flags)
    {
    }

    bool isShaderCompiledFromBinary() const
    {
        return !((flags & ComputePipelineFlags_CompileShaderFromSource) ==
                 ComputePipelineFlags_CompileShaderFromSource);
    }

    std::string          name;
    std::vector<size_t>  bindings;
    ComputePipelineFlags flags;
};

struct XPWGPUComputePipeline
{
    std::string                                           functionName;
    WGPUComputePipeline                                   state;
    std::optional<std::function<void(WGPUCommandBuffer)>> completionHandler;
    std::vector<WGPUBuffer>                               buffers;
    bool                                                  isAsynchrounous;
};

struct XPWGPUMesh;

struct XPWGPUMeshObject
{
    XPWGPUMeshObject(XPWGPUMesh& mesh, const XPBoundingBox& boundingBox)
      : mesh(mesh)
      , boundingBox(boundingBox)
    {
    }

    XPWGPUMesh&   mesh;
    std::string   name;
    uint32_t      vertexOffset;
    uint32_t      indexOffset;
    uint32_t      numIndices;
    XPBoundingBox boundingBox;
};

struct XPWGPUMesh
{
    const XPMeshAsset*            meshAsset = nullptr;
    WGPUBuffer                    vertexBuffer;
    WGPUBuffer                    normalBuffer;
    WGPUBuffer                    uvBuffer;
    WGPUBuffer                    indexBuffer;
    std::vector<XPWGPUMeshObject> objects;
};

struct XPWGPUTexture
{
    WGPUTextureDescriptor textureDescriptor;
    WGPUTexture           texture = nullptr;
};

struct XPWGPUShader
{};

struct XPWGPUMaterial
{};

struct XPWGPUViewport
{
    double originX;
    double originY;
    double width;
    double height;
    double znear;
    double zfar;
};

struct XPWGPUFrameBuffer
{
    std::vector<XPWGPUTexture> colorAttachments = {};
    XPWGPUTexture              depthAttachment  = {};
    WGPURenderPassDescriptor   passDescriptor;
};