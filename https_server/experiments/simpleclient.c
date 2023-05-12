#include "../tcp.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    fprintf(stderr, "connection attempted\n");
    int client = connect_to_server("localhost", 9086);
    sleep(5);
    char *s = "GET http://www.cs.tufts.edu/comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    checked_write(client, s, strlen(s));
    clean_close(client, NULL);
    return 0;
}