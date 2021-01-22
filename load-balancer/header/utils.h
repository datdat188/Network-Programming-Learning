#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>
#include <syslog.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <netdb.h>

#pragma once

#define PORT_SERVER 18899
#define PORT_NETD	18900

#define	WAIT_TIME 103
#define BUFFER_MEMORY_REGULATIONS 100
#define	PATH_NAME "utils.h"
#define failedCase -1
#define activeSession 1

#define PROJECT_ID_SERVER1 188
#define	PROJECT_ID_SERVER2 99
#define MAX_CONNECTION 4096   
#define PERMS 0644

#define LOCALHOST "127.0.0.1"

#define createSocketFailedCase "Socket creation failed"
#define connectSocketFailedCase "Connection failed"
#define sendMessageSocketFailedCase "Send socket failed"
#define receiveMessageSocketFailedCase "Receive socket failed"
#define ipInitializedFailedCase "IP not initialized failed"
#define endConversationWithServerWord "End"
#define bindSocketFailedCase "Socket bind failed"
typedef	void	Sigfunc(int);

struct msg
{
    long msg_type;
    char mgs[BUFFER_MEMORY_REGULATIONS];
};



void sig_chld(int signo);
void handleError(const char* caseError);
Sigfunc *Signal(int signo, Sigfunc *func);