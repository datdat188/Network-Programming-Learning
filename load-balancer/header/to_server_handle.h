#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netdb.h>

#include "backend_socket.h"
#include "socket_utils.h"
#include "to_client_handle.h"

#define MAX_LISTEN_BACKLOG 4096

// info of incoming connection 
struct server_socket_event_data 
{
    int epoll_fd; 
    string backend_addr;
    string backend_port;
};

extern struct epoll_event_handler* create_server_socket_handler(int epoll_fd, string const& server_port /*, string backend_addr, string backend_port*/);
