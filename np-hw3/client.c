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

#define BUFFER_MEMORY_REGULATIONS 256
#define failedCase -1
#define activeSession 1

#define notEnoughAgrumentCase "Less no of arguments"
#define createSocketFailedCase "Socket creation failed"
#define ipInitializedFailedCase "IP not initialized failed"
#define connectSocketFailedCase "Connection failed"
#define sendMessageSocketFailedCase "Send socket failed"
#define receiveMessageSocketFailedCase "Receive socket failed"

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

int main(int argc, char *argv[])
{
	int fileDescriptor = 0;
	char memoryBuffer[BUFFER_MEMORY_REGULATIONS];
	
	if(argc < 2)
		handleError(notEnoughAgrumentCase);

	memset(memoryBuffer, '0',sizeof(memoryBuffer));	

	fileDescriptor = createSocket();
    if(fileDescriptor <= failedCase)
		handleError(createSocketFailedCase);
	
	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

	//Initialize with argv[1] is port
	server.sin_family = AF_INET;
	server.sin_port =  htons(atoi(argv[1]));

	int in = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
	if(in <= failedCase)
		handleError(ipInitializedFailedCase);

	in = connect(fileDescriptor, (struct sockaddr *)&server, sizeof(server));
	if(in <= failedCase)
		handleError(connectSocketFailedCase); 	

	while(activeSession)
	{
		printf("\nPlease enter the message: ");
    		bzero(memoryBuffer,BUFFER_MEMORY_REGULATIONS);
    		fgets(memoryBuffer,BUFFER_MEMORY_REGULATIONS ,stdin);
    		
		    printf("Sending to SERVER: %s ",memoryBuffer);

    		in = send(fileDescriptor,memoryBuffer,strlen(memoryBuffer),0);
		    if (in <= failedCase) 
		    	handleError(sendMessageSocketFailedCase);

		    bzero(memoryBuffer,BUFFER_MEMORY_REGULATIONS);
		    in = recv(fileDescriptor,memoryBuffer,BUFFER_MEMORY_REGULATIONS,0);
		    if (in <= failedCase) 
				handleError(receiveMessageSocketFailedCase);
		    
			printf("\nReceived FROM SERVER: %s ",memoryBuffer);
	}
	close(fileDescriptor);
	return 0;
}
