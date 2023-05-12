#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../tcp.h"

#define WRITE_SIZE 2001

#include <time.h>
#include <errno.h>    

/* msleep(): Sleep for the requested number of milliseconds. */
// Taken from here: https://stackoverflow.com/a/1157217
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main() {
    char bytes[WRITE_SIZE];
    memset(bytes, 'a', WRITE_SIZE);

    int client = connect_to_server("localhost", 9086);
    while (1) {
        checked_write(client, bytes, WRITE_SIZE);
        // msleep(1);
    }
    return 0;
}