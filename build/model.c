#include "model.h"

Mat4 ModelMatrix(Model* model){
    Mat4 scale = MatScale(model->scale.x, model->scale.y, model->scale.z);
    Mat4 pitch = MatPitch(model->rot.x);
    Mat4 yaw = MatYaw(model->rot.y);
    Mat4 roll = MatRoll(model->rot.z);
    Mat4 rot = MatMatMul(&roll, &pitch);
    rot = MatMatMul(&yaw, &rot);
    Mat4 m = MatMatMul(&rot, &scale);
    Mat4 trans = MatTranslate(model->pos.x, model->pos.y, model->pos.z);
    m = MatMatMul(&trans, &m);
    return m;
}
