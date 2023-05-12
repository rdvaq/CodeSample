#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

int main() {
    char client1_request[] =
        "GET /testcat_ac.html HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";
    char client2_request[] =
        "GET /testcat_al.html HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";
    char client3_halt[] = "HALT\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    int client2 = connect_to_server("localhost", 9086);
    int client3 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request, strlen(client1_request));
    HTTP_Response_T client1_response = block_until_response(client1);
    checked_write(client2, client2_request, strlen(client2_request));
    HTTP_Response_T client2_response = block_until_response(client2);
    checked_write(client3, client3_halt, strlen(client3_halt));
    close(client1);
    close(client2);
    close(client3);
    int client1_fd = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd, client1_response->body, client1_response->content_len);
    close(client1_fd);
    int client2_fd = checked_open_for_writing("out_files/proxy_response2");
    write(client2_fd, client2_response->body, client2_response->content_len);
    close(client2_fd);
    HTTP_Response_free(&client1_response);
    HTTP_Response_free(&client2_response);
    return 0;
}