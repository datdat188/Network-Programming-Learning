#include "header/lb_proxy_server.h"

#include "header/socket_utils.h"
#include "header/net.h"
#include "header/epoll_support.h"  


struct server_socket_event_data {
    string backend_addr;
    string backend_port_str;
};

struct proxy_data {
    struct epoll_data_handler* client;
    struct epoll_data_handler* backend;
};



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

string get_cookie_serverid(string request)
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

void set_cookie_serverid(string response, string ws_ip)
{
    string insert_str = "Set-cookie: SERVERID=";
    insert_str.append(ws_ip);
    response.append(insert_str);
}

void handle_client_connection(int client_socket_fd, 
                              string backend_host, 
                              string backend_port_str) 
{

}

void handle_server_socket_event(struct epoll_data_handler* self, uint32_t events)
{
    struct server_socket_event_data* closure = (struct server_socket_event_data*) self->closure_event;
    int client_socket_fd;
    
    while (activeSession) {
        client_socket_fd = Accept(self->fd);
        if (client_socket_fd == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else {
                perror("Could not accept");
                exit(1);
            }
        }
        cout << "accepted";

        // handle_client_connection(client_socket_fd,
        //                          closure->backend_addr,
        //                          closure->backend_port_str);
    }
}

int create_and_bind(string port)
{
    int socketfd = Socket();
    Setsockopt(socketfd);
    if (Bind(socketfd, stoi(port)) != 0)
    {
        cerr << "bind error" << endl;
        close(socketfd);
        exit(1);
    }
    return socketfd;
}

struct epoll_data_handler* startServer(
    string port, string ws_hostid, string ws_port)
{
    int epoll_fd;

    epoll_fd = create_and_bind(port);
    make_socket_non_blocking(epoll_fd);

    Listen(epoll_fd);

    cout << "Listen on " << port << endl;

    struct server_socket_event_data* closure = (server_socket_event_data*)malloc(sizeof(struct server_socket_event_data));
    closure->backend_addr = ws_hostid;
    closure->backend_port_str = ws_port;

    struct epoll_data_handler* result = (epoll_data_handler*)malloc(sizeof(struct epoll_data_handler));
    result->fd = epoll_fd;
    result->handle = handle_server_socket_event;
    result->closure_event = closure;

    cout << "success";

    epoll_add_handler(result, EPOLLIN | EPOLLET);

    return result;
    
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