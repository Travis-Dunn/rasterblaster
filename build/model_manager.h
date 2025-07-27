#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "model_types.h"
#include "plyfile.h"

int              ModelManagerInit   ();

ModelCol*        GetModelCol        (char* filename);
ModelTex_stub*   GetModelTex_stub   (char* filename);

#endif /* MODEL_MANAGER_H */
