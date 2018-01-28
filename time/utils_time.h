#ifndef UTILS_TIME_H
#define UTILS_TIME_H

#include <time.h>
#include <sys/time.h>


#define RFC3339_SIZE 26     /* 2006-01-02T15:04:05+00:00 */

/* rfc3339 formats a cdtime_t time as UTC in RFC 3339 zulu format with second
 * precision, e.g., "2006-01-02T15:04:05Z". */
int rfc3339(char *buffer, size_t buffer_size, time_t t);

/* rfc3339 formats a cdtime_t time as local in RFC 3339 format with second
 * precision, e.g., "2006-01-02T15:04:05+00:00". */
int rfc3339_local(char *buffer, size_t buffer_size, time_t t);


int timeval_cmp(struct timeval tv0, struct timeval tv1, struct timeval *delta);

/* make sure tv_usec stores less than a second */
#define NORMALIZE_TIMEVAL(tv)                                                  \
  do {                                                                         \
    (tv).tv_sec += (tv).tv_usec / 1000000;                                     \
    (tv).tv_usec = (tv).tv_usec % 1000000;                                     \
  } while (0)

/* make sure tv_sec stores less than a second */
#define NORMALIZE_TIMESPEC(tv)                                                 \
  do {                                                                         \
    (tv).tv_sec += (tv).tv_nsec / 1000000000;                                  \
    (tv).tv_nsec = (tv).tv_nsec % 1000000000;                                  \
  } while (0)

#endif /* UTILS_TIME_H */
