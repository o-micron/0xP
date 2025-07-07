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

#include <Renderer/Metal/XPMetalUpscaler.h>
#include <cstddef>

XPMetalUpscaler::XPMetalUpscaler() {}

XPMetalUpscaler::~XPMetalUpscaler() {}

void
XPMetalUpscaler::initialize()
{
}

void
XPMetalUpscaler::resize(int width, int height)
{
    XP_UNUSED(width)
    XP_UNUSED(height)
}

void
XPMetalUpscaler::finalize()
{
}

void
XPMetalUpscaler::setup()
{
    releaseResources();
    if (_mode == XPMetalUpscalerMode_SpacialSampling) {
        setupSpatialScaler();
    } else if (_mode == XPMetalUpscalerMode_TemporalScaling) {
        setupTemporalScaler();
    }
}

void
XPMetalUpscaler::releaseResources()
{
    _makeMotionVectors = false;

    // Free the spatial upscaling effect if you're not using it.
    if (_mode != XPMetalUpscalerMode_SpacialSampling) { _spatialScaler = NULL; }
    if (_mode != XPMetalUpscalerMode_TemporalScaling) { _temporalScaler = NULL; }
}

void
XPMetalUpscaler::adjustScale(float newRendererScale)
{
    XP_UNUSED(newRendererScale)
}

void
XPMetalUpscaler::setupSpatialScaler()
{
    // auto descriptor = MTL::FX

    // desc.inputWidth = renderTarget.renderSize.width
    // desc.inputHeight = renderTarget.renderSize.height
    // desc.outputWidth = renderTarget.windowSize.width
    // desc.outputHeight = renderTarget.windowSize.height
    // desc.colorTextureFormat = renderTarget.currentFrameColor.pixelFormat
    // desc.outputTextureFormat = renderTarget.currentFrameUpscaledColor.pixelFormat
    // desc.colorProcessingMode = .perceptual

    // guard let spatialScaler = desc.makeSpatialScaler(device: device) else {
    //     print("The spatial scaler effect is not usable!")
    //     mfxScalingMode = .defaultScaling
    //     return
    // }

    // mfxSpatialScaler = spatialScaler
}

void
XPMetalUpscaler::setupTemporalScaler()
{
}
