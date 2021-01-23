src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
CC=/usr/bin/g++
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

proxy_lb_main: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)
	rm -f $(obj)

.PHONY: clean
clean:
	rm -f proxy_lb_main $(obj)