#include "modl.h"
#include "model_manager.h"

static Modl modls[MODL_COUNT];
static Modl nullModl = {0};

void ModlInit() {
    modls[MODL_TREE].flags = FLAG_INIT | FLAG_MTYPE_COL;
    modls[MODL_TREE].m.mCol = GetModelCol("tree0_experimental.ply");
}

Modl ModlGet(ModlEntry e) {
    if (e >= 0 && e < MODL_COUNT) {
        return modls[e];
    } return nullModl;
}
