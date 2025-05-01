#ifndef RASTERBLASTER_H 
#define RASTERBLASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "K&R C Software Renderer"

typedef struct {
	int width;
	int height;
	void* buffer;
} Framebuffer;

typedef struct {
	int running;
	Framebuffer framebuffer;
	/* Windows */
	HWND hwnd;
	HDC hdc;
	BITMAPINFO bmi;
} RendererState;

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam,
        LPARAM lParam);
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow);
void ResizeDIBSection(int w, int h);
void PaintWindow(HDC hdc, RECT* r, int x, int y, int w, int h);
void RenderWeirdGradient(int xOffset, int yOffset);

int running = 0;
BITMAPINFO bitmapInfo;
void* bitmapMemory;
int bitmapW;
int bitmapH;
int xanim = 0;
int yanim = 0;
#endif /* RASTERBLASTER_H */
