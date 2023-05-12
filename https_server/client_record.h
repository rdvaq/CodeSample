#ifndef _CLIENT_RECORD_H
#define _CLIENT_RECORD_H

#include "char_vector.h"
#include "http.h"
#include "times_queue.h"
#include "SSL.h"


// Mode of a client
enum RecordMode { UNKNOWN, GET_MODE, CONNECT_MODE, SSL_MODE };

// Client record type - users should use this
typedef struct ClientRecord_T *ClientRecord_T;

struct ClientRecord_T {
    // Proxy->client socket
    int client_sd;

    // Proxy->server socket
    int server_sd;

    // SSL context
    SSL_CTX* ctx;

    // Client side SSL session
    SSL* ssl_c;

    // Server side SSL session
    SSL* ssl_s;

    // See above comment
    enum RecordMode mode;

    // Vector of current read progress on client_sd
    CharVector_T client_vector;

    // Tracks progress on reading a request (for persistent connections, will be
    // < size of client_vector)
    size_t request_progress;

    // Current request sent to record's client - is just a shallow copy of
    // request if it's also in cache (will have to revisit for persistent
    // connections)
    HTTP_Request_T request;

    // Vector of current read progress on server_sd
    CharVector_T server_vector;

    // Tracks progress on reading a response (for persistent connections, will
    // be < size of client_vector)
    size_t response_progress;

    // Current response sent to record's client - is just a shallow copy of
    // response if it's also in cache (will have to revisit for persistent
    // connections)
    HTTP_Response_T response;

    // Sliding window to enforce rate limiting on the client
    TimesQueue_T limiter;

    // Link to next record (records will be stored in a list which is iterated
    // over in select)
    ClientRecord_T next;

    // Whether this record is currently waiting to forward a GET response to the
    // client. Further requests from this client will be staged for later
    // parsing
    int waiting_to_forward_get_response;
};

extern ClientRecord_T ClientRecord_new(int client_sd, size_t byte_limit,
                                       struct timeval *time_limit);
extern void ClientRecord_free(ClientRecord_T *record_ptr);
extern void free_records(ClientRecord_T records);

#endif