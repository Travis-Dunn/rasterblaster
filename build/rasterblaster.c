#include "rasterblaster.h"

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam, 
        LPARAM lParam){
    LRESULT ret = 0;
    switch(msg){
    case WM_SIZE:{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    ResizeDIBSection(clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top);
    OutputDebugStringA("WM_SIZE\n");    
    } break;
    case WM_DESTROY:{
    running = 0;                        
    OutputDebugStringA("WM_DESTROY\n");
    } break;
    case WM_CLOSE:{
    running = 0;
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
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    PaintWindow(hdc, &clientRect, paint.rcPaint.left, paint.rcPaint.top, w, h);
    EndPaint(hWnd, &paint);
    } break;
    default:{
    ret = DefWindowProc(hWnd, msg, wParam, lParam);
    } break;
    }
    return ret;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow){
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
    running = 1;
    while (running){
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            if (msg.message == WM_CLOSE) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        RenderWeirdGradient(xanim, yanim);
        xanim++;
        yanim++;

        HDC hdc = GetDC(windowHandle);
        RECT clientRect;
        GetClientRect(windowHandle, &clientRect);
        int windowWidth = clientRect.right - clientRect.left;
        int windowHeight = clientRect.bottom - clientRect.top;
        PaintWindow(hdc, &clientRect, 0, 0, windowWidth, windowHeight);
        ReleaseDC(windowHandle, hdc);
   }
    return 0;
}

void ResizeDIBSection(int w, int h){
    /* bulletproof this */
    if(bitmapMemory){
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }
    bitmapW = w;
    bitmapH = h;
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = bitmapW;
    bitmapInfo.bmiHeader.biHeight = -bitmapH;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    int bytesPerPixel = 4;
    int bitmapMemorySize = bitmapW * bitmapH * bytesPerPixel;
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT,
            PAGE_READWRITE);
}

void PaintWindow(HDC hdc, RECT* r, int x, int y, int w, int h){
    int windowW = r->right - r->left;
    int windowH = r->bottom - r->top;
    StretchDIBits(hdc, x, y, bitmapW, bitmapH, x, y, windowW, windowH,
            bitmapMemory, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

void RenderWeirdGradient(int xOffset, int yOffset){
    int pitch = bitmapW * 4;
    unsigned char* row = (unsigned char*)bitmapMemory;
    for (int y = 0; y < bitmapH; ++y){
        unsigned char* pixel = (unsigned char*)row;
        for (int x = 0; x < bitmapW; ++x){
            *pixel++ = (unsigned char)(x + xOffset);
            *pixel++ = 0;
            *pixel++ = (unsigned char)(y + yOffset);
            *pixel++ = 0;
        }
        row += pitch;
    }
}
