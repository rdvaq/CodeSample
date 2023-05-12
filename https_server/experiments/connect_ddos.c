#include "../tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include "../debugging.h"

int main(int argc, char *argv[]) {
    int num_sockets = atoi(argv[1]);
    int i;
    int sockets_created = 0; 
    for (i = 0; i < num_sockets; i++) {
        int socket = connect_to_server("localhost", 9086);
        if (socket < 0) {
            break;
        }
        debug("created socket %d", socket);
        sockets_created++;

    }
    debug("%d sockets created before gethostbyname failure", sockets_created);
    while (1) {}
    return 0; 
}