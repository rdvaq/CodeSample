#ifndef _PROXY_H
#define _PROXY_H

#include <stddef.h>
#include <time.h>

#define T Proxy_T
typedef struct Proxy_T* Proxy_T;

// Initializes proxy that will be bound to a certain port with certain cache RAM
// capacity
extern Proxy_T Proxy_new(unsigned short portno, size_t cache_ram_capacity,
                         const char* banned_sites, size_t total_bytes,
                         time_t time_limit, int modify_responses,
                         size_t unused_server_socket_capacity, int http_mode);

// Has proxy start listening for connections and service clients
extern void Proxy_start(Proxy_T proxy);

// Clean up proxy
extern void Proxy_free(Proxy_T* proxy_p);

#undef T
#endif