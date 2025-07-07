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

#include <DataPipeline/XPRiscvBinaryBuffer.h>

#include <string>

class XPFile;
class XPRiscvBinaryBuffer;

class XPRiscvBinaryAsset
{
  public:
    XPRiscvBinaryAsset(XPFile* const file, uint32_t id)
      : _file(file)
      , _id(id)
      , _riscvBinaryBuffer(nullptr)
    {
    }
    ~XPRiscvBinaryAsset() {}
    XPFile*              getFile() const { return _file; }
    uint32_t             getId() const { return _id; }
    XPRiscvBinaryBuffer* getRiscvBinaryBuffer() const { return _riscvBinaryBuffer; }
    void setRiscvBinaryBuffer(XPRiscvBinaryBuffer* riscvBinaryBuffer) { _riscvBinaryBuffer = riscvBinaryBuffer; }

  private:
    XPFile* const        _file;
    const uint32_t       _id;
    XPRiscvBinaryBuffer* _riscvBinaryBuffer;
};
