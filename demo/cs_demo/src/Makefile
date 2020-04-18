CC = gcc
CFLAGS = -g -Wall
#LDFLAGS = -lev -lpthread

PROC = ctl_server ctl_client
all: $(PROC)
ctl_server: ctl_server.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

ctl_client: ctl_client.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm *.o $(PROC)
