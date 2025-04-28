@echo off
echo Building Raster Blaster...
cd build
gcc *.c -g -o ..\bin\rasterblaster.exe -lgdi32
if %ERRORLEVEL% == 0 (
    echo Build successful!
) else (
    echo Build failed with error code %ERRORLEVEL%
)
pause