#include "utils.h"
#include "socket_utils.h"

#define MAXEVENTS 64

void startServer(char*, int);
static int sock_set_non_blocking(int fd);

void active_handler();
void standby_handler();

char* get_cookie_serverid(char* msg);
void set_cookie_serverid();

void proxy_forward_request(char* msg);