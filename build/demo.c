#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    PutPixel(128, 256); 
}

void Update(){
    UpdateTimer();
}
