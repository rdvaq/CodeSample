#ifndef _MORE_TIME_H
#define _MORE_TIME_H

#include <sys/time.h>

// Subtracts time2 from time1 and returns a timeval. If time2 occurred after
// time1, a 0 timeval is returned
extern struct timeval subtract_times(const struct timeval *time1,
                                     const struct timeval *time2);

// Get current time
extern struct timeval get_current_time();

// Compares two times like strcmp
extern int compare_times(const struct timeval *time1, const struct timeval *time2);

#endif