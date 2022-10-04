#include <stdlib.h>

// creat and bind parent socket, returns parent socket fd
int server_up(char* arg_portno);

// accepts a client from the parent socket 
int accept_client(int parent_socket);

// returns the header stored in char* array, set h_len to bytes read
char* read_get_header(int child_sockfd, int* h_len);

// returns the parsed fields of the header: 1.get request 2.host name [3.port]
char** parse_get_header(char* header, int header_len, int* arg_length);

// connect from proxy to requested server
int connect_server(char** header_info, int num_hargs);

// read server response, parse the header from the content 
char* read_from_server(int sockfd, int* rh_len, char** file, int* f_len, int* age);

// perror wrapper
void error(char *msg);