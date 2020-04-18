CC=gcc
CFLAGS=-g
LDFLAGS=-luci -lubox -lblobmsg_json

PROC=test_ubox
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(PROC)

$(PROC):$(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(PROC) $(OBJS)
