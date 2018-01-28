#ifndef SSTRING_H
#define SSTRING_H

#include <stddef.h>

int strarray_add(char ***ret_array, size_t *ret_array_len, char const *str);
void strarray_free(char **array, size_t array_len);

char *sstrncpy(char *dest, const char *src, size_t n);
char *ssnprintf_alloc(char const *format, ...);
char *sstrdup(const char *s);

int strsplit(char *string, char **fields, size_t size, const char *delim);
int strsplit_alloc(char *string, const char *delim, char ***fileds, size_t *size);
int strjoin(char *buffer, size_t buffer_size,
            char **fields, size_t fields_num, const char *sep);

int escape_string(char *buffer, size_t buffer_size);
int strunescape(char *buf, size_t buf_len);

int sstrtoll(const char *string, long long int *ret_value);
int sstrtod(const char *string, double *ret_value);

#endif /* SSTRING_H */
