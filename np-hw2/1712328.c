#include    <limits.h>
#include    <sys/errno.h>
#include    <sys/select.h>

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */

#include "HTTP.h"
#include "StringFunctions.h"

#define	SA	struct sockaddr
#define	LISTENQ		1024	/* 2nd argument to listen() */
// Another source
#define BUFFER_SIZE 2048

#define MAX_CLIENTS 128
#define MAX_THREADS 128

#define START_INDEX_FILTERS 2 

//=============================VARIABLE DECLARE====================
typedef	void	Sigfunc(int);
/// A structure to hold a file descriptor and a message.
typedef struct {
	/// Socket/File Descriptor.
	int sock;
	
	/// The address in the communications space of the socket.
	struct sockaddr_in address;
	
	/// Message sent by fd.
	char *msg;
} sock_msg;

int errno;
unsigned int *filtersCount;
const char **filters;

/// The total number of successful requests.
unsigned int *numberOfSuccessfulRequests;
/// The total number of filtered requests.
unsigned int *numberOfFilteredRequests;
/// The total number of errored requests.
unsigned int *numberOfErroredRequests;

//===========================UTILS FUNCTION===========================
void error(char* msg)
{
    perror(msg);
    exit(0);
}

//==========================UNPV13E LIB FUNCTION======================
/* include Socket */
int
Socket(int family, int type, int protocol)
{
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		error("socket error");
	return(n);
}
/* end Socket */

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		error("bind error");
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
		error("listen error");
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
			error("accept error");
	}
	return(n);
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		error("connect error");
}

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t		n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
		error("recv error");
	return(n);
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		error("send error");
}

void
Close(int fd)
{
	if (close(fd) == -1)
		error("close error");
}

pid_t
Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
		error("fork error");
	return(pid);
}

Sigfunc *
Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		error("signal error");
	return(sigfunc);
}

void 
//======================================SIGNAL FUNCTION=====================
sigintHandler(int sig_num)
{
	/* Reset handler to catch SIGINT next time. 
       Refer http://en.cppreference.com/w/c/program/signal */
	Signal(SIGINT, sigintHandler);
	fflush(stdout);
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
void handleSIGUSR1()
{
	// Received signal.
	printf("Received SIGUSR1...reporting status:\n");
	
	// Report the number of requests.
	if ( *numberOfSuccessfulRequests == 1 ) {
		printf("-- Processed %u request successfully.\n", *numberOfSuccessfulRequests);
	} else {
		printf("-- Processed %u requests successfully.\n", *numberOfSuccessfulRequests);
	}
	
	// Report the filters being used.
	printf("-- Filtering:");
	for ( unsigned int i=0; i<*filtersCount; ++i ) {
		printf(" %s;", filters[i]);
	}
	printf("\n");
	
	// Report the number of filtered requests.
	if ( *numberOfFilteredRequests == 1 ) {
		printf("-- Filtered %u request.\n", *numberOfFilteredRequests);
	} else {
		printf("-- Filtered %u requests.\n", *numberOfFilteredRequests);
	}
	
	// Report the number of requests that resulted in errors.
	if ( *numberOfErroredRequests == 1 ) {
		printf("-- Encountered %u request in error\n", *numberOfErroredRequests);
	} else {
		printf("-- Encountered %u requests in error\n", *numberOfErroredRequests);
	}
}

void handleSIGUSR2()
{
	printf("Received SIGUSR2...");
	exit(1);
}

//=====================================================================

int main(int argc, char* argv[])
{
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,serv_addr;
    struct hostent* host;

    int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
    
    if(argc<2)
        error("./1712328 <port>");

    // The port number to use for the socket.
	unsigned short port = 8888; // The default port is 8127 (if no arguments are given).
	if ( argc > 1 ) {
		int conversion = atoi(argv[1]);
		// Cannot convert int to unsigned short.
		if ( conversion < 0 || conversion > USHRT_MAX ) {
			error("Invalid port");
		}
		// Assign the port value.
		port = (unsigned short) conversion;
	}

    // /*
	//  Your proxy server must filter based on domain name (or IP address) prefixes/suffixes as described above. All command-line arguments following the port number are domain or IP address prefixes/suffixes that must be filtered. There can be zero or more of these; and there is no limit to the number of arguments.
	//  */
	
	// filtersCount = malloc(sizeof(unsigned int));
	// *filtersCount = 0;
	
	// // Contains at least one filter.
	// if ( argc > START_INDEX_FILTERS ) {
	// 	// Filters are START_INDEX_FILTERS less than the number of arguments to the program.
	// 	*filtersCount = argc - START_INDEX_FILTERS;
	// 	// Create a pointer to the start of the filters in argv.
	// 	filters = &argv[START_INDEX_FILTERS];
	// }
    
    // set all 0 for memory buffer
    bzero((char*)&servaddr,sizeof(servaddr));
    bzero((char*)&cliaddr, sizeof(cliaddr));
    
    // Create the server
    servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));
    
    // Create the listener 
    listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // Bind
    Bind(listenfd, (SA *) &servaddr,sizeof(servaddr));
 
    // Number of maximum accept client
    Listen(listenfd, LISTENQ);
    printf("Listener socket created and bound to port %d\n", port);
    
    Signal(SIGCHLD, sig_chld);
    Signal(SIGINT, sigintHandler);
    Signal(SIGUSR1, &handleSIGUSR1);
	Signal(SIGUSR2, &handleSIGUSR2);

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				error("accept error");
		}
        if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			char buffer[BUFFER_SIZE]; // Packet size

            // Receive the message.
            bzero((char*)buffer,BUFFER_SIZE);
			ssize_t n = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
			// Stream has errored or ended.
			if (n <= 0)
			{
				// Errored.
				perror("recv()");
				continue;
			}
			else
			{
				// Stream received message.
				buffer[n] = '\0';
#ifdef DEBUG
				//printf("Received message from fd %d: %s\n", fd, buffer);
#endif
			}
        /* 
		 6. Your server does must be a concurrent server (i.e. do not use an iterative server).
		 */

			// Create a thread to handle message.
			sock_msg *arg = malloc(sizeof(sock_msg));
			arg->sock = connfd;
			arg->address = servaddr;
			arg->msg = stringDuplicate(buffer);
			//handleRequest(arg);
            exit(0);
		}
		Close(connfd);
    }
    
    return EXIT_SUCCESS;
}