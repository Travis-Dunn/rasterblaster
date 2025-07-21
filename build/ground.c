#include "ground.h"
#include "stdlib.h"
#include "math.h"
#include "assert.h"

#define MIN_X -16
#define MAX_X 16
#define MIN_Z -16
#define MAX_Z 16
#define TILE_W 32
#define TILE_H 32
#define VERT_W (TILE_W + 1)
#define VERT_H (TILE_H + 1)
#define VERTS(x, z) verts[(z) * VERT_W + (x)]
#define MAP(x, z) map->m[(z) * TILE_W + (x)]

int HeightMapBuild(PLY_Mesh* mesh, HeightMap* map) {
    if (!mesh || !mesh->triangles || !mesh->triCount || !map) return -1;

    map->m = (float*)malloc(TILE_W * TILE_H * sizeof(float));
    if (!map->m) return -2;

    float* verts = (float*)malloc(VERT_W * VERT_H * sizeof(float));
    if (!verts) {
        free(map->m);
        return -2;
    }

    for (int i = 0; i < mesh->triCount; i++) {
        float y0 = mesh->triangles[i].v0.pos.y;
        int x0 = (int)floorf(mesh->triangles[i].v0.pos.x) - MIN_X;
        int z0 = (int)floorf(mesh->triangles[i].v0.pos.z) - MIN_Z;
        assert(x0 >= 0 && x0 <= TILE_W);
        assert(z0 >= 0 && z0 <= TILE_H);
        VERTS(x0, z0) = y0;
        float y1 = mesh->triangles[i].v1.pos.y;
        int x1 = (int)floorf(mesh->triangles[i].v1.pos.x) - MIN_X;
        int z1 = (int)floorf(mesh->triangles[i].v1.pos.z) - MIN_Z;
        assert(x1 >= 0 && x1 <= TILE_W);
        assert(z1 >= 0 && z1 <= TILE_H);
        VERTS(x1, z1) = y1;
        float y2 = mesh->triangles[i].v2.pos.y;
        int x2 = (int)floorf(mesh->triangles[i].v2.pos.x) - MIN_X;
        int z2 = (int)floorf(mesh->triangles[i].v2.pos.z) - MIN_Z;
        assert(x2 >= 0 && x2 <= TILE_W);
        assert(z2 >= 0 && z2 <= TILE_H);
        VERTS(x2, z2) = y2;
    }
    
    for (int i = 0; i < TILE_W; i++) {
        for (int j = 0; j < TILE_H; j++) {
            float tl = VERTS(i, j);
            float tr = VERTS(i + 1, j);
            float bl = VERTS(i, j + 1);
            float br = VERTS(i + 1, j + 1);
            MAP(i, j) = (tl + tr + bl + br) / 4.f;
        }
    }
    free(verts);
    return 0;
}
