# BUILD

0xP is using cmake and cmake presets to generate xcode or visual studio solutions to build.
0xP is mainly relying on `Ninja` to generate `compile_commands.json` database.
So even if you're using `Xcode` or `Visual Studio`, you have to first let cmake use `Ninja` generator first to also emmit the `compile_commands.json`. I would recommend simply using `Ninja` for all platforms.
I know some time you need to use `Visual Studio` or `Xcode` to debug and do what you're used to do, so that's also available. Pick whatever works for you. For simplicity, just always run `build.sh` or `build.bat`. Then you can launch Xcode or Visual Studio from `build/XPENGINE.vs` or `build/XPENGINE.xcodeproj`.

The 0xP engine is build in multiple subsystems. \
Rendering is a subsystem. Physics is a subsystem. etc .. \
Each subsystem is expected to have multiple available implementations for you to choose from. \
You can edit that in `CMakePresets.json`


# Dependencies
- Xcode, Visual Studio, GCC (some C++20 capable compiler)
- Python3
- VulkanSDK
- CUDA (install toolkit) if you're running the RISC-V emulator on the GPU through CUDA

# MacOS
- `git clone https://github.com/o-micron/0xP.git`
- `download_artifacts.sh` \
  For the download script to run correctly you need to have [github cli](https://cli.github.com/) installed, it basically fetches binaries from releases from [0xTP](https://github.com/o-micron/0xTP/releases/tag/v0.0.1), you can download them all manually instead if you want from [0xTP release v0.0.1](https://github.com/o-micron/0xTP/releases/tag/v0.0.1)
- `build.sh`
- `run_engine.sh`
- `run_editor.sh`
- `launch_tracy.sh`


# Linux
- `git clone https://github.com/o-micron/0xP.git`
- `download_artifacts.sh` \
  For the download script to run correctly you need to have [github cli](https://cli.github.com/) installed, it basically fetches binaries from releases from [0xTP](https://github.com/o-micron/0xTP/releases/tag/v0.0.1), you can download them all manually instead if you want from [0xTP release v0.0.1](https://github.com/o-micron/0xTP/releases/tag/v0.0.1)
- `build.sh`
- `run_engine.sh`
- `run_editor.sh`
- `launch_tracy.sh`

# Windows
- `git clone https://github.com/o-micron/0xP.git`
- `download_artifacts.bat` \
  For the download script to run correctly you need to have [github cli](https://cli.github.com/) installed, it basically fetches binaries from releases from [0xTP](https://github.com/o-micron/0xTP/releases/tag/v0.0.1), you can download them all manually instead if you want from [0xTP release v0.0.1](https://github.com/o-micron/0xTP/releases/tag/v0.0.1)
- `build.bat`
- `run_engine.bat`
- `run_editor.bat`
- `launch_tracy.bat`