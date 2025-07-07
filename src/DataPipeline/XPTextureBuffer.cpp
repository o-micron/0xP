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

#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPTextureBuffer.h>

XPTextureBuffer::XPTextureBuffer(XPTextureAsset* const textureAsset, uint32_t id)
  : _textureAsset(textureAsset)
  , _id(id)
{
}

XPTextureBuffer::~XPTextureBuffer() {}

XPTextureAsset*
XPTextureBuffer::getTextureAsset() const
{
    return _textureAsset;
}

uint32_t
XPTextureBuffer::getId() const
{
    return _id;
}

const std::vector<unsigned char>&
XPTextureBuffer::getPixels() const
{
    return _pixels;
}

const XPVec2<uint32_t>&
XPTextureBuffer::getDimensions() const
{
    return _dimensions;
}

XPETextureBufferFormat
XPTextureBuffer::getFormat() const
{
    return _format;
}

void
XPTextureBuffer::setPixels(std::vector<unsigned char>&& pixels)
{
    _pixels = pixels;
}

void
XPTextureBuffer::setDimensions(XPVec2<uint32_t> dimensions)
{
    _dimensions = dimensions;
}

void
XPTextureBuffer::setFormat(XPETextureBufferFormat format)
{
    _format = format;
}
