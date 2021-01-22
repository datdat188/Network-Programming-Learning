#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>

#include "epoll_interface.h"
#include "socket_utils.h"
#include "to_client_handle.h"

#define BUFFER_SIZE 4096


struct backend_socket_event_data 
{
    struct epoll_event_handler* client_handler;
};

static int sock_set_non_blocking(int fd);

extern void handle_backend_socket_event(struct epoll_event_handler* self, uint32_t events);

extern void close_backend_socket(struct epoll_event_handler*);

extern struct epoll_event_handler* create_backend_socket_handler(int epoll_fd, int backend_socket_fd);
