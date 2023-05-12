CC = gcc
LDFLAGS = -lnsl -lssl -lcrypto

interleaved_clients.out: test02.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o  ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

test02.o: test02.c ../tcp.h ../http.h client_utils.h ../more_file.h

early_leaving_client.out: test03.o ../tcp.o ../errors.o
	$(CC) -o $@ $^ $(LDFLAGS)

test03.o: test03.c ../tcp.h

client_utils.o: client_utils.c ../more_math.h ../http.h ../tcp.h ../more_file.h 

conn_client.out: conn_client.o
	$(CC) -o $@ $^ $(LDFLAGS)

halt_client.out: halt_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

halt_client.o: halt_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

misbehaving_client.out: misbehaving_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o  ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

misbehaving_client.o: misbehaving_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

halt_client2.out: halt_client2.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

halt_client2.o: halt_client2.c ../tcp.h ../http.h client_utils.h ../more_file.h

get_stream_client.out: get_stream_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

get_stream_client.o: get_stream_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

rate_limit_client.out: rate_limit_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

rate_limit_client.o: rate_limit_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

modified_response_client.out: modified_response_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

modified_response_client.o: modified_response_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

get_slow_stream_client.out: get_slow_stream_client.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

get_slow_stream_client.o: get_slow_stream_client.c ../tcp.h ../http.h client_utils.h ../more_file.h

halt_client3.out: halt_client3.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

halt_client3.o: halt_client3.c ../tcp.h ../http.h client_utils.h ../more_file.h

closing_server.out: closing_server.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

closing_server.o: closing_server.c ../tcp.h ../http.h client_utils.h ../more_file.h

non_closing_server.out: non_closing_server.o ../tcp.o ../http.o client_utils.o ../more_file.o ../char_vector.o ../errors.o ../more_memory.o ../more_string.o ../more_time.o ../debugging.o ../SSL.o
	$(CC) -o $@ $^ $(LDFLAGS)

non_closing_server.o: closing_server.c ../tcp.h ../http.h client_utils.h ../more_file.h