@echo off

rmdir /s /q "artifacts\windows\"

mkdir artifacts
cd artifacts

: -------------------------------------------------------------------------------------------------
: Download artifacts
: -------------------------------------------------------------------------------------------------
gh release download v0.0.1 --repo o-micron/0xTP --pattern Assimp-windows-x64.zip
powershell -command "Expand-Archive -Path 'Assimp-windows-x64.zip' -DestinationPath './assimp' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Boost-windows-x64.zip
powershell -command "Expand-Archive -Path 'Boost-windows-x64.zip' -DestinationPath './boost' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern FMT-windows-x64.zip
powershell -command "Expand-Archive -Path 'FMT-windows-x64.zip' -DestinationPath './fmt' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern GfxReconstruct-windows-x64.zip
powershell -command "Expand-Archive -Path 'GfxReconstruct-windows-x64.zip' -DestinationPath './gfxreconstruct' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern GLM-windows-x64.zip
powershell -command "Expand-Archive -Path 'GLM-windows-x64.zip' -DestinationPath './glm' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern ImGui-windows-x64.zip
powershell -command "Expand-Archive -Path 'ImGui-windows-x64.zip' -DestinationPath './imgui' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Implot-windows-x64.zip
powershell -command "Expand-Archive -Path 'Implot-windows-x64.zip' -DestinationPath './implot' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Inja-windows-x64.zip
powershell -command "Expand-Archive -Path 'Inja-windows-x64.zip' -DestinationPath './inja' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern JoltPhysics-windows-x64.zip
powershell -command "Expand-Archive -Path 'JoltPhysics-windows-x64.zip' -DestinationPath './joltphysics' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Json-windows-x64.zip
powershell -command "Expand-Archive -Path 'Json-windows-x64.zip' -DestinationPath './json' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Kaguya-windows-x64.zip
powershell -command "Expand-Archive -Path 'Kaguya-windows-x64.zip' -DestinationPath './kaguya' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern LLVM-windows-x64.zip
powershell -command "Expand-Archive -Path 'LLVM-windows-x64.zip' -DestinationPath './llvm' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Lua-windows-x64.zip
powershell -command "Expand-Archive -Path 'Lua-windows-x64.zip' -DestinationPath './lua' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern MeshOptimizer-windows-x64.zip
powershell -command "Expand-Archive -Path 'MeshOptimizer-windows-x64.zip' -DestinationPath './meshoptimizer' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern SDL2-windows-x64.zip
powershell -command "Expand-Archive -Path 'SDL2-windows-x64.zip' -DestinationPath './SDL' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern SpirvCross-windows-x64.zip
powershell -command "Expand-Archive -Path 'SpirvCross-windows-x64.zip' -DestinationPath './SPIRV-Cross' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Stb-windows-x64.zip
powershell -command "Expand-Archive -Path 'Stb-windows-x64.zip' -DestinationPath './stb' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern TinyExr-windows-x64.zip 
powershell -command "Expand-Archive -Path 'TinyExr-windows-x64.zip' -DestinationPath './tinyexr' -Force"
gh release download v0.0.1 --repo o-micron/0xTP --pattern Tracy-windows-x64.zip 
powershell -command "Expand-Archive -Path 'Tracy-windows-x64.zip' -DestinationPath './tracy' -Force"
: -------------------------------------------------------------------------------------------------


: -------------------------------------------------------------------------------------------------
: Copy artifacts
: -------------------------------------------------------------------------------------------------
rmdir /s /q "windows"
mkdir windows\compiler
mkdir windows\bin
mkdir windows\include
mkdir windows\lib
mkdir windows\src

xcopy /s /f /y "assimp" "windows"
xcopy /s /f /y "boost" "windows"
xcopy /s /f /y "fmt" "windows"
xcopy /s /f /y "glm" "windows"
xcopy /s /f /y "gfxreconstruct" "windows"
xcopy /s /f /y "imgui" "windows"
xcopy /s /f /y "implot" "windows"
xcopy /s /f /y "inja" "windows"
xcopy /s /f /y "joltphysics" "windows"
xcopy /s /f /y "json" "windows"
xcopy /s /f /y "kaguya" "windows"
xcopy /s /f /y "llvm" "windows\compiler"
xcopy /s /f /y "lua" "windows"
xcopy /s /f /y "meshoptimizer" "windows"
xcopy /s /f /y "SDL" "windows"
xcopy /s /f /y "SPIRV-Cross" "windows"
xcopy /s /f /y "stb" "windows"
xcopy /s /f /y "tinyexr" "windows"
xcopy /s /f /y "tracy" "windows"

rmdir /s /q "assimp"
rmdir /s /q "boost"
rmdir /s /q "fmt"
rmdir /s /q "glm"
rmdir /s /q "gfxreconstruct"
rmdir /s /q "imgui"
rmdir /s /q "implot"
rmdir /s /q "inja"
rmdir /s /q "joltphysics"
rmdir /s /q "json"
rmdir /s /q "kaguya"
rmdir /s /q "llvm"
rmdir /s /q "lua"
rmdir /s /q "meshoptimizer"
rmdir /s /q "SDL"
rmdir /s /q "SPIRV-Cross"
rmdir /s /q "stb"
rmdir /s /q "tinyexr"
rmdir /s /q "tracy"
: -------------------------------------------------------------------------------------------------

cd ..