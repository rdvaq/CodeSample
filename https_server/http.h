#ifndef _HTTP_H
#define _HTTP_H

#include <stddef.h>
#include <string.h>
#include <sys/time.h>

#include "SSL.h"
#include "char_vector.h"

// Number of bytes set in proxy->client reads - 2^10 bytes
#define DEFAULT_CLIENT_READ_SIZE 1024

// Number of bytes set in proxy->server reads - 2^15 bytes as server (HTTP
// response) reads tend to be bigger than client (HTTP request) reads
#define DEFAULT_SERVER_READ_SIZE 32768

// Request type - users should use HTTP_Request_T
typedef struct HTTP_Request_T *HTTP_Request_T;

// Response type - users should use HTTP_Response_T
typedef struct HTTP_Response_T *HTTP_Response_T;

// 2 Types of requests
enum Request_T { TYPE_UNSET, TYPE_GET, TYPE_CONNECT };

// 2 Types of responses
enum Response_T { UN, CHUNKED, STREAM };

// Delimiter for end of HTTP request, response header
extern const char HEADER_END[];

// Delimiter for lines in HTTP request, response header
extern const char LINE_DELIMITER[];

// Special string to mark proxy should shut down
extern const char PROXY_SHUTDOWN_SIGNAL[];

// Defines request members
struct HTTP_Request_T {
    // Null-terminated hostname (will be created as a heap-alloc'd copy)
    char *hostname;

    // Port number of request
    unsigned short portno;

    // Null-terminated resource name (will be created as a heap alloc'd copy)
    char *resource;

    // Type of request
    enum Request_T type;
};

// Defines response members
struct HTTP_Response_T {
    // Content length of response (i.e. length of the body)
    size_t content_len;

    // Max-age of the response
    int max_age;

    // Header of the response
    char *header;

    // Body of the response
    char *body;

    // Length of the header
    size_t header_len;

    // Type of response
    enum Response_T type;
};

// Heap allocs a request from a character string of a certain length
HTTP_Request_T parse_request_from_http_header(char *header, size_t header_len,
                                              int http);

// Parses an HTTP Request from a null terminated URL. If short_resource = 0, the
// entire URL is deemed the resource (like what CURL seems to put in GET
// requests). If short_reosurce = 1, then only the text following the port
// number is deemed the resource.
HTTP_Request_T parse_request_from_url(char *url, int short_resource);

// Frees an alloc'd request
extern void HTTP_Request_free(HTTP_Request_T *request_ptr);

// Heap allocs a response from a character string of a certain length
HTTP_Response_T parse_response(char *header, size_t header_len);

// Sets header, body of a response object freeing previous header and body if
// they exist.
extern void set_response_data(HTTP_Response_T response, char *header,
                              size_t header_len, char *body);

// Sends a response over a socket and uses last_put_time to specify the age of
// the sent response data
extern void send_response(HTTP_Response_T response,
                          struct timeval *last_put_time, int socket, void *ssl);

// Checks if 2 hostnames are equal (assumed lowercase - explained in below
// function header), starting http:// is ignored.
extern int http_hostname_equals(char *hostname1, char *hostname2);

// Checks if 2 requests are equal (deemed equal if hostname, resource, and port
// number match). It is assumed that hostname, resource are lowercase (as parsed
// by parse_request_from_http_header and parse_request_from_url). Above function
// is used for hostname equality check
extern int HTTP_Request_equals(HTTP_Request_T request1,
                               HTTP_Request_T request2);

// Hashes a hostname, http:// is ignored
extern size_t http_hostname_hash(char *hostname);

// Frees an alloc'd response
extern void HTTP_Response_free(HTTP_Response_T *response_ptr);

// Sends a bad request response over a particular socket
extern void send_bad_request_response(int socket, SSL *ssl);

// Sends a too many requests response over a particular socket
extern void send_too_many_requests_response(int socket, SSL *ssl);

// Sends an OK response over a particular socket
extern void send_ok_response(int socket);

// Sends a Forbidden response if client requests banned host
extern void send_forbidden_response(int socket, SSL *ssl);

// Sends a 500 code error to client
extern void send_internal_error_response(int socket);

// Sends a 500 code error to client with more info
extern void send_server_closed_error_response(int socket);

// Checks to see if HTTP header end exists in vector starting from search_start.
// If so, 1 beyond the header end is returned (e.g. for abc\r\n\r\n, 7 is
// returned). Otherwise -1 is returned
extern int read_header_end(CharVector_T vector, int search_start);

// Tests if data contains all the chunks
extern int got_all_chunks(char *data, int len);

// Turns a short URL <url> into a "full" URL meaning
// http://<hostname>:<port><url> If <url> starts with . then it is removed
// before being appended
extern char *make_full_url(char *url, HTTP_Request_T request);

// Creates a new response header with a new content length but all other fields
// copied from header
CharVector_T modify_contentlen(char *header, size_t header_len,
                               size_t content_len);

#endif