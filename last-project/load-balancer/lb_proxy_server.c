#include "header/lb_proxy_server.h"

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

void startServer(char* ip, int port){
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,serv_addr;
    struct hostent* host;

    int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    int i,ret, nready, len;

    // set all 0 for memory buffer
    bzero((char*)&servaddr,sizeof(servaddr));
    bzero((char*)&cliaddr, sizeof(cliaddr));
    
    // Create the server
    servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
    
    // Create the listener 
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    printf("Listener socket created and bound to port %d\n", port);

    int rc = sock_set_non_blocking(listenfd);

    int efd = epoll_create1(0);
	if (efd == -1)
        handleError("epoll_create1 error");

    struct epoll_event ev = {
        .data.fd = listenfd,
        .events = EPOLLIN | EPOLLET,
    };

	if(epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
        handleError("epoll_ctl error");

    struct epoll_event *pevents = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));

    while(activeSession){
        nready = epoll_wait(efd, pevents, MAXEVENTS, -1);
        for(i = 0; i < nready; i++){
            if(listenfd == pevents[i].data.fd){
                /* we hava one or more incoming connections */
                while (activeSession)
                {
                    socklen_t inlen = sizeof(cliaddr);
                    struct sockaddr_in clientaddr;
                    int infd = accept(listenfd, (struct sockaddr *) &clientaddr, &inlen);
                    if (infd < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            /* we have processed all incoming connections */
                            break;
                        }
                        handleError("accept");
                        break;
                    }

                    rc = sock_set_non_blocking(infd);
                    
                }
            }
            else {
                if( (pevents[i].events & EPOLLERR) ||
                    (pevents[i].events & EPOLLHUP) ||
                  (!(pevents[i].events & EPOLLIN))){
                    handleError("epoll_wait");
                    close(pevents[i].data.fd);
                    continue;
			    }

                // Handle something
            }
        }
    }
}