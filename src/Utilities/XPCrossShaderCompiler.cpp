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

#include <Utilities/XPCrossShaderCompiler.h>
#include <Utilities/XPLogger.h>
#include <Utilities/XPPlatforms.h>

#include <exception>
#include <optional>
#include <sstream>
#include <string>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#endif
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/resource_limits_c.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/SPIRV/SPVRemapper.h>
#include <glslang/SPIRV/disassemble.h>
#include <glslang/SPIRV/doc.h>

#include <spirv_cross/spirv.hpp>
#include <spirv_cross/spirv_cross_c.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <spirv_cross/spirv_msl.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// static void
// crossShaderErrorHandler(void* userdata, const char* error)
// {
//     XP_UNUSED(userdata)
//     XP_UNUSED(error)

//     XP_LOGV(XPLoggerSeverityError, "[CROSS_SHADER] %s", error);
// }

static std::optional<EShLanguage>
getMappedStage(const XPCrossShaderInputInfo& inputInfo)
{
    switch (inputInfo.stage) {
        case XPCrossShaderStage_Vertex: return { EShLanguage::EShLangVertex };
        case XPCrossShaderStage_Fragment: return { EShLanguage::EShLangFragment };
        case XPCrossShaderStage_Compute: return { EShLanguage::EShLangCompute };
        default: return std::nullopt;
    }
}

static EShMessages
getMappedMessages(const XPCrossShaderInputInfo& inputInfo)
{
    XP_UNUSED(inputInfo)

    return static_cast<EShMessages>(EShMsgSpvRules | EShMsgKeepUncalled | EShMsgSuppressWarnings);
}

static unsigned int
getMappedShaderVersion(const XPCrossShaderInputInfo& inputInfo)
{
    switch (inputInfo.format) {
        case XPCrossShaderFormat_GLSL_ES_300: return 300;
        case XPCrossShaderFormat_GLSL_CORE_410: return 410;
        case XPCrossShaderFormat_GLSL_CORE_450: return 450;
        default: return 0;
    }
}

extern bool
crossShaderCompile(const XPCrossShaderInputInfo& inputInfo, XPCrossShaderOutputInfo& output)
{
    if (inputInfo.format > XPCrossShaderFormat_GLSL_CORE_450) {
        // Metal Shader Language is not supported as an input format (maybe this
        // should be typesafe, but it probably will be supported in the future).
        output.errors += "\nOnly GLSL is currently supported as an input format.";
        return false;
    }

    std::vector<uint32_t> spirvSource;

    if (inputInfo.format <= XPCrossShaderFormat_GLSL_CORE_450) {
        const char* strs = inputInfo.source.c_str();

        glslang::InitializeProcess();

        EShLanguage      stage = getMappedStage(inputInfo).value();
        glslang::TShader shader(stage);
        shader.setStrings(&strs, 1);

        const TBuiltInResource DefaultTBuiltInResource = { /* .MaxLights = */ 32,
                                                           /* .MaxClipPlanes = */ 6,
                                                           /* .MaxTextureUnits = */ 32,
                                                           /* .MaxTextureCoords = */ 32,
                                                           /* .MaxVertexAttribs = */ 64,
                                                           /* .MaxVertexUniformComponents = */ 4096,
                                                           /* .MaxVaryingFloats = */ 64,
                                                           /* .MaxVertexTextureImageUnits = */ 32,
                                                           /* .MaxCombinedTextureImageUnits = */ 80,
                                                           /* .MaxTextureImageUnits = */ 32,
                                                           /* .MaxFragmentUniformComponents = */ 4096,
                                                           /* .MaxDrawBuffers = */ 32,
                                                           /* .MaxVertexUniformVectors = */ 128,
                                                           /* .MaxVaryingVectors = */ 8,
                                                           /* .MaxFragmentUniformVectors = */ 16,
                                                           /* .MaxVertexOutputVectors = */ 16,
                                                           /* .MaxFragmentInputVectors = */ 15,
                                                           /* .MinProgramTexelOffset = */ -8,
                                                           /* .MaxProgramTexelOffset = */ 7,
                                                           /* .MaxClipDistances = */ 8,
                                                           /* .MaxComputeWorkGroupCountX = */ 65535,
                                                           /* .MaxComputeWorkGroupCountY = */ 65535,
                                                           /* .MaxComputeWorkGroupCountZ = */ 65535,
                                                           /* .MaxComputeWorkGroupSizeX = */ 1024,
                                                           /* .MaxComputeWorkGroupSizeY = */ 1024,
                                                           /* .MaxComputeWorkGroupSizeZ = */ 64,
                                                           /* .MaxComputeUniformComponents = */ 1024,
                                                           /* .MaxComputeTextureImageUnits = */ 16,
                                                           /* .MaxComputeImageUniforms = */ 8,
                                                           /* .MaxComputeAtomicCounters = */ 8,
                                                           /* .MaxComputeAtomicCounterBuffers = */ 1,
                                                           /* .MaxVaryingComponents = */ 60,
                                                           /* .MaxVertexOutputComponents = */ 64,
                                                           /* .MaxGeometryInputComponents = */ 64,
                                                           /* .MaxGeometryOutputComponents = */ 128,
                                                           /* .MaxFragmentInputComponents = */ 128,
                                                           /* .MaxImageUnits = */ 8,
                                                           /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
                                                           /* .MaxCombinedShaderOutputResources = */ 8,
                                                           /* .MaxImageSamples = */ 0,
                                                           /* .MaxVertexImageUniforms = */ 0,
                                                           /* .MaxTessControlImageUniforms = */ 0,
                                                           /* .MaxTessEvaluationImageUniforms = */ 0,
                                                           /* .MaxGeometryImageUniforms = */ 0,
                                                           /* .MaxFragmentImageUniforms = */ 8,
                                                           /* .MaxCombinedImageUniforms = */ 8,
                                                           /* .MaxGeometryTextureImageUnits = */ 16,
                                                           /* .MaxGeometryOutputVertices = */ 256,
                                                           /* .MaxGeometryTotalOutputComponents = */ 1024,
                                                           /* .MaxGeometryUniformComponents = */ 1024,
                                                           /* .MaxGeometryVaryingComponents = */ 64,
                                                           /* .MaxTessControlInputComponents = */ 128,
                                                           /* .MaxTessControlOutputComponents = */ 128,
                                                           /* .MaxTessControlTextureImageUnits = */ 16,
                                                           /* .MaxTessControlUniformComponents = */ 1024,
                                                           /* .MaxTessControlTotalOutputComponents = */ 4096,
                                                           /* .MaxTessEvaluationInputComponents = */ 128,
                                                           /* .MaxTessEvaluationOutputComponents = */ 128,
                                                           /* .MaxTessEvaluationTextureImageUnits = */ 16,
                                                           /* .MaxTessEvaluationUniformComponents = */ 1024,
                                                           /* .MaxTessPatchComponents = */ 120,
                                                           /* .MaxPatchVertices = */ 32,
                                                           /* .MaxTessGenLevel = */ 64,
                                                           /* .MaxViewports = */ 16,
                                                           /* .MaxVertexAtomicCounters = */ 0,
                                                           /* .MaxTessControlAtomicCounters = */ 0,
                                                           /* .MaxTessEvaluationAtomicCounters = */ 0,
                                                           /* .MaxGeometryAtomicCounters = */ 0,
                                                           /* .MaxFragmentAtomicCounters = */ 8,
                                                           /* .MaxCombinedAtomicCounters = */ 8,
                                                           /* .MaxAtomicCounterBindings = */ 1,
                                                           /* .MaxVertexAtomicCounterBuffers = */ 0,
                                                           /* .MaxTessControlAtomicCounterBuffers = */ 0,
                                                           /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
                                                           /* .MaxGeometryAtomicCounterBuffers = */ 0,
                                                           /* .MaxFragmentAtomicCounterBuffers = */ 1,
                                                           /* .MaxCombinedAtomicCounterBuffers = */ 1,
                                                           /* .MaxAtomicCounterBufferSize = */ 16384,
                                                           /* .MaxTransformFeedbackBuffers = */ 4,
                                                           /* .MaxTransformFeedbackInterleavedComponents = */ 64,
                                                           /* .MaxCullDistances = */ 8,
                                                           /* .MaxCombinedClipAndCullDistances = */ 8,
                                                           /* .MaxSamples = */ 4,
                                                           /* .maxMeshOutputVerticesNV = */ 256,
                                                           /* .maxMeshOutputPrimitivesNV = */ 512,
                                                           /* .maxMeshWorkGroupSizeX_NV = */ 32,
                                                           /* .maxMeshWorkGroupSizeY_NV = */ 1,
                                                           /* .maxMeshWorkGroupSizeZ_NV = */ 1,
                                                           /* .maxTaskWorkGroupSizeX_NV = */ 32,
                                                           /* .maxTaskWorkGroupSizeY_NV = */ 1,
                                                           /* .maxTaskWorkGroupSizeZ_NV = */ 1,
                                                           /* .maxMeshViewCountNV = */ 4,
                                                           /* .maxMeshOutputVerticesEXT = */ 256,
                                                           /* .maxMeshOutputPrimitivesEXT = */ 256,
                                                           /* .maxMeshWorkGroupSizeX_EXT = */ 128,
                                                           /* .maxMeshWorkGroupSizeY_EXT = */ 128,
                                                           /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
                                                           /* .maxTaskWorkGroupSizeX_EXT = */ 128,
                                                           /* .maxTaskWorkGroupSizeY_EXT = */ 128,
                                                           /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
                                                           /* .maxMeshViewCountEXT = */ 4,
                                                           /* .maxDualSourceDrawBuffersEXT = */ 6,

                                                           /* .limits = */
                                                           {
                                                             /* .nonInductiveForLoops = */ 1,
                                                             /* .whileLoops = */ 1,
                                                             /* .doWhileLoops = */ 1,
                                                             /* .generalUniformIndexing = */ 1,
                                                             /* .generalAttributeMatrixVectorIndexing = */ 1,
                                                             /* .generalVaryingIndexing = */ 1,
                                                             /* .generalSamplerIndexing = */ 1,
                                                             /* .generalVariableIndexing = */ 1,
                                                             /* .generalConstantMatrixVectorIndexing = */ 1,
                                                           } };

        TBuiltInResource builtInResources = DefaultTBuiltInResource;
        builtInResources.maxDrawBuffers   = true;
        EShMessages messages              = getMappedMessages(inputInfo);

        shader.setAutoMapBindings(true);
        shader.setAutoMapLocations(true);

        shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, getMappedShaderVersion(inputInfo));
        shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);

        shader.parse(&builtInResources, getMappedShaderVersion(inputInfo), true, messages);

        glslang::SpvOptions spvOptions;
        spvOptions.validate         = false;
        spvOptions.disableOptimizer = true;
        spvOptions.optimizeSize     = false;

        spv::SpvBuildLogger logger;

        const char* log = shader.getInfoLog();

        if (strlen(log) > 0) {
            output.errors += std::string(log);
            return false;
        }

        glslang::TIntermediate* inter = shader.getIntermediate();

        try {
            glslang::GlslangToSpv(*inter, spirvSource, &logger, &spvOptions);
        } catch (std::exception& exp) {
            output.errors += exp.what();
            return false;
        }

        glslang::FinalizeProcess();
    }

    if (output.format == XPCrossShaderFormat_GLSL_ES_300) {
        spirv_cross::CompilerGLSL          glsl(spirvSource);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 300;
        options.es      = true;
        glsl.set_common_options(options);
        output.source = glsl.compile();
        return true;
    } else if (output.format == XPCrossShaderFormat_GLSL_CORE_410) {
        spirv_cross::CompilerGLSL          glsl(spirvSource);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 410;
        options.es      = false;
        glsl.set_common_options(options);
        output.source = glsl.compile();
        return true;
    } else if (output.format == XPCrossShaderFormat_GLSL_CORE_450) {
        spirv_cross::CompilerGLSL          glsl(spirvSource);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 450;
        options.es      = false;
        glsl.set_common_options(options);
        output.source = glsl.compile();
        return true;
    } else if (output.format == XPCrossShaderFormat_MSL) {
        spirv_cross::CompilerMSL msl(spirvSource);
        output.source = msl.compile();
        return true;
    } else if (output.format == XPCrossShaderFormat_SPIRV) {
        std::stringstream result;
        std::copy(spirvSource.begin(), spirvSource.end(), std::ostream_iterator<uint32_t>(result, " "));
        output.source = result.str().c_str();
        return true;
    }

    output.errors += "\nUnknown or unsupported output format.";
    return false;
}
