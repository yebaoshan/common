#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include<stdbool.h>

typedef enum
{
    E_LOG_LEVEL_CRITICAL = 1,
    E_LOG_LEVEL_ERROR,
    E_LOG_LEVEL_WARNING,
    E_LOG_LEVEL_INFO,
    E_LOG_LEVEL_DEBUG,
} ELogLevel;

#define CRITICAL(...) log_write(E_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define ERROR(...) log_write(E_LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define WARNING(...) log_write(E_LOG_LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define INFO(...) log_write(E_LOG_LEVEL_INFO, __FILE__, __LINE__,  __FUNCTION__, __VA_ARGS__)
#define DEBUG(...) log_write(E_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

int log_init();
int log_shutdown();

int set_log_level(ELogLevel level);
int set_log_dir(const char *dir);
int set_log_to_file(_Bool enable);

int log_write(int level, const char *file, int line, const char *function,
              const char *format, ...) __attribute__((format(printf, 5, 6)));

#endif /* !UTILS_LOG_H */
