#include "header/log.h"
#include <string>

using namespace std;

void log_init()
{
    FILE* logfd = fopen("log/log.txt", "w");
    if (logfd != NULL)
    {
        fputs ("=========== Server Log ===============\n",logfd);
    }
    fclose(logfd);
}

void log_write_msg(char* msg){
    FILE* logfd = fopen("log/log.txt", "w");
    if (logfd != NULL)
    {
        fputs (msg,logfd);
    }
    fclose(logfd);
}

static void timer_handler(void)
{
    static time_t   last_time;
    time_t          now = time(NULL);
    struct tm      *tm;

    tm = localtime(&now);
    sprintf(cached_log_time, LOG_TIME_FORMAT,
            tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
}