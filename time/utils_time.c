#include "utils_time.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
//#include <sys/time.h>

/**********************************************************************
 Time retrieval functions
***********************************************************************/

static int get_utc_time(time_t t, struct tm *t_tm)
{
    if (gmtime_r(&t, t_tm) == NULL) {
        int status = errno;
        printf("get_utc_time: gmtime_r failed: %s", strerror(status));
        return status;
    }

    return 0;
}

static int get_local_time(time_t t, struct tm *t_tm)
{
    if (localtime_r(&t, t_tm) == NULL) {
        int status = errno;
        printf("get_local_time: localtime_r failed: %s", strerror(status));
        return status;
    }

    return 0;
}

/**********************************************************************
 Formatting functions
***********************************************************************/

static const char zulu_zone[] = "Z";

/* format_zone reads time zone information from "extern long timezone", exported
 * by <time.h>, and formats it according to RFC 3339. This differs from
 * strftime()'s "%z" format by including a colon between hour and minute. */
static int format_zone(char *buffer, size_t buffer_size, struct tm const *tm)
{
    char tmp[7];
    size_t sz;

    if ((buffer == NULL) || (buffer_size < 7))
        return EINVAL;

    sz = strftime(tmp, sizeof(tmp), "%z", tm);
    if (sz == 0)
        return ENOMEM;
    if (sz != 5) {
        printf("format_zone: strftime(\"%%z\") = \"%s\", want \"+hhmm\"", tmp);
        strncpy(buffer, tmp, buffer_size);
        return 0;
    }

    buffer[0] = tmp[0];
    buffer[1] = tmp[1];
    buffer[2] = tmp[2];
    buffer[3] = ':';
    buffer[4] = tmp[3];
    buffer[5] = tmp[4];
    buffer[6] = 0;

    return 0;
}

int format_rfc3339(char *buffer, size_t buffer_size, struct tm const *t_tm, char const *zone)
{
    int len;
    char *pos = buffer;
    size_t size_left = buffer_size;

    if ((len = strftime(pos, size_left, "%Y-%m-%dT%H:%M:%S", t_tm)) == 0)
        return ENOMEM;
    pos += len;
    size_left -= len;

    strncpy(pos, zone, size_left);
    return 0;
}

int format_rfc3339_utc(char *buffer, size_t buffer_size, time_t t)
{
    struct tm t_tm;
    int status;

    if ((status = get_utc_time(t, &t_tm)) != 0)
        return status; /* The error should have already be reported. */

    return format_rfc3339(buffer, buffer_size, &t_tm, zulu_zone);
}

int format_rfc3339_local(char *buffer, size_t buffer_size, time_t t)
{
    struct tm t_tm;
    int status;
    char zone[7]; /* +00:00 */

    if ((status = get_local_time(t, &t_tm)) != 0)
        return status; /* The error should have already be reported. */

    if ((status = format_zone(zone, sizeof(zone), &t_tm)) != 0)
        return status;

    return format_rfc3339(buffer, buffer_size, &t_tm, zone);
}

/**********************************************************************
 Public functions
***********************************************************************/

int rfc3339(char *buffer, size_t buffer_size, time_t t)
{
    if (buffer_size < RFC3339_SIZE)
        return ENOMEM;

    return format_rfc3339_utc(buffer, buffer_size, t);
}

int rfc3339_local(char *buffer, size_t buffer_size, time_t t)
{
    if (buffer_size < RFC3339_SIZE)
        return ENOMEM;

    return format_rfc3339_local(buffer, buffer_size, t);
}

// vim: set ts=4 sw=4 sts=4 et:
