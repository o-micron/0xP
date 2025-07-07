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

#include <DataPipeline/XPMaterialBuffer.h>

XPMaterialBuffer::XPMaterialBuffer(XPMaterialAsset* const materialAsset, const uint32_t id)
  : _materialAsset(materialAsset)
  , _id(id)
{
}

XPMaterialBuffer::~XPMaterialBuffer() {}

XPMaterialAsset*
XPMaterialBuffer::getMaterialAsset() const
{
    return _materialAsset;
}

uint32_t
XPMaterialBuffer::getId() const
{
    return _id;
}

std::optional<XPPhongSystem>
XPMaterialBuffer::getPhongSystem() const
{
    if (std::holds_alternative<XPPhongSystem>(_system)) { return { std::get<0>(_system) }; }
    return std::nullopt;
}

std::optional<XPPBRSystem>
XPMaterialBuffer::getPBRSystem() const
{
    if (std::holds_alternative<XPPBRSystem>(_system)) { return { std::get<1>(_system) }; }
    return std::nullopt;
}

void
XPMaterialBuffer::setSystem(XPPhongSystem phongSystem)
{
    _system = phongSystem;
}

void
XPMaterialBuffer::setSystem(XPPBRSystem pbrSystem)
{
    _system = pbrSystem;
}
