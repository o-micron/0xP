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

#include <Renderer/Metal/XPShaderTypes.h>
#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#include <Renderer/Metal/metal_cpp.h>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

static const size_t XPMaxBuffersInFlight    = 3;
static const size_t XPNumPerFrameBuffers    = XPMaxBuffersInFlight + 1;
static const size_t XPMaxCopyBufferSize     = 10'000;
static const size_t XPMaxNumObjectsPerScene = 200'000;
static const size_t XPMaxNumMeshesPerScene  = 400'000;

#define XP_DEFAULT_CLEAR_COLOR 0.0, 0.0, 0.0, 1.0

#define NS_STRING_FROM_CSTRING(STR) NS::String::string(STR, NS::UTF8StringEncoding)

#define METAL_CHECK(PTR, MSG)                                                                                          \
    if (!PTR) { XP_LOG(XPLoggerSeverityFatal, MSG) }

class XPScene;
class XPMeshBuffer;
class XPShaderBuffer;
class XPTextureBuffer;
class XPMeshAsset;
class XPTextureAsset;

struct XPMetalTexture
{
    NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor;
    NS::SharedPtr<MTL::Texture>           texture;
    const XPTextureAsset*                 textureAsset = nullptr;
};

struct XPMetalRenderPipeline
{
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = nullptr;
    MTL::RenderPipelineState*      renderPipelineState      = nullptr;
    MTL::DepthStencilDescriptor*   depthStencilDescriptor   = nullptr;
    MTL::DepthStencilState*        depthStencilState        = nullptr;
    NS::SharedPtr<MTL::Buffer>     vertexShaderArgumentBuffer0;
};

struct XPMetalFrameBuffer
{
    std::vector<XPMetalTexture> colorAttachments = {};
    XPMetalTexture              depthAttachment  = {};
    MTL::RenderPassDescriptor*  passDescriptor   = nullptr;
    MTL::ClearColor             clearColor       = MTL::ClearColor(XP_DEFAULT_CLEAR_COLOR);
};

struct XPMetalGBuffer
{
    std::unique_ptr<XPMetalFrameBuffer> frameBuffer    = nullptr;
    XPMetalRenderPipeline*              renderPipeline = nullptr;
};

struct XPMetalMainBuffer
{
    std::unique_ptr<XPMetalFrameBuffer> frameBuffer    = nullptr;
    XPMetalRenderPipeline*              renderPipeline = nullptr;
};

struct XPMetalOCBuffer
{
    std::unique_ptr<XPMetalFrameBuffer> frameBuffer    = nullptr;
    XPMetalRenderPipeline*              renderPipeline = nullptr;
};

struct XPMetalFramePipeline
{
    std::unique_ptr<XPMetalGBuffer>    gBuffer = nullptr;
    std::unique_ptr<XPMetalMainBuffer> mBuffer = nullptr;
};

struct XPMetalMesh;

struct XPMetalMeshObject
{
    XPMetalMeshObject(XPMetalMesh& mesh, const XPBoundingBox& boundingBox)
      : mesh(mesh)
      , boundingBox(boundingBox)
    {
    }

    XPMetalMesh&  mesh;
    std::string   name;
    uint32_t      vertexOffset;
    uint32_t      indexOffset;
    uint32_t      numIndices;
    XPBoundingBox boundingBox;
};

struct XPMetalMesh
{
    const XPMeshAsset*             meshAsset = nullptr;
    NS::SharedPtr<MTL::Buffer>     vertexBuffer;
    NS::SharedPtr<MTL::Buffer>     normalBuffer;
    NS::SharedPtr<MTL::Buffer>     uvBuffer;
    NS::SharedPtr<MTL::Buffer>     indexBuffer;
    std::vector<XPMetalMeshObject> objects;
};

struct XPMetalShader
{};

struct XPMetalMaterial
{};

struct XPMetalComputePipeline
{
    std::string                                             functionName;
    MTL::ComputePipelineState*                              state;
    std::optional<std::function<void(MTL::CommandBuffer*)>> completionHandler;
    std::vector<MTL::Buffer*>                               buffers;
    bool                                                    isAsynchrounous;
};

struct XPMetalComputePipelineCreationInfo
{
    enum ComputePipelineFlags : uint8_t
    {
        ComputePipelineFlags_None                    = 0,
        ComputePipelineFlags_CompileShaderFromSource = 1
    };

    XPMetalComputePipelineCreationInfo(std::string name, std::vector<size_t> bindings, ComputePipelineFlags flags)
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

struct XPMetalRenderPipelineCreationInfo
{
    enum RenderPipelineFlags : uint8_t
    {
        RenderPipelineFlags_None                    = 0,
        RenderPipelineFlags_CompileShaderFromSource = 1,
        RenderPipelineFlags_IncludeFragmentFunction = 2,
        RenderPipelineFlags_EnableDepthWrite        = 4,
        RenderPipelineFlags_EnableStencilWrite      = 8
    };

    XPMetalRenderPipelineCreationInfo(
      std::string                          name,
      NS::SharedPtr<MTL::VertexDescriptor> vertexDescriptor,
      RenderPipelineFlags                  flags,
      std::vector<MTL::PixelFormat>        colorAttachmentPixelFormats = {},
      std::optional<MTL::PixelFormat>      depthAttachmentPixelFormat  = {},
      std::optional<MTL::PixelFormat>      stencilPixelFormat          = {},
      MTL::CompareFunction                 depthComparisonFunction     = MTL::CompareFunction::CompareFunctionLess,
      int                                  sampleCount                 = 1)
      : name(name)
      , vertexDescriptor(vertexDescriptor)
      , sampleCount(sampleCount)
      , flags(flags)
      , depthComparisonFunction(depthComparisonFunction)
      , colorAttachmentPixelFormats(colorAttachmentPixelFormats)
      , depthAttachmentPixelFormat(depthAttachmentPixelFormat)
      , stencilPixelFormat(stencilPixelFormat)
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

    std::string                          name;
    NS::SharedPtr<MTL::VertexDescriptor> vertexDescriptor;
    int                                  sampleCount;
    RenderPipelineFlags                  flags;
    MTL::CompareFunction                 depthComparisonFunction;
    std::vector<MTL::PixelFormat>        colorAttachmentPixelFormats;
    std::optional<MTL::PixelFormat>      depthAttachmentPixelFormat;
    std::optional<MTL::PixelFormat>      stencilPixelFormat;
};

struct XPMetalLineRenderer
{
    std::vector<XPLine>        lines;
    NS::SharedPtr<MTL::Buffer> vertexBuffer;
};

struct XPMetalFramesInFlightLock
{
    XPMetalFramesInFlightLock(const std::vector<dispatch_semaphore_t> semaphores)
      : _semaphores(semaphores)
    {
        for (size_t i = 0; i < _semaphores.size(); ++i) {
            dispatch_semaphore_wait(_semaphores[i], DISPATCH_TIME_FOREVER);
        }
    }

    ~XPMetalFramesInFlightLock()
    {
        for (size_t i = 0; i < _semaphores.size(); ++i) {
            __block dispatch_semaphore_t block_sema = _semaphores[i];
            dispatch_semaphore_signal(block_sema);
        }
    }

    // delete copy and move constructors and assign operators
    XPMetalFramesInFlightLock(XPMetalFramesInFlightLock const&)            = delete; // Copy construct
    XPMetalFramesInFlightLock(XPMetalFramesInFlightLock&&)                 = delete; // Move construct
    XPMetalFramesInFlightLock& operator=(XPMetalFramesInFlightLock const&) = delete; // Copy assign
    XPMetalFramesInFlightLock& operator=(XPMetalFramesInFlightLock&&)      = delete; // Move assign

  private:
    std::vector<dispatch_semaphore_t> _semaphores;
};

static inline const char*
typeToStr(MTL::DataType dt)
{
    switch (dt) {
        case MTL::DataType::DataTypeNone: return "None";
        case MTL::DataType::DataTypeStruct: return "Struct";
        case MTL::DataType::DataTypeArray: return "Array";
        case MTL::DataType::DataTypeFloat: return "Float";
        case MTL::DataType::DataTypeFloat2: return "Float2";
        case MTL::DataType::DataTypeFloat3: return "Float3";
        case MTL::DataType::DataTypeFloat4: return "Float4";
        case MTL::DataType::DataTypeFloat2x2: return "Float2x2";
        case MTL::DataType::DataTypeFloat2x3: return "Float2x3";
        case MTL::DataType::DataTypeFloat2x4: return "Float2x4";
        case MTL::DataType::DataTypeFloat3x2: return "Float3x2";
        case MTL::DataType::DataTypeFloat3x3: return "Float3x3";
        case MTL::DataType::DataTypeFloat3x4: return "Float3x4";
        case MTL::DataType::DataTypeFloat4x2: return "Float4x2";
        case MTL::DataType::DataTypeFloat4x3: return "Float4x3";
        case MTL::DataType::DataTypeFloat4x4: return "Float4x4";
        case MTL::DataType::DataTypeHalf: return "Half";
        case MTL::DataType::DataTypeHalf2: return "Half2";
        case MTL::DataType::DataTypeHalf3: return "Half3";
        case MTL::DataType::DataTypeHalf4: return "Half4";
        case MTL::DataType::DataTypeHalf2x2: return "Half2x2";
        case MTL::DataType::DataTypeHalf2x3: return "Half2x3";
        case MTL::DataType::DataTypeHalf2x4: return "Half2x4";
        case MTL::DataType::DataTypeHalf3x2: return "Half3x2";
        case MTL::DataType::DataTypeHalf3x3: return "Half3x3";
        case MTL::DataType::DataTypeHalf3x4: return "Half3x4";
        case MTL::DataType::DataTypeHalf4x2: return "Half4x2";
        case MTL::DataType::DataTypeHalf4x3: return "Half4x3";
        case MTL::DataType::DataTypeHalf4x4: return "Half4x4";
        case MTL::DataType::DataTypeInt: return "Int";
        case MTL::DataType::DataTypeInt2: return "Int2";
        case MTL::DataType::DataTypeInt3: return "Int3";
        case MTL::DataType::DataTypeInt4: return "Int4";
        case MTL::DataType::DataTypeUInt: return "UInt";
        case MTL::DataType::DataTypeUInt2: return "UInt2";
        case MTL::DataType::DataTypeUInt3: return "UInt3";
        case MTL::DataType::DataTypeUInt4: return "UInt4";
        case MTL::DataType::DataTypeShort: return "Short";
        case MTL::DataType::DataTypeShort2: return "Short2";
        case MTL::DataType::DataTypeShort3: return "Short3";
        case MTL::DataType::DataTypeShort4: return "Short4";
        case MTL::DataType::DataTypeUShort: return "UShort";
        case MTL::DataType::DataTypeUShort2: return "UShort2";
        case MTL::DataType::DataTypeUShort3: return "UShort3";
        case MTL::DataType::DataTypeUShort4: return "UShort4";
        case MTL::DataType::DataTypeChar: return "Char";
        case MTL::DataType::DataTypeChar2: return "Char2";
        case MTL::DataType::DataTypeChar3: return "Char3";
        case MTL::DataType::DataTypeChar4: return "Char4";
        case MTL::DataType::DataTypeUChar: return "UChar";
        case MTL::DataType::DataTypeUChar2: return "UChar2";
        case MTL::DataType::DataTypeUChar3: return "UChar3";
        case MTL::DataType::DataTypeUChar4: return "UChar4";
        case MTL::DataType::DataTypeBool: return "Bool";
        case MTL::DataType::DataTypeBool2: return "Bool2";
        case MTL::DataType::DataTypeBool3: return "Bool3";
        case MTL::DataType::DataTypeBool4: return "Bool4";
        case MTL::DataType::DataTypeTexture: return "Texture";
        case MTL::DataType::DataTypeSampler: return "Sampler";
        case MTL::DataType::DataTypePointer: return "Pointer";
        case MTL::DataType::DataTypeR8Unorm: return "R8Unorm";
        case MTL::DataType::DataTypeR8Snorm: return "R8Snorm";
        case MTL::DataType::DataTypeR16Unorm: return "R16Unorm";
        case MTL::DataType::DataTypeR16Snorm: return "R16Snorm";
        case MTL::DataType::DataTypeRG8Unorm: return "RG8Unorm";
        case MTL::DataType::DataTypeRG8Snorm: return "RG8Snorm";
        case MTL::DataType::DataTypeRG16Unorm: return "RG16Unorm";
        case MTL::DataType::DataTypeRG16Snorm: return "RG16Snorm";
        case MTL::DataType::DataTypeRGBA8Unorm: return "RGBA8Unorm";
        case MTL::DataType::DataTypeRGBA8Unorm_sRGB: return "RGBA8Unorm_sRGB";
        case MTL::DataType::DataTypeRGBA8Snorm: return "RGBA8Snorm";
        case MTL::DataType::DataTypeRGBA16Unorm: return "RGBA16Unorm";
        case MTL::DataType::DataTypeRGBA16Snorm: return "RGBA16Snorm";
        case MTL::DataType::DataTypeRGB10A2Unorm: return "RGB10A2Unorm";
        case MTL::DataType::DataTypeRG11B10Float: return "RG11B10Float";
        case MTL::DataType::DataTypeRGB9E5Float: return "RGB9E5Float";
        case MTL::DataType::DataTypeRenderPipeline: return "RenderPipeline";
        case MTL::DataType::DataTypeComputePipeline: return "ComputePipeline";
        case MTL::DataType::DataTypeIndirectCommandBuffer: return "IndirectCommandBuffer";
        case MTL::DataType::DataTypeLong: return "Long";
        case MTL::DataType::DataTypeLong2: return "Long2";
        case MTL::DataType::DataTypeLong3: return "Long3";
        case MTL::DataType::DataTypeLong4: return "Long4";
        case MTL::DataType::DataTypeULong: return "ULong";
        case MTL::DataType::DataTypeULong2: return "ULong2";
        case MTL::DataType::DataTypeULong3: return "ULong3";
        case MTL::DataType::DataTypeULong4: return "ULong4";
        case MTL::DataType::DataTypeVisibleFunctionTable: return "VisibleFunctionTable";
        case MTL::DataType::DataTypeIntersectionFunctionTable: return "IntersectionFunctionTable";
        case MTL::DataType::DataTypePrimitiveAccelerationStructure: return "PrimitiveAccelerationStructure";
        case MTL::DataType::DataTypeInstanceAccelerationStructure: return "InstanceAccelerationStructure";
    };
    return "";
}
