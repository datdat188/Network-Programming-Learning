#include "header/lb_proxy_server.h"
#include "header/log.h"

string lb_ip("127.0.0.1");
string const lb_port = "8888";

int main(int argc, char **argv)
{
    log_init();
    startServer(lb_ip, lb_port);
    return 0;
}