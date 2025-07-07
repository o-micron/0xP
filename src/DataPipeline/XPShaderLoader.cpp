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

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPShaderAsset.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPShaderLoader.h>
#include <Utilities/XPFS.h>

#include <filesystem>

void
XPShaderLoader::loadShader(XPShaderAsset* shaderAsset, XPDataPipelineStore& dataPipelineStore)
{
#if defined(XP_RENDERER_METAL)
#elif defined(XP_RENDERER_VULKAN)
    if (XPFS::isFilePathContaining(shaderAsset->getFile()->getPath(), ".surface.spv")) {

        std::filesystem::path shaderParentDir = std::filesystem::path(shaderAsset->getFile()->getPath()).parent_path();
        std::filesystem::path shaderStem      = std::filesystem::path(shaderAsset->getFile()->getPath()).stem().stem();
        std::string vertexShaderSpirvPath     = (shaderParentDir / shaderStem.string().append(".vert.spv")).string();
        std::string fragmentShaderSpirvPath   = (shaderParentDir / shaderStem.string().append(".frag.spv")).string();

        std::vector<std::unique_ptr<XPShader>> shaders;

        auto vertexShader = std::make_unique<XPShader>();
        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(vertexShaderSpirvPath.c_str());
            assert(optShaderFileSourceCode.has_value());
            vertexShader->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage vertexShaderStage = {};
        vertexShaderStage.type          = XPEShaderStageType_Vertex;
        vertexShaderStage.entryFunction = "main";
        vertexShader->stages            = { vertexShaderStage };
        shaders.emplace_back(std::move(vertexShader));

        auto fragmentShader = std::make_unique<XPShader>();
        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(fragmentShaderSpirvPath.c_str());
            assert(optShaderFileSourceCode.has_value());
            fragmentShader->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage fragmentShaderStage = {};
        fragmentShaderStage.type          = XPEShaderStageType_Fragment;
        fragmentShaderStage.entryFunction = "main";
        fragmentShader->stages            = { fragmentShaderStage };
        shaders.emplace_back(std::move(fragmentShader));

        shaderAsset->getShaderBuffer()->setShaders(std::move(shaders));
    } else if (XPFS::isFilePathContaining(shaderAsset->getFile()->getPath(), ".compute.spv")) {
        std::vector<std::unique_ptr<XPShader>> shaders;

        auto computeShader = std::make_unique<XPShader>();
        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(shaderAsset->getFile()->getPath().c_str());
            assert(optShaderFileSourceCode.has_value());
            computeShader->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage computeShaderStage = {};
        computeShaderStage.type          = XPEShaderStageType_Compute;
        computeShaderStage.entryFunction = "main";
        computeShader->stages            = { computeShaderStage };
        shaders.emplace_back(std::move(computeShader));

        shaderAsset->getShaderBuffer()->setShaders(std::move(shaders));
    }
#endif
}

void
XPShaderLoader::reloadShader(XPShaderAsset* shaderAsset, XPDataPipelineStore& dataPipelineStore)
{
#if defined(XP_RENDERER_METAL)
#elif defined(XP_RENDERER_VULKAN)
    if (XPFS::isFilePathContaining(shaderAsset->getFile()->getPath(), ".surface.spv")) {

        std::filesystem::path shaderParentDir = std::filesystem::path(shaderAsset->getFile()->getPath()).parent_path();
        std::filesystem::path shaderStem      = std::filesystem::path(shaderAsset->getFile()->getPath()).stem().stem();
        std::string vertexShaderSpirvPath     = (shaderParentDir / shaderStem.string().append(".vert.spv")).string();
        std::string fragmentShaderSpirvPath   = (shaderParentDir / shaderStem.string().append(".frag.spv")).string();

        const std::vector<std::unique_ptr<XPShader>>& shaders = shaderAsset->getShaderBuffer()->getShaders();

        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(vertexShaderSpirvPath.c_str());
            assert(optShaderFileSourceCode.has_value());
            shaders[0]->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage vertexShaderStage = {};
        vertexShaderStage.type          = XPEShaderStageType_Vertex;
        vertexShaderStage.entryFunction = "main";
        shaders[0]->stages              = { vertexShaderStage };

        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(fragmentShaderSpirvPath.c_str());
            assert(optShaderFileSourceCode.has_value());
            shaders[1]->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage fragmentShaderStage = {};
        fragmentShaderStage.type          = XPEShaderStageType_Fragment;
        fragmentShaderStage.entryFunction = "main";
        shaders[1]->stages                = { fragmentShaderStage };
    } else if (XPFS::isFilePathContaining(shaderAsset->getFile()->getPath(), ".compute.spv")) {
        const std::vector<std::unique_ptr<XPShader>>& shaders = shaderAsset->getShaderBuffer()->getShaders();

        {
            auto optShaderFileSourceCode = XPFS::readShaderFile(shaderAsset->getFile()->getPath().c_str());
            assert(optShaderFileSourceCode.has_value());
            shaders[0]->sourceCode = optShaderFileSourceCode.value();
        }
        XPShaderStage computeShaderStage = {};
        computeShaderStage.type          = XPEShaderStageType_Compute;
        computeShaderStage.entryFunction = "main";
        shaders[0]->stages               = { computeShaderStage };
    }
#endif
}