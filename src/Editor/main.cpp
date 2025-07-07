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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <DataPipeline/XPAssimpModelLoader.h>
#include <DataPipeline/XPFile.h>
#if defined(XP_PLATFORM_EMSCRIPTEN)
    #include <DataPipeline/XPFileWatchEmscripten.h>
#elif defined(XP_PLATFORM_MACOS)
    #include <DataPipeline/XPFileWatchUnix.h>
#elif defined(XP_PLATFORM_WINDOWS)
    #include <DataPipeline/XPFileWatchWindows.h>
#endif
#include <DataPipeline/XPDataPipelineStore.h>
#include <Engine/XPAllocators.h>
#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <Mcp/XPMcpServer.h>
#include <Profiler/XPProfiler.h>
#if defined(XP_PHYSICS_BULLET)
    #include <Physics/Bullet/XPBulletPhysics.h>
#elif defined(XP_PHYSICS_JOLT)
    #include <Physics/Jolt/XPJoltPhysics.h>
#elif defined(XP_PHYSICS_PHYSX4)
    #include <Physics/PhysX4/XPPhysX4Physics.h>
#elif defined(XP_PHYSICS_PHYSX5)
    #include <Physics/PhysX5/XPPhysX5Physics.h>
#else
    #include <Physics/Empty/XPEmptyPhysics.h>
#endif
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#if defined(XP_EDITOR_MODE)
    #include <UI/ImGUI/XPImGUI.h>
#endif
#include <Utilities/XPAnnotations.h>
#include <Utilities/XPFS.h>

#include <Renderer/Interface/XPRendererGraph.h>
#if defined(XP_PLATFORM_EMSCRIPTEN)
    #include <Renderer/WebGPU/WGSL/XPWGSLLang.h>
    #include <Renderer/WebGPU/XPWGPURenderer.h>
#elif defined(XP_PLATFORM_MACOS)
    #if defined(XP_RENDERER_METAL)
        #include <Renderer/Metal/MSL/XPMSLLang.h>
        #include <Renderer/Metal/XPMetalRenderer.h>
    #elif defined(XP_RENDERER_VULKAN)
        #include <Renderer/Vulkan/XPVulkanRenderer.h>
    #elif defined(XP_RENDERER_WGPU)
        #include <Renderer/WebGPU/XPWGPURenderer.h>
    #endif
#elif defined(XP_PLATFORM_WINDOWS)
    #if defined(XP_RENDERER_DX12)
        #include <Renderer/DX12/XPDX12Renderer.h>
    #elif defined(XP_RENDERER_VULKAN)
        #include <Renderer/Vulkan/XPVulkanRenderer.h>
    #endif
#endif
#if defined(XP_RENDERER_SW)
    #include <Renderer/SW/XPSWRenderer.h>
#endif
#if defined(XP_MCP_SERVER)
    #include <Mcp/XPMcpServer.h>
#endif
#if defined(XP_USE_COMPUTE)
    #include <Compute/XPCompute.h>
#endif

#include <iostream>
#include <memory>

static void
onFileCreatedCallback(XPFile* file)
{
    switch (file->getResourceType()) {
        case XPEFileResourceType::Unknown: break;

        case XPEFileResourceType::PreloadedMesh: {
        } break;

        case XPEFileResourceType::Mesh: {
        } break;

        case XPEFileResourceType::Shader: {
        } break;

        case XPEFileResourceType::Texture: {
        } break;

        case XPEFileResourceType::Scene: {
        } break;

        case XPEFileResourceType::Plugin: break;

        case XPEFileResourceType::Count: break;
    }
}

static void
onFileModifiedCallback(XPFile* file)
{
    switch (file->getResourceType()) {
        case XPEFileResourceType::Unknown: break;

        case XPEFileResourceType::PreloadedMesh: {
            file->stageChanges();
        } break;

        case XPEFileResourceType::Mesh: {
            file->stageChanges();
        } break;

        case XPEFileResourceType::Shader: {
            file->stageChanges();
        } break;

        case XPEFileResourceType::Texture: {
            file->stageChanges();
        } break;

        case XPEFileResourceType::Plugin: break;

        case XPEFileResourceType::Scene: file->stageChanges(); break;

        case XPEFileResourceType::Count: break;
    }
}

static void
onFileDeletedCallback(XPFile* file)
{
    XP_UNUSED(file)
}

static void
onMeshAssetStageChangeCallback(XPFile* file, XPMeshAsset* meshAsset)
{
    XP_UNUSED(file)
    XP_UNUSED(meshAsset)
}

static void
onMeshAssetCommitChangeCallback(XPFile* file, XPMeshAsset* meshAsset)
{
    XP_UNUSED(meshAsset)

    file->reload();
}

static void
onShaderAssetStageChangeCallback(XPFile* file, XPShaderAsset* shaderAsset)
{
    XP_UNUSED(file)
    XP_UNUSED(shaderAsset)
}

static void
onShaderAssetCommitChangeCallback(XPFile* file, XPShaderAsset* shaderAsset)
{
    XP_UNUSED(shaderAsset)

    file->reload();
}

static void
onTextureAssetStageChangeCallback(XPFile* file, XPTextureAsset* textureAsset)
{
    XP_UNUSED(file)
    XP_UNUSED(textureAsset)
}

static void
onTextureAssetCommitChangeCallback(XPFile* file, XPTextureAsset* textureAsset)
{
    XP_UNUSED(textureAsset)

    file->reload();
}

int
shouldBeOptimized()
{
    return -99;
}

#ifdef __clang__
__attribute__((optnone)) __attribute__((used))
#endif
int
shouldKeep()
{
    return 0;
}

#if defined(__EMSCRIPTEN__)
static XPRegistry* EmscriptenRegistry = nullptr;
XP_EXTERN void
c_resize_browser_window(int width, int height)
{
    EmscriptenRegistry->getRenderer()->setWindowSize(XPVec2<int>(width, height));
}

XP_EXTERN void
c_on_paste(const char* text)
{
    EmscriptenRegistry->getRenderer()->simulateCopy(text);
}

XP_EXTERN const char*
c_on_copy()
{
    static std::string text;
    text = EmscriptenRegistry->getRenderer()->simulatePaste();
    return text.c_str();
}
#endif

XPProfilable int
start()
{
    puts("\n\n----- XPENGINE [" XP_PLATFORM_NAME "][" XP_CONFIG_NAME "] -----");

    XPFile::setOnFileCreated(onFileCreatedCallback);
    XPFile::setOnFileModified(onFileModifiedCallback);
    XPFile::setOnFileDeleted(onFileDeletedCallback);
    XPFile::setOnMeshAssetStageChange(onMeshAssetStageChangeCallback);
    XPFile::setOnMeshAssetCommitChange(onMeshAssetCommitChangeCallback);
    XPFile::setOnShaderAssetStageChange(onShaderAssetStageChangeCallback);
    XPFile::setOnShaderAssetCommitChange(onShaderAssetCommitChangeCallback);
    XPFile::setOnTextureAssetStageChange(onTextureAssetStageChangeCallback);
    XPFile::setOnTextureAssetCommitChange(onTextureAssetCommitChangeCallback);

    // int  ret                      = start();
    auto engine                   = XP_NEW XPEngine();
    auto                 console  = std::make_unique<XPConsole>(engine);
    auto                 registry = std::make_unique<XPRegistry>(engine);
    auto allocators               = XP_NEW XPAllocators(1 * 1024 * 1024 * 1024);
#if defined(__EMSCRIPTEN__)
    EmscriptenRegistry = registry.get();
#endif
    auto sceneDescriptorStore = std::make_unique<XPSceneDescriptorStore>(registry.get());
    auto dataPipelineStore    = XP_NEW XPDataPipelineStore(registry.get());
#if defined(XP_PLATFORM_EMSCRIPTEN)
    auto fileWatch = XP_NEW XPFileWatchEmscripten(registry.get());
#elif defined(XP_PLATFORM_MACOS)
    auto fileWatch = XP_NEW XPFileWatchUnix(registry.get());
#elif defined(XP_PLATFORM_WINDOWS)
    auto fileWatch = XP_NEW XPFileWatchWindows(registry.get());
#endif
#if defined(XP_EDITOR_MODE)
    auto ui = XP_NEW XPImGuiUIImpl(registry.get());
#else
    XPIUI* ui = nullptr;
#endif

#if defined(XP_PLATFORM_EMSCRIPTEN)
    auto renderer        = XP_NEW XPWGPURenderer(registry.get(), ui);
    auto shaderBlockLang = XP_NEW XPWGSLLang(registry.get());
#elif defined(XP_PLATFORM_MACOS)
    #if defined(XP_RENDERER_METAL)
    auto renderer = XP_NEW XPMetalRenderer(registry.get());
    #elif defined(XP_RENDERER_VULKAN)
    auto renderer = XP_NEW XPVulkanRenderer(registry.get());
    #elif defined(XP_RENDERER_WGPU)
    auto renderer = XP_NEW XPWGPURenderer(registry.get(), ui);
    #endif
#elif defined(XP_PLATFORM_WINDOWS)
    #if defined(XP_RENDERER_DX12)
    auto renderer = XP_NEW XPDX12Renderer(registry.get());
    #elif defined(XP_RENDERER_VULKAN)
    auto renderer = XP_NEW XPVulkanRenderer(registry.get());
    #elif defined(XP_RENDERER_WGPU)
    auto renderer = XP_NEW XPWGPURenderer(registry.get(), ui);
    #endif
#endif
#if defined(XP_RENDERER_SW)
    auto swRenderer = XP_NEW XPSWRenderer(registry.get());
#endif
#if defined(XP_PHYSICS_BULLET)
    auto physics = XP_NEW XPBulletPhysics(registry.get());
#elif defined(XP_PHYSICS_JOLT)
    auto physics = XP_NEW XPJoltPhysics(registry.get());
#elif defined(XP_PHYSICS_PHYSX4)
    auto physics = XP_NEW XPPhysX4Physics(registry.get());
#elif defined(XP_PHYSICS_PHYSX5)
    auto physics = XP_NEW XPPhysX5Physics(registry.get());
#else
    auto physics = XP_NEW XPEmptyPhysics(registry.get());
#endif

    auto scene = sceneDescriptorStore->createScene("empty").value();

    auto rendererGraph = XP_NEW XPRendererGraph(registry.get());

#if defined(XP_MCP_SERVER)
    auto mcpServer = XP_NEW XPMcpServer(registry.get());
#endif

    registry->setSWRendererBuffered(swRenderer);
    registry->setRendererBuffered(renderer);
    registry->setPhysicsBuffered(physics);
    registry->setUIBuffered(ui);
    registry->setDataPipelineStoreBuffered(dataPipelineStore);
    registry->setSceneBuffered(scene);
    registry->setFileWatchBuffered(fileWatch);
    registry->setRendererGraphBuffered(rendererGraph);
    registry->setAllocatorsBuffered(allocators);
#if defined(XP_MCP_SERVER)
    registry->setMcpServerBuffered(mcpServer);
#endif

    registry->triggerAllChangesIfAny();

    engine->setRegistry(std::move(registry));
    engine->setConsole(std::move(console));

    engine->initialize();

    fileWatch->start(dataPipelineStore,
                     XPFS::getExecutableDirectoryPath(),
                     XPFS::getMeshAssetsDirectory(),
                     XPFS::getSceneAssetsDirectory(),
                     XPFS::getShaderAssetsDirectory(),
                     XPFS::getTextureAssetsDirectory(),
                     XPFS::getPrototypeAssetsDirectory(),
                     XPFS::getPluginAssetsDirectory(),
                     XPFS::getRiscvBinaryAssetsDirectory());

    // {
    //     auto path = std::filesystem::path(XPFS::getExecutableDirectoryPath()) / "meshes" / "stalingrad.glb";
    //     XPAssimpModelLoader::loadScene(
    //       dataPipelineStore->getFile(path.string()).value()->getPath(), *dataPipelineStore, *sceneDescriptorStore);
    // }

    // for (auto& meshBuferPair : dataPipelineStore->getMeshBuffers()) {
    //     renderer->uploadMeshBuffer(*meshBuferPair.second);
    // }

    auto gbufferPositionU                               = std::make_unique<XPRendererGraphTarget>();
    auto gbufferNormalV                                 = std::make_unique<XPRendererGraphTarget>();
    auto gbufferAlbedo                                  = std::make_unique<XPRendererGraphTarget>();
    auto gbufferMetallicRoughnessAmbientObjectIdTexture = std::make_unique<XPRendererGraphTarget>();

    rendererGraph->resources.push_back(std::move(gbufferPositionU));
    rendererGraph->resources.push_back(std::move(gbufferNormalV));
    rendererGraph->resources.push_back(std::move(gbufferAlbedo));
    rendererGraph->resources.push_back(std::move(gbufferMetallicRoughnessAmbientObjectIdTexture));

    auto gBufferPass = std::make_unique<XPRendererGraphPass>();
    auto lightPass   = std::make_unique<XPRendererGraphPass>();

    rendererGraph->passes.push_back(std::move(gBufferPass));
    rendererGraph->passes.push_back(std::move(lightPass));

    engine->run();

#if defined(XP_PLATFORM_EMSCRIPTEN)
#else
    engine->finalize();

    #if defined(XP_MCP_SERVER)
    XP_DELETE mcpServer;
    #endif
    XP_DELETE rendererGraph;
    XP_DELETE physics;
    XP_DELETE swRenderer;
    XP_DELETE renderer;
    XP_DELETE ui;
    XP_DELETE fileWatch;
    XP_DELETE dataPipelineStore;
    XP_DELETE allocators;
    XP_DELETE engine;
    std::cout << "CLOSING SAFELY\n";
#endif

    return 0;
}

int
main(int, char**)
{
    XPProfilerInitialize();
    int res = start();
    XPProfilerFinalize();

    return res;
}
