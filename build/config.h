#ifndef CONFIG_H
#define CONFIG_H

#define USE_SAFE_PIXEL_FUNCTIONS

/* Renderer defaults */
#define DEFAULT_ENABLE_CULLING                  1
#define DEFAULT_CULL_FACE                       0
#define DEFAULT_ENABLE_DEPTH_TEST               1

typedef enum {
    LOAD_ASSETS_AS_R8B8G8A8,
    LOAD_ASSETS_AS_R5G6B5,
    LOAD_ASSETS_AS_ALL,
} AssetLoadingColorMode;

#endif /* CONFIG_H */

/* why is this outside the header guards? address this when as add logging */
#define PRINTFLOGGING
