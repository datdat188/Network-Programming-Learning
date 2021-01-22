#include "utils.h"
#include "time.h"

#define LOG_TIME_FORMAT     "%4d/%02d/%02d %02d:%02d:%02d"
#define LOG_TIME_STR_LEN    sizeof("1970/09/28 12:00:00")

//void log_open();

void log_init();

//char* log(char* time, char* ip, int port, char* ws_ip);

void log_write_msg( char* msg);

//void log_terminal(char* time, char* ip, int port);

//void log_close();

static char cached_log_time[LOG_TIME_STR_LEN];

static void timer_handler(void);