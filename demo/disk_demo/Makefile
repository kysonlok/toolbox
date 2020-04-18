CC=gcc
CFLAGS+=
LDFLAGS+=

PROC=disk_info
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
