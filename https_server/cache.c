#include "cache.h"

#include <string.h>

#include "debugging.h"
#include "more_memory.h"
#include "more_string.h"
#include "more_time.h"
#include "set.h"

// May want to turn into binary min heap with internal hash table
// min heap based on get time, hash table for look up of particular request
struct Cache_T {
    Set_T blocks;
    size_t capacity;
};

struct cache_block {
    // last put time used for determining freshness
    struct timeval last_put_time;
    // last get time used for evicdtion
    struct timeval last_get_time;
    HTTP_Response_T response;
    HTTP_Request_T request;
};

/*
Frees a block and all heap allocated members in the block. It is assumed any
information required from the block has already been saved.
*/
static void free_block(struct cache_block *block) {
    HTTP_Request_free(&block->request);
    HTTP_Response_free(&block->response);
    free(block);
}

// Sets both last put time and last get time to both be the current time
static void set_access_times(struct cache_block *block) {
    struct timeval curr_time = get_current_time();
    block->last_put_time = curr_time;
    block->last_get_time = curr_time;
}

// This allocates and initializes a block.
static struct cache_block *make_block(HTTP_Request_T request,
                                      HTTP_Response_T response) {
    struct cache_block *block;
    block = checked_malloc(sizeof(*block));
    set_access_times(block);
    block->request = request;
    block->response = response;
    return block;
}

static int is_stale(const struct cache_block *block) {
    /*
    Why >= here? Suppose we are at time 0 seconds and have a data that has
    MAX-AGE equal to 1. 1 second passes. Time becomes 1 second. Therefore,
    the data should now be stale. That is, when the current time >=
    expiration time, block is stale.
    */
    struct timeval curr_time = get_current_time();
    struct timeval exp_time = block->last_put_time;
    exp_time.tv_sec += block->response->max_age;
    return compare_times(&curr_time, &exp_time) >= 0;
}

static int eq_cmp(const void *x, const void *y) {
    return !HTTP_Request_equals(((const struct cache_block *)x)->request,
                                ((const struct cache_block *)y)->request);
}

static size_t hash(const void *key) {
    const struct cache_block *block = (const struct cache_block *)key;
    return http_hostname_hash(block->request->hostname) +
           13 * strhash(block->request->resource) +
           169 * block->request->portno;
}

Cache_T Cache_new(size_t capacity) {
    Cache_T cache;
    cache = checked_malloc(sizeof(*cache));
    cache->blocks = Set_new(capacity, eq_cmp, hash);
    cache->capacity = capacity;
    return cache;
}

int Cache_contains(Cache_T cache, HTTP_Request_T request) {
    struct cache_block dummy = {{0, 0}, {0, 0}, NULL, request};
    struct cache_block *block = Set_get(cache->blocks, &dummy);
    return block != NULL && !is_stale(block);
}

HTTP_Response_T Cache_get(Cache_T cache, HTTP_Request_T request,
                          struct timeval *last_put_time) {
    struct cache_block dummy = {{0, 0}, {0, 0}, NULL, request};
    struct cache_block *block = Set_get(cache->blocks, &dummy);
    if (block != NULL && !is_stale(block)) {
        block->last_get_time = get_current_time();
        *last_put_time = block->last_put_time;
        return block->response;
    }
    return NULL;
}

static void removal_finder(const void *member, void *cl) {
    struct cache_block **to_remove_ptr = (struct cache_block **)cl;
    struct cache_block *member_block = (struct cache_block *)member;
    if (*to_remove_ptr == NULL ||
        (!is_stale(*to_remove_ptr) && is_stale(member_block) ||
         (!is_stale(*to_remove_ptr) &&
          compare_times(&(member_block->last_get_time),
                        &((*to_remove_ptr)->last_get_time)) < 0))) {
        *to_remove_ptr = member_block;
    }
}

void Cache_put(Cache_T cache, HTTP_Request_T request,
               HTTP_Response_T response) {
    struct cache_block *block = make_block(request, response);
    if (Set_member(cache->blocks, block)) {
        debug("Overwriting in cache!");
        struct cache_block *old_block = Set_put(cache->blocks, block);
        free_block(old_block);
    } else {
        if (Set_length(cache->blocks) == cache->capacity) {
            struct cache_block *to_remove = NULL;
            Set_map(cache->blocks, removal_finder, &to_remove);
            Set_remove(cache->blocks, to_remove);
            debug("Removed %s:%hu:%s from cache", to_remove->request->hostname,
                  to_remove->request->portno, to_remove->request->resource);
            free_block(to_remove);
        }
        Set_put(cache->blocks, block);
    }
}

static void freer(const void *member, void *cl) {
    free_block((struct cache_block *)member);
}

void Cache_free(Cache_T *cache_ptr) {
    Set_map((*cache_ptr)->blocks, freer, NULL);
    Set_free(&(*cache_ptr)->blocks);
    free(*cache_ptr);
    *cache_ptr = NULL;
}
