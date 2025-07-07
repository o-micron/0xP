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

#include <Renderer/DX12/dx12al/Shader.hpp>

namespace dx12al {

Shader::Shader() {}

Shader::~Shader() {}

void
Shader::create(const char* filename)
{
    std::string  sfn(filename);
    std::wstring wsfn(sfn.begin(), sfn.end());
    DX12_ASSERT(D3DReadFileToBlob(wsfn.c_str(), &_blob));
}

void
Shader::destroy()
{
    _blob.Reset();
}

ID3DBlob*
Shader::getBlob() const
{
    return _blob.Get();
}

} // namespace dx12al
