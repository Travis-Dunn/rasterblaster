@echo off
echo Building and Running Raster Blaster...

pushd "%~dp0"

cd build

C:\msys64\mingw32\bin\i686-w64-mingw32-gcc *.c *.s -g -static -o ..\bin\rasterblaster.exe -lgdi32

if %ERRORLEVEL% neq 0 (
    echo Build failed with error code %ERRORLEVEL%
    popd
    pause
    exit /b 1
)

cd ..

echo Build successful! Running…
start "" bin\rasterblaster.exe

popd
echo Application launched.