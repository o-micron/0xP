@echo off

IF not exist "%~f3" ( mkdir "%~f3" )

IF exist "%~f3%~f2.vs.fxc" ( del "%~f3%~f2.vs.fxc" )
IF exist "%~f3%~f2.ps.fxc" ( del "%~f3%~f2.ps.fxc" )

fxc /Od /Zi /T vs_5_1 /E "VSMain" /Fo %3%2.vs.fxc %1/%2.hlsl
fxc /Od /Zi /T ps_5_1 /E "PSMain" /Fo %3%2.ps.fxc %1/%2.hlsl