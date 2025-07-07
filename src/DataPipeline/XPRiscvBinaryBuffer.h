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

#include <Utilities/XPMemoryPool.h>

#include <string>

class XPRiscvBinaryAsset;

class XPRiscvBinaryBuffer
{
    XP_MPL_MEMORY_POOL(XPRiscvBinaryBuffer)

  public:
    explicit XPRiscvBinaryBuffer(XPRiscvBinaryAsset* const riscvBinaryAsset, const uint32_t id)
      : _riscvBinaryAsset(riscvBinaryAsset)
      , _id(id)
      , _source("")
    {
    }
    ~XPRiscvBinaryBuffer() {}
    [[nodiscard]] XPRiscvBinaryAsset* getRiscvBinaryAsset() const { return _riscvBinaryAsset; }
    [[nodiscard]] uint32_t            getId() const { return _id; }
    [[nodiscard]] std::string         getSource() const { return _source; }
    void                              setSource(std::string source) { _source = source; }

  private:
    XPRiscvBinaryAsset* const _riscvBinaryAsset;
    const uint32_t            _id;
    std::string               _source;
};
