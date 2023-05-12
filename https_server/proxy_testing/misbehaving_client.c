#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

// Misbehaving client - first sends bad request, banned request, then a real one
// all in a stream

int main() {
    char client1_request1[] =
        "GEwT index.html HTTP/1.1\r\nHost: www.badwebsite.com\r\n\r\n";
    char client1_request2[] =
        "GET index.html HTTP/1.1\r\nHost: www.badwebsite.com\r\n\r\n";
    char client1_request3[] =
        "CONNECT www.badwebsite.com HTTP/1.1\r\nHost: "
        "www.badwebsite.com\r\n\r\n";
    char client1_request4[] =
        "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    int client1 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request1, strlen(client1_request1));
    HTTP_Response_T client1_response1 = block_until_response(client1);
    checked_write(client1, client1_request2, strlen(client1_request2));
    HTTP_Response_T client1_response2 = block_until_response(client1);
    checked_write(client1, client1_request3, strlen(client1_request3));
    HTTP_Response_T client1_response3 = block_until_response(client1);
    checked_write(client1, client1_request4, strlen(client1_request4));
    HTTP_Response_T client1_response4 = block_until_response(client1);
    close(client1);
    int client1_fd1 = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd1, client1_response1->header,
          client1_response1->header_len);
    close(client1_fd1);
    HTTP_Response_free(&client1_response1);
    int client1_fd2 = checked_open_for_writing("out_files/proxy_response2");
    write(client1_fd2, client1_response2->header,
          client1_response2->header_len);
    close(client1_fd2);
    HTTP_Response_free(&client1_response2);
    int client1_fd3 = checked_open_for_writing("out_files/proxy_response3");
    write(client1_fd3, client1_response3->header,
          client1_response3->header_len);
    close(client1_fd3);
    HTTP_Response_free(&client1_response3);
    int client1_fd4 = checked_open_for_writing("out_files/proxy_response4");
    write(client1_fd4, client1_response4->body, client1_response4->content_len);
    close(client1_fd4);
    HTTP_Response_free(&client1_response4);
    return 0;
}