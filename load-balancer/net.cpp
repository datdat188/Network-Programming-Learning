#include "header/net.h"

void make_socket_non_blocking(int socket_fd)
{
    int flags;

    flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) 
    {
        perror("Couldn't get socket flags");
        exit(1);
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, flags) == -1) 
    {
        perror("Couldn't set socket flags");
        exit(-1);
    }
}

int connect_to_backend(char* backend_host,
                       char* backend_port_str)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int getaddrinfo_error;
    struct addrinfo* addrs;
    getaddrinfo_error = getaddrinfo(backend_host, backend_port_str, &hints, &addrs);
    if (getaddrinfo_error != 0) {
        if (getaddrinfo_error == EAI_SYSTEM) {
            perror("Couldn't find backend");
        } else {
            perror("Couldn't find backend");
        }
        exit(1);
    }

    int backend_socket_fd;
    struct addrinfo* addrs_iter;
    for (addrs_iter = addrs;
         addrs_iter != NULL;
         addrs_iter = addrs_iter->ai_next)
    {
        backend_socket_fd = socket(addrs_iter->ai_family,
                                   addrs_iter->ai_socktype,
                                   addrs_iter->ai_protocol);
        if (backend_socket_fd == -1) {
            continue;
        }

        if (connect(backend_socket_fd,
                    addrs_iter->ai_addr,
                    addrs_iter->ai_addrlen) != -1) {
            cout << "connected" << endl;
            break;
        }

        close(backend_socket_fd);
    }

    if (addrs_iter == NULL) {
        perror("Couldn't connect to backend");
        exit(1);
    }

    freeaddrinfo(addrs);

    return backend_socket_fd;
}