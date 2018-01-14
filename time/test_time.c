#include <time.h>
#include <stdio.h>

#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "utils_time.h"

void test_time()
{
    time_t seconds;
    seconds = time((time_t *)NULL);
    printf("========%s========\n", __FUNCTION__);
    printf("time:%lu\n", seconds);
}

// char *ctime_r(const time_t *timep, char *buf);
void test_ctime()
{
    time_t timep;
    timep = time(NULL);
    printf("========%s========\n", __FUNCTION__);
    printf("time:%s\n", ctime(&timep));
}

//struct tm *gmtime_r(const time_t *timep, struct tm *result);
void test_gmtime()
{
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t timep;
    struct tm *p_tm;
    timep = time(NULL);
    p_tm = gmtime(&timep); /*获取GMT时间*/
    printf("========%s========\n", __FUNCTION__);
    printf("%d-%d-%d ", (p_tm->tm_year+1900), (p_tm->tm_mon+1), p_tm->tm_mday);
    printf("%s %d:%d:%d", wday[p_tm->tm_wday], p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
    printf("    yday:%d, isdst:%d\n", p_tm->tm_yday, p_tm->tm_isdst);
}

// struct tm *localtime_r(const time_t *timep, struct tm *result);
void test_localtime()
{
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t timep;
    struct tm *p_tm;
    timep = time(NULL);
    p_tm = localtime(&timep); /*获取本地时区时间*/
    printf("========%s========\n", __FUNCTION__);
    printf("%d-%d-%d ", (p_tm->tm_year+1900), (p_tm->tm_mon+1), p_tm->tm_mday);
    printf("%s %d:%d:%d", wday[p_tm->tm_wday], p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
    printf("    yday:%d, isdst:%d\n", p_tm->tm_yday, p_tm->tm_isdst);
}

void test_mktime()
{
    time_t timep;
    struct tm *p_tm;
    timep = time(NULL);
    printf("========%s========\n", __FUNCTION__);
    printf("time( ):%lu\n", timep);
    p_tm = localtime(&timep);
    timep = mktime(p_tm);
    printf("time( )->localtime( )->mktime( ):%lu\n", timep);

}

// char *asctime_r(const struct tm *tm, char *buf);
void test_asctime()
{
    time_t timep;
    timep = time(NULL);
    printf("========%s========\n", __FUNCTION__);
    printf("%s\n", asctime(gmtime(&timep)));
}

void test_difftime()
{
    time_t timep1, timep2;
    timep1 = time(NULL);
    sleep(1);
    timep2 = time(NULL);
    printf("========%s========\n", __FUNCTION__);
    printf("the difference is %f seconds\n", difftime(timep1, timep2));
    printf("the difference is %f seconds\n", difftime(timep2, timep1));
}

void test_strftime()
{
    char strtime[30] = {0};
    time_t timep;
    struct tm *p_tm;
    timep = time(NULL);
    p_tm = localtime(&timep);
    printf("========%s========\n", __FUNCTION__);
    strftime(strtime, sizeof(strtime), "%Y-%m-%d %H:%M:%S %z", p_tm);
    printf("time:%s\n", strtime);
}

void test_strpftime()
{
    struct tm tm;
    char buf[255];
    memset(&tm, 0, sizeof(struct tm));
    strptime("2001-11-12 18:31:01", "%Y-%m-%d %H:%M:%S", &tm);
    printf("========%s========\n", __FUNCTION__);
    printf("%s\n", asctime(&tm));
}

void test_gettimeofday()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    printf("========%s========\n", __FUNCTION__);
    printf("tv_sec:%lu, tv_usec:%lu; tz_minuteswest:%d, tz_dsttime:%d\n",
            tv.tv_sec, tv.tv_usec, tz.tz_minuteswest, tz.tz_dsttime);

}

void test_settimeofday()
{
    char t_string[] = "2012-04-28 22:30:00";
    struct tm time_tm;
    struct timeval time_tv;
    time_t timep;
    int ret = 0;

    printf("========%s========\n", __FUNCTION__);
    sscanf(t_string, "%d-%d-%d %d:%d:%d", &time_tm.tm_year, &time_tm.tm_mon, &time_tm.tm_mday, &time_tm.tm_hour, &time_tm.tm_min, &time_tm.tm_sec);
    time_tm.tm_year -= 1900;
    time_tm.tm_mon -= 1;
    time_tm.tm_wday = 0;
    time_tm.tm_yday = 0;
    time_tm.tm_isdst = 0;

    timep = mktime(&time_tm);
    time_tv.tv_sec = timep;
    time_tv.tv_usec = 0;

    ret = settimeofday(&time_tv, NULL);
    if(ret != 0)
        printf("settimeofday failed. errno:%d, strerror:%s\n", errno, strerror(errno));
}

void test_clock()
{
    long i = 10000000L;
    clock_t start, finish;
    double TheTimes;
    printf("========%s========\n", __FUNCTION__);
    printf("做%ld次空循环需要的时间为",i);
    start = clock();
    while (i--);
    finish = clock();
    TheTimes = (double)(finish-start) / CLOCKS_PER_SEC;
    printf("%f秒。\n", TheTimes);
}

void test_tzset()
{
    time_t ret;
    char *tz;
    time_t td;

    tz = getenv("TZ");
    if (tz)
        tz = strdup(tz);
    setenv("TZ", "", 1);
    tzset();

    time(&td);
    printf("========%s========\n", __FUNCTION__);
    printf("Current time=%s",asctime(localtime(&td)));

    if (tz) {
        setenv("TZ", tz, 1);
        free(tz);
    } else
        unsetenv("TZ");
    tzset();

    time(&td);
    printf("Current time=%s",asctime(localtime(&td)));
}


// Test clock function begin
//int clock_getres(clockid_t clk_id, struct timespec *res);
//int clock_gettime(clockid_t clk_id, struct timespec *tp);
//int clock_settime(clockid_t clk_id, const struct timespec *tp);

static void ShowTime(const char* desc, const struct timespec* res)
{
     printf("%s: \t %lu\t %lu\n", desc, res->tv_sec, res->tv_nsec);
}

static void TestTime(clockid_t clk_id)
{
     struct timespec res;

     clock_getres(clk_id, &res);
     ShowTime("clock_getres", &res);

     clock_gettime(clk_id, &res);
     ShowTime("clock_gettime", &res);
}

void test_clock_function()
{
     printf("========%s========\n", __FUNCTION__);
     printf("CLOCK_REALTIME: \n");
     TestTime(CLOCK_REALTIME);

     printf("\n\nCLOCK_REALTIME_COARSE:\n");
     TestTime(CLOCK_REALTIME_COARSE);

     printf("\n\nCLOCK_MONOTONIC:\n");
     TestTime(CLOCK_MONOTONIC);

}

// Test clock function end

void test_utils_time()
{
    int ret = 0;
    char buffer[26] = {0};
    time_t timep = time(NULL);

    ret = rfc3339(buffer, sizeof(buffer), timep);
    if (ret == 0)
        printf("rfc3339:%s\t", buffer);
    else
        printf("rfc3339 error\t");

    ret = rfc3339_local(buffer, sizeof(buffer), timep);
    if (ret == 0)
        printf("rfc3339_local:%s\n", buffer);
    else
        printf("rfc3339_local error\n");
}


void main()
{
    test_time();
    test_ctime();
    test_gmtime();
    test_localtime();
    test_mktime();
    test_asctime();
    test_difftime();
    test_strftime();
    test_strpftime();

    test_gettimeofday();
    test_settimeofday();

    test_clock();
    test_tzset();
    test_clock_function();

    test_utils_time();

}




// vim: set ts=4 sw=4 sts=4 et:
