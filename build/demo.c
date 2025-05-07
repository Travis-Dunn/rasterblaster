#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    int c = RGBA_INT(212, 44, 162, 255);
    int x0 = 38;
    int y0 = 44;
    int x1 = 139;
    int y1 = 67;
    int x2 = 41;
    int y2 = 82;
    DrawTri(x0, y0, x1, y1, x2, y2, c);
}

void Update(){
    UpdateTimer();
}
