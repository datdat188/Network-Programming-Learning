#include "utils.h"

struct msg message1;

int main(int argc, char const *argv[])
{
    int fileDescriptor = 0;
	char message[BUFFER_MEMORY_REGULATIONS];

    memset(message, '0',sizeof(message));

	key_t keyToGetMessageQueue;
	struct msg lineMessageQueue;

	keyToGetMessageQueue = ftok(PATH_NAME, PROJECT_ID_SERVER1);
	
	int msgid = messageQueueGet(keyToGetMessageQueue);

    fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(fileDescriptor < 0)
		handleError(createSocketFailedCase);

	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 

    //Initialize	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_NETD); 
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    bzero(&server.sin_zero, 8);

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
            printf ("NETDCLIENT connected\n");
			close (fileDescriptor);
	
			bzero(message,BUFFER_MEMORY_REGULATIONS);
							
			while ((resultAction = recv(connectionWithClient, message, BUFFER_MEMORY_REGULATIONS,0)) > 0)  
			{
				lineMessageQueue.msg_type = 1;
				bzero(&lineMessageQueue.mgs, BUFFER_MEMORY_REGULATIONS);
				(void)strcpy(lineMessageQueue.mgs, message);
				
				msgsnd(msgid, &lineMessageQueue, sizeof(lineMessageQueue), 0);

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

    return 0;
}