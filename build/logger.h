#ifndef LOGGER_H
#define LOGGER_H

#include "stdio.h"
#include "time.h"

#define LOG_BUFFER_SIZE 8192
#define MAX_LOG_ENTRY_SIZE 512

/* Attention - when adding one to this, also add to the str array in logger.c */
typedef enum {
    LOG_ERROR,
    LOG_PRECISION,
    LOG_SUSPICIOUS,
    LOG_PATHOLOGICAL,
    LOG_INFO,
    LOG_WARNING,
} LogLevel;

int  LoggerInit      (char* filename);
void LoggerFlush     ();
void LoggerShutdown  ();
/*
 * LogStr - General purpose string logging function.
 *
 * Logs 'str' at specified 'level' to the log file. Strings longer than
 * MAX_LOG_ENTRY_SIZE are truncated with a warning logged first.
 * Output is buffered; call LoggerFlush to force immediate write.
 *
 * Not optimized for high-frequency logging, for performance critical code,
 * consider specialized logging functions.
 */
void LogStr          (LogLevel level, char* str);
/* special single use debug functions */
void LogNDCValidation(LogLevel level, float x, float y, float z, float error);
    
#endif /* LOGGER_H */
