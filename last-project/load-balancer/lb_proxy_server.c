#include "lb_proxy_server.h"

void startServer(char* ip, int port){
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,serv_addr;
    struct hostent* host;

    int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

    // set all 0 for memory buffer
    bzero((char*)&servaddr,sizeof(servaddr));
    bzero((char*)&cliaddr, sizeof(cliaddr));
    
    // Create the server
    servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
    
    // Create the listener 
    listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);
    printf("Listener socket created and bound to port %d\n", port);

    while(activeSession){
        
    }
}