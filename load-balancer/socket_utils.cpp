#include "header/socket_utils.h"

int
Socket()
{
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		perror("socket error");
	return(sockfd);
}

void
Send(int sockfd, const void *ptr, size_t nbytes, int flags)
{
	if (send(sockfd, ptr, nbytes, flags) != (ssize_t)nbytes)
		perror("send error");
}

ssize_t
Recv(int sockfd, void *ptr, size_t nbytes, int flags)
{
	ssize_t	n;

	if ( (n = recv(sockfd, ptr, nbytes, flags)) < 0)
		perror("recv error");
	return(n);
}

void
Listen(int lsockfd)
{
	if (listen(lsockfd, MAX_CLIENTS) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
}

int
Bind(int lsockfd, int port)
{
    struct sockaddr_in servaddr;
    socklen_t salen;

    bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

    //inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr.s_addr);
	//inet_pton(AF_INET, ip, &servaddr.sin_addr);

	return bind(lsockfd, (SA *) &servaddr, sizeof(servaddr));
} 

void
Connect(int sockfd, char* ip, int port)
{
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
   
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
        exit(EXIT_FAILURE);
    }
}

int
Accept(int lsockfd)
{
	int	n;
    struct sockaddr_in *cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    
again:
	if ( (n = accept(lsockfd, (SA *) &cliaddr, &clilen)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			perror("accept error");
	}
	return(n);
}

void
Setsockopt(int lsockfd)
{
	int optval;
	if (setsockopt(lsockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt error");
		exit(EXIT_FAILURE);
	}
}