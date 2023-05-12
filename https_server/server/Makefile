CC = gcc
LDFLAGS = -lnsl -lssl -lcrypto

a.out: server.o ../errors.o ../more_file.o ../more_memory.o ../tcp.o ../http.o ../char_vector.o ../more_string.o ../more_time.o ../debugging.o  ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

server.o: server.c ../errors.h ../more_file.h ../more_memory.h ../tcp.h ../http.h ../char_vector.h ../more_string.h ../debugging.h