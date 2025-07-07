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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <vulkan/vulkan.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <spirv_cross/spirv_cross_c.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

class XPShaderBuffer;
namespace val {

struct Buffer;

struct ShaderStage
{
    VkPipelineShaderStageCreateInfo stage;
    VkShaderModule                  module;
    std::vector<uint32_t>           spirvCode;
    std::string                     entryFunction;
};

class Shader
{
  public:
    Shader(VkDevice device, const XPShaderBuffer& buffer);
    virtual ~Shader();
    virtual void               load()   = 0;
    virtual void               reload() = 0;
    virtual void               unload() = 0;
    virtual const std::string& getName() const;

  protected:
    std::string           name;
    VkDevice              device;
    const XPShaderBuffer& buffer;
};

class VertexFragmentShader : public Shader
{
  public:
    VertexFragmentShader(VkDevice device, const XPShaderBuffer& buffer);
    virtual ~VertexFragmentShader();
    virtual void                                                         load() override;
    virtual void                                                         reload() override;
    virtual void                                                         unload() override;
    virtual const VkPipelineShaderStageCreateInfo&                       getVertexShaderStageCreateInfo() const;
    virtual const VkPipelineShaderStageCreateInfo&                       getFragmentShaderStageCreateInfo() const;
    virtual const VkShaderModule&                                        getVertexShaderModule() const;
    virtual const VkShaderModule&                                        getFragmentShaderModule() const;
    const std::map<uint32_t, VkVertexInputBindingDescription>&           getVSInputBindings() const;
    const std::map<uint32_t, VkVertexInputAttributeDescription>&         getVSInputAttributes() const;
    const std::map<uint32_t, VkFormat>&                                  getFSOutputFormats() const;
    const std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>& getDescriptorSetLayoutBindings() const;
    const std::vector<VkDescriptorSetLayout>&                            getDescriptorSetLayouts() const;
    const std::vector<VkDescriptorSetLayoutCreateInfo>&                  getDescriptorSetLayoutCreateInfos() const;
    const std::map<std::string, VkPushConstantRange>&                    getPushConstantRanges() const;

  private:
    void populate_uniform_buffers(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_storage_buffers(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_stage_inputs(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_stage_outputs(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_subpass_inputs(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_storage_images(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_sampled_images(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_atomic_counters(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_acceleration_structures(spvc_compiler      compiler,
                                          spvc_resources     resources,
                                          VkShaderStageFlags stageFlags);
    void populate_push_constant_buffers(spvc_compiler      compiler,
                                        spvc_resources     resources,
                                        VkShaderStageFlags stageFlags);
    void populate_shader_record_buffers(spvc_compiler      compiler,
                                        spvc_resources     resources,
                                        VkShaderStageFlags stageFlags);
    void populate_separate_images(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_separate_samplers(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_builtin_inputs(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);
    void populate_builtin_outputs(spvc_compiler compiler, spvc_resources resources, VkShaderStageFlags stageFlags);

    void fill_fs_output_format(spvc_compiler compiler, const spvc_reflected_resource* resource);
    void fill_descriptor_set_layout_bindings(spvc_compiler                  compiler,
                                             const spvc_reflected_resource* resource,
                                             VkShaderStageFlags             stageFlags,
                                             VkDescriptorType               descriptorType);
    void fill_vs_binding_description(spvc_compiler compiler, const spvc_reflected_resource* resource);
    void fill_vs_attribute_description(spvc_compiler compiler, const spvc_reflected_resource* resource);

    VkPipelineShaderStageCreateInfo                               vertexShaderStageCreateInfo;
    VkPipelineShaderStageCreateInfo                               fragmentShaderStageCreateInfo;
    VkShaderModule                                                vertexShaderModule;
    VkShaderModule                                                fragmentShaderModule;
    std::map<uint32_t, VkVertexInputBindingDescription>           vsInputBindings;
    std::map<uint32_t, VkVertexInputAttributeDescription>         vsInputAttributes;
    std::map<uint32_t, VkFormat>                                  fsOutputFormats;
    std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
    std::vector<VkDescriptorSetLayout>                            descriptorSetLayouts;
    std::vector<VkDescriptorSetLayoutCreateInfo>                  descriptorSetLayoutCreateInfos;
    std::map<std::string, VkPushConstantRange>                    pushConstantRanges;
};

class ComputeShader : public Shader
{
  public:
    ComputeShader(VkDevice device, const XPShaderBuffer& buffer);
    virtual ~ComputeShader();
    virtual void                                   load() override;
    virtual void                                   reload() override;
    virtual void                                   unload() override;
    virtual const VkPipelineShaderStageCreateInfo& getComputeShaderStageCreateInfo() const;
    virtual const VkShaderModule&                  getComputeShaderModule() const;

  private:
    VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo;
    VkShaderModule                  computeShaderModule;
};

} // namespace val
