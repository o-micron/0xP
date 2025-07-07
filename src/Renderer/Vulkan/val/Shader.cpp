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

#include "Shader.hpp"

#include "Defs.h"

#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>

#include <Renderer/Vulkan/val/Buffer.h>
#include <Renderer/Vulkan/val/Shader.hpp>

#include <Utilities/XPLogger.h>

#include <filesystem>
#include <utility>

namespace val {

Shader::Shader(VkDevice device, const XPShaderBuffer& buffer)
  : device(device)
  , buffer(buffer)
{
    std::filesystem::path p(buffer.getShaderAsset()->getFile()->getPath());
    name = p.stem().string();
}

Shader::~Shader() {}

const std::string&
Shader::getName() const
{
    return name;
}

VertexFragmentShader::VertexFragmentShader(VkDevice device, const XPShaderBuffer& buffer)
  : Shader(device, buffer)
  , vertexShaderModule(nullptr)
  , fragmentShaderModule(nullptr)
{
    load();
}

VertexFragmentShader::~VertexFragmentShader() { unload(); }

void
VertexFragmentShader::load()
{
    if (vertexShaderModule != nullptr || fragmentShaderModule != nullptr) {
        XP_UNIMPLEMENTED("Unreachable");
        return;
    }

    ShaderStage vertexStage   = {};
    ShaderStage fragmentStage = {};
    if (buffer.getShaders().size() == 2) {
        XPShader* vertexShader   = nullptr;
        XPShader* fragmentShader = nullptr;
        if (buffer.getShaders()[0]->stages[0].type == XPEShaderStageType_Vertex) {
            vertexShader   = buffer.getShaders()[0].get();
            fragmentShader = buffer.getShaders()[1].get();
        } else if (buffer.getShaders()[0]->stages[0].type == XPEShaderStageType_Fragment) {
            fragmentShader = buffer.getShaders()[0].get();
            vertexShader   = buffer.getShaders()[1].get();

        } else {
            assert(false);
        }
        // copy source code buffer bytes (in this case it should be actually a spirv byte code byffer)
        assert((vertexShader->sourceCode.size() % sizeof(uint32_t)) == 0 &&
               "Vertex Shader Source Code is not a valid SPIRV Code");
        vertexStage.spirvCode.resize(vertexShader->sourceCode.size() / sizeof(uint32_t));
        memcpy(vertexStage.spirvCode.data(),
               vertexShader->sourceCode.data(),
               sizeof(uint32_t) * vertexStage.spirvCode.size());
        vertexStage.entryFunction = vertexShader->stages[0].entryFunction;

        // copy source code buffer bytes (in this case it should be actually a spirv byte code byffer)
        assert((fragmentShader->sourceCode.size() % sizeof(uint32_t)) == 0 &&
               "Fragment Shader Source Code is not a valid SPIRV Code");
        fragmentStage.spirvCode.resize(fragmentShader->sourceCode.size() / sizeof(uint32_t));
        memcpy(fragmentStage.spirvCode.data(),
               fragmentShader->sourceCode.data(),
               sizeof(uint32_t) * fragmentStage.spirvCode.size());
        fragmentStage.entryFunction = fragmentShader->stages[0].entryFunction;
    } else {
        assert(false);
    }

    VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {};
    vertexShaderModuleCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertexShaderModuleCreateInfo.pNext                    = nullptr;
    vertexShaderModuleCreateInfo.flags                    = 0;
    vertexShaderModuleCreateInfo.codeSize                 = vertexStage.spirvCode.size() * sizeof(uint32_t);
    vertexShaderModuleCreateInfo.pCode                    = vertexStage.spirvCode.data();

    VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {};
    fragmentShaderModuleCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragmentShaderModuleCreateInfo.pNext                    = nullptr;
    fragmentShaderModuleCreateInfo.flags                    = 0;
    fragmentShaderModuleCreateInfo.codeSize                 = fragmentStage.spirvCode.size() * sizeof(uint32_t);
    fragmentShaderModuleCreateInfo.pCode                    = fragmentStage.spirvCode.data();

    VULKAN_ASSERT(vkCreateShaderModule(device, &vertexShaderModuleCreateInfo, nullptr, &vertexShaderModule),
                  "Failed to create a vertex shader module");
    VULKAN_ASSERT(vkCreateShaderModule(device, &fragmentShaderModuleCreateInfo, nullptr, &fragmentShaderModule),
                  "Failed to create a fragment shader module");
    vertexStage.module   = vertexShaderModule;
    fragmentStage.module = fragmentShaderModule;

    vertexShaderStageCreateInfo        = {};
    vertexShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.pNext  = nullptr;
    vertexShaderStageCreateInfo.flags  = 0;
    vertexShaderStageCreateInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName  = "main";

    fragmentShaderStageCreateInfo        = {};
    fragmentShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.pNext  = nullptr;
    fragmentShaderStageCreateInfo.flags  = 0;
    fragmentShaderStageCreateInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName  = "main";

    // reflection
    // This is crucial as we need to automatically find the number of color attachments in fragment shader for example
    // Expect to use more information gathered from there !
    {
        {
            // Create SPIRV-Cross compiler instance
            spvc_context context;
            spvc_context_create(&context);

            spvc_parsed_ir parsed_ir;
            spvc_context_parse_spirv(
              context, (const uint32_t*)vertexStage.spirvCode.data(), vertexStage.spirvCode.size(), &parsed_ir);

            spvc_compiler compiler;
            spvc_context_create_compiler(
              context, SPVC_BACKEND_NONE, parsed_ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);

            spvc_compiler_options compiler_options;
            spvc_compiler_create_compiler_options(compiler, &compiler_options);

            spvc_compiler_options_set_bool(
              compiler_options, SPVC_COMPILER_OPTION_GLSL_SEPARATE_SHADER_OBJECTS, SPVC_TRUE);

            // Retrieve shader resources
            spvc_resources resources;
            spvc_compiler_create_shader_resources(compiler, &resources);

            XP_LOG(XPLoggerSeverityInfo, "Vertex Shader Resources");

            populate_uniform_buffers(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_storage_buffers(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_stage_inputs(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_stage_outputs(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_subpass_inputs(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_storage_images(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_sampled_images(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_atomic_counters(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_acceleration_structures(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_push_constant_buffers(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_shader_record_buffers(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_separate_images(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_separate_samplers(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_builtin_inputs(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);
            populate_builtin_outputs(compiler, resources, VK_SHADER_STAGE_VERTEX_BIT);

            // Cleanup
            spvc_context_destroy(context);
        }
        {
            // Create SPIRV-Cross compiler instance
            spvc_context context;
            spvc_context_create(&context);

            spvc_parsed_ir parsed_ir;
            spvc_context_parse_spirv(
              context, (const uint32_t*)fragmentStage.spirvCode.data(), fragmentStage.spirvCode.size(), &parsed_ir);

            spvc_compiler compiler;
            spvc_context_create_compiler(
              context, SPVC_BACKEND_NONE, parsed_ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);

            spvc_compiler_options compiler_options;
            spvc_compiler_create_compiler_options(compiler, &compiler_options);

            spvc_compiler_options_set_bool(
              compiler_options, SPVC_COMPILER_OPTION_GLSL_SEPARATE_SHADER_OBJECTS, SPVC_TRUE);

            // Retrieve shader resources
            spvc_resources resources;
            spvc_compiler_create_shader_resources(compiler, &resources);

            XP_LOG(XPLoggerSeverityInfo, "Fragment Shader Resources");

            populate_uniform_buffers(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_storage_buffers(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_stage_inputs(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_stage_outputs(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_subpass_inputs(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_storage_images(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_sampled_images(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_atomic_counters(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_acceleration_structures(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_push_constant_buffers(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_shader_record_buffers(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_separate_images(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_separate_samplers(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_builtin_inputs(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);
            populate_builtin_outputs(compiler, resources, VK_SHADER_STAGE_FRAGMENT_BIT);

            // Cleanup
            spvc_context_destroy(context);
        }

        // Now after populating descriptor set layout bindings from all shaders
        // Create descriptor set layout
        for (const auto& it : descriptorSetLayoutBindings) {
            const auto&                     bindings   = it.second;
            VkDescriptorSetLayoutCreateInfo layoutInfo = {};
            layoutInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount                    = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings                       = bindings.data();

            descriptorSetLayoutCreateInfos.push_back(layoutInfo);

            VkDescriptorSetLayout layout;
            VULKAN_ASSERT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout),
                          "Failed to create descriptor set layout");
            descriptorSetLayouts.push_back(layout);
        }

        // print everything for debugging purposes
        printf("Shader: %s\n", name.c_str());
        XP_LOG(XPLoggerSeverityInfo, "VS Input Bindings:");
        for (const auto& pair : vsInputBindings) {
            XP_LOGV(XPLoggerSeverityInfo,
                    "\t[%u], binding: %u, stride: %u, rate: %u",
                    pair.first,
                    pair.second.binding,
                    pair.second.stride,
                    pair.second.inputRate);
        }
        XP_LOG(XPLoggerSeverityInfo, "VS Input Attributes:");
        for (const auto& pair : vsInputAttributes) {
            XP_LOGV(XPLoggerSeverityInfo,
                    "\t[%u], binding: %u, location: %u, offset: %u, format: %u",
                    pair.first,
                    pair.second.binding,
                    pair.second.location,
                    pair.second.offset,
                    pair.second.format);
        }
        XP_LOG(XPLoggerSeverityInfo, "FS Output Formats:");
        for (const auto& pair : fsOutputFormats) {
            XP_LOGV(XPLoggerSeverityInfo, "\t[%u], format: %u", pair.first, pair.second);
        }
        XP_LOG(XPLoggerSeverityInfo, "Descriptor Set Layout Bindings:");
        for (const auto& pair : descriptorSetLayoutBindings) {
            XP_LOGV(XPLoggerSeverityInfo, "\tDescriptorSetLayoutBinding[%u] {", pair.first);
            for (size_t i = 0; i < pair.second.size(); ++i) {
                XP_LOGV(XPLoggerSeverityInfo,
                        "\t\tbinding: %u, descriptorCount, %u, descriptorType: %u, stageFlags: %u",
                        pair.second[i].binding,
                        pair.second[i].descriptorCount,
                        pair.second[i].descriptorType,
                        pair.second[i].stageFlags);
            }
            XP_LOGV(XPLoggerSeverityInfo, "\t}");
        }
    }
}

void
VertexFragmentShader::reload()
{
    unload();
    load();
}

void
VertexFragmentShader::unload()
{
    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
    vertexShaderModule   = nullptr;
    fragmentShaderModule = nullptr;
    vsInputBindings.clear();
    vsInputAttributes.clear();
    fsOutputFormats.clear();
    descriptorSetLayoutBindings.clear();
    for (auto& dsl : descriptorSetLayouts) { vkDestroyDescriptorSetLayout(device, dsl, nullptr); }
    descriptorSetLayouts.clear();
    descriptorSetLayoutCreateInfos.clear();
    pushConstantRanges.clear();
}

const VkPipelineShaderStageCreateInfo&
VertexFragmentShader::getVertexShaderStageCreateInfo() const
{
    return vertexShaderStageCreateInfo;
}

const VkPipelineShaderStageCreateInfo&
VertexFragmentShader::getFragmentShaderStageCreateInfo() const
{
    return fragmentShaderStageCreateInfo;
}

const VkShaderModule&
VertexFragmentShader::getVertexShaderModule() const
{
    return vertexShaderModule;
}

const VkShaderModule&
VertexFragmentShader::getFragmentShaderModule() const
{
    return fragmentShaderModule;
}

const std::map<uint32_t, VkVertexInputBindingDescription>&
VertexFragmentShader::getVSInputBindings() const
{
    return vsInputBindings;
}

const std::map<uint32_t, VkVertexInputAttributeDescription>&
VertexFragmentShader::getVSInputAttributes() const
{
    return vsInputAttributes;
}

const std::map<uint32_t, VkFormat>&
VertexFragmentShader::getFSOutputFormats() const
{
    return fsOutputFormats;
}

const std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>&
VertexFragmentShader::getDescriptorSetLayoutBindings() const
{
    return descriptorSetLayoutBindings;
}

const std::vector<VkDescriptorSetLayout>&
VertexFragmentShader::getDescriptorSetLayouts() const
{
    return descriptorSetLayouts;
}

const std::vector<VkDescriptorSetLayoutCreateInfo>&
VertexFragmentShader::getDescriptorSetLayoutCreateInfos() const
{
    return descriptorSetLayoutCreateInfos;
}

const std::map<std::string, VkPushConstantRange>&
VertexFragmentShader::getPushConstantRanges() const
{
    return pushConstantRanges;
}

ComputeShader::ComputeShader(VkDevice device, const XPShaderBuffer& buffer)
  : Shader(device, buffer)
  , computeShaderModule(nullptr)
{
    load();
}

ComputeShader::~ComputeShader() { unload(); }

void
ComputeShader::load()
{
    if (computeShaderModule != nullptr) {
        XP_UNIMPLEMENTED("Unreachable");
        return;
    }

    val::ShaderStage computeStage = {};
    if (buffer.getShaders().size() == 1) {
        XPShader* computeShader = buffer.getShaders()[0].get();
        if (computeShader->stages.size() == 1 && computeShader->stages[0].type == XPEShaderStageType_Compute) {
            // copy source code buffer bytes (in this case it should be actually a spirv byte code byffer)
            assert((computeShader->sourceCode.size() % sizeof(uint32_t)) == 0 &&
                   "Compute Shader Source Code is not a valid SPIRV Code");
            computeStage.spirvCode.resize(computeShader->sourceCode.size() / sizeof(uint32_t));
            memcpy(computeStage.spirvCode.data(),
                   computeShader->sourceCode.data(),
                   sizeof(uint32_t) * computeStage.spirvCode.size());
            computeStage.entryFunction = computeShader->stages[0].entryFunction;
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }

    VkShaderModuleCreateInfo computeShaderModuleCreateInfo = {};
    computeShaderModuleCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    computeShaderModuleCreateInfo.pNext                    = nullptr;
    computeShaderModuleCreateInfo.flags                    = 0;
    computeShaderModuleCreateInfo.codeSize                 = computeStage.spirvCode.size() * sizeof(uint32_t);
    computeShaderModuleCreateInfo.pCode                    = computeStage.spirvCode.data();

    VULKAN_ASSERT(vkCreateShaderModule(device, &computeShaderModuleCreateInfo, nullptr, &computeShaderModule),
                  "Failed to create a compute shader module");

    computeShaderStageCreateInfo        = {};
    computeShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageCreateInfo.pNext  = nullptr;
    computeShaderStageCreateInfo.flags  = 0;
    computeShaderStageCreateInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageCreateInfo.module = computeShaderModule;
    computeShaderStageCreateInfo.pName  = "main";
}

void
ComputeShader::reload()
{
    unload();
    load();
}

void
ComputeShader::unload()
{
    vkDestroyShaderModule(device, computeShaderModule, nullptr);
    computeShaderModule = nullptr;
}

const VkPipelineShaderStageCreateInfo&
ComputeShader::getComputeShaderStageCreateInfo() const
{
    return computeShaderStageCreateInfo;
}

const VkShaderModule&
ComputeShader::getComputeShaderModule() const
{
    return computeShaderModule;
}

void
VertexFragmentShader::populate_uniform_buffers(spvc_compiler      compiler,
                                               spvc_resources     resources,
                                               VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* uniform_buffers;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &uniform_buffers, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &uniform_buffers[i], stageFlags, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    }
}

void
VertexFragmentShader::populate_storage_buffers(spvc_compiler      compiler,
                                               spvc_resources     resources,
                                               VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* storage_buffers;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_BUFFER, &storage_buffers, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &storage_buffers[i], stageFlags, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    }
}

void
VertexFragmentShader::populate_stage_inputs(spvc_compiler      compiler,
                                            spvc_resources     resources,
                                            VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* stage_inputs;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STAGE_INPUT, &stage_inputs, &count);
    for (size_t i = 0; i < count; i++) {
        if ((stageFlags & VK_SHADER_STAGE_VERTEX_BIT) == VK_SHADER_STAGE_VERTEX_BIT) {
            fill_vs_binding_description(compiler, &stage_inputs[i]);
            fill_vs_attribute_description(compiler, &stage_inputs[i]);
        }
    }
}

void
VertexFragmentShader::populate_stage_outputs(spvc_compiler      compiler,
                                             spvc_resources     resources,
                                             VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* stage_outputs;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STAGE_OUTPUT, &stage_outputs, &count);
    for (size_t i = 0; i < count; i++) {
        if ((stageFlags & VK_SHADER_STAGE_FRAGMENT_BIT) == VK_SHADER_STAGE_FRAGMENT_BIT) {
            fill_fs_output_format(compiler, &stage_outputs[i]);
        }
    }
}

void
VertexFragmentShader::populate_subpass_inputs(spvc_compiler      compiler,
                                              spvc_resources     resources,
                                              VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* subpass_inputs;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SUBPASS_INPUT, &subpass_inputs, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &subpass_inputs[i], stageFlags, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
    }
}

void
VertexFragmentShader::populate_storage_images(spvc_compiler      compiler,
                                              spvc_resources     resources,
                                              VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* storage_images;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_IMAGE, &storage_images, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(compiler, &storage_images[i], stageFlags, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    }
}

void
VertexFragmentShader::populate_sampled_images(spvc_compiler      compiler,
                                              spvc_resources     resources,
                                              VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* sampled_images;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &sampled_images, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &sampled_images[i], stageFlags, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    }
}

void
VertexFragmentShader::populate_atomic_counters(spvc_compiler      compiler,
                                               spvc_resources     resources,
                                               VkShaderStageFlags stageFlags)
{
}

void
VertexFragmentShader::populate_acceleration_structures(spvc_compiler      compiler,
                                                       spvc_resources     resources,
                                                       VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* acceleration_structures;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(
      resources, SPVC_RESOURCE_TYPE_ACCELERATION_STRUCTURE, &acceleration_structures, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &acceleration_structures[i], stageFlags, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);
    }
}

void
VertexFragmentShader::populate_push_constant_buffers(spvc_compiler      compiler,
                                                     spvc_resources     resources,
                                                     VkShaderStageFlags stageFlags)
{
    // Vulkan only allows 1 VkPushConstantRange per VkPipelineLayout covering all stages
    // We need to combine stageFlags and we assume their ranges are not overlapping and that they're distinct.
    const spvc_reflected_resource* push_constant_buffers;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(
      resources, SPVC_RESOURCE_TYPE_PUSH_CONSTANT, &push_constant_buffers, &count);
    for (size_t i = 0; i < count; i++) {
        // Push constants in SPIRV-Cross are represented as uniform buffers.
        // We need to get the size and accessed stages.
        std::string name   = spvc_compiler_get_name(compiler, push_constant_buffers[i].id);
        uint32_t    offset = 0; // Push constants start at offset  0
        size_t      size;
        spvc_type   type = spvc_compiler_get_type_handle(compiler, push_constant_buffers[i].type_id);
        spvc_compiler_get_declared_struct_size(compiler, type, &size);

        // iterate over all push constant ranges from all shader stages and check using the offset and size
        bool found = false;
        auto it    = pushConstantRanges.find(name);
        if (it != pushConstantRanges.end()) {
            if (it->second.size == size) {
                pushConstantRanges[name].stageFlags |= stageFlags;
                found = true;
            } else {
                XP_UNIMPLEMENTED("Push Constant Ranges must be distinct and must not have overlapping ranges even "
                                 "across different shader stages");
            }
        }

        if (!found) {
            // we need to create a new one here
            VkPushConstantRange pcr  = {};
            pcr.offset               = offset;
            pcr.size                 = size;
            pcr.stageFlags           = stageFlags;
            pushConstantRanges[name] = pcr;
        }
    }
}

void
VertexFragmentShader::populate_shader_record_buffers(spvc_compiler      compiler,
                                                     spvc_resources     resources,
                                                     VkShaderStageFlags stageFlags)
{
}

void
VertexFragmentShader::populate_separate_images(spvc_compiler      compiler,
                                               spvc_resources     resources,
                                               VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* separate_images;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &separate_images, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(
          compiler, &separate_images[i], stageFlags, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    }
}

void
VertexFragmentShader::populate_separate_samplers(spvc_compiler      compiler,
                                                 spvc_resources     resources,
                                                 VkShaderStageFlags stageFlags)
{
    const spvc_reflected_resource* separate_samplers;
    size_t                         count;
    spvc_resources_get_resource_list_for_type(
      resources, SPVC_RESOURCE_TYPE_SEPARATE_SAMPLERS, &separate_samplers, &count);
    for (size_t i = 0; i < count; i++) {
        fill_descriptor_set_layout_bindings(compiler, &separate_samplers[i], stageFlags, VK_DESCRIPTOR_TYPE_SAMPLER);
    }
}

void
VertexFragmentShader::populate_builtin_inputs(spvc_compiler      compiler,
                                              spvc_resources     resources,
                                              VkShaderStageFlags stageFlags)
{
}

void
VertexFragmentShader::populate_builtin_outputs(spvc_compiler      compiler,
                                               spvc_resources     resources,
                                               VkShaderStageFlags stageFlags)
{
}

static VkFormat
getFormatFromType(const spvc_type type)
{
    spvc_basetype baseType    = spvc_type_get_basetype(type);
    unsigned      typeVecSize = spvc_type_get_vector_size(type);

    switch (baseType) {
        case SPVC_BASETYPE_FP16: {
            if (typeVecSize == 1) {
                return VK_FORMAT_R16_SFLOAT;
            } else if (typeVecSize == 2) {
                return VK_FORMAT_R16G16_SFLOAT;
            } else if (typeVecSize == 3) {
                return VK_FORMAT_R16G16B16_SFLOAT;
            } else if (typeVecSize == 4) {
                return VK_FORMAT_R16G16B16A16_SFLOAT;
            }
            break;
        }
        case SPVC_BASETYPE_FP32: {
            if (typeVecSize == 1) {
                return VK_FORMAT_R32_SFLOAT;
            } else if (typeVecSize == 2) {
                return VK_FORMAT_R32G32_SFLOAT;
            } else if (typeVecSize == 3) {
                return VK_FORMAT_R32G32B32_SFLOAT;
            } else if (typeVecSize == 4) {
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            break;
        }
        case SPVC_BASETYPE_FP64: {
            if (typeVecSize == 1) {
                return VK_FORMAT_R64_SFLOAT;
            } else if (typeVecSize == 2) {
                return VK_FORMAT_R64G64_SFLOAT;
            } else if (typeVecSize == 3) {
                return VK_FORMAT_R64G64B64_SFLOAT;
            } else if (typeVecSize == 4) {
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            }
            break;
        }
        case SPVC_BASETYPE_INT8: {
            if (typeVecSize == 1) { return VK_FORMAT_R8_SINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R8G8_SINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R8G8B8_SINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R8G8B8A8_SINT; }
            break;
        }
        case SPVC_BASETYPE_INT16: {
            if (typeVecSize == 1) { return VK_FORMAT_R16_SINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R16G16_SINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R16G16B16_SINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R16G16B16A16_SINT; }
            break;
        }
        case SPVC_BASETYPE_INT32: {
            if (typeVecSize == 1) { return VK_FORMAT_R32_SINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R32G32_SINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R32G32B32_SINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R32G32B32A32_SINT; }
            break;
        }
        case SPVC_BASETYPE_INT64: {
            if (typeVecSize == 1) { return VK_FORMAT_R64_SINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R64G64_SINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R64G64B64_SINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R64G64B64A64_SINT; }
            break;
        }
        case SPVC_BASETYPE_UINT8: {
            if (typeVecSize == 1) { return VK_FORMAT_R8_UINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R8G8_UINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R8G8B8_UINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R8G8B8A8_UINT; }
            break;
        }
        case SPVC_BASETYPE_UINT16: {
            if (typeVecSize == 1) { return VK_FORMAT_R16_UINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R16G16_UINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R16G16B16_UINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R16G16B16A16_UINT; }
            break;
        }
        case SPVC_BASETYPE_UINT32: {
            if (typeVecSize == 1) { return VK_FORMAT_R32_UINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R32G32_UINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R32G32B32_UINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R32G32B32A32_UINT; }
            break;
        }
        case SPVC_BASETYPE_UINT64: {
            if (typeVecSize == 1) { return VK_FORMAT_R64_UINT; }
            if (typeVecSize == 2) { return VK_FORMAT_R64G64_UINT; }
            if (typeVecSize == 3) { return VK_FORMAT_R64G64B64_UINT; }
            if (typeVecSize == 4) { return VK_FORMAT_R64G64B64A64_UINT; }
            break;
        }
        default: {
            return VK_FORMAT_UNDEFINED;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

void
VertexFragmentShader::fill_fs_output_format(spvc_compiler compiler, const spvc_reflected_resource* resource)
{
    uint32_t  location = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationLocation);
    spvc_type type     = spvc_compiler_get_type_handle(compiler, resource->type_id);

    fsOutputFormats[location] = getFormatFromType(type);
}

void
VertexFragmentShader::fill_descriptor_set_layout_bindings(spvc_compiler                  compiler,
                                                          const spvc_reflected_resource* resource,
                                                          VkShaderStageFlags             stageFlags,
                                                          VkDescriptorType               descriptorType)
{
    uint32_t set     = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationDescriptorSet);
    uint32_t binding = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationBinding);

    // check if the binding already exists in the set
    // if used, then only append shader stage flags
    bool found = false;
    for (auto& existingBinding : descriptorSetLayoutBindings[set]) {
        if (existingBinding.binding == binding) {
            assert(existingBinding.descriptorType == descriptorType);
            // if it exists just add the flag
            existingBinding.stageFlags |= stageFlags;
            found = true;
            break;
        }
    }

    if (!found) {
        // if not found, add new binding
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding                      = binding;
        layoutBinding.descriptorType               = descriptorType;
        layoutBinding.descriptorCount              = 1;
        layoutBinding.stageFlags                   = stageFlags;
        layoutBinding.pImmutableSamplers           = nullptr;

        // Handle arrays: if the resource is an array, get its size
        spvc_type type              = spvc_compiler_get_type_handle(compiler, resource->type_id);
        unsigned  numTypeDimensions = spvc_type_get_num_array_dimensions(type);
        if (numTypeDimensions > 0) {
            // Assuming 1D array
            // TODO: needs more attention here .. not robust.
            layoutBinding.descriptorCount = spvc_type_get_array_dimension(type, 0);
        }

        descriptorSetLayoutBindings[set].push_back(layoutBinding);
    }
}

void
VertexFragmentShader::fill_vs_binding_description(spvc_compiler compiler, const spvc_reflected_resource* resource)
{
    uint32_t     location    = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationLocation);
    uint32_t     binding     = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationBinding);
    spvc_type    type        = spvc_compiler_get_type_handle(compiler, resource->type_id);
    unsigned int typeWidth   = spvc_type_get_bit_width(type);
    unsigned     typeVecSize = spvc_type_get_vector_size(type);
    uint32_t     stride      = (typeWidth / 8) * typeVecSize;

    vsInputBindings[location].binding   = location;
    vsInputBindings[location].stride    = stride;
    vsInputBindings[location].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    XP_LOGV(XPLoggerSeverityInfo,
            "vs_binding: [%lu][%lu] %s",
            location,
            binding,
            spvc_compiler_get_name(compiler, resource->id));
}

void
VertexFragmentShader::fill_vs_attribute_description(spvc_compiler compiler, const spvc_reflected_resource* resource)
{
    uint32_t     location    = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationLocation);
    uint32_t     binding     = spvc_compiler_get_decoration(compiler, resource->id, SpvDecorationBinding);
    spvc_type    type        = spvc_compiler_get_type_handle(compiler, resource->type_id);
    unsigned int typeWidth   = spvc_type_get_bit_width(type);
    unsigned     typeVecSize = spvc_type_get_vector_size(type);
    uint32_t     stride      = (typeWidth / 8) * typeVecSize;

    vsInputAttributes[location].binding  = location;
    vsInputAttributes[location].location = location;
    vsInputAttributes[location].format   = getFormatFromType(type);
    vsInputAttributes[location].offset   = 0;

    XP_LOGV(XPLoggerSeverityInfo,
            "vs_attribute: [%lu][%lu] %s",
            location,
            binding,
            spvc_compiler_get_name(compiler, resource->id));
}

} // namespace val
