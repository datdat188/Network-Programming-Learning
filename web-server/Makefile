TARGET= web_server $(wildcard *.o)
C_FILES := $(wildcard *.c)
HEADER_FILES := $(wildcard *.h)
OBJ_FILES := $(patsubst %.c,%.o,$(C_FILES))

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

normal: $(TARGET)

all: web_server

%.o: %.c $(HEADER_FILES)
	$(CC) -c $<

web_server: $(OBJ_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)