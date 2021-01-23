TARGET=netdclient netdsrv client $(wildcard *.o)
CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 
normal: $(TARGET)
netdclient: netdclient.c
	$(CC) -o $@ $^ $(CFLAGS)

netdsrv: netdsrv.c 
	$(CC) -o $@ $^ $(CFLAGS)

client: client.cpp
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)