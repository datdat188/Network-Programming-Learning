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

#define PORT_SERVER 18899
#define PORT_NETD	18900

#define	WAIT_TIME 10
#define BUFFER_MEMORY_REGULATIONS 100
#define	PATH_NAME "utils.h"
#define failedCase -1
#define activeSession 1

#define PROJECT_ID_SERVER1 188
#define	PROJECT_ID_SERVER2 99
#define MAX_CONNECTION 10
#define	SA	struct sockaddr
#define	LISTENQ	1024
#define MAXEVENTS 64
#define PERMS 0644

#define LOCALHOST "127.0.0.1"

#define createSocketFailedCase "Socket creation failed"
#define connectSocketFailedCase "Connection failed"
#define sendMessageSocketFailedCase "Send socket failed"
#define receiveMessageSocketFailedCase "Receive socket failed"
#define ipInitializedFailedCase "IP not initialized failed"
#define endConversationWithServerWord "End"
#define bindSocketFailedCase "Socket bind failed"
#define portProxyInvalidCase "Invalid port"
typedef	void	Sigfunc(int);

struct msg
{
    long msg_type;
    char mgs[BUFFER_MEMORY_REGULATIONS];
};


Sigfunc *
Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		error("signal error");
	return(sigfunc);
}

void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated\n", pid);
	}
	return;
}

void 
handleError(const char* caseError){
	//caseError = strcat("[#] Client: ",caseError);
	perror(caseError);
	exit(1);
}

int 
endConversationWithServer(char* message){
	if (strcmp(message, (char*)endConversationWithServerWord) == 0)
	{
		return 1;
	}
	return 0;
}

int 
messageQueueGet(key_t key)
{
	int msgid;
	if ((msgid = msgget(key, PERMS | IPC_CREAT)) == -1)
		return -1;

	return msgid;
}

static int sock_set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    int s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int
Socket(int family, int type, int protocol)
{
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		handleError(createSocketFailedCase);
	return(n);
}
/* end Socket */

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		handleError(bindSocketFailedCase);
}

/* include Listen */
void
Listen(int fd, int backlog)
{
	char	*ptr;

		/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		handleError("listen error");
}
/* end Listen */

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int		n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			handleError("accept error");
	}
	return(n);
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		handleError(connectSocketFailedCase);
}

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t		n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
		handleError(receiveMessageSocketFailedCase);
	return(n);
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		handleError(sendMessageSocketFailedCase);
}

void
Close(int fd)
{
	if (close(fd) == -1)
		handleError("close error");
}