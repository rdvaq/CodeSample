#ifndef _TCP_H
#define _TCP_H

#include <stddef.h>
#include <sys/select.h>

// Initializes and binds a server socket to a certain port
extern int init_server_socket(unsigned short portno);

// Accepts a client connecting on a listening socket
extern int get_incoming_client(int server_sd);

// Connects a client to a particular host and port number
extern int connect_to_server(char *host, unsigned short portno);

// Performs a write on a socket, if write fails program aborts
extern void checked_write(int socket, char *buf, size_t len);

// Performs a read on a socket, if read fails program aborts
extern size_t checked_read(int socket, char *buf, size_t buf_len);

// Cleaner version of close. If socket is not being selected on, pass NULL
extern void clean_close(int socket, fd_set *select_set);

#endif