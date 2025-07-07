@echo off

IF not exist "%~f3" ( mkdir "%~f3" )

IF exist "%~f3%~f2.surface.spv" ( del "%~f3%~f2.surface.spv" )
IF exist "%~f3%~f2.vert.spv" ( del "%~f3%~f2.vert.spv" )
IF exist "%~f3%~f2.frag.spv" ( del "%~f3%~f2.frag.spv" )

%VULKAN_SDK%/Bin/glslc.exe -c -fshader-stage=vertex "%1/%2.vert" -o "%3%2.vert.spv"
%VULKAN_SDK%/Bin/glslc.exe -c -fshader-stage=fragment "%1/%2.frag" -o "%3%2.frag.spv"
%VULKAN_SDK%/Bin/spirv-link.exe "%3%2.vert.spv" "%3%2.frag.spv" -o "%3%2.surface.spv"