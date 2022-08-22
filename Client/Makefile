CC = gcc
CFlags = -Wall -c
LDFlags = -lm
Objs = client.o

all: client

client: $(Objs)
	$(CC) $(LDFlags) $< -lclargparse -o $@

%.o: %.c
	$(CC) $(CFlags) $< -o $@

.PHONY: clean
clean:
	rm -f client $(Objs)
