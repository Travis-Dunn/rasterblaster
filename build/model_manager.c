#include "assert.h"
#include "model_manager.h"
#include "logger.h"

#define MAX_FILENAME        64
#define MODELS_DIR          "models"

/* return codes */
#define SUCCESS              0
#define NO_FILES_FOUND      -1
#define ALLOCATION_FAILURE  -2
#define FAILURE             -3

typedef enum { /* add here */
    TYPE_COL,
    TYPE_TEX_STUB,
    TYPE_COUNT,
} ModelType;

#define MODEL_TYPE_NAME_MAX_SIZE 32

#define MCOL_TYPE_NAME      "ModelCol" /* add here */
#define MTEX_STUB_TYPE_NAME "ModelTex_stub"

static char* modelTypeNames[TYPE_COUNT] = { /* add here */
    MCOL_TYPE_NAME,
    MTEX_STUB_TYPE_NAME,
};

static char ModelTypeNameSizeCheck[
    (sizeof(MCOL_TYPE_NAME) <= MODEL_TYPE_NAME_MAX_SIZE && /* add here */
     sizeof(MTEX_STUB_TYPE_NAME) <= MODEL_TYPE_NAME_MAX_SIZE) ? 1 : -1
];

static int modelManagerInit = 0;
static AssetLoadingColorMode assetColorMode;

static ModelCol* mColArr; /* add here */
static int mColCount;
static char (*mColFilenames)[MAX_FILENAME];

static ModelTex_stub* mTex_stubArr;
static int mTex_stubCount;
static char (*mTex_stubFilenames)[MAX_FILENAME];

static ModelType DetermineModelType(char* filename) { /* update this */
    FILE* file = fopen(filename, "r");
    if (!file)  { return TYPE_COUNT; } /* treat as error */

    char line[256];
    int hasColor = 0;
    int hasUV = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "property uchar red", 18) == 0 ||
                strncmp(line, "property uint red", 17) == 0) {
            hasColor = 1;
        }
        if (strncmp(line, "property float s", 16) == 0 ||
                strncmp(line, "property float u", 16) == 0) {
            hasUV = 1;
        }
        if (strncmp(line, "end_header", 10) == 0) break;
    }
    fclose(file);

    if (hasColor && !hasUV) return TYPE_COL;
    if (hasUV && !hasColor) return TYPE_TEX_STUB;

    return TYPE_COUNT; /* treat as error */
}

static inline int HandleIdxBoundsError_(int idx, int count, char* cFileName,
        ModelType type) {
    assert(cFileName);
    if (idx < count) return SUCCESS;
    char str[MAX_LOG_ENTRY_SIZE]; /* macro from logger.h */
    sprintf(str, "Model manager: Out of bounds index [%d], count [%d],"
            "ModelType [%d], filename [%s]\n", idx, count, type, cFileName);
    LogStr(LOG_ERROR, str);
    return FAILURE;
}

static inline void HandleLoadResult_(int ret, char* filename, int* idx,
        char (*filenames)[MAX_FILENAME], ModelType type) {
    assert(filename);
    assert(idx);
    assert(filenames);
    assert(type >= 0);
    assert(type < sizeof(modelTypeNames) / sizeof(modelTypeNames[0]));

    char* modelTypeName = modelTypeNames[type];
    char str[MAX_LOG_ENTRY_SIZE]; /* macro from logger.h */

    if (ret) {
        sprintf(str, "Error loading [%s] as [%s]: [%d]\n", filename,
                modelTypeName, ret);
        LogStr(LOG_ERROR, str);
    } else {
        strncpy(filenames[*idx], filename, MAX_FILENAME - 1);
        filenames[(*idx)++][MAX_FILENAME - 1] = '\0';
        sprintf(str, "Model manager loaded [%s] as '%s'\n", filename,
                modelTypeName);
        LogStr(LOG_INFO, str);
    }
}

int ModelManagerInit(AssetLoadingColorMode acm) { /* update this too */
    assert(!modelManagerInit);

    assetColorMode = acm;

    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char searchPath[MAX_PATH];

    mColCount = 0;
    mTex_stubCount = 0;

    sprintf(searchPath, "%s\\*.ply", MODELS_DIR);

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return NO_FILES_FOUND;

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char fullPath[MAX_PATH];
            sprintf(fullPath, "%s\\%s", MODELS_DIR, findData.cFileName);

            ModelType type = DetermineModelType(fullPath);
            if (type == TYPE_COL) mColCount++;
            else if (type == TYPE_TEX_STUB) mTex_stubCount++;
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    
    mColArr = (ModelCol*)malloc(mColCount * sizeof(ModelCol));
    if (!mColArr) return ALLOCATION_FAILURE;
    mColFilenames = malloc(mColCount * MAX_FILENAME);
    if (!mColFilenames) {
        free(mColArr);
        return ALLOCATION_FAILURE;
    }
    mTex_stubArr =
        (ModelTex_stub*)malloc(mTex_stubCount * sizeof(ModelTex_stub));
    if (!mTex_stubArr) {
        free(mColArr);
        free(mColFilenames);
        return ALLOCATION_FAILURE;
    }
    mTex_stubFilenames = malloc(mTex_stubCount * MAX_FILENAME);
    if (!mTex_stubFilenames) {
        free(mColArr);
        free(mColFilenames);
        free(mTex_stubArr);
        return ALLOCATION_FAILURE;
    }

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return NO_FILES_FOUND;

    int mColIdx = 0;
    int mTex_stubIdx = 0;

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char fullPath[MAX_PATH];
            sprintf(fullPath, "%s\\%s", MODELS_DIR, findData.cFileName);

            ModelType type = DetermineModelType(fullPath);
            switch (type) {
            case TYPE_COL: {
                if (HandleIdxBoundsError_(mColIdx, mColCount,
                    findData.cFileName, type)) break;
                int ret = ModelColLoadPLY(&mColArr[mColIdx], fullPath);
                HandleLoadResult_(ret, findData.cFileName, &mColIdx,
                        mColFilenames, TYPE_COL);
            } break;
            case TYPE_TEX_STUB: {
                if (HandleIdxBoundsError_(mTex_stubIdx, mTex_stubCount,
                    findData.cFileName, type)) break;
 
                int ret = ModelTex_stubLoadPLY(&mTex_stubArr[mTex_stubIdx],
                        fullPath);
                if (ret) {
                    char str[128];
                    sprintf(str,
                            "Error loading .ply file as 'ModelTex_stub': %d\n",
                            ret);
                    LogStr(LOG_ERROR, str);
                } else {
                    strncpy(mTex_stubFilenames[mTex_stubIdx],
                            findData.cFileName, MAX_FILENAME - 1);
                    mTex_stubFilenames[mTex_stubIdx++][MAX_FILENAME - 1] =
                        '\0';
                    char str[128];
                    sprintf(str,
                            "Model manager loaded [%s] as 'ModelTex_stub'\n",
                            findData.cFileName);
                    LogStr(LOG_INFO, str);
                }
            } break;
            }
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    modelManagerInit = 1; 
    return 0;
}
/* lastly, add a getter */
ModelCol* GetModelCol(char* filename) {
    for (int i = 0; i < mColCount; i++) {
        if (strcmp(mColFilenames[i], filename) == 0) {
            return &mColArr[i];
        }
    }
    return 0;
}

ModelTex_stub* GetModelTex_stub(char* filename) {
    for (int i = 0; i < mTex_stubCount; i++) {
        if (strcmp(mTex_stubFilenames[i], filename) == 0) {
            return &mTex_stubArr[i];
        }
    }
    return 0;
}
