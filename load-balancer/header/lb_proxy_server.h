#include "utils.h"
#include "socket_utils.h"
#include "epoll_interface.h"
#include "to_server_handle.h"
#include "to_client_handle.h"

#define MAXEVENTS 64

void startServer(string, string const&);

string get_host(string request);
string get_cookie(string request);
int check_valid_uri(string request);

void active_handler();
void standby_handler();

char* get_cookie_serverid(char* msg);
void set_cookie_serverid();

void proxy_forward_request(char* msg);