@echo off
echo Building and Running Raster Blaster...

cd build
C:\msys64\mingw32\bin\i686-w64-mingw32-gcc *.c *.s -g -static -o ..\bin\rasterblaster.exe -lgdi32

if %ERRORLEVEL% == 0 (
    echo Build successful! Running application...
    cd ..\bin
    start rasterblaster.exe
) else (
    echo Build failed with error code %ERRORLEVEL%
    cd ..
    pause
    exit /b 1
)

echo Application launched.
