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

// #pragma once

// #include <Utilities/XPMaths.h>
// #include <Utilities/XPPlatforms.h>

// #include <Renderer/Metal/metal_cpp.h>
// #include <SceneDescriptor/Attachments/XPFreeCamera.h>
// #include <SceneDescriptor/XPScene.h>

// #include <torch/torch.h>

// #include <memory>
// #include <stdint.h>
// #include <vector>

// class XPMetalRenderer;

// struct Net : torch::nn::Module
// {
//     Net(FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices);
//     torch::Tensor forward(torch::Tensor x);

//   private:
//     torch::nn::Linear fc1{ nullptr };
//     torch::nn::Linear fc2{ nullptr };
//     torch::nn::Linear fc3{ nullptr };
//     torch::nn::ReLU   relu1{ nullptr };
//     torch::nn::ReLU   relu2{ nullptr };
// };

// class XPMetalNN
// {
//   public:
//     XPMetalNN(XPMetalRenderer* renderer);

//     void reset(FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices);
//     void train(float* texture, FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices);
//     void infer(float* texture, FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices);

//   private:
//     XPMetalRenderer*                   _renderer;
//     std::unique_ptr<Net>               _net;
//     std::unique_ptr<torch::optim::SGD> _netOptimizer;
//     size_t                             _epoch;
// };