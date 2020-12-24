/* Reference
=======================================================
https://github.com/ruchirsharma1993/Chat-application-in-c-using-Socket-programming.git
=======================================================
*/

#include "utils.h"

int main(int argc, char *argv[])
{
	int fileDescriptor = 0;
	char message[BUFFER_MEMORY_REGULATIONS];

	memset(message, '0',sizeof(message));	

	fileDescriptor = createSocket();
    if(fileDescriptor <= failedCase)
		handleError(createSocketFailedCase);

	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

	server.sin_family = AF_INET;
	server.sin_port =  htons(atoi(PORT_SERVER1));
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int in = connect(fileDescriptor, (struct sockaddr *)&server, sizeof(server));
	if(in <= failedCase)
		handleError(connectSocketFailedCase); 	

	while(activeSession)
	{
		printf("\nPlease enter the message: ");
		bzero(message,BUFFER_MEMORY_REGULATIONS);
    	fgets(message,BUFFER_MEMORY_REGULATIONS ,stdin);
    		
		printf("Sending to SERVER: %s ",message);

    	in = send(fileDescriptor,message,strlen(message),0);
		if (in <= failedCase) 
			handleError(sendMessageSocketFailedCase);

		if (endConversationWithServer(message))
		{
			printf("Closed connection");
			close(fileDescriptor);
			exit(0);
		}

		wait(WAIT_TIME);

	    bzero(message,BUFFER_MEMORY_REGULATIONS);
	    in = recv(fileDescriptor,message,BUFFER_MEMORY_REGULATIONS,0);
		if (in <= failedCase) 
			handleError(receiveMessageSocketFailedCase);
	    
		printf("\nReceived FROM SERVER: %s ",message);
	}
	close(fileDescriptor);
	return 0;
}
