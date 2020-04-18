CC=gcc
CFLAGS +=
LDFLAGS +=

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
PROC=unittest

all: $(PROC)
	@echo "compile done!"

$(PROC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -fr $(PROC) $(OBJS)
