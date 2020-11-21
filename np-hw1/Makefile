src = $(wildcard *.c)
obj = $(src:.c=.o)

CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings

1712328: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) 1712328