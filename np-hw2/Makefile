src = $(wildcard *.c)
obj = $(src:.c=.o)

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall -Wno-unknown-pragmas -std=c99 -D_GNU_SOURCE

1712328: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) 1712328