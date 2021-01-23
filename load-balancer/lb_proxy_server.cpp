#include "header/lb_proxy_server.h"

#include "header/socket_utils.h"
#include "header/net.h"
#include "header/epoll_support.h"  
#include "header/connection.h"
#include "header/log.h"
#include "time.h"
#include <ctime>
#include <chrono>

#define BUFFER_SIZE 4096


struct server_socket_event_data {
    string backend_addr;
    string backend_port_str;
};

struct proxy_data {
    struct epoll_data_handler* client;
    struct epoll_data_handler* backend;
};

struct message {
   long msg_type;
   char msg[100];
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

void on_client_read(void* closure, char* buffer, int len)
{
    struct proxy_data* data = (struct proxy_data*) closure;
    if (data->backend == NULL) {
        return;
    }
    cout << "buffet on client read: " << buffer << endl;
    connection_write(data->backend, buffer, len);
}


void on_client_close(void* closure)
{
    struct proxy_data* data = (struct proxy_data*) closure;
    if (data->backend == NULL) {
        return;
    }
    connection_close(data->backend);
    data->client = NULL;
    data->backend = NULL;
    epoll_add_to_free_list(closure);
}


void on_backend_read(void* closure, char* buffer, int len)
{
    struct proxy_data* data = (struct proxy_data*) closure;
    if (data->client == NULL) {
        return;
    }
    cout << "buffet on backend read: " << buffer << endl;
    connection_write(data->client, buffer, len);
}

void on_backend_close(void* closure)
{
    struct proxy_data* data = (struct proxy_data*) closure;
    if (data->client == NULL) {
        return;
    } 
    connection_close(data->client);
    data->client = NULL;
    data->backend = NULL;
    epoll_add_to_free_list(closure);
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
    struct epoll_data_handler* client_connection;
    client_connection = create_connection(client_socket_fd);

    int backend_socket_fd = connect_to_backend((char*)backend_host.c_str(), (char*)backend_port_str.c_str());
    struct epoll_data_handler* backend_connection;
    backend_connection = create_connection(backend_socket_fd);

    struct proxy_data* proxy = (proxy_data*)malloc(sizeof(struct proxy_data));
    proxy->client = client_connection;
    proxy->backend = backend_connection;

    struct connection_closure* client_closure = (struct connection_closure*) client_connection->closure_event;
    client_closure->on_read = on_client_read;
    client_closure->on_read_closure = proxy;
    client_closure->on_close = on_client_close;
    client_closure->on_close_closure = proxy;

    struct connection_closure* backend_closure = (struct connection_closure*) backend_connection->closure_event;
    backend_closure->on_read = on_backend_read;
    backend_closure->on_read_closure = proxy;
    backend_closure->on_close = on_backend_close;
    backend_closure->on_close_closure = proxy;
}

void close_backend_socket(struct epoll_data_handler* self)
{
    close(self->fd);
    free(self->closure_event);
    free(self);
}

void handle_backend_socket_event(struct epoll_data_handler* self, uint32_t events)
{
    struct backend_socket_event_data* closure = (struct backend_socket_event_data*) self->closure_event;

    char buffer[BUFFER_SIZE];
    int bytes_read;

    if (events & EPOLLIN) 
    {
        // make sure that read every thing
        while ((bytes_read = read(self->fd, buffer, BUFFER_SIZE)) != -1 && bytes_read != 0) 
        {
            if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
            {
                return;
            }

            if (bytes_read == 0 || bytes_read == -1) 
            {
                connection_close(closure->client_handler);
                close_backend_socket(self);
                return;
            }
            // send to client
            connection_write(closure->client_handler, buffer, bytes_read);
        }
    }

    if ((events & EPOLLERR) | (events & EPOLLHUP) | (events & EPOLLRDHUP)) 
    {
        connection_close(closure->client_handler);
        close_backend_socket(self);
        return;
    }

}

void handle_server_socket_event(struct epoll_data_handler* self, uint32_t events)
{
    struct server_socket_event_data* closure = (struct server_socket_event_data*) self->closure_event;
    int client_socket_fd;
    
    while (activeSession) {
        client_socket_fd = accept(self->fd, NULL, NULL);
        if (client_socket_fd == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else {
                perror("Could not accept");
                exit(1);
            }
        }
        cout << "accepted" << endl;

        handle_client_connection(client_socket_fd,
                                 closure->backend_addr,
                                 closure->backend_port_str);
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

    key_t my_key=123;
    int msqid = msgget(my_key, 0666 | IPC_CREAT);

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

void active_handler(int client_sock, int msqid)
{
    sockaddr_in client;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(client_sock, (struct sockaddr *)&client, &addr_size);
    sockaddr_in sa = client;
    int port = htons(sa.sin_port);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    // get current time
    string timeNowLog = ctime(&timenow);

    char * msgR;
    strcat(msgR, ip);
    log_write_msg(msgR);
    // add to msg queue
    struct message msgSend;
    msgSend.msg_type = 1;
    (void)strcpy(msgSend.msg, msgR);
    msgsnd(msqid, &msgSend, sizeof(message), 0);
}

void standby_handler(int msqid)
{
    
    while (1)
    {
        message msgRecv;
        if (msgrcv(msqid, &msgRecv, sizeof(msgRecv), 0, 0) < 0)
        {
            cerr << "msgrcv() error in handle_log_standby !" << endl;
        }

        else 
        {
            log_write_msg(msgRecv.msg);
        }
    }
}

char* get_cookie_serverid(char* msg)
{
    return msg;
}

void set_cookie_serverid()
{

}