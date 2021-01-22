#include "header/lb_proxy_server.h"

static char *not_found_response =
    "HTTP/1.1 404 Not Found\n"
    "Content-type: text/html\n" "\n"
    "<html>\n"
    " <body>\n"
    "  <h1>Not Found</h1>\n"
    "  <p>The requested URL was not found on this server.</p>\n"
    " </body>\n" 
    "</html>\n";

/*
    if != lienhe.html || != '' || !='index.html'
    send not found response
*/

string get_host(string request){
    string host;
    if (request.find("Host: ") != string::npos)
    {
        int ip_start = request.find("Host: ") + strlen("Host: ");
        int ip_end = request.find_first_of("\r\n", ip_start);
        host = request.substr(ip_start, ip_end - ip_start);
    }
    return host;
}

string get_cookie(string request)
{
    string ip;
    if (request.find("Cookie:") != string::npos)
    {
        int ip_start = request.find("SERVERID=") + strlen("SERVERID=");
        int ip_end = request.find_first_of("\r\n", ip_start);
        ip = request.substr(ip_start, ip_end - ip_start);
    }
    return ip;
}

int check_valid_uri(string request)
{
    if (request.find("/lienhe.html") == string::npos 
    && request.find("/index.html")  == string::npos
    && request.find("/ ") == string::npos)
        return 0;
    return 1;
}

void startServer(string ip, string const& port)
{
    int epoll_fd;
    epoll_init(&epoll_fd);

    struct epoll_event_handler* server_socket_event_handler;
    server_socket_event_handler = create_server_socket_handler(epoll_fd, port /*, ip, backend_port*/);

    

    // struct epoll_event ev; 
    // ev.data.fd = lsockfd;
    // ev.events = EPOLLIN | EPOLLET;

	// if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lsockfd, &ev) < 0)
    //     handleError("epoll_ctl error");

    // struct epoll_event *pevents = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));

    // while(activeSession){
    //     int nready = epoll_wait(epoll_fd, pevents, MAXEVENTS, -1);
    //     for(int i = 0; i < nready; i++){
    //         if(lsockfd == pevents[i].data.fd){
    //             /* we hava one or more incoming connections */
    //             while (activeSession)
    //             {
    //                 socklen_t inlen = sizeof(cliaddr);
    //                 struct sockaddr_in clientaddr;
    //                 int infd = accept(lsockfd, (struct sockaddr *) &clientaddr, &inlen);
    //                 if (infd < 0) {
    //                     if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
    //                         /* we have processed all incoming connections */
    //                         break;
    //                     }
    //                     handleError("accept");
    //                     break;
    //                 }

    //                 rc = sock_set_non_blocking(infd);
                    
    //             }
    //         }
    //         else {
    //             if( (pevents[i].events & EPOLLERR) ||
    //                 (pevents[i].events & EPOLLHUP) ||
    //               (!(pevents[i].events & EPOLLIN))){
    //                 handleError("epoll_wait");
    //                 close(pevents[i].data.fd);
    //                 continue;
	// 		    }

    //             // Handle something
    //         }
    //     }
    // }
}

void active_handler(/*int client_sock, shared_ptr<backend> ws*/)
{
    // get client addr
    // sockaddr_in sa = get_client_addr(client_sock);
    // int port = htons(sa.sin_port);
    // char ip[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

    // // get current time
    // string timenow = get_time();

    // // write to log
    // string msg_log = log(timenow, ip, port, ws->get_host());
    // //log_terminal(timenow, ip, port);

    // // add to msg queue
    // struct message msgSend;
    // msgSend.msg_type = 1;
    // (void)strcpy(msgSend.msg, msg_log.c_str());
    // msgsnd(msqid, &msgSend, sizeof(message), 0);
}

void standby_handler()
{
    // doc tu msg_queue
    // while (1)
    // {
    //     message msgRecv;
    //     if (msgrcv(msqid, &msgRecv, sizeof(message), 0, 0) < 0)
    //     {
    //         cerr << "msgrcv() error in handle_log_standby !" << endl;
    //     }

    //     else // ghi vao log
    //     {
    //         log(msgRecv.msg);
    //     }
    // }
}

char* get_cookie_serverid(char* msg)
{
    return msg;
}

void set_cookie_serverid()
{

}

void proxy_forward_request(char* msg)
{
    
}
