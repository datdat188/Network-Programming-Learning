/*
=======================================================
https://github.com/ruchirsharma1993/Chat-application-in-c-using-Socket-programming.git
=======================================================
*/

#include "utils.h"



int main()
{
	int fileDescriptor = 0;
	char message[BUFFER_MEMORY_REGULATIONS];
	char handlemessage[BUFFER_MEMORY_REGULATIONS];

	key_t keyToGetMessageQueue;
	struct msg lineMessageQueue;
	
	memset(message, '0',sizeof(message));	

	keyToGetMessageQueue = ftok(PATH_NAME, PROJECT_ID_SERVER1);
	
	int msgid = messageQueueGet(keyToGetMessageQueue);

	fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(fileDescriptor<0)
		handleError(createSocketFailedCase);

	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

	//Initialize	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_SERVER1); 
    server.sin_addr.s_addr = htonl(INADDR_ANY);
   

	if(bind(fileDescriptor, (struct sockaddr*)&server, sizeof(server)) <= failedCase)
		handleError(bindSocketFailedCase);

	printf ("Server running...\n");

	int connectionWithClient;
	
	listen(fileDescriptor, MAX_CONNECTION); 

	Signal(SIGCHLD, sig_chld);

	while(connectionWithClient = accept(fileDescriptor, (struct sockaddr*)NULL, NULL))
	{		
		int childpid, resultAction;
		if ((childpid = fork ()) == 0) 
		{
			printf ("\nClient connected\n");
			close (fileDescriptor);
	
			bzero(message,BUFFER_MEMORY_REGULATIONS);
			bzero(handlemessage,BUFFER_MEMORY_REGULATIONS);
							
			while ((resultAction = recv(connectionWithClient, message, BUFFER_MEMORY_REGULATIONS,0)) > 0)  
			{
				printf("Server Received: %s",message);
				
				char *handlemessage = upcase(message);
				send(connectionWithClient, handlemessage, strlen(handlemessage), 0);

				lineMessageQueue.msg_type = 1;
				bzero(&lineMessageQueue.mgs, BUFFER_MEMORY_REGULATIONS);
				(void)strcpy(lineMessageQueue.mgs, message);
				
				msgsnd(msgid, &lineMessageQueue, sizeof(lineMessageQueue), 0);
				printf("Line in message queue 1: %s \n", lineMessageQueue.mgs);

				wait(WAIT_TIME);

				if (endConversationWithServer(message))
				{
					printf("Closed connection");
					close(connectionWithClient);
					exit(0);
				}
		
				bzero(message,BUFFER_MEMORY_REGULATIONS);
			}

			close(connectionWithClient);
			exit(0);
		}
	}
}
