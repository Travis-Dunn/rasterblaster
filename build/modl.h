#ifndef MODL_H
#define MODL_H

#include "model_types.h"

typedef enum {
    MODL_TREE,
    MODL_BARREL,
    MODL_COUNT
} ModlEntry;

/* for use with 'flags' */
#define FLAG_INIT           0x1
#define FLAG_MTYPE_COL      0x0
#define FLAG_MTYPE_TEX      0x2
#define FLAG_MTYPE_2        0x4     /* 2 through 7 not yet in use */
#define FLAG_MTYPE_3        0x6
#define FLAG_MTYPE_4        0x8
#define FLAG_MTYPE_5        0xA
#define FLAG_MTYPE_6        0xC
#define FLAG_MTYPE_7        0xE
#define FLAG_MTYPE_MASK     0xE

typedef struct {
    int flags;
    union {
        ModelCol* mCol;
        ModelTex* mTex;
    } m;
} Modl;

void ModlInit();
Modl ModlGet(ModlEntry e);

#endif /* MODL_H */
