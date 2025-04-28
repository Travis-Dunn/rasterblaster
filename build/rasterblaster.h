#ifndef RENDERER_H
#define RENDERER_H

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

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif /* RENDERER_H */
