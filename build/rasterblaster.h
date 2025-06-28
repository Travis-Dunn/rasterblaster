#ifndef RASTERBLASTER_H 
#define RASTERBLASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

/* This is the only non-platform file we need. It contains the system that
 * communicates between the platform layer and the application layer */
#include "event.h"

/* Display scaling */
#define INTERNAL_WIDTH 250
#define INTERNAL_HEIGHT 250
#define PIXEL_SCALE 4

#define WINDOW_WIDTH (PIXEL_SCALE * INTERNAL_WIDTH)
#define WINDOW_HEIGHT (PIXEL_SCALE * INTERNAL_HEIGHT)
#define WINDOW_TITLE "Rasterblaster"

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam,
        LPARAM lParam);
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow);

void ResizeDIBSection       (int w, int h);
void PaintWindow            (HDC hdc, RECT* r, int x, int y, int w, int h);
void printOffsets           (); /* debug */
/* Call from application to link event queue */
void SetPlatformEventQueue  (EventQueue* q);

extern int          running;
extern BITMAPINFO   bitmapInfo;
extern void*        bitmapMemory;
extern int          bitmapW;
extern int          bitmapH;

#endif /* RASTERBLASTER_H */
