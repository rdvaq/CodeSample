#include "../tcp.h"

#include <unistd.h>

// Check on server side that a record is being booted

int main() {
    char client1_request[] = "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    int client = connect_to_server("localhost", 9086);
    checked_write(client, client1_request, 10);
    sleep(1);
    close(client);

    return 0;
}