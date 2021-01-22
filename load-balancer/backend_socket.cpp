#include "header/backend_socket.h"

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

void close_backend_socket(struct epoll_event_handler* self)
{
    close(self->fd);
    free(self->closure);
    free(self);
}

// read data from backend and send to client
void handle_backend_socket_event(struct epoll_event_handler* self, uint32_t events)
{
    struct backend_socket_event_data* closure = (struct backend_socket_event_data*) self->closure;

    char buffer[BUFFER_SIZE];
    int bytes_read;

    if (events & EPOLLIN) 
    {
        // make sure that read every thing
        while ((bytes_read = Recv(self->fd, buffer, BUFFER_SIZE, 0)) != -1 && bytes_read != 0) 
        {
            if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
            {
                return;
            }

            if (bytes_read == 0 || bytes_read == -1)    
            {
                close_client_socket(closure->client_handler);
                close_backend_socket(self);
                return;
            }
            // send to client
            write_to_client(closure->client_handler, buffer, bytes_read);
        }
    }

    if ((events & EPOLLERR) | (events & EPOLLHUP) | (events & EPOLLRDHUP)) 
    {
        close_client_socket(closure->client_handler);
        close_backend_socket(self);
        return;
    }

}


struct epoll_event_handler* create_backend_socket_handler(int epoll_fd, int backend_socket_fd)
{
    sock_set_non_blocking(backend_socket_fd);

    struct backend_socket_event_data* closure = (backend_socket_event_data*)malloc(sizeof(struct backend_socket_event_data));
    

    struct epoll_event_handler* result = (epoll_event_handler*)malloc(sizeof(struct epoll_event_handler));
    result->fd = backend_socket_fd;
    result->handle = handle_backend_socket_event;
    result->closure = closure;

    // add  epoll event 
    epoll_add_handler(epoll_fd, result, EPOLLIN | EPOLLRDHUP | EPOLLET);

    return result;
}

