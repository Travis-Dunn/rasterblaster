#include "rasterblaster.h"
#include "renderer.h"
#include "demo.h"
#include "mouse.h"

int running = 0;
BITMAPINFO bitmapInfo;
void* bitmapMemory;
int bitmapW;
int bitmapH;

static EventQueue*  g_EventQueue = 0;
static int          g_WindowActive = 1;

void SetPlatformEventQueue(EventQueue* q){
    g_EventQueue = q;
}

LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam, 
        LPARAM lParam){
    LRESULT ret = 0;
    switch(msg){
    case WM_SIZE:{
/* Right now the window is not resizable, so we won't get this message */                     
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
        g_WindowActive = (wParam != 0);
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
    case WM_LBUTTONDOWN:{
        if (g_EventQueue){
            Event evt;
            evt.type = EVT_LBUTTONDOWN;
            evt.buf[0] = (int)LOWORD(lParam);
            evt.buf[1] = (int)HIWORD(lParam);
            EventEnqueue(g_EventQueue, &evt);
        }
        int mouseX = (int)LOWORD(lParam);
        int mouseY = (int)HIWORD(lParam);
        int ret = GetClicked(mouseX, mouseY);
        /*
        if (ret == 12){
            printf("clicked on the carpfish!\n");
        } else {
            printf("clicked elsewhere, more specifically: %d, %d\n", mouseX, mouseY);
        }
        */
    } break;
    case WM_KEYDOWN: {
        if (g_EventQueue){
            Event evt;
            evt.type = EVT_KEYDOWN;
            evt.buf[0] = (int)wParam;
            EventEnqueue(g_EventQueue, &evt);
        }
    } break;
    case WM_KEYUP: {
        if (g_EventQueue){
            Event evt;
            evt.type = EVT_KEYUP;
            evt.buf[0] = (int)wParam;
            EventEnqueue(g_EventQueue, &evt);
        }
    } break;
    case WM_INPUT: {
        if (!g_WindowActive) break;
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        RAWINPUT* raw = (RAWINPUT*)malloc(dwSize);
        if (!raw) break;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &dwSize, sizeof(RAWINPUTHEADER));
        if (raw->header.dwType == RIM_TYPEMOUSE){
            int deltaX = raw->data.mouse.lLastX;
            int deltaY = raw->data.mouse.lLastY;
            Event evt;
            evt.type = EVT_MOUSEMOVE;
            evt.buf[1] = deltaX;
            evt.buf[0] = deltaY;
            EventEnqueue(g_EventQueue, &evt);
        }
        free(raw);
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

    RECT windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
        WS_MINIMIZEBOX;
    AdjustWindowRect(&windowRect, windowStyle, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    HWND windowHandle = CreateWindowExA(0, WindowClass.lpszClassName, "RasterBlaster", 
/*            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
 *            CW_USEDEFAULT, CW_USEDEFAULT,*/
            windowStyle | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth,
            windowHeight,
            0, 0, hInstance, 0);
    if (!windowHandle){
        /* TODO: logging */
        return 1;
    }
    
    HDC hdc = GetDC(windowHandle);
    SetStretchBltMode(hdc, COLORONCOLOR);
    ReleaseDC(windowHandle, hdc);
    
    /* register for WM_INPUT */
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; /* HID_USAGE_PAGE_GENERIC */
    rid.usUsage = 0x02; /* HID_USAGE_GENERIC_MOUSE */
    rid.dwFlags = 0;
    rid.hwndTarget = windowHandle;
    RegisterRawInputDevices(&rid, 1, sizeof(rid)); 

    MSG msg;
    running = 1;

    ResizeDIBSection(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    renderer.framebuffer.w = INTERNAL_WIDTH;
    renderer.framebuffer.h = INTERNAL_HEIGHT;
    renderer.framebuffer.buf = bitmapMemory;
    renderer.running = 1;

    Init();

    while (running){
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            if (msg.message == WM_CLOSE) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Update();
        Render();
/*        printf("%.1f\n", timer.fpsAvg);*/

        HDC hdc = GetDC(windowHandle);
        RECT clientRect;
        GetClientRect(windowHandle, &clientRect);
        int windowWidth = clientRect.right - clientRect.left;
        int windowHeight = clientRect.bottom - clientRect.top;
        PaintWindow(hdc, &clientRect, 0, 0, windowWidth, windowHeight);
        ReleaseDC(windowHandle, hdc);
    }
    printf("Press any key to exit...\n");
    getchar();
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
    StretchDIBits(hdc, 0, 0, windowW, windowH, 
            0, 0, INTERNAL_WIDTH, INTERNAL_HEIGHT,
            bitmapMemory, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

void printOffsets(){
    printf("Offset of renderer.running: %lu\n", (unsigned long)&(((Renderer*)0)
                ->running));
    printf("Offset of renderer.framebuffer: %lu\n", (unsigned long)&(((Renderer*
                        )0)->framebuffer));
    printf("Offset of framebuffer.w: %lu\n", (unsigned long)&(((Framebuffer*)0)
                ->w));
    printf("Offset of framebuffer.h: %lu\n", (unsigned long)&(((Framebuffer*)0)
                ->h));
    printf("Offset of framebuffer.buf %lu\n", (unsigned long)&(((Framebuffer*)0)
                ->buf));
}
