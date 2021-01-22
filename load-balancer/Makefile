src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
CC=/usr/bin/gcc
CFLAGS=-g -Wall -pthread -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wall 

main_lb_srv: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)
.PHONY: clean
clean:
	rm -f main_lb_srv $(obj)