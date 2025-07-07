@echo off

IF not exist "%~f3" ( mkdir "%~f3" )

IF exist "%~f3%~f2.cs.fxc" ( del "%~f3%~f2.cs.fxc" )

fxc /Od /Zi /T cs_5_1 /E "CSMain" /Fo %3%2.cs.fxc %1/%2.hlsl