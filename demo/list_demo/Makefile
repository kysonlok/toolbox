CC=gcc
CFLAGS+=
LDFLAGS+=

PROC=list_test
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(PROC)
	@echo "compile done"

$(PROC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(PROC) $(OBJS)

.PHONY: all clean
