#include "utils.h"

#define	SA	struct sockaddr
#define	LISTENQ	1024
#define MAXEVENTS 64


void startServer(char*, int);
static int sock_set_non_blocking(int fd);