#CFLAGS += $(shell pkg-config --cflags json-c)
#LDFLAGS += $(shell pkg-config --libs json-c) -luci
LDFLAGS +=-ljson-c -luci

SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

PROC=shadowsock_api

all : $(PROC)
	@echo "compile done"

$(PROC) : $(OBJS)
	gcc $^ -o $@ $(LDFLAGS)

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@ 

clean:
	rm -fr $(PROC) $(OBJS)
