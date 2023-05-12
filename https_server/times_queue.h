#ifndef _TIMES_QUEUE_H
#define _TIMES_QUEUE_H

#include <stddef.h>
#include <sys/time.h>

// Queue (sliding window) used for rate limiting
typedef struct TimesQueue_T *TimesQueue_T;

// Creates a new queue that will enforce at most byte_limit bytes can be
// transferred within time_limit
extern TimesQueue_T TimesQueue_new(size_t byte_limit,
                                   struct timeval *time_limit);

// Checks if bytes will violate the rate limit imposed by queue. If bytes is
// acceptable, it is added into the queue registered with current time and 1 is
// returned. 0 is returned if the bytes addition goes over the limit. None of
// the bytes are added into the queue.
extern int TimesQueue_push(TimesQueue_T queue, size_t bytes);

// Frees queue
extern void TimesQueue_free(TimesQueue_T *queue_p);

#endif