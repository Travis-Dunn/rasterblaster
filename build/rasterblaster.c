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

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	LRESULT ret = 0;
	switch(msg){
	case WM_SIZE:{
	OutputDebugStringA("WM_SIZE\n");	
        } break;
	case WM_DESTROY:{
	OutputDebugStringA("WM_DESTROY\n");
	} break;
	case WM_CLOSE:{
	PostQuitMessage(0);
	OutputDebugStringA("WM_CLOSE\n");
	} break;
	case WM_ACTIVATEAPP:{
	OutputDebugStringA("WM_ACTIVATEAPP\n");
	} break;
	case WM_PAINT:{
	PAINTSTRUCT paint;
	HDC hdc = BeginPaint(hWnd, &paint);
	int w = paint.rcPaint.right - paint.rcPaint.left;
	int h = paint.rcPaint.bottom - paint.rcPaint.top;
	PatBlt(hdc, paint.rcPaint.left, paint.rcPaint.top, w, h, BLACKNESS);
	EndPaint(hWnd, &paint);
	} break;
	default:{
	ret = DefWindowProc(hWnd, msg, wParam, lParam);
	} break;
	}
	return ret;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	WNDCLASS WindowClass = { 0 };
	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "RasterBlasterClass";
	if (!RegisterClass(&WindowClass)){
		/* TODO: logging */
		return 1;	
	}
	HWND windowHandle = CreateWindowExA(0, WindowClass.lpszClassName, "RasterBlaster", 
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			0, 0, hInstance, 0);
	if (!windowHandle){
		/* TODO: logging */
		return 1;
	}
	MSG msg;
	while (1){
		BOOL msgResult = GetMessage(&msg, 0, 0, 0);
		if (!msgResult > 0) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

#endif /* RENDERER_H */
