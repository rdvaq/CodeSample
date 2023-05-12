#include "tcp.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include "errors.h"
#include <unistd.h>
#include <string.h>

// Initializes the proxy's server at a certain port, the proxy's socket is
// initialized and binded to to that port
extern int init_server_socket(unsigned short portno) {
    // AF_INET: Internet domain, SOCK_STREAM: stream socket, 0: chooses TCP as
    // transport layer for stream socket
    int server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0) {
        errno_err("Error opening server socket");
    }

    // Sets SO_REUSEADDR option at SOCKET level to 1. That is, so address can be
    // reused (allows for easier debugging)
    int optval = 1;
    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,
               sizeof(int));

    struct sockaddr_in this_server_addr;

    // 0-out structure (last field of sockaddr_in must be 0s)
    bzero((char *)&this_server_addr, sizeof(this_server_addr));

    // Must always be set as this
    this_server_addr.sin_family = AF_INET;

    // Store port number of server in network byte order
    this_server_addr.sin_port = htons(portno);

    // Get host machine's IP address
    this_server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binds socket (like a FILE) to server address (like a file path)
    if (bind(server_sd, (struct sockaddr *)&this_server_addr,
             sizeof(this_server_addr)) < 0) {
        errno_err("Error server binding");
    }

    return server_sd;
}

// Has proxy server block until a connection is made by a client. When a
// connection is detected, socket to read from that connection is returned
extern int get_incoming_client(int server_sd) {
    // Will store address information of current client that has connected
    struct sockaddr_in curr_client_addr;

    // Needed for accept()
    int curr_client_addr_size = sizeof(curr_client_addr);

    // Blocks until a client connection comes in, then returns socket
    // descriptor which we will use to read data from the client
    int curr_client_sd = accept(server_sd, (struct sockaddr *)&curr_client_addr,
                                &curr_client_addr_size);
    if (curr_client_sd < 0) {
        errno_err("Error server accepting");
    }

    return curr_client_sd;
}

extern int connect_to_server(char *host, unsigned short portno) {
    // Creates client socket thats stream socket over the internet
    int this_client_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (this_client_sd < 0) {
        errno_err("Error opening client socket");
    }

    struct hostent *curr_server = gethostbyname(host);
    if (curr_server == NULL) {
        custom_err("Could not find hostname");
        // To test connection ddos, comment above and uncomment below
        // return -1;
    }

    // Build server information from gethostbyname (i.e. IP and port)
    struct sockaddr_in curr_server_addr;
    bzero((char *)&curr_server_addr, sizeof(curr_server_addr));
    curr_server_addr.sin_family = AF_INET;
    bcopy((char *)curr_server->h_addr,
          (char *)&curr_server_addr.sin_addr.s_addr, curr_server->h_length);
    curr_server_addr.sin_port = htons(portno);

    // Establish connection
    if (connect(this_client_sd, (const struct sockaddr *)&curr_server_addr,
                sizeof(curr_server_addr)) < 0) {
        errno_err("Could not connect proxy to server");
    }

    return this_client_sd;
}

// Writes data via a socket (either client <- proxy, or proxy -> server)
// from vector
extern void checked_write(int socket, char *buf, size_t len) {
    if (write(socket, buf, len) < 0) {
        errno_err("Error writing");
    }
}

extern size_t checked_read(int socket, char *buf, size_t buf_len) {
    int num_read_bytes = read(socket, buf, buf_len);
    if (num_read_bytes < 0) {
        errno_err("Error reading");
    }
    return (size_t)num_read_bytes;
}

extern void clean_close(int socket, fd_set *select_set) {
    if (select_set != NULL) {
        FD_CLR(socket, select_set);
    }
    close(socket);
}