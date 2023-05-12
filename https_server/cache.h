#ifndef _CACHE_H
#define _CACHE_H

#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>

#include "http.h"

// Cache type - users should use this. Singly linked list that stores up to a
// capacity (i.e. all RAM)
typedef struct Cache_T *Cache_T;

// Initializes cache of a certain capacity
extern Cache_T Cache_new(size_t capacity);

// Gets response stored in cache corresponding to a particular request.
// last_put_time is set to the put time of the response in the cache. If the
// response stored in the cache is stale or doesn't exist NULL is returned and
// last_put_time is left untouched.
extern HTTP_Response_T Cache_get(Cache_T cache, HTTP_Request_T request,
                                 struct timeval *last_put_time);

// Puts (request, response) into the cache. If request already exists, old
// response is freed
extern void Cache_put(Cache_T cache, HTTP_Request_T request,
                      HTTP_Response_T response);

// Frees cache
extern void Cache_free(Cache_T *cache_ptr);

// Checks if the cache contains the resource associated with a particular host
// and port number packaged into a request. This should not be used to implement
// a GET from the cache, but more a cache check (i.e. by HTML editing). It does
// not modify the corresponding entry's last get time.
int Cache_contains(Cache_T cache, HTTP_Request_T request);

#endif