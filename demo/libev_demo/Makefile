CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lev -lpthread

PROC = server client
all: $(PROC)
server: server.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

client: client.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm *.o $(PROC)
