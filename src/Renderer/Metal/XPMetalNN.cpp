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

// #include <Renderer/Metal/XPMetalNN.h>

// // - Camera
// //      - float4x4 (view)
// //      - float4x4 (projection)
// // - Scene
// //     - [Layers]
// //          - [Nodes]
// //              - float4x4 (model)

// Net::Net(FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices)
// {
//     size_t inputSize = (modelMatrices.size() * 16) // all nodes with mesh renderer attachments model matrices
//                        + 16                        // camera view matrix (f4x4)
//                        + 16;                       // camera projection matrix (f4x4)
//     size_t outputSize = (size_t)camera.activeProperties.width * (size_t)camera.activeProperties.height * 4; // RGBA
//     size_t hiddenLayer2Size = outputSize * 2;
//     size_t hiddenLayer1Size = hiddenLayer2Size * 2;

//     fc1   = register_module("fc1", torch::nn::Linear(inputSize, hiddenLayer1Size));
//     relu1 = register_module("relu1", torch::nn::ReLU());
//     fc2   = register_module("fc2", torch::nn::Linear(hiddenLayer1Size, hiddenLayer2Size));
//     relu2 = register_module("relu2", torch::nn::ReLU());
//     fc3   = register_module("fc3", torch::nn::Linear(hiddenLayer2Size, outputSize));
// }

// torch::Tensor
// Net::forward(torch::Tensor x)
// {
//     x = fc1(x);
//     x = relu1(x);
//     x = fc2(x);
//     x = relu2(x);
//     x = fc3(x);
//     return x;
// }

// XPMetalNN::XPMetalNN(XPMetalRenderer* renderer)
//   : _renderer(renderer)
// {
// }

// void
// XPMetalNN::reset(FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices)
// {
//     _net          = std::make_unique<Net>(camera, modelMatrices);
//     _netOptimizer = std::make_unique<torch::optim::SGD>(_net->parameters(), /*lr=*/0.01);
//     _epoch        = 1;

//     // Reset gradients.
//     _netOptimizer->zero_grad();
// }

// void
// XPMetalNN::train(float* texture, FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices)
// {
//     size_t inputSize = (modelMatrices.size() * 16) // all nodes with mesh renderer attachments model matrices
//                        + 16                        // camera view matrix (f4x4)
//                        + 16;                       // camera projection matrix (f4x4)
//     std::vector<float> input(inputSize);
//     memcpy(&input[0], &camera.activeProperties.viewMatrix._00, sizeof(float) * 16);
//     memcpy(&input[16], &camera.activeProperties.projectionMatrix._00, sizeof(float) * 16);
//     memcpy(&input[32], &modelMatrices[0]._00, sizeof(XPMat4<float>) * modelMatrices.size());

//     // inputTensor
//     at::Tensor inputTensor = torch::from_blob(input.data(), { 1, (long long)(input.size()) }, at::kFloat);

//     // the ideal result
//     at::Tensor targetTensor =
//       torch::from_blob(
//         texture,
//         { 0, 0, (long long)camera.activeProperties.width * 4, (long long)camera.activeProperties.height * 4 },
//         at::kFloat)
//         .metal();

//     // Execute the model on the input data.
//     torch::Tensor prediction = _net->forward(inputTensor);
//     // Compute a loss value to judge the prediction of our model.
//     torch::Tensor loss = torch::nll_loss(prediction, targetTensor);
//     // Compute gradients of the loss w.r.t. the parameters of our model.
//     loss.backward();
//     // Update the parameters based on the calculated gradients.
//     _netOptimizer->step();
// }

// void
// XPMetalNN::infer(float* texture, FreeCamera& camera, const std::vector<XPMat4<float>>& modelMatrices)
// {
//     size_t inputSize = (modelMatrices.size() * 16) // all nodes with mesh renderer attachments model matrices
//                        + 16                        // camera view matrix (f4x4)
//                        + 16;                       // camera projection matrix (f4x4)
//     std::vector<float> input(inputSize);
//     memcpy(&input[0], &camera.activeProperties.viewMatrix._00, sizeof(float) * 16);
//     memcpy(&input[16], &camera.activeProperties.projectionMatrix._00, sizeof(float) * 16);
//     memcpy(&input[32], &modelMatrices[0]._00, sizeof(XPMat4<float>) * modelMatrices.size());

//     // inputTensor
//     at::Tensor inputTensor = torch::from_blob(input.data(), { 1, (long long)(input.size()) }, at::kFloat);

//     torch::Tensor prediction = _net->forward(inputTensor);

//     size_t outputSize = (size_t)camera.activeProperties.width * (size_t)camera.activeProperties.height * 4; // RGBA

//     memcpy(texture, prediction.data_ptr<float>(), sizeof(float) * outputSize);

//     // std::vector<float> v(prediction.data_ptr<float>(), prediction.data_ptr<float>() + prediction.numel());
// }
