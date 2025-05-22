#ifndef MODEL_H
#define MODEL_H

#include "arithmetic.h"
#include "wavefront.h"
#include "texture.h"

typedef struct {
    Vec3 scale;
    Vec3 rot;
    Vec3 pos;
    Mesh* mesh;
    Texture* tex;
} Model;

Mat4 ModelMatrix(Model* model);
#endif /* MODEL_H */
