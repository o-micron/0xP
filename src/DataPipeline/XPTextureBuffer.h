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

#include <Utilities/XPPlatforms.h>

#include <Utilities/XPMaths.h>
#include <Utilities/XPMemoryPool.h>

#include <stdint.h>
#include <string>
#include <vector>

class XPTextureAsset;

enum class XPETextureBufferFormat
{
    R8,
    R8_G8,
    R8_G8_B8,
    R8_G8_B8_A8,

    R16,
    R16_G16,
    R16_G16_B16,
    R16_G16_B16_A16,

    R32,
    R32_G32,
    R32_G32_B32,
    R32_G32_B32_A32,
};

class XPTextureBuffer
{
    XP_MPL_MEMORY_POOL(XPTextureBuffer)

  public:
    explicit XPTextureBuffer(XPTextureAsset* const textureAsset, uint32_t id);
    ~XPTextureBuffer();

    [[nodiscard]] XPTextureAsset*                   getTextureAsset() const;
    [[nodiscard]] uint32_t                          getId() const;
    [[nodiscard]] const std::vector<unsigned char>& getPixels() const;
    [[nodiscard]] const XPVec2<uint32_t>&           getDimensions() const;
    [[nodiscard]] XPETextureBufferFormat            getFormat() const;
    void                                            setPixels(std::vector<unsigned char>&& pixels);
    void                                            setDimensions(XPVec2<uint32_t> dimensions);
    void                                            setFormat(XPETextureBufferFormat format);

  private:
    XPTextureAsset* const      _textureAsset;
    const uint32_t             _id;
    std::vector<unsigned char> _pixels;
    XPVec2<uint32_t>           _dimensions;
    XPETextureBufferFormat     _format;
};
