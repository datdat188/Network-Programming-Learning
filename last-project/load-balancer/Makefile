TARGET= load_balancer_main $(wildcard *.o)
C_FILES := $(wildcard *.c)
HEADER_FILES := $(wildcard *.h)
OBJ_FILES := $(patsubst %.c,%.o,$(C_FILES))

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

normal: $(TARGET)

all: load_balancer_main

%.o: %.c $(HEADER_FILES)
	$(CC) -c $<

load_balancer_main: $(OBJ_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(CFLAGS)

netdclient: netdclient.c
	$(CC) -o $@ $^ $(CFLAGS)

netdsrv: netdsrv.c 
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)
