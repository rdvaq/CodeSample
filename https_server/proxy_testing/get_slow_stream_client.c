#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

// 4 gets are made from same client, then it CONNECTs and sends another GET
// request through the tunnel. here we use our server when it has been set up 
// to respond slowly, this makes 

int main() {
    char client1_request1[] =
        "GET /file01 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client1_request2[] =
        "GET /file02 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client1_request3[] =
        "GET /file03 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client1_request4[] =
        "GET /file04 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client1_request5[] =
        "CONNECT localhost:9087 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";
    char client1_request6[] =
        "GET /file05 HTTP/1.1\r\nHost: localhost:9087\r\n\r\n";

    int client1 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request1, strlen(client1_request1));
    checked_write(client1, client1_request2, strlen(client1_request2));
    sleep(1);
    checked_write(client1, client1_request3, strlen(client1_request3));
    checked_write(client1, client1_request4, strlen(client1_request4));
    sleep(1);
    checked_write(client1, client1_request5, strlen(client1_request5));
    checked_write(client1, client1_request6, strlen(client1_request6));
    HTTP_Response_T client1_response1 = block_until_response(client1);
    fprintf(stderr, "got response 1");
    HTTP_Response_T client1_response2 = block_until_response(client1);
    fprintf(stderr, "got response 2");
    HTTP_Response_T client1_response3 = block_until_response(client1);
    fprintf(stderr, "got response 3");
    HTTP_Response_T client1_response4 = block_until_response(client1);
    fprintf(stderr, "got response 4");
    HTTP_Response_T client1_response5 = block_until_response(client1);
    fprintf(stderr, "got response 5");
    HTTP_Response_T client1_response6 = block_until_response(client1);
    fprintf(stderr, "got response 6");
    close(client1);
    int client1_fd1 = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd1, client1_response1->body, client1_response1->content_len);
    close(client1_fd1);
    HTTP_Response_free(&client1_response1);
    int client1_fd2 = checked_open_for_writing("out_files/proxy_response2");
    write(client1_fd2, client1_response2->body, client1_response2->content_len);
    close(client1_fd2);
    HTTP_Response_free(&client1_response2);
    int client1_fd3 = checked_open_for_writing("out_files/proxy_response3");
    write(client1_fd3, client1_response3->body, client1_response3->content_len);
    close(client1_fd3);
    HTTP_Response_free(&client1_response3);
    int client1_fd4 = checked_open_for_writing("out_files/proxy_response4");
    write(client1_fd4, client1_response4->body, client1_response4->content_len);
    close(client1_fd4);
    HTTP_Response_free(&client1_response4);
    int client1_fd5 = checked_open_for_writing("out_files/proxy_response5");
    write(client1_fd5, client1_response5->header,
          client1_response5->header_len);
    close(client1_fd5);
    HTTP_Response_free(&client1_response5);
    int client1_fd6 = checked_open_for_writing("out_files/proxy_response6");
    write(client1_fd6, client1_response6->body, client1_response6->content_len);
    close(client1_fd6);
    HTTP_Response_free(&client1_response6);
    return 0;
}