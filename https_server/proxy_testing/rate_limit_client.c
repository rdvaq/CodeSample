#include <stdio.h>
#include <unistd.h>

#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"
#include "client_utils.h"

// Similar to get_stream_client but here we actually run into too many requests

int main() {
    char client1_request1[] =
        "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    char client1_request2[] =
        "GET /index.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n";
    char client1_request3[] =
        "GET /~prs/bio.html HTTP/1.1\r\nHost: www.cs.cmu.edu\r\n\r\n";
    char client1_request4[] =
        "CONNECT www.testingmcafeesites.com HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";
    char client1_request5[] =
        "GET /testcat_al.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n";
    char client1_request6[] =
        "GET /testcat_ac.html HTTP/1.1\r\nHost: "
        "www.testingmcafeesites.com\r\n\r\n";

    // printf("Sizes: %zu %zu %zu %zu %zu %zu\n", strlen(client1_request1),
    //        strlen(client1_request2), strlen(client1_request3),
    //        strlen(client1_request4), strlen(client1_request5),
    //        strlen(client1_request6));

    int client1 = connect_to_server("localhost", 9086);
    checked_write(client1, client1_request1, strlen(client1_request1));
    HTTP_Response_T client1_response1 = block_until_response(client1);
    checked_write(client1, client1_request2, strlen(client1_request2));
    HTTP_Response_T client1_response2 = block_until_response(client1);
    checked_write(client1, client1_request3, strlen(client1_request3));
    HTTP_Response_T client1_response3 = block_until_response(client1);
    sleep(1);
    checked_write(client1, client1_request4, strlen(client1_request4));
    HTTP_Response_T client1_response4 = block_until_response(client1);
    checked_write(client1, client1_request5, strlen(client1_request5));
    HTTP_Response_T client1_response5 = block_until_response(client1);
    checked_write(client1, client1_request6, strlen(client1_request6));
    HTTP_Response_T client1_response6 = block_until_response(client1);
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
    write(client1_fd3, client1_response3->header,
          client1_response3->header_len);
    close(client1_fd3);
    HTTP_Response_free(&client1_response3);
    int client1_fd4 = checked_open_for_writing("out_files/proxy_response4");
    write(client1_fd4, client1_response4->header,
          client1_response4->header_len);
    close(client1_fd4);
    HTTP_Response_free(&client1_response4);
    int client1_fd5 = checked_open_for_writing("out_files/proxy_response5");
    write(client1_fd5, client1_response5->body, client1_response5->content_len);
    close(client1_fd5);
    HTTP_Response_free(&client1_response5);
    int client1_fd6 = checked_open_for_writing("out_files/proxy_response6");
    write(client1_fd6, client1_response6->header,
          client1_response6->header_len);
    close(client1_fd6);
    HTTP_Response_free(&client1_response6);
    return 0;
}