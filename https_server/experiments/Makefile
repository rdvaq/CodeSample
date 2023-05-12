CC = gcc -g
LDFLAGS = -lnsl -lssl -lcrypto

experiment2.out: experiment2.o ../tcp.o ../http.o ../proxy_testing/client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

experiment2.o: experiment2.c ../tcp.h ../http.h ../proxy_testing/client_utils.h ../more_file.h

connect_ddos.out: connect_ddos.o ../tcp.o ../http.o ../proxy_testing/client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

connect_ddos.o: connect_ddos.c ../tcp.h ../http.h ../proxy_testing/client_utils.h ../more_file.h

spam_dos.out: spam_dos.o ../tcp.o ../http.o ../proxy_testing/client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

spam_dos.o: spam_dos.c ../tcp.h ../http.h ../proxy_testing/client_utils.h ../more_file.h

client_vector_overflow.out: client_vector_overflow.o ../tcp.o ../http.o ../proxy_testing/client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

client_vector_overflow.o: client_vector_overflow.c ../tcp.h ../http.h ../proxy_testing/client_utils.h ../more_file.h

simpleclient.out: simpleclient.o ../tcp.o ../http.o ../proxy_testing/client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

simpleclient.o: simpleclient.c ../tcp.h ../http.h ../proxy_testing/client_utils.h ../more_file.h