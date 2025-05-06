#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    int c = RGBA_INT(212, 44, 162, 255);
    DrawLine(80, 50, 0, 0, c);
    DrawLine(80, 50, 20, 0, c);
    DrawLine(80, 50, 40, 0, c);
    DrawLine(80, 50, 60, 0, c);
    DrawLine(80, 50, 80, 0, c);
    DrawLine(80, 50, 100, 0, c);
    DrawLine(80, 50, 120, 0, c);
    DrawLine(80, 50, 140, 0, c);
    DrawLine(80, 50, 159, 0, c);
    DrawLine(80, 50, 159, 12, c);
    DrawLine(80, 50, 159, 25, c);
    DrawLine(80, 50, 159, 37, c);
    DrawLine(80, 50, 159, 50, c);
    DrawLine(80, 50, 159, 62, c);
    DrawLine(80, 50, 159, 75, c);
    DrawLine(80, 50, 159, 87, c);
    DrawLine(80, 50, 159, 99, c);
    DrawLine(80, 50, 140, 99, c);
    DrawLine(80, 50, 120, 99, c);
    DrawLine(80, 50, 100, 99, c);
    DrawLine(80, 50, 80, 99, c);
    DrawLine(80, 50, 60, 99, c);
    DrawLine(80, 50, 40, 99, c);
    DrawLine(80, 50, 20, 99, c);
    DrawLine(80, 50, 0, 99, c);
    DrawLine(80, 50, 0, 87, c);
    DrawLine(80, 50, 0, 75, c);
    DrawLine(80, 50, 0, 62, c);
    DrawLine(80, 50, 0, 50, c);
    DrawLine(80, 50, 0, 37, c);
    DrawLine(80, 50, 0, 25, c);
    DrawLine(80, 50, 0, 12, c);
}

void Update(){
    UpdateTimer();
}
