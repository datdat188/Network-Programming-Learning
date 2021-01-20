#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */

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

#define	SA	struct sockaddr
#define MAX_CLIENTS 100

int Socket();
void Setsockopt(int lsockfd);
void Bind(int lsockfd, int port);
void Listen(int lsockfd);
int Accept(int lsockfd);
void Send(int sockfd, const void *ptr, size_t bufflen, int flags);
ssize_t Recv(int sockfd, void *ptr, size_t bufflen, int flags);
void Connect(int sockfd, char* ip, int port);
void Setsockopt(int lsockfd);
