#include "header/lb_proxy_server.h"
#include "header/log.h" 
#include "header/epoll_support.h"

using namespace std;

int main(int argc, char **argv)
{
    string lb_port = "8888";
    string ws_host = "192.168.174.131";
    string ws_port = "8081";

    epoll_init();

    log_init();
    startServer(lb_port, ws_host, ws_port);

    epoll_do_reactor_loop();
    return 0;
}