PROC=zlib_example
OBJS=zlib_example.o zlib_api.o
CFLAGS=
LDFLAGS=-lz

all : $(PROC)
$(PROC) : $(OBJS)
	gcc $^ -o $@  $(LDFLAGS)

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm $(PROC) $(OBJS)
