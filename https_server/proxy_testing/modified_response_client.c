#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

int main() {
    char client1_request1[] =
        "GET /testcat_ac.html HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";
    char client1_request2[] =
        "GET /testcat_al.html HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";
    char client1_request3[] =
        "GET /index.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request1, strlen(client1_request1));
    HTTP_Response_T client1_response1 = block_until_response(client1);
    checked_write(client1, client1_request2, strlen(client1_request2));
    HTTP_Response_T client1_response2 = block_until_response(client1);
    checked_write(client1, client1_request3, strlen(client1_request3));
    HTTP_Response_T client1_response3 = block_until_response(client1);
    close(client1);
    int client1_fd1 = checked_open_for_writing("out_files/proxy_response1.html");
    write(client1_fd1, client1_response1->body, client1_response1->content_len);
    close(client1_fd1);
    HTTP_Response_free(&client1_response1);
    int client1_fd2 = checked_open_for_writing("out_files/proxy_response2.html");
    write(client1_fd2, client1_response2->body, client1_response2->content_len);
    close(client1_fd2);
    HTTP_Response_free(&client1_response2);
    int client1_fd3 = checked_open_for_writing("out_files/proxy_response3.html");
    write(client1_fd3, client1_response3->body, client1_response3->content_len);
    close(client1_fd3);
    HTTP_Response_free(&client1_response3);
    return 0;
}