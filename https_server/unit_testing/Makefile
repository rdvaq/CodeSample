src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc -g
LDFLAGS = -lnsl

a.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) a.out
