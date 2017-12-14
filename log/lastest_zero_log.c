#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>


#define LOG_SIZE 512000
#define LOG_NUMBER 5
#define LOG_NAME_LEN 256

static char (*log_names_g)[LOG_NAME_LEN] = NULL;
static char log_dir_g[LOG_NAME_LEN] = "log";
static ELogLevel log_level_g = E_LOG_LEVEL_ERROR;
static _Bool log_to_file_g = 1;
static FILE *log_fp_g = NULL;
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;


static int file_size(FILE *fp)
{
    int pos, len;

    pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, pos, SEEK_SET);

    return len;
}

static int insure_dir(const char *dir)
{
    errno = 0;
    struct stat stat_buf = {0};
    int status = stat(dir, &stat_buf);
    if (status != 0) {
      if (errno != ENOENT)
        return -1;

      if (mkdir(dir, 0600) != 0)
        return -2;
    } else if (!S_ISDIR(stat_buf.st_mode)) {
        return -3;
    }

    return 0;
}
static int get_oldest_logfile_index()
{
    if (log_names_g == NULL) {
        printf("Can't get logfile, log_names_g is null");
        return -1;
    }

    struct stat log_stat;
    int log_idx = -1;
    for (int i = 0; i < LOG_NUMBER; i++) {
        struct stat stat_buf;

        const char *name = (char *)(log_names_g + i);
        int ret = stat(name, &stat_buf);
        if (ret == -1 && errno == ENOENT) {
            log_idx = i;
            break;
        }

        if (log_idx == -1) {
            log_idx = i;
            log_stat = stat_buf;
            continue;
        }

        if (log_stat.st_mtime > stat_buf.st_mtime) {
            log_stat = stat_buf;
            log_idx = i;
        }
    }

    if (log_idx == -1) {
        printf("Can't find oldest logfile");
        return -1;
    }

    return log_idx;
}

int set_log_level(ELogLevel level)
{
    log_level_g = level;
    return 0;
}

int set_log_dir(const char *dir)
{
    strncpy(log_dir_g, dir, sizeof(log_dir_g));
    log_dir_g[sizeof(log_dir_g) - 1] = '\0';
    return 0;
}

int set_log_to_file(_Bool enable)
{
    log_to_file_g = enable;
    return 0;
}

int log_init()
{
    int slash = log_dir_g[strlen(log_dir_g)] == '/' ? 1 : 0;
    if (log_names_g != NULL)
        free(log_names_g);

    log_names_g = (char(*)[LOG_NAME_LEN]) calloc(LOG_NUMBER, LOG_NAME_LEN);
    if (log_names_g == NULL) {
        printf("Failed to calloc log file");
        return -1;
    }

    int ret = 0;
    if ((ret = insure_dir(log_dir_g)) != 0) {
      printf("(%s) is invaild dir. ret:%d, errstr:%s",
             log_dir_g, ret, strerror(errno));
      return -1;
    }

    for (int i = 0; i < LOG_NUMBER; i++) {
        char *name = (char *)(log_names_g + i);
        if(slash == 1)
            sprintf(name, "%slogger%d.log", log_dir_g, i);
        else
            sprintf(name, "%s/logger%d.log", log_dir_g, i);
    }

    return 0;
}

int log_shutdown()
{
    if (log_fp_g != NULL) {
        fclose(log_fp_g);
        log_fp_g = NULL;
    }

    if (log_names_g != NULL) {
        free(log_names_g);
        log_names_g = NULL;
    }

    return 0;
}

int log_write(int level, const char *file, int line,
              const char *function, const char *format, ...)
{
    if (level > log_level_g)
        return 0;

    char msg[1024];
    va_list ap;
    va_start(ap, format);
    vsnprintf(msg, sizeof(msg), format, ap);
    msg[sizeof(msg) - 1] = '\0';
    va_end(ap);

    char detail_msg[1024] = {0};
    snprintf(detail_msg, sizeof(detail_msg), "[%s:%d][%s] %s\n",
             file, line, function,  msg);
    printf("%s", detail_msg);

    if (log_to_file_g == 0)
        return 0;

    pthread_mutex_lock(&log_lock);
    if (log_fp_g == NULL && (log_fp_g = fopen(log_names_g[0], "a+")) == NULL) {
        printf("Failed to open file:%s, errno:%d, strerr:%s", log_names_g[0], errno, strerror(errno));
        pthread_mutex_unlock(&log_lock);
        return -1;
    }

    if (file_size(log_fp_g) >= LOG_SIZE) {
        int index = get_oldest_logfile_index();
        if (index < 0) {
            pthread_mutex_unlock(&log_lock);
            return -1;
        }

        fclose(log_fp_g);
        int ret = rename(log_names_g[0], log_names_g[index]);
        if (ret != 0) {
            printf("Failed to rename log:%s, to index:%d", log_names_g[0], index);
            pthread_mutex_unlock(&log_lock);
            return -1;
        }

        log_fp_g = fopen(log_names_g[0], "w+");
    }

    if (log_fp_g == NULL) {
        printf("Failed to open log file:%s, errno:%d", log_names_g[0], errno);
        pthread_mutex_unlock(&log_lock);
        return -1;
    }

    fputs(detail_msg, log_fp_g);
    fflush(log_fp_g);

    pthread_mutex_unlock(&log_lock);
    return 0;
}
