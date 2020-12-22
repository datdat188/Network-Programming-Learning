/* Reference
=======================================================
https://github.com/ruchirsharma1993/Chat-application-in-c-using-Socket-programming.git
=======================================================
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF 256

int main(int argc, char *argv[])
{
	int fd = 0;
	char buff[BUFF];
	
	if(argc < 2)
	{
		printf("Less no of arguments !!");
		return 0;
	}

	//Setup Buffer Array
	memset(buff, '0',sizeof(buff));	

	//Create Socket
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
	{
		perror("Client Error: Socket not created succesfully");
		return 0;
	}
	
	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

	//Initialize	
	server.sin_family = AF_INET;
	server.sin_port =  htons(atoi(argv[1]));

	int in = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
	if(in<0)
	{
		perror("Client Error: IP not initialized succesfully");
		return 0;
	}

	//Connect to given server	
	in = connect(fd, (struct sockaddr *)&server, sizeof(server));
	if(in<0)
	{
		perror("Client Error: Connection Failed.");
		return 0;
	}    	

	while(1)
	{
		printf("\nPlease enter the message: ");
    		bzero(buff,BUFF);
    		fgets(buff,BUFF ,stdin);
    		
		    printf("Sending to SERVER: %s ",buff);
            // buff[BUFF] = '\0';
		/* Send message to the server */
    		in = send(fd,buff,strlen(buff),0);
		    if (in < 0) 
		    {
			 perror("\nClient Error: Writing to Server");
		    	return 0;
		    }
		    
		/* Now read server response */
		    bzero(buff,BUFF);
		    in = recv(fd,buff,BUFF,0);
		    if (in < 0) 
		    {
			    perror("\nClient Error: Reading from Server");
			    return 0;
		    }
		    printf("\nReceived FROM SERVER: %s ",buff);
	}
	close(fd);
	return 0;
}
