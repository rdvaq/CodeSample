#include <stdio.h>

#include "../tcp.h"
#include "../http.h"
#include "client_utils.h"
#include "../more_file.h"
#include <unistd.h>

int main() {
    // this will be program that interleaves requests between multiple clients
    // these requests are taken from running curl -v
    int chunk_sizes[] = {20, 15};
    char client1_request[] = "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n";
    char client2_request[] = "GET /~prs/bio.html HTTP/1.1\r\nHost: www.cs.cmu.edu\r\n\r\n";
    char client3_request[] = "GET /index.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n";
    char client4_request[] = "GET /testcat_ac.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n";
    // connect clients to proxy
    int client1 = connect_to_server("localhost", 9086);
    int client2 = connect_to_server("localhost", 9086);
    int client3 = connect_to_server("localhost", 9086);
    int client4 = connect_to_server("localhost", 9086);
    // send part 1 of all requests
    checked_write(client1, client1_request, chunk_sizes[0]);
    checked_write(client2, client2_request, chunk_sizes[0]);
    checked_write(client3, client3_request, chunk_sizes[0]);
    checked_write(client4, client4_request, chunk_sizes[0]);
    sleep(1);
    // send part 2 of all requests
    checked_write(client1, client1_request + chunk_sizes[0], chunk_sizes[1]);
    checked_write(client2, client2_request + chunk_sizes[0], chunk_sizes[1]);
    checked_write(client3, client3_request + chunk_sizes[0], chunk_sizes[1]);
    checked_write(client4, client4_request + chunk_sizes[0], chunk_sizes[1]);
    sleep(1);
    // send rest of all requests
    int progress_so_far = chunk_sizes[0] + chunk_sizes[1];
    checked_write(client1, client1_request + progress_so_far, strlen(client1_request) - progress_so_far);
    checked_write(client2, client2_request + progress_so_far, strlen(client2_request) - progress_so_far);
    checked_write(client3, client3_request + progress_so_far, strlen(client3_request) - progress_so_far);
    checked_write(client4, client4_request + progress_so_far, strlen(client4_request) - progress_so_far);
    // clients block sequentially till they get full responses
    HTTP_Response_T client1_response = block_until_response(client1);
    HTTP_Response_T client2_response = block_until_response(client2);
    HTTP_Response_T client3_response = block_until_response(client3);
    HTTP_Response_T client4_response = block_until_response(client4);
    // Write proxy results to files
    int client1_fd = checked_open_for_writing("out_files/proxy_response1");
    int client2_fd = checked_open_for_writing("out_files/proxy_response2");
    int client3_fd = checked_open_for_writing("out_files/proxy_response3");
    int client4_fd = checked_open_for_writing("out_files/proxy_response4");
    write(client1_fd, client1_response->body, client1_response->content_len);
    write(client2_fd, client2_response->body, client2_response->content_len);
    write(client3_fd, client3_response->body, client3_response->content_len);
    write(client4_fd, client4_response->body, client4_response->content_len);
    close(client1_fd);
    close(client2_fd);
    close(client3_fd);
    close(client4_fd);
    close(client1);
    close(client2);
    close(client3);
    close(client4);
    HTTP_Response_free(&client1_response);
    HTTP_Response_free(&client2_response);
    HTTP_Response_free(&client3_response);
    HTTP_Response_free(&client4_response);
    return 0;
}