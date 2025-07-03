#ifndef MODEL_H
#define MODEL_H

#include "arithmetic.h"
#include "wavefront.h"
#include "texture.h"
#include "plyfile.h"

typedef struct {
    Mesh* mesh;
    Texture* tex;
    PLY_Mesh plymesh;
} Model;

#endif /* MODEL_H */
