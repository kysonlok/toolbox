CC = gcc
CFLAGS = -g -Wall
LDFLAGS = 

PROC = parse_arg getopts getopts_long

all: $(PROC)
parse_arg: parse_arg.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

getopts: getopts.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

getopts_long: getopts_long.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm $(PROC) *.o
