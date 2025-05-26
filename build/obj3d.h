#ifndef OBJ3D_H
#define OBJ3D_H

#include "model.h"

typedef struct {
    Model* model;
    Vec3 scale, rot, pos;
    Mat4 matModel;
    int id;
} Obj3D;

void UpdateObj3DModelMatrix(Obj3D* o);

#endif /* OBJ3D_H */
