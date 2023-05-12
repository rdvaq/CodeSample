#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "proxy.h"

int main(int argc, char *argv[]) {
    if (argc != 9) {
        printf(
            "Usage:\n./a.out <port> <cache ram capacity> <banned hosts file> "
            "<byte total> <rate time> <modify responses> <unused server socket "
            "capacity> <http mode>\n");
        return 1;
    }

    Proxy_T proxy =
        Proxy_new((unsigned short)atoi(argv[1]), atoi(argv[2]), argv[3],
                  (size_t)atoi(argv[4]), (time_t)atoi(argv[5]), atoi(argv[6]),
                  atoi(argv[7]), atoi(argv[8]));
    Proxy_start(proxy);
    Proxy_free(&proxy);
    return 0;
}