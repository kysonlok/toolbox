CFLAGS+=
LDFLAGS+=-luci

PROC=test
OBJS=test.o guci.o

all : $(PROC)
$(PROC) : $(OBJS)
	gcc $^ -o $@ $(LDFLAGS)

%.o : %.c
	gcc -c $< $(CFLAGS) -o $@

.PHONY: all clean
clean:
	rm $(PROC) $(OBJS)
