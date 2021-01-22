#include "to_server_handle.h"

int connect_to_backend(char* backend_host, int backend_port)
{
    int backend_socket_fd = Socket();
    Connect(backend_socket_fd, backend_host, backend_port);
    return backend_socket_fd;
}



void handle_client_connection(int epoll_fd, int client_socket_fd, char* backend_host, int backend_port) 
{

    struct epoll_event_handler *client_socket_event_handler;
    client_socket_event_handler = create_client_socket_handler(client_socket_fd, epoll_fd, backend_host);

    // connect to backend server
    struct epoll_event_handler* backend_socket_event_handler;
    int backend_socket_fd = connect_to_backend(backend_host, backend_port);
    backend_socket_event_handler = create_backend_socket_handler(epoll_fd, backend_socket_fd);

    struct client_socket_event_data* client_closure = (struct client_socket_event_data*) client_socket_event_handler->closure;
    client_closure->backend_handler = backend_socket_event_handler;

    // add client_handler

    struct backend_socket_event_data* backend_closure = (struct backend_socket_event_data*) backend_socket_event_handler->closure;
    backend_closure->client_handler = client_socket_event_handler;


}


// accept clinet connect && handle
void handle_server_socket_event(struct epoll_event_handler* self, uint32_t events)
{
    struct server_socket_event_data* closure = (struct server_socket_event_data*) self->closure;

    int client_socket_fd;
    while (1) 
    {
        client_socket_fd = Accept(self->fd);

        handle_client_connection(closure->epoll_fd, 
                                client_socket_fd, 
                                closure->backend_addr, 
                                closure->backend_port);
    }
}


struct epoll_event_handler* create_server_socket_handler(int epoll_fd, 
string const& server_port /*, string backend_addr, string const& backend_port*/)
{
    // Create a server proxy
    int lsockfd = epoll_proxy::create_and_bind(server_port);
    sock_set_non_blocking(lsockfd);

    Listen(lsockfd);

    printf("Listener socket created and bound to port %d\n", server_port);

    struct server_socket_event_data* closure = (server_socket_event_data*)malloc(sizeof(struct server_socket_event_data));
    closure->epoll_fd = epoll_fd;
    closure->backend_addr = backend_addr;
    closure->backend_port = backend_port;

    struct epoll_event_handler* server_socket_handler = (epoll_event_handler*)malloc(sizeof(struct epoll_event_handler));
    server_socket_handler->fd = lsockfd;
    server_socket_handler->handle = handle_server_socket_event;
    server_socket_handler->closure = closure;

    // add epoll event
    epoll_add_handler(epoll_fd, server_socket_handler, EPOLLIN | EPOLLET);


    return server_socket_handler;
}


