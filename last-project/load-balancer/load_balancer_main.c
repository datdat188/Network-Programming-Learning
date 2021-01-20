#include "header/lb_proxy_server.h"

char lb_ip[16] = "127.0.0.1";
int lb_port = 8888;

int main(int argc, char **argv)
{
    // if (argc != 3 && argc != 4)
    // {
    //     printf("Usage:%s <proxy-port> <host | ip> <service-name | port-number>\n", argv[0]);
    //     return 0;
    // }
    // int idx, portLength, proxy_port;
    // char portBuf[32];
    
	// strcpy(portBuf, "8888");
	// portLength = strlen(portBuf);
    // for(idx=0 ; idx < portLength; idx++)
    //     if(!isdigit(portBuf[idx]))
    //         break;
    
    // if(portLength != idx){
    //     handleError("Port");
	// }
    // proxy_port = atoi(argv[1]);

    startServer(lb_ip, lb_port);
    return 0;
}