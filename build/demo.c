#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    int xtl = 100;
    int xtr = 200;
    int xbl = 100;
    int xbr = 200;
    int ytl = 130;
    int ytr = 130;
    int ybl = 200;
    int ybr = 200;
    DrawLine(xtl, ytl, xtr, ytr);
    DrawLine(xtr, ytr, xbr, ybr);
    DrawLine(xbr, ybr, xbl, ybl);
    DrawLine(xbl, ybl, xtl, ytl);
    DrawLine(xbl, ybl, xtr, ytr);
    DrawLine(xbr, ybr, xtl, ytl);
}

void Update(){
    UpdateTimer();
}
