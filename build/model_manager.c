#include "assert.h"
#include "model_manager.h"
#include "logger.h"

#define MAX_FILENAME        64
#define MODELS_DIR          "models"

/* return codes */
#define NO_FILES_FOUND      -1
#define ALLOCATION_FAILURE  -2

/* TODO: LOGGING! More important than most places, errors loading files are a 
 * fact of life */

typedef enum {
    TYPE_COL,
    TYPE_TEX_STUB,
    TYPE_UNKNOWN,
} ModelType;

static int modelManagerInit = 0;

static ModelCol* mColArr;
static int mColCount;
static char (*mColFilenames)[MAX_FILENAME];

static ModelTex_stub* mTex_stubArr;
static int mTex_stubCount;
static char (*mTex_stubFilenames)[MAX_FILENAME];

static ModelType DetermineModelType(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return TYPE_UNKNOWN;

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

    return TYPE_UNKNOWN;
}

int ModelManagerInit() {
    assert(!modelManagerInit);
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
                if (mColIdx >= mColCount) {
                    char str[128];
                    sprintf(str,
                            "Model manager error: Out of bounds index [%d]\n",
                            mColIdx);
                    LogStr(LOG_ERROR, str);
                    break;
                }
                int ret = ModelColLoadPLY(&mColArr[mColIdx], fullPath);
                if (ret) {
                    char str[128];
                    sprintf(str, "Error loading .ply file as 'ModelCol': %d\n", ret);
                    LogStr(LOG_ERROR, str);
                } else {
                    strncpy(mColFilenames[mColIdx], findData.cFileName,
                            MAX_FILENAME - 1);
                    mColFilenames[mColIdx++][MAX_FILENAME - 1] = '\0';
                    char str[128];
                    sprintf(str, "Model manager loaded [%s] as 'ModelCol'\n",
                            findData.cFileName);
                    LogStr(LOG_INFO, str);
                }
            } break;
            case TYPE_TEX_STUB: {
                if (mTex_stubIdx >= mTex_stubCount) {
                    char str[128];
                    sprintf(str,
                            "Model manager error: Out of bounds index [%d]\n",
                            mTex_stubIdx);
                    LogStr(LOG_ERROR, str);
                    break; 
                }
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
