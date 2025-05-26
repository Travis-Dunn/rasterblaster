#include "obj3d.h"

void UpdateObj3DModelMatrix(Obj3D* o){
    Mat4 scale = MatScale(o->scale.x, o->scale.y, o->scale.z);
    Mat4 pitch = MatPitch(o->rot.x);
    Mat4 yaw = MatYaw(o->rot.y);
    Mat4 roll = MatRoll(o->rot.z);
    Mat4 rot = MatMatMul(&roll, &pitch);
    rot = MatMatMul(&yaw, &rot);
    Mat4 m = MatMatMul(&rot, &scale);
    Mat4 trans = MatTranslate(o->pos.x, o->pos.y, o->pos.z);
    o->matModel = MatMatMul(&trans, &m);
}
