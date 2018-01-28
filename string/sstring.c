#include "sstring.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

char *sstrncpy(char *dest, const char *src, size_t n)
{
    strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return (dest);
}

#if 0
int ssnprintf(char *dest, size_t n, const char *format, ...)
{
    int ret = 0;
    va_list ap;

    va_start(ap, format);
    ret = vsnprintf(dest, n, format, ap);
    dest[n - 1] = '\0';
    va_end(ap);

    return (ret);
}
#endif

char *ssnprintf_alloc(char const *format, ...)
{
    char static_buffer[1024] = "";
    char *alloc_buffer = NULL;
    size_t alloc_buffer_size;
    int status;
    va_list ap;

    /* Try printing into the static buffer. In many cases it will be
     * sufficiently large and we can simply return a strdup() of this
     * buffer. */
    va_start(ap, format);
    status = vsnprintf(static_buffer, sizeof(static_buffer), format, ap);
    va_end(ap);
    if (status < 0)
        return (NULL);

    /* "status" does not include the null byte. */
    alloc_buffer_size = (size_t)(status + 1);
    if (alloc_buffer_size <= sizeof(static_buffer))
        return (strdup(static_buffer));

    /* Allocate a buffer large enough to hold the string. */
    alloc_buffer = calloc(1, alloc_buffer_size);
    if (alloc_buffer == NULL)
        return (NULL);

    /* Print again into this new buffer. */
    va_start(ap, format);
    status = vsnprintf(alloc_buffer, alloc_buffer_size, format, ap);
    va_end(ap);
    if (status < 0) {
        free(alloc_buffer);
        return (NULL);
    }

    return (alloc_buffer);
}

char *sstrdup(const char *s)
{
    char *r;
    size_t sz;

    if (s == NULL)
        return (NULL);

    /* Do not use `strdup' here, because it's not specified in POSIX. It's
     * ``only'' an XSI extension. */
    sz = strlen(s) + 1;
    r = malloc(sz);
    if (r == NULL) {
        printf("sstrdup: Out of memory.");
        exit(3);
    }
    memcpy(r, s, sizeof(char) * sz);

    return (r);
}

// delim = " \t\r\n"
int strsplit(char *string, char **fields, size_t size, const char *delim)
{
    size_t i = 0;
    char *saveptr = NULL;
    char *ptr = string;

    while ((fields[i] = strtok_r(ptr, delim, &saveptr)) != NULL) {
        ptr = NULL;
        i++;

        if (i >= size)
            break;
    }

    return ((int)i);
}

int strsplit_alloc(char *string, const char *delim, char ***fileds, size_t *size)
{
    size_t item_size = 0;
    char **items = NULL;
    char *saveptr = NULL;
    char *field = NULL;
    char *ptr = string;


    while ((field = strtok_r(ptr, delim, &saveptr)) != NULL) {
        ptr = NULL;

        if (strarray_add(&items, &item_size, field) != 0) {
            strarray_free(items, item_size);
            return -1;
        }
    }

    *fileds = items;
    *size = item_size;
    return 0;
}

int strjoin(char *buffer, size_t buffer_size,
            char **fields, size_t fields_num, const char *sep)
{
    size_t avail = 0;
    char *ptr = buffer;
    size_t sep_len = 0;

    size_t buffer_req = 0;

    if (((fields_num != 0) && (fields == NULL)) ||
            ((buffer_size != 0) && (buffer == NULL)))
        return (-EINVAL);

    if (buffer != NULL)
        buffer[0] = 0;

    if (buffer_size != 0)
        avail = buffer_size - 1;

    if (sep != NULL)
        sep_len = strlen(sep);

    for (size_t i = 0; i < fields_num; i++) {
        size_t field_len = strlen(fields[i]);

        if (i != 0)
            buffer_req += sep_len;
        buffer_req += field_len;

        if ((i != 0) && (sep_len > 0)) {
            if (sep_len >= avail) {
                /* prevent subsequent iterations from writing to the
                 * buffer. */
                avail = 0;
                continue;
            }

            memcpy(ptr, sep, sep_len);

            ptr += sep_len;
            avail -= sep_len;
        }

        if (field_len > avail)
            field_len = avail;

        memcpy(ptr, fields[i], field_len);
        ptr += field_len;

        avail -= field_len;
        if (ptr != NULL)
            *ptr = 0;
    }

    return (int)buffer_req;
}

int escape_string(char *buffer, size_t buffer_size)
{
    char *temp;
    size_t j;

    /* Check if we need to escape at all first */
    temp = strpbrk(buffer, " \t\"\\");
    if (temp == NULL)
        return (0);

    if (buffer_size < 3)
        return (EINVAL);

    temp = calloc(1, buffer_size);
    if (temp == NULL)
        return (ENOMEM);

    temp[0] = '"';
    j = 1;

    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == 0) {
            break;
        } else if ((buffer[i] == '"') || (buffer[i] == '\\')) {
            if (j > (buffer_size - 4))
                break;
            temp[j] = '\\';
            temp[j + 1] = buffer[i];
            j += 2;
        } else {
            if (j > (buffer_size - 3))
                break;
            temp[j] = buffer[i];
            j++;
        }
    }

    assert((j + 1) < buffer_size);
    temp[j] = '"';
    temp[j + 1] = 0;

    sstrncpy(buffer, temp, buffer_size);
    free(temp);
    return (0);
}

int strunescape(char *buf, size_t buf_len)
{
    for (size_t i = 0; (i < buf_len) && (buf[i] != '\0'); ++i) {
        if (buf[i] != '\\')
            continue;

        if (((i + 1) >= buf_len) || (buf[i + 1] == 0)) {
            printf("string unescape: backslash found at end of string.");
            /* Ensure null-byte at the end of the buffer. */
            buf[i] = 0;
            return (-1);
        }

        switch (buf[i + 1]) {
        case 't':
            buf[i] = '\t';
            break;
        case 'n':
            buf[i] = '\n';
            break;
        case 'r':
            buf[i] = '\r';
            break;
        default:
            buf[i] = buf[i + 1];
            break;
        }

        /* Move everything after the position one position to the left.
         * Add a null-byte as last character in the buffer. */
        memmove(buf + i + 1, buf + i + 2, buf_len - i - 2);
        buf[buf_len - 1] = 0;
    }
    return (0);
}

int sstrtoll(const char *string, long long int *ret_value)
{
    long long int tmp;
    char *endptr;

    if ((string == NULL) || (ret_value == NULL))
        return (EINVAL);

    errno = 0;
    endptr = NULL;
    tmp = (long long int)strtoll(string, &endptr, /* base = */ 0);
    if ((endptr == string) || (errno != 0))
        return (-1);

    *ret_value = tmp;
    return (0);
}

int sstrtod(const char *string, double *ret_value)
{
    double tmp;
    char *endptr = NULL;

    if ((string == NULL) || (ret_value == NULL))
        return (EINVAL);

    errno = 0;
    endptr = NULL;
    tmp = (double)strtod(string, &endptr);
    if (errno != 0)
        return (errno);
    else if ((endptr == NULL) || (*endptr != 0))
        return (EINVAL);

    *ret_value = tmp;
    return (0);
}

int strarray_add(char ***ret_array, size_t *ret_array_len, char const *str)
{
    char **array;
    size_t array_len = *ret_array_len;

    if (str == NULL)
        return (EINVAL);

    array = realloc(*ret_array, (array_len + 1) * sizeof(*array));
    if (array == NULL)
        return (ENOMEM);
    *ret_array = array;

    array[array_len] = strdup(str);
    if (array[array_len] == NULL)
        return (ENOMEM);

    array_len++;
    *ret_array_len = array_len;
    return (0);
}

void strarray_free(char **array, size_t array_len)
{
    for (size_t i = 0; i < array_len; i++)
        free(array[i]);
    free(array);
    array = NULL;
}

