#include "utils.h"
#include "socket_utils.h"
#include <string>
#include <sys/epoll.h>
#include <iostream>

using namespace std;

struct epoll_event_handler {
    int fd;
    void (*handle)(struct epoll_event_handler*, uint32_t);
    void* closure;
};

extern void epoll_init(int *epoll_fd);

extern void epoll_add_handler(int epoll_fd, struct epoll_event_handler* handler, uint32_t event_mask);

extern void epoll_remove_handler(int epoll_fd, struct epoll_event_handler* handler);

extern void epoll_add_to_free_list(void* block);

extern void epoll_do_reactor_loop(int epoll_fd);

namespace epoll_proxy
{
    constexpr int max_events = 32;

    auto create_and_bind(string const& port);

    auto sock_set_non_blocking(int socketfd);

    auto accept_connection(int socketfd, struct epoll_event& event, int epollfd);

    auto read_data(int fd);
}