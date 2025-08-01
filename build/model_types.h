#ifndef MODEL_TYPES_H
#define MODEL_TYPES_H

#include "arithmetic.h"
#include "Texture.h"
#include "rgb565.h"
#include "sf610.h"

/* Some of these (e.g. ModelCol) require only the data in the 3d file, and are
 * complete after being built by the "Load from file" function.
 * Others (e.g. ModelTex) require additional resources, such a texture to be 
 * complete. The others have, in addition to the main struct that starts
 * with "Model", a second struct with the suffix "_stub" that represents the
 * portion of the main "Model" struct that is comprised of data loaded from the
 * 3d file. In short, "_stub" is the main struct minus any textures. These stub
 * structs get converted into the main struct when building the Modl resources
 */

/* Position and vertex color */
typedef struct {
    Vec3 pos;
    int color;
} VertexCol;

typedef struct { VertexCol v0, v1, v2; } TriCol;

typedef struct {
    TriCol* tris;
    int count;
    int init;
} ModelCol;

/* Position and vertex color, but uses the 6.10 fixed point format
 * defined in sf610.h for position, and the RGB565 format defined in rgb565.h
 * for color. */
typedef struct {
    short pos[3];
    short color;
} VertCol16;

typedef struct { VertCol16 v0, v1, v2; } TriCol16;

typedef struct {
    TriCol16* tris;
    int count;
    int init;
} ModelCol16;

/* Position and texture mapping */
typedef struct {
    Vec3 pos;
    Vec2 uv;
} VertexTex;

typedef struct { VertexTex v0, v1, v2; } TriTex;

typedef struct {
    TriTex* tris;
    int count;
    int init;
    Texture* tex;
} ModelTex;

typedef struct {
    TriTex* tris;
    int count;
    int init;
} ModelTex_stub;

#endif /* MODEL_TYPES_H */
