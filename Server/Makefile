CC = gcc
CFlags = -Wall -c
LDFlags = -lm
Objs = server.o

all: server

server: $(Objs)
	$(CC) $(LDFlags) $< -lclargparse -o $@

%.o: %.c
	$(CC) $(CFlags) $< -o $@

.PHONY: clean
clean:
	rm -f server $(Objs)
