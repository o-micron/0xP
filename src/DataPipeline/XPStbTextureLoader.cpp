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
#include <DataPipeline/XPStbTextureLoader.h>
#include <DataPipeline/XPTextureAsset.h>
#include <DataPipeline/XPTextureBuffer.h>
#include <Utilities/XPLogger.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

void
XPStbTextureLoader::load(XPTextureAsset* textureAsset, XPDataPipelineStore& dataPipelineStore)
{
    int            width, height, channels;
    unsigned char* textureData = stbi_load(textureAsset->getFile()->getPath().c_str(), &width, &height, &channels, 0);
    if (!textureData) {
        XP_LOGV(XPLoggerSeverityWarning, "Texture file not found %s", textureAsset->getFile()->getPath().c_str());
        return;
    }

    if (channels == 3) {
        stbi_image_free(textureData);
        textureData = stbi_load(textureAsset->getFile()->getPath().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!textureData) {
            XP_LOGV(XPLoggerSeverityWarning,
                    "Failed to load rgb texture as rgba %s",
                    textureAsset->getFile()->getPath().c_str());
            return;
        }
        channels = 4;
    }

    XPTextureBuffer* textureBuffer = textureAsset->getTextureBuffer();
    textureBuffer->setFormat(
      (channels == 1 ? XPETextureBufferFormat::R8
                     : (channels == 2 ? XPETextureBufferFormat::R8_G8 : XPETextureBufferFormat::R8_G8_B8_A8)));
    textureBuffer->setDimensions(XPVec2<uint32_t>(width, height));
    textureBuffer->setPixels(std::vector<uint8_t>(textureData, textureData + (width * height * channels)));
    stbi_image_free(textureData);
}
