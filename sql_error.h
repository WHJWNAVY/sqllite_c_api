#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum _error_e {
    E_OK = 0,

    E_BAD_REQ,
    E_ROUTER_IO,
    E_ERROR_NUM
} error_t;

#define SQL_DEBUG_TO_FILE(file, ...)                             \
    do {                                                         \
        FILE *__SQL_DBG_FD__ = fopen(file, "a+");                \
        if (__SQL_DBG_FD__ == NULL)                              \
            __SQL_DBG_FD__ = stderr;                             \
        fprintf(__SQL_DBG_FD__, "(%s:%d) ", __func__, __LINE__); \
        fprintf(__SQL_DBG_FD__, __VA_ARGS__);                    \
        fprintf(__SQL_DBG_FD__, "\n");                           \
        if (__SQL_DBG_FD__ != stderr)                            \
            fclose(__SQL_DBG_FD__);                              \
    } while (0)

#define SQL_DEBUG(...)                        \
    do {                                      \
        SQL_DEBUG_TO_FILE(NULL, __VA_ARGS__); \
    } while (0)

#define SQL_ERROR SQL_DEBUG

#endif