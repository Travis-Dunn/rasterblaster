#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    PutPixel(40, 40, RGBA_INT(255, 0, 0, 255));    
    PutPixel(41, 40, RGBA_INT(0, 255, 0, 255));
    PutPixel(42, 40, RGBA_INT(0, 0, 255, 255));
}

void Update(){
    UpdateTimer();
}
