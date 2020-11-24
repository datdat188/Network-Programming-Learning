#include<stdio.h>
#include<stdlib.h>

#include<limits.h>
#include<unistd.h>
#include<string.h>

#include<arpa/inet.h>
#include<netinet/in.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/errno.h>
#include<sys/select.h>
#include<netdb.h>
#include<errno.h>

extern int errno;
extern unsigned int *filtersCount;
extern const char **filters;

// Another source
#define BUFFER_SIZE 2048

#define MAX_CLIENTS 128
#define MAX_THREADS 128

#define START_INDEX_FILTERS 2 
//===========================

void error(char* msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char* argv[])
{
    pid_t pid;
    struct sockaddr_in addr_in,cli_addr,serv_addr;
    struct hostent* host;
    int sockfd,newsockfd;
    
    if(argc<2)
        error("./1712328 <port>");

    // The port number to use for the socket.
	unsigned short port = 8127; // The default port is 8127 (if no arguments are given).
	if ( argc > 1 ) {
		int conversion = atoi(argv[1]);
		// Cannot convert int to unsigned short.
		if ( conversion < 0 || conversion > USHRT_MAX ) {
			error("Invalid port");
		}
		// Assign the port value.
		port = (unsigned short) conversion;
	}

    // /*
	//  Your proxy server must filter based on domain name (or IP address) prefixes/suffixes as described above. All command-line arguments following the port number are domain or IP address prefixes/suffixes that must be filtered. There can be zero or more of these; and there is no limit to the number of arguments.
	//  */
	
	// filtersCount = malloc(sizeof(unsigned int));
	// *filtersCount = 0;
	
	// // Contains at least one filter.
	// if ( argc > START_INDEX_FILTERS ) {
	// 	// Filters are START_INDEX_FILTERS less than the number of arguments to the program.
	// 	*filtersCount = argc - START_INDEX_FILTERS;
	// 	// Create a pointer to the start of the filters in argv.
	// 	filters = &argv[START_INDEX_FILTERS];
	// }
    
    // set all 0 for memory buffer
    bzero((char*)&serv_addr,sizeof(serv_addr));
    bzero((char*)&cli_addr, sizeof(cli_addr));
    
    // Create the server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Create the listener 
    sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (sockfd<0)
        error("socket()");
    
    // Bind
    if (bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("bind()");
 
    // Number of maximum accept client
    listen(sockfd,50);
    socklen_t clilen = sizeof(cli_addr);
    printf("Listener socket created and bound to port %d\n", port);

    while (1)
    {
        // Accept client connection
        newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
        if (newsockfd<0)
            error("Problem in accepting connection");
        
        pid = fork(); // Use fork to create child process
        if(pid == 0)
        {
            struct sockaddr_in host_addr;
            char buffer[BUFFER_SIZE]; // Packet size

            int flag = 0,newsockfd1,n,port = 0,i,sockfd1;
            char t1[300],t2[300],t3[10];
            char* temp=NULL;

            // Receive the message.
            bzero((char*)buffer,BUFFER_SIZE);
            ssize_t n = recv(newsockfd,buffer,BUFFER_SIZE - 1,0);
            if ( n <= 0 ) {
                // Errored.
                perror("recv()");
                continue;
            }
            else {
                // Stream received message.
                buffer[n] = '\0';
            }
            
            sscanf(buffer,"%s %s %s",t1,t2,t3);
            
            if(((strncmp(t1,"GET",3) == 0))
                &&((strncmp(t3,"HTTP/1.1",8) == 0)
                ||(strncmp(t3,"HTTP/1.0",8) == 0))
                &&(strncmp(t2,"http://",7) ==0 ))
            {
                strcpy(t1,t2);
    
                flag=0;
                
                for(i = 7; i<strlen(t2); i++)
                {
                    if(t2[i]==':')
                    {
                        flag=1;
                        break;
                    }
                }
    
                temp=strtok(t2,"//");
                if(flag == 0)
                {
                    port=80;
                    temp=strtok(NULL,"/");
                }
                else
                {
                    temp=strtok(NULL,":");
                }
                
                sprintf(t2,"%s",temp);
                printf("host = %s",t2);
                host = gethostbyname(t2);
    
                if(flag == 1)
                {
                    temp = strtok(NULL,"/");
                    port = atoi(temp);
                }
    
    
                strcat(t1,"^]");
                temp = strtok(t1,"//");
                temp = strtok(NULL,"/");
                if(temp != NULL)
                temp = strtok(NULL,"^]");
                printf("\npath = %s\nPort = %d\n",temp,port);
                
    
                bzero((char*)&host_addr,sizeof(host_addr));
                host_addr.sin_port=htons(port);
                host_addr.sin_family=AF_INET;
                bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
                
                sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
                newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
                sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
                if(newsockfd1<0)
                    error("Error in connecting to remote server");
    
                printf("\n%s\n",buffer);
                //send(newsockfd,buffer,strlen(buffer),0);
                bzero((char*)buffer,sizeof(buffer));
                if(temp!=NULL)
                    sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
                else
                    printf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);


                n=send(sockfd1,buffer,strlen(buffer),0);
                printf("\n%s\n",buffer);
                if(n<0)
                    error("Error writing to socket");
                else{
                    do
                    {
                        bzero((char*)buffer,500);
                        n=recv(sockfd1,buffer,500,0);
                        if(!(n<=0))
                        send(newsockfd,buffer,n,0);
                    }while(n>0);
                }
            }
            else
            {
                send(newsockfd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0);
            }
                close(sockfd1);
                close(newsockfd);
                close(sockfd);
                _exit(0);
            }
        else
        {
            close(newsockfd);
        }
    }
    return 0;
}