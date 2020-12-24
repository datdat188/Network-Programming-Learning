#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT_SERVER1 18899

#define BUFFER_MESSAGE_QUEUE 100
#define	WAIT_TIME 10
#define BUFFER_MEMORY_REGULATIONS 256
#define	PATH_NAME "utils.h"
#define failedCase -1
#define activeSession 1

#define PROJECT_ID_SERVER1 188
#define	PROJECT_ID_SERVER2 99
#define MAX_CONNECTION 10
#define PERMS 0644

#define createSocketFailedCase "Socket creation failed"
#define connectSocketFailedCase "Connection failed"
#define sendMessageSocketFailedCase "Send socket failed"
#define receiveMessageSocketFailedCase "Receive socket failed"
#define endConversationWithServerWord "End"
#define bindSocketFailedCase "Socket bind failed"
typedef	void	Sigfunc(int);

struct msg
{
    long msg_type;
    char mgs[BUFFER_MESSAGE_QUEUE];
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
	if (strcmp(message, endConversationWithServerWord) == 0)
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