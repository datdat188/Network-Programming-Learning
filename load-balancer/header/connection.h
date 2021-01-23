#include "utils.h"

struct connection_closure {
    void (*on_read)(void* closure, string buffer, int len);
    void* on_read_closure;

    void (*on_close)(void* closure);
    void* on_close_closure;
    
    struct data_buffer_entry* write_buffer;
};

extern void connection_write(struct epoll_data_handler* self, string data, int len);

extern void connection_close(struct epoll_data_handler* self);

extern struct epoll_data_handler* create_connection(int connection_fd);