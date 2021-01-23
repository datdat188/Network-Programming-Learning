#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netdb.h> 
#include <string.h>


#include "header/epoll_support.h"
#include "header/connection.h"
#include "header/log.h"
#include "header/net.h"


#define BUFFER_SIZE 4096

struct data_buffer_entry {
    int is_close_message;
    string data;
    int current_offset;
    int len;
    struct data_buffer_entry* next;
};

void connection_handle_event(struct epoll_data_handler* self, uint32_t events)
{
    struct connection_closure* closure = (struct connection_closure* ) self->closure_event;
    closure->on_read = NULL;
    closure->on_close = NULL;
}

struct epoll_data_handler* create_connection(int client_socket_fd)
{
    make_socket_non_blocking(client_socket_fd);

    struct connection_closure* closure = (connection_closure*)malloc(sizeof(struct connection_closure));
    closure->write_buffer = NULL;

    struct epoll_data_handler* result = (epoll_data_handler*)malloc(sizeof(struct epoll_data_handler));

    log_write_msg(cached_log_time);

    result->fd = client_socket_fd;
    result->handle = connection_handle_event;
    result->closure_event = closure;


    epoll_add_handler(result, EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLOUT);

    return result;
}