@echo off
@echo on
set MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe
echo %MSBUILD%

:: Default location for Unreal's build batch script
:: set BUILD_SCRIPT="C:\Program Files\Epic Games\UE_4.27\Engine\Build\BatchFiles\Build.bat"

:: Find full path for our *.uproject file, assuming we're running from project root
:: Couldn't find a nice way to do this without either a loop or a temp file
:: for /f "delims=" %%a in ('dir /s /b *.uproject') do set "PROJECT_DIR=%%a"

:: Build command lifted from Rider
:: Should run after branch change or merge
:: %BUILD_SCRIPT% ProjectTentacleEditor Win64 Development -Project=%PROJECT_DIR% -WaitMutex -FromMsBuild

:: Unset env variables and turn echo back on
:: set BUILD_SCRIPT=
:: set PROJECT_DIR=
set MSBUILD=
@echo on