@echo off

IF not exist "%~f3" ( mkdir "%~f3" )

IF exist "%~f3%~f2.compute.spv" ( del "%~f3%~f2.compute.spv" )

%VULKAN_SDK%/Bin/glslc.exe -c -fshader-stage=compute "%1/%2.comp" -o "%3%2.compute.spv"