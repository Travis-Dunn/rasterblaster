#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    Point2D tl = {96, 96};
    Point2D br = {128, 256};
    DrawFilledRect(tl, br);
}

void Update(){
    UpdateTimer();
}
