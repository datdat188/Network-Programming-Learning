#include "utils.h"


int main(int argc, char const *argv[])
{
    /* create message queue 1*/
    struct msg lineMessageQueue;

    key_t keyToGetMessageQueue;
    keyToGetMessageQueue = ftok(PATH_NAME, PROJECT_ID_SERVER1);
    
    int msgid = messageQueueGet(keyToGetMessageQueue);
    printf("Server running...\n");

    while(activeSession)
	{
		bzero(&lineMessageQueue,BUFFER_MEMORY_REGULATIONS);
        msgrcv(msgid, &lineMessageQueue,sizeof(lineMessageQueue), 1, 0);
    		
        printf("Line message queue, Received FROM NETDSERVER: %s\n", lineMessageQueue.mgs);

		if (endConversationWithServer(lineMessageQueue.mgs))
		{
			printf("Closed connection");
            msgctl(msgid, IPC_RMID, NULL); 
			exit(0);
		}
	}
    
    msgctl(msgid, IPC_RMID, NULL); 
    return 0;
}