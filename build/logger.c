#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "logger.h"

static struct {
    FILE* file;
    char buf[LOG_BUFFER_SIZE];
    int idx;
    int init;
    int entryCount;
} logger = {0};

static char* levelStrs[] = {
    "Precision",
    "Suspicious",
    "Pathological",
    "Info"
};


int LoggerInit(char* filename){
    logger.file = fopen(filename, "wb");
    if (!logger.file) return -1;
    logger.idx = 0;
    logger.entryCount = 0;
    logger.init = 1;
    time_t now = time(0);
    char header[256];
    sprintf(header, "Start time: %s\n", ctime(&now));
    fwrite(header, strlen(header), 1, logger.file);
    return 0;
}

void LoggerFlush(){
    if (!logger.init || logger.idx == 0) return;

    fwrite(logger.buf, logger.idx, 1, logger.file);
    fflush(logger.file);
    
    logger.idx = 0;
}

void LoggerShutdown(){
    if (!logger.init) return;
    LoggerFlush();
    char footer[128];
    sprintf(footer, "Entries: %d. End of log\n", logger.entryCount);
    fwrite(footer, strlen(footer), 1, logger.file);

    fclose(logger.file);
    logger.init = 0;
}

void LogStr(LogLevel level, char* str){
    assert(logger.file);

    char entry[MAX_LOG_ENTRY_SIZE];
    int entryLen = sprintf(entry, "%d, %s, %s\n", logger.entryCount++,
            levelStrs[level], str);

    if (logger.idx + entryLen >= LOG_BUFFER_SIZE) LoggerFlush();
    memcpy(logger.buf + logger.idx, entry, entryLen);
    logger.idx += entryLen;
}

void LogNDCValidation(LogLevel level, float x, float y, float z, float error){
    assert(logger.file);

    char entry[MAX_LOG_ENTRY_SIZE];
    int entryLen = sprintf(entry, "%d, %s, %8.5f, %8.5f, %8.5f, %8.5f\n",
            logger.entryCount++, levelStrs[level], x, y, z, error);

    if (logger.idx + entryLen >= LOG_BUFFER_SIZE) LoggerFlush();
    
    memcpy(logger.buf + logger.idx, entry, entryLen);
    logger.idx += entryLen;
}
