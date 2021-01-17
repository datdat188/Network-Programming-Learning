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

#define	WAIT_TIME 103
#define BUFFER_MEMORY_REGULATIONS 100
#define	PATH_NAME "utils.h"
#define failedCase -1
#define activeSession 1

#define PROJECT_ID_SERVER1 188
#define	PROJECT_ID_SERVER2 99
#define MAX_CONNECTION 10
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
Sigfunc *
Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		handleError("signal error");
	return(sigfunc);
}

int
createSocket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}

char*
upcase(char *str)
{
	for (int i = 0; str[i]!='\0'; i++) 
	{
   		if(str[i] >= 'a' && str[i] <= 'z') 
		{
      		str[i] = str[i] -32;
		}
	}
	return str;
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