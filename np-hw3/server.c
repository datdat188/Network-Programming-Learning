/*
=======================================================
https://github.com/ruchirsharma1993/Chat-application-in-c-using-Socket-programming.git
=======================================================
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define BUFFER_MEMORY_REGULATIONS 256
#define failedCase -1
#define activeSession 1

#define createSocketFailedCase "Socket creation failed"

char *upcase(char *str)
{
	for (int i = 0; str[i]!='\0'; i++) 
	{
   		if(str[i] >= 'a' && str[i] <= 'z') 
		{
      		str[i] = str[i] -32;
		}
	}
	return str;
}

void 
handleError(const char* caseError){
	//caseError = strcat("[#] Client: ",caseError);
	perror(caseError);
	exit(1);
}

int
createSocket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}


int main()
{
	int fileDescriptor = 0;
	char memoryBuffer[BUFFER_MEMORY_REGULATIONS];
	char handleMemoryBuffer[BUFFER_MEMORY_REGULATIONS];
	
	memset(memoryBuffer, '0',sizeof(memoryBuffer));	

	fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(fileDescriptor<0)
		handleError(createSocketFailedCase);

	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

	//Initialize	
	server.sin_family = AF_INET;
	server.sin_port = htons(18899); 
    server.sin_addr.s_addr = htonl(INADDR_ANY);
   

	bind(fileDescriptor, (struct sockaddr*)&server, sizeof(server)); 
	printf ("Server running...\n");

	int in;
	
	listen(fileDescriptor, 10); 
	while(	in = accept(fileDescriptor, (struct sockaddr*)NULL, NULL))
	{		
		int childpid,n;
		if ((childpid = fork ()) == 0) 
		{
			printf ("\nClient connected\n");
			close (fileDescriptor);
	
			bzero(memoryBuffer,BUFFER_MEMORY_REGULATIONS);
			bzero(handleMemoryBuffer,BUFFER_MEMORY_REGULATIONS);
							
			while ((n = recv(in, memoryBuffer, BUFFER_MEMORY_REGULATIONS,0)) > 0)  
			{
			
				printf("Server Received: %s",memoryBuffer);
				
				char *handleMemoryBuffer = upcase(memoryBuffer);
				send(in, handleMemoryBuffer, strlen(handleMemoryBuffer), 0);
		
				bzero(memoryBuffer,BUFFER_MEMORY_REGULATIONS);
										
			}
			close(in);
			exit(0);
		}

	/*	int inp;
		printf("In is: %d",in);		
		// Read server response 
		bzero(memoryBuffer,256);
		inp = recv(in,memoryBuffer,256,0);
		if (inp < 0) 
		{
			perror("\nServer Error: Reading from Client");
			return 0;
		}
		printf("Server Received: %s",memoryBuffer);
		printf("\nIn is: %d",in);
		
		inp = send(in,memoryBuffer,strlen(memoryBuffer),0);
		    if (inp < 0) 
		    {
			 perror("\nServer Error: Writing to Server");
		    	return 0;
		    }
	*/	
	//	sleep(1);
		
	}
}
