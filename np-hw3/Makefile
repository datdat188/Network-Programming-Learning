TARGET=client server $(wildcard *.o)

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

normal: $(TARGET)

client: client.c
	$(CC) -o $@ $^ $(CFLAGS)

server: server.c
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean
clean:
	rm -f $(TARGET)