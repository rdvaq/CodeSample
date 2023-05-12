#include "more_time.h"

#include <stddef.h>

static const suseconds_t MICROSEC_PER_SEC = 1000000;

extern struct timeval get_current_time() {
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    return curr_time;
}

extern struct timeval subtract_times(const struct timeval *time1,
                                     const struct timeval *time2) {
    struct timeval difference;
    if (compare_times(time2, time1) > 0) {
        // time2 > time1 - 0 timeval
        difference.tv_sec = 0;
        difference.tv_usec = 0;
    } else if (time1->tv_usec >= time2->tv_usec) {
        // Both time1 members bigger => difference is subtraction of both
        // members
        difference.tv_usec = time1->tv_usec - time2->tv_usec;
        difference.tv_sec = time1->tv_sec - time2->tv_sec;
    } else {
        // time1 bigger but time1 microseconds < time2 microseconds, we do
        // "carry" by adding MICROSEC_PER_SEC to time1 microseconds then
        // subtract microseconds. Then, subtract 1 from seconds difference
        difference.tv_usec = MICROSEC_PER_SEC + time1->tv_usec - time2->tv_usec;
        difference.tv_sec = time1->tv_sec - time2->tv_sec - 1;
    }
    return difference;
}

extern int compare_times(const struct timeval *time1,
                         const struct timeval *time2) {
    // If seconds match, compare on microseconds, otherwise compare seconds
    return (time1->tv_sec == time2->tv_sec) ? time1->tv_usec - time2->tv_usec
                                            : time1->tv_sec - time2->tv_sec;
}