#!/bin/bash

# ---------------------------------------------------------------------------------
# for d3d12, make sure you copy the following then remove after capture is done
# d3d12.dll 
# dxgi.dll
# d3d12_capture.dll
# ---------------------------------------------------------------------------------

cd build/Debug/
set GFXRECON_QUIT_AFTER_CAPTURE_FRAMES=true
set GFXRECON_CAPTURE_TRIGGER=F3
set GFXRECON_CAPTURE_TRIGGER_FRAMES=1
./XPENGINE
cd ../../