CC=gcc
CFLAGS=-g
LDFLAGS=-lubox

PROC=avl_demo
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(PROC)

$(PROC):$(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(PROC) $(OBJS)
