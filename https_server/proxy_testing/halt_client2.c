#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

int main() {
    char client1_request[] =
        "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    char client4_halt[] = "HALT\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    int client4 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request, strlen(client1_request));
    HTTP_Response_T client1_response = block_until_response(client1);
    checked_write(client4, client4_halt, strlen(client4_halt));
    close(client1);
    close(client4);
    int client1_fd = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd, client1_response->body, client1_response->content_len);
    close(client1_fd);
    HTTP_Response_free(&client1_response);
    return 0;
}