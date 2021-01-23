#include "utils.h"

#define MAXEVENTS 4096

struct backend_socket_event_data 
{
    struct epoll_data_handler* client_handler;
};

extern void handle_backend_socket_event(struct epoll_data_handler* self, uint32_t events);

extern void close_backend_socket(struct epoll_data_handler*);

extern struct epoll_data_handler* startServer(string, string, string);


// string get_host(string request);
// string get_cookie(string request);
// int check_valid_uri(string request);

// void active_handler();
// void standby_handler();

// char* get_cookie_serverid(char* msg);
// void set_cookie_serverid(string response, string ws_ip);