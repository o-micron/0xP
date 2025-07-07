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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

class MTLFXSpatialScaler;
class MTLFXTemporalScaler;

enum XPMetalUpscalerMode
{
    XPMetalUpscalerMode_DefaultScaling,
    XPMetalUpscalerMode_SpacialSampling,
    XPMetalUpscalerMode_TemporalScaling
};

class XPMetalUpscaler
{
  public:
    XPMetalUpscaler();
    ~XPMetalUpscaler();
    void initialize();
    void resize(int width, int height);
    void finalize();

  private:
    void setup();
    void releaseResources();
    void adjustScale(float newRendererScale);
    void setupSpatialScaler();
    void setupTemporalScaler();

    XPMetalUpscalerMode  _mode;
    bool                 _makeMotionVectors;
    MTLFXSpatialScaler*  _spatialScaler;
    MTLFXTemporalScaler* _temporalScaler;
};
