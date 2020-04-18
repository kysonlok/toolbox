CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

PROC=test

all : $(PROC)
	@echo "compile done"

test : test.o json_api.o
	gcc $^ -o $@ $(LDFLAGS)

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@ 

clean:
	rm -fr $(PROC) $(OBJS)
