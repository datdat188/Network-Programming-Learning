#include "utils.h"


int main(int argc, char const *argv[])
{
    int fileDescriptor;
    struct sockaddr_in server;

    struct msg lineMessageQueue;

    key_t keyToGetMessageQueue;
    keyToGetMessageQueue = ftok(PATH_NAME, PROJECT_ID_SERVER1);
    
    int msgid = messageQueueGet(keyToGetMessageQueue);

    fileDescriptor = createSocket();
    if(fileDescriptor <= failedCase)
		handleError(createSocketFailedCase);
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT_NETD);
    bzero(&server.sin_zero, 8);

    if(connect(fileDescriptor, (struct sockaddr*)& server, sizeof(server)) < 0)
        handleError(connectSocketFailedCase);

    printf("Connected\n");
    
    while(activeSession)
	{
		bzero(&lineMessageQueue,BUFFER_MEMORY_REGULATIONS);
        msgrcv(msgid, &lineMessageQueue,sizeof(lineMessageQueue), 1, 0);

    	int resultAction = send(fileDescriptor, lineMessageQueue.mgs, strlen(lineMessageQueue.mgs),0);
        
		if (resultAction <= failedCase) 
			handleError(sendMessageSocketFailedCase);

		if (endConversationWithServer(lineMessageQueue.mgs))
		{
			printf("Closed connection");
			close(fileDescriptor);
            msgctl(msgid, IPC_RMID, NULL); 
			exit(0);
		}
	}
    
    msgctl(msgid, IPC_RMID, NULL); 
    close(fileDescriptor);

    return 0;
}
