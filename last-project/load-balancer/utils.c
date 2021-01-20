#include "header/utils.h"
void sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated\n", pid);
	}
	return;
}

void handleError(const char* caseError){
	//caseError = strcat("[#] Client: ",caseError);
	perror(caseError);
	exit(1);
}
Sigfunc *Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		handleError("signal error");
	return(sigfunc);
}

int createSocket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}

char* upcase(char *str)
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

int endConversationWithServer(char* message){
	if (strcmp(message, (char*)endConversationWithServerWord) == 0)
	{
		return 1;
	}
	return 0;
}

int messageQueueGet(key_t key)
{
	int msgid;
	if ((msgid = msgget(key, PERMS | IPC_CREAT)) == -1)
		return -1;

	return msgid;
}
