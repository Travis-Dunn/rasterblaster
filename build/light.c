#include "light.h"
#include "renderer.h"


Light MakeAmbient(unsigned char r, unsigned char g, unsigned char b){
    Light l;
    l.rgb = RGBA_INT(r, g, b, 255);
    l.type = LIGHT_AMBIENT;
    return l;
}

Light MakeDirectional(unsigned char r, unsigned char g, unsigned char b,
        Vec3 dir){
    Light l;
    l.rgb = RGBA_INT(r, g, b, 255);
    l.type = LIGHT_DIRECTIONAL;
    l.dir = dir;
    l.inverseDir = Vec3Make(-dir.x, -dir.y, -dir.z);
    return l;
}

