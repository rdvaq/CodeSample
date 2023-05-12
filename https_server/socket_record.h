#ifndef _SOCKET_RECORD_H
#define _SOCKET_RECORD_H

#include <stddef.h>
#include <sys/time.h>

typedef struct ServerSocketTable_T *ServerSocketTable_T;

// Initializes a table that will hold up to capacity open, unused server sockets
extern ServerSocketTable_T ServerSocketTable_new(size_t capacity);

// Adds a new unused socket associated with a host and port number. If space had
// to be made for it, the removed socket is returned. It needs to be closed by
// the caller. If it could be added with no removal, -1 is returned.
extern int ServerSocketTable_add(ServerSocketTable_T table, char *host,
                                 unsigned short portno, int socket);

// Removes a socket from the unused table for use by the caller. If no socket
// could be found for host and portno, -1 is returned.
extern int ServerSocketTable_remove(ServerSocketTable_T table, char *host,
                                    unsigned short portno);

// Frees table
extern void ServerSocketTable_free(ServerSocketTable_T *table_ptr);

#endif