#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

int main() {
    char client1_request[] =
        "GET /file01 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client2_request[] =
        "GET /file01 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client3_request[] =
        "GET /file01 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client4_halt[] = "HALT\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    int client2 = connect_to_server("localhost", 9086);
    int client3 = connect_to_server("localhost", 9086);
    int client4 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request, strlen(client1_request));
    HTTP_Response_T client1_response = block_until_response(client1);
    sleep(5);
    checked_write(client2, client2_request, strlen(client2_request));
    HTTP_Response_T client2_response = block_until_response(client2);
    sleep(7);
    checked_write(client3, client3_request, strlen(client3_request));
    HTTP_Response_T client3_response = block_until_response(client3);
    checked_write(client4, client4_halt, strlen(client4_halt));
    close(client1);
    close(client2);
    close(client3);
    close(client4);
    int client1_fd = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd, client1_response->body, client1_response->content_len);
    close(client1_fd);
    int client2_fd = checked_open_for_writing("out_files/proxy_response2");
    write(client2_fd, client2_response->body, client2_response->content_len);
    close(client2_fd);
    int client3_fd = checked_open_for_writing("out_files/proxy_response3");
    write(client3_fd, client3_response->body, client3_response->content_len);
    close(client3_fd);
    HTTP_Response_free(&client1_response);
    HTTP_Response_free(&client2_response);
    HTTP_Response_free(&client3_response);
    return 0;
}