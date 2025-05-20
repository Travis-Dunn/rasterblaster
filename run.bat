@echo off
echo Running Raster Blaster...

pushd "%~dp0"

start "" bin\rasterblaster.exe

popd

echo Application launched.

exit /b 1
