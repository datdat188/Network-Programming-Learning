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
