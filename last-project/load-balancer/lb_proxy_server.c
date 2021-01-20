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

    int lsockfd = Socket();
    Setsockopt(lsockfd);
    Bind(lsockfd,  port);
    Listen(lsockfd);

    struct sockaddr_in cliaddr;
    
    printf("Listener socket created and bound to port %d\n", port);

    int rc = sock_set_non_blocking(lsockfd);

    int efd = epoll_create1(0);
	if (efd == -1)
        handleError("epoll_create1 error");

    struct epoll_event ev = {
        .data.fd = lsockfd,
        .events = EPOLLIN | EPOLLET,
    };

	if(epoll_ctl(efd, EPOLL_CTL_ADD, lsockfd, &ev) < 0)
        handleError("epoll_ctl error");

    struct epoll_event *pevents = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));

    while(activeSession){
        int nready = epoll_wait(efd, pevents, MAXEVENTS, -1);
        for(int i = 0; i < nready; i++){
            if(lsockfd == pevents[i].data.fd){
                /* we hava one or more incoming connections */
                while (activeSession)
                {
                    socklen_t inlen = sizeof(cliaddr);
                    struct sockaddr_in clientaddr;
                    int infd = accept(lsockfd, (struct sockaddr *) &clientaddr, &inlen);
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