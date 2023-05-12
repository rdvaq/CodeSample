#include "../tcp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../more_memory.h"

#define WRITE_SIZE 1000000000

int main() {
    char *bytes = checked_malloc(WRITE_SIZE);
    memset(bytes, 'a', WRITE_SIZE);

    int client = connect_to_server("localhost", 9086);
    
    size_t total = 0;

    while (1) {
        printf("%zu bytes written successfully thus far\n", total);
        checked_write(client, bytes, WRITE_SIZE);        
        total += WRITE_SIZE;
    }

    free(bytes);

    return 0;
}