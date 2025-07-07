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

struct FramebufferRef;
struct FrameMemoryPool;

template<typename T>
class Rasterizer;

template<typename T>
struct Scene;

class Renderer
{
  public:
    Renderer();
    ~Renderer();
    void render(FramebufferRef& fbref, FrameMemoryPool& fmp, Scene<float>* scene);

  private:
    Rasterizer<float>* rasterizer;
};