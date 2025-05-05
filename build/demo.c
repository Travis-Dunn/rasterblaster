#include "demo.h"
#include "renderer.h"

void Init(){
    InitTimer(1024);
}

void Render(){
    DrawLine(5, 11, 152, 98);
    DrawLine(136, 22, 6, 82);
}

void Update(){
    UpdateTimer();
}
