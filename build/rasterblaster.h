#ifndef RASTERBLASTER_H 
#define RASTERBLASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define INTERNAL_WIDTH 1440
#define INTERNAL_HEIGHT 900
#define PIXEL_SCALE 1

#define WINDOW_WIDTH (PIXEL_SCALE * INTERNAL_WIDTH)
#define WINDOW_HEIGHT (PIXEL_SCALE * INTERNAL_HEIGHT)
#define WINDOW_TITLE "K&R C Software Renderer"

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam,
        LPARAM lParam);
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow);
void ResizeDIBSection(int w, int h);
void PaintWindow(HDC hdc, RECT* r, int x, int y, int w, int h);
void printOffsets();

int running = 0;
BITMAPINFO bitmapInfo;
void* bitmapMemory;
int bitmapW;
int bitmapH;
#endif /* RASTERBLASTER_H */
