#include <emscripten.h>
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>

#include "R5RImporter.h"
#include "R5RMaths.h"
#include "R5RRasterizer.h"
#include "R5RRenderer.h"
#include "wasm_renderer.h"
#include "wasm_window.h"

#include <atomic>
#include <cstdint>
#include <map>
#include <sstream>
#include <string>

Window*                                                          window;
Renderer*                                                        renderer;
R5RRenderer                                                      r5r;
Scene<float>                                                     ImportedScene = {};
RasterizerEventListener                                          rasterizerEventListener;
size_t                                                           cameraIndex           = 0;
std::atomic<int64_t>                                             synchronousFetchCount = 0;
std::map<std::string, std::tuple<unsigned char*, int, int, int>> loadedTextures;

static void
renderAsync()
{
    r5r.render(&ImportedScene, rasterizerEventListener);
}

int
update(double time, void* userData)
{
    if (ImportedScene.cameras.size() > 0) {
        static auto time = std::chrono::high_resolution_clock::now();
        auto        now  = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - time).count() > 5) {
            time        = now;
            cameraIndex = (cameraIndex + 1) % ImportedScene.cameras.size();
            rasterizerEventListener.onFrameSetColorBufferPtr(ImportedScene.cameras[cameraIndex].colorBuffer,
                                                             ImportedScene.cameras[cameraIndex].resolution.x,
                                                             ImportedScene.cameras[cameraIndex].resolution.y);
            rasterizerEventListener.onFrameRenderBoundingSquare(
              0, ImportedScene.cameras[cameraIndex].resolution.x, 0, ImportedScene.cameras[cameraIndex].resolution.y);
        }
    }

    window->refresh();
    renderer->renderToScreen();

    // return true to keep the loop running
    return 1;
}

int
main(int, char**)
{
    window   = new Window();
    renderer = new Renderer();

    emscripten_request_animation_frame_loop(update, nullptr);

    rasterizerEventListener.colorBufferPreviewWidth     = 0;
    rasterizerEventListener.colorBufferPreviewHeight    = 0;
    rasterizerEventListener.colorBufferPreviewPtr       = nullptr;
    rasterizerEventListener.onFrameRenderBoundingSquare = [&](size_t xmin, size_t xmax, size_t ymin, size_t ymax) {
        renderer->textureDataPtr = rasterizerEventListener.colorBufferPreviewPtr;
        renderer->textureWidth   = rasterizerEventListener.colorBufferPreviewWidth;
        renderer->textureHeight  = rasterizerEventListener.colorBufferPreviewHeight;
        renderer->recreateGLTexture();
        renderer->updateSubImageData(xmin, xmax, ymin, ymax);
    };
    rasterizerEventListener.onFrameSetColorBufferPtr = [&](float* data, size_t width, size_t height) {
        rasterizerEventListener.colorBufferPreviewPtr    = data;
        rasterizerEventListener.colorBufferPreviewWidth  = width;
        rasterizerEventListener.colorBufferPreviewHeight = height;
    };
    rasterizerEventListener.activated = true;

    const std::vector<std::string> queuedTextures = {
        "/assets/sponza_textures/arch_stone_wall_01_BaseColor.jpg",
        "/assets/sponza_textures/arch_stone_wall_01_Normal.jpg",
        "/assets/sponza_textures/arch_stone_wall_01_Roughnessarch_stone_wall_01_Metalness.jpg",
        "/assets/sponza_textures/brickwall_01_BaseColor.jpg",
        "/assets/sponza_textures/brickwall_01_Normal.jpg",
        "/assets/sponza_textures/brickwall_01_Roughnessbrickwall_01_Metalness.jpg",
        "/assets/sponza_textures/brickwall_02_BaseColor.jpg",
        "/assets/sponza_textures/brickwall_02_Normal.jpg",
        "/assets/sponza_textures/brickwall_02_Roughnessbrickwall_02_Metalness.jpg",
        "/assets/sponza_textures/ceiling_plaster_01_BaseColor.jpg",
        "/assets/sponza_textures/ceiling_plaster_01_Normal.jpg",
        "/assets/sponza_textures/ceiling_plaster_01_Roughnessceiling_plaster_01_Metalness.jpg",
        "/assets/sponza_textures/ceiling_plaster_02_BaseColor.jpg",
        "/assets/sponza_textures/ceiling_plaster_02_Roughnessceiling_plaster_01_Metalness.jpg",
        "/assets/sponza_textures/col_1stfloor_BaseColor.jpg",
        "/assets/sponza_textures/col_1stfloor_Normal.jpg",
        "/assets/sponza_textures/col_1stfloor_Roughnesscol_1stfloor_Metalness.jpg",
        "/assets/sponza_textures/col_brickwall_01_BaseColor.jpg",
        "/assets/sponza_textures/col_brickwall_01_Normal.jpg",
        "/assets/sponza_textures/col_brickwall_01_Roughnesscol_brickwall_01_Metalness.jpg",
        "/assets/sponza_textures/col_head_1stfloor_BaseColor.jpg",
        "/assets/sponza_textures/col_head_1stfloor_Normal.jpg",
        "/assets/sponza_textures/col_head_1stfloor_Roughnesscol_head_1stfloor_Metalness.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_02_BaseColor.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_02_Normal.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_02_Roughnesscol_head_2ndfloor_02_Metalness.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_03_BaseColor.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_03_Normal.jpg",
        "/assets/sponza_textures/col_head_2ndfloor_03_Roughnesscol_head_2ndfloor_03_Metalness.jpg",
        "/assets/sponza_textures/curtain_fabric_blue_BaseColor.jpg",
        "/assets/sponza_textures/curtain_fabric_green_BaseColor.jpg",
        "/assets/sponza_textures/curtain_fabric_Normal.jpg",
        "/assets/sponza_textures/curtain_fabric_red_BaseColor.jpg",
        "/assets/sponza_textures/curtain_fabric_Roughnesscurtain_fabric_Metalness.jpg",
        "/assets/sponza_textures/dirt_decal_01_dirt_decal_01_mask_gltf_alpha_dirt_decal_Opacity.png",
        "/assets/sponza_textures/door_stoneframe_01_BaseColor.jpg",
        "/assets/sponza_textures/door_stoneframe_01_Normal.jpg",
        "/assets/sponza_textures/door_stoneframe_01_Roughnessdoor_stoneframe_01_Metalness.jpg",
        "/assets/sponza_textures/door_stoneframe_02_BaseColor.jpg",
        "/assets/sponza_textures/door_stoneframe_02_Normal.jpg",
        "/assets/sponza_textures/door_stoneframe_02_Roughnessdoor_stoneframe_02_Metalness.jpg",
        "/assets/sponza_textures/floor_tiles_01_BaseColor.jpg",
        "/assets/sponza_textures/floor_tiles_01_Normal.jpg",
        "/assets/sponza_textures/floor_tiles_01_Roughnessfloor_tiles_01_Metalness.jpg",
        "/assets/sponza_textures/IvyLeaf_BaseColor.jpg",
        "/assets/sponza_textures/IvyLeaf_Normal.jpg",
        "/assets/sponza_textures/IvyLeaf_Roughness0.jpg",
        "/assets/sponza_textures/lionhead_01_BaseColor.jpg",
        "/assets/sponza_textures/lionhead_01_Normal.jpg",
        "/assets/sponza_textures/lionhead_01_Roughnesslionhead_01_Metalness.jpg",
        "/assets/sponza_textures/metal_door_01_BaseColor.jpg",
        "/assets/sponza_textures/metal_door_01_Normal.jpg",
        "/assets/sponza_textures/metal_door_01_Roughnessmetal_door_01_Metalness.jpg",
        "/assets/sponza_textures/ornament_01_BaseColor.jpg",
        "/assets/sponza_textures/ornament_01_Normal.jpg",
        "/assets/sponza_textures/ornament_01_Roughnessornament_01_Metalness.jpg",
        "/assets/sponza_textures/roof_tiles_01_BaseColor.jpg",
        "/assets/sponza_textures/roof_tiles_01_Normal.jpg",
        "/assets/sponza_textures/roof_tiles_01_Roughnessroof_tiles_01_Metalness.jpg",
        "/assets/sponza_textures/stone_01_tile_BaseColor.jpg",
        "/assets/sponza_textures/stone_01_tile_Normal.jpg",
        "/assets/sponza_textures/stone_01_tile_Roughnessstone_01_tile_Metalness.jpg",
        "/assets/sponza_textures/stone_trims_01_BaseColor.jpg",
        "/assets/sponza_textures/stone_trims_01_Normal.jpg",
        "/assets/sponza_textures/stone_trims_01_Roughnessstone_trims_01_Metalness.jpg",
        "/assets/sponza_textures/stone_trims_02_BaseColor.jpg",
        "/assets/sponza_textures/stone_trims_02_Normal.jpg",
        "/assets/sponza_textures/stone_trims_02_Roughnessstone_trims_02_Metalness.jpg",
        "/assets/sponza_textures/stones_2ndfloor_01_BaseColor.jpg",
        "/assets/sponza_textures/stones_2ndfloor_01_Normal.jpg",
        "/assets/sponza_textures/stones_2ndfloor_01_Roughnessstones_2ndfloor_01_Metalness.jpg",
        "/assets/sponza_textures/window_frame_01_BaseColor.jpg",
        "/assets/sponza_textures/window_frame_01_Normal.jpg",
        "/assets/sponza_textures/window_frame_01_Roughnesswindow_frame_01_Metalness.jpg",
        "/assets/sponza_textures/wood_door_01_BaseColor.jpg",
        "/assets/sponza_textures/wood_door_01_Normal.jpg",
        "/assets/sponza_textures/wood_door_01_Roughnesswood_door_01_Metalness.jpg",
        "/assets/sponza_textures/wood_tile_01_BaseColor.jpg",
        "/assets/sponza_textures/wood_tile_01_Normal.jpg",
        "/assets/sponza_textures/wood_tile_01_Roughnesswood_tile_01_Metalness.jpg",
    };
    const std::vector<std::string> queuedFiles = { "/assets/sponza.bin" };
    synchronousFetchCount.store(queuedTextures.size() + queuedFiles.size());
    for (size_t i = 0; i < queuedTextures.size(); ++i) { downloadTexture(queuedTextures[i].c_str()); }
    for (size_t i = 0; i < queuedFiles.size(); ++i) { downloadFile(queuedFiles[i].c_str()); }

    //    delete renderer;
    //    delete window;
    return 0;
}

void
FileAssetDownloadProgress(emscripten_fetch_t* fetch)
{
    if (fetch->totalBytes) {
        printf("Downloading file %s.. %.2f%% complete.\n", fetch->url, fetch->dataOffset * 100.0 / fetch->totalBytes);
    } else {
        printf("Downloading file %s.. %lld bytes complete.\n", fetch->url, fetch->dataOffset + fetch->numBytes);
    }
}

void
FileAssetDownloadSucceeded(emscripten_fetch_t* fetch)
{
    printf("Finished file downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    std::filesystem::path path(fetch->url);
    FILE*                 fout = fopen(path.filename().string().c_str(), "wb");
    if (fout != NULL) { fwrite(fetch->data, sizeof(unsigned char), fetch->totalBytes, fout); }
    fclose(fout);
    emscripten_fetch_close(fetch); // Free data associated with the fetch.
    synchronousFetchCount.store(synchronousFetchCount.load() - 1);
    if (synchronousFetchCount.load() <= 0) { downloadMeshAsset("/assets/sponza.gltf"); }
}

void
FileAssetDownloadFailed(emscripten_fetch_t* fetch)
{
    printf("Downloading file %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

extern "C" void
downloadFile(const char* url)
{
    // attempt to download asset and then load it
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes   = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess    = FileAssetDownloadSucceeded;
    attr.onprogress   = FileAssetDownloadProgress;
    attr.onerror      = FileAssetDownloadFailed;
    attr.timeoutMSecs = UINT32_MAX;
    emscripten_fetch(&attr, url);
}

void
TextureAssetDownloadProgress(emscripten_fetch_t* fetch)
{
    if (fetch->totalBytes) {
        printf(
          "Downloading texture %s.. %.2f%% complete.\n", fetch->url, fetch->dataOffset * 100.0 / fetch->totalBytes);
    } else {
        printf("Downloading texture %s.. %lld bytes complete.\n", fetch->url, fetch->dataOffset + fetch->numBytes);
    }
}

void
TextureAssetDownloadSucceeded(emscripten_fetch_t* fetch)
{
    printf(
      "Finished texture downloading %llu/%llu bytes from URL %s.\n", fetch->numBytes, fetch->totalBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    std::stringstream ss;
    ss << "." << fetch->url;
    // auto p = std::filesystem::path(ss.str()).parent_path();
    // if (!std::filesystem::exists(p)) { std::filesystem::create_directories(p); }
    // FILE* fout = fopen(ss.str().c_str(), "wb");
    // if (fout != NULL) { fwrite(fetch->data, sizeof(unsigned char), fetch->totalBytes, fout); }
    // fclose(fout);
    if (loadedTextures.find(ss.str()) == loadedTextures.end()) {
        int            width, height, channels;
        unsigned char* data =
          stbi_load_from_memory((const stbi_uc*)fetch->data, fetch->totalBytes, &width, &height, &channels, 4);
        loadedTextures.insert({ ss.str(), std::make_tuple(data, width, height, channels) });
    }
    // std::filesystem::remove(std::filesystem::path(ss.str()));
    emscripten_fetch_close(fetch); // Free data associated with the fetch.
    synchronousFetchCount.store(synchronousFetchCount.load() - 1);
    if (synchronousFetchCount.load() <= 0) { downloadMeshAsset("/assets/sponza.gltf"); }
}

void
TextureAssetDownloadFailed(emscripten_fetch_t* fetch)
{
    printf("Downloading texture %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

extern "C" void
downloadTexture(const char* url)
{
    // attempt to download asset and then load it
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes   = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess    = TextureAssetDownloadSucceeded;
    attr.onprogress   = TextureAssetDownloadProgress;
    attr.onerror      = TextureAssetDownloadFailed;
    attr.timeoutMSecs = UINT32_MAX;
    emscripten_fetch(&attr, url);
}

static void
importMeshAsync(const void* data, size_t numBytes)
{
    importDownloadedMeshAsset<float>(ImportedScene, data, numBytes);
}

void
MeshAssetDownloadProgress(emscripten_fetch_t* fetch)
{
    if (fetch->totalBytes) {
        printf("Downloading mesh %s.. %.2f%% complete.\n", fetch->url, fetch->dataOffset * 100.0 / fetch->totalBytes);
    } else {
        printf("Downloading mesh %s.. %lld bytes complete.\n", fetch->url, fetch->dataOffset + fetch->numBytes);
    }
}

void
MeshAssetDownloadSucceeded(emscripten_fetch_t* fetch)
{
    printf("Finished mesh downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    ImportedScene.filepath = fetch->url;
    importMeshAsync(&fetch->data[0], fetch->totalBytes);
    emscripten_fetch_close(fetch); // Free data associated with the fetch.
    renderAsync();
}

void
MeshAssetDownloadFailed(emscripten_fetch_t* fetch)
{
    printf("Downloading mesh %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

extern "C" void
downloadMeshAsset(const char* url)
{
    // attempt to download asset and then load it
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes   = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess    = MeshAssetDownloadSucceeded;
    attr.onprogress   = MeshAssetDownloadProgress;
    attr.onerror      = MeshAssetDownloadFailed;
    attr.timeoutMSecs = UINT32_MAX;
    emscripten_fetch(&attr, url);
}

extern "C" void
getMappedTexture(const char* texturePath, unsigned char** data, int* width, int* height, int* channels)
{
    std::string key = texturePath;
    auto        it  = loadedTextures.find(key);
    if (it != loadedTextures.end()) {
        auto [tdata, twidth, theight, tchannels] = it->second;
        *data                                    = tdata;
        *width                                   = twidth;
        *height                                  = theight;
        *channels                                = tchannels;
    } else {
        *data     = nullptr;
        *width    = 0;
        *height   = 0;
        *channels = 0;
    }
}