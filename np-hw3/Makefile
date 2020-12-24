TARGET=client server1 server2 netdclient netdsrv $(wildcard *.o)

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

normal: $(TARGET)

client: client.c
	$(CC) -o $@ $^ $(CFLAGS)

server1: server1.c
	$(CC) -o $@ $^ $(CFLAGS)

server2: server2.c
	$(CC) -o $@ $^ $(CFLAGS)

netdclient: netdclient.c
	$(CC) -o $@ $^ $(CFLAGS)

netdsrv: netdsrv.c 
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)