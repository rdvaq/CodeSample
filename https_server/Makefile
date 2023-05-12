src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc -g
CFLAGS = -O2
LDFLAGS = -lssl -lcrypto
# -lnsl add back later

a.out: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) a.out
