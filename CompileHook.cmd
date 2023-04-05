@echo off

:: Project name
set SOLUTION=ProjectTentacle
:: Default MSBuild location
set MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe
:: UE4 Version Selector, used to regenerate project files in case they are missing
set UEVersionSelect=%ProgramFiles(x86)%\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe

:: If solution already exists, skip generation
if exist %SOLUTION%.sln goto :soutiongenerated

:: Generate solution files
"%UEVersionSelect%" /projectfiles %~dp0\%SOLUTION%.uproject

:: Once solution is generated, run msbuild with relevant params
:soutiongenerated
@echo on
echo %MSBUILD%
echo %SOLUTION%.sln
"%MSBUILD%" %~dp0\%SOLUTION%.sln /p:Configuration=Development /p:Platform="Win64"
@echo off
:: Unset env variables and turn echo back on
set MSBUILD=
set SOLUTION=
set UEVersionSelect=
@echo on