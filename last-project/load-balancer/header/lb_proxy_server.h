#include "utils.h"
#include "socket_utils.h"

#define MAXEVENTS 64

void startServer(char*, int);
static int sock_set_non_blocking(int fd);