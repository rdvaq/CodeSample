#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

int main() {
    char client1_request1[] =
        "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    char client1_request2[] =
        "GET /~prs/bio.html HTTP/1.1\r\nHost: www.cs.cmu.edu\r\n\r\n";
    char client4_halt[] = "HALT\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    int client4 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request1, strlen(client1_request1));
    HTTP_Response_T client1_response1 = block_until_response(client1);
    checked_write(client1, client1_request2, strlen(client1_request2));
    HTTP_Response_T client1_response2 = block_until_response(client1);
    checked_write(client4, client4_halt, strlen(client4_halt));
    close(client1);
    close(client4);
    int client1_fd = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd, client1_response1->body, client1_response1->content_len);
    close(client1_fd);
    HTTP_Response_free(&client1_response1);
    int client2_fd = checked_open_for_writing("out_files/proxy_response2");
    write(client2_fd, client1_response2->body, client1_response2->content_len);
    close(client2_fd);
    HTTP_Response_free(&client1_response2);
    return 0;
}