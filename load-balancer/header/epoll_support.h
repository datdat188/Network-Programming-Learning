#include "utils.h"

struct epoll_data_handler {
    int fd;
    void (*handle)(struct epoll_data_handler* self, uint32_t events);
    void* closure_event;
};

extern void epoll_init();

//static void epoll_ctl_add(int epfd, epoll_data *data, uint32_t events);

extern void epoll_add_handler(struct epoll_data_handler* handler, uint32_t event_mask);

extern void epoll_add_handler(struct epoll_data_handler* handler, uint32_t event_mask);

extern void epoll_remove_handler(struct epoll_data_handler* handler);

extern void epoll_add_to_free_list(void* block);

extern void epoll_do_reactor_loop();