@echo off

if defined VSINSTALLDIR (
    echo Running inside "Developer Command Prompt VS %VisualStudioVersion%"
) else (
    echo Not running inside VS Developer Command Prompt. Exiting...
    exit /b 1
)

: -------------------------------------------------------------------------------------------------
: BUILD WITH CLANG
: -------------------------------------------------------------------------------------------------
rmdir /s /q "build"
cmake --preset NinjaWin
cmake --build --preset NinjaWin
: ------------------------------------------------------------------------------------------------

: ------------------------------------------------------------------------------------------------
: Build WITH MSVC
: ------------------------------------------------------------------------------------------------
: You have to first generate "compile_command.json" so for now use preset NinjaWin to do that
@REM cmake --preset NinjaWin

@REM cd build
@REM for %%f in (*) do (
@REM     if /i not "%%~xf"==".json" del "%%f"
@REM )
@REM cd ..
@REM cmake --preset VisualStudio
@REM cmake --build --preset VisualStudio
: -------------------------------------------------------------------------------------------------