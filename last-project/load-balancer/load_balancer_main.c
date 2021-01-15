#include "utils.h"
#include "lb_proxy_server.h"


int main(int argc, char **argv)
{
    if (argc != 3 && argc != 4)
    {
        printf("Usage:%s <proxy-port> <host | ip> <service-name | port-number>\n", argv[0]);
        return 0;
    }
    int idx, portLength, proxy_port;
    char portBuf[32];
	strcpy(portBuf, argv[1]);
	portLength = strlen(portBuf);
    for(idx=0 ; idx < portLength; idx++)
        if(!isdigit(portBuf[idx]))
            break;
    
    if(portLength != idx){
        handleError(portProxyInvalidCase);
	}
    
    proxy_port = atoi(argv[1]);
    startServer(argv[2], proxy_port);
}