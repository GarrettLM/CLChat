CC = gcc
CFlags = -Wall -c
LDFlags = -lm
Objs = server.o

all: server

client: $(Objs)
	$(CC) $(LDFlags) $^ -o $@

%.o: %.c
	$(CC) $(CFlags) $< -o $@

.PHONY: clean
clean:
	rm -f server $(Objs)
