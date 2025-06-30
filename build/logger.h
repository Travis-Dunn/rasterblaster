#ifndef LOGGER_H
#define LOGGER_H

#include "stdio.h"
#include "time.h"

#define LOG_BUFFER_SIZE 8192
#define MAX_LOG_ENTRY_SIZE 256

typedef enum {
    LOG_PRECISION,
    LOG_SUSPICIOUS,
    LOG_PATHOLOGICAL,
    LOG_INFO
} LogLevel;

int  LoggerInit      (char* filename);
void LoggerFlush     ();
void LoggerShutdown  ();
void LogStr          (LogLevel level, char* str);
/* special single use debug functions */
void LogNDCValidation(LogLevel level, float x, float y, float z, float error);
    
#endif /* LOGGER_H */
