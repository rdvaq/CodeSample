#include "client_record.h"

#include <unistd.h>

#include "more_memory.h"
#include "tcp.h"
#include "http.h"

extern ClientRecord_T ClientRecord_new(int client_sd, size_t byte_limit,
                                       struct timeval *time_limit) {
    ClientRecord_T record;
    record = checked_malloc(sizeof(*record));
    record->client_sd = client_sd;
    record->server_sd = -1;
    record->ctx = NULL;
    record->ssl_c = NULL;
    record->ssl_s = NULL;
    record->mode = UNKNOWN;
    record->client_vector = CharVector_new(DEFAULT_CLIENT_READ_SIZE);
    record->server_vector = CharVector_new(DEFAULT_SERVER_READ_SIZE);
    record->next = NULL;
    record->request_progress = 0;
    record->response_progress = 0;
    record->limiter = TimesQueue_new(byte_limit, time_limit);
    record->waiting_to_forward_get_response = 0;
    // these are not to be freed - these will have a corresponding
    // cache entry and those will be freed as they should exist
    // longer in the cache - for nonpersistent connections, these
    // will be destroyed after servicing a GET. However, for
    // persistent connections, these will need to be used more
    record->request = NULL;
    record->response = NULL;
    return record;
}

extern void ClientRecord_free(ClientRecord_T *record_ptr) {
    if (record_ptr != NULL && (*record_ptr) != NULL) {
        if ((*record_ptr)->mode == SSL_MODE) {
            // fprintf(stderr, "in ssl shutdown in cr free\n");
            SSL_free_sess((*record_ptr)->ssl_c);
            SSL_free_sess((*record_ptr)->ssl_s);
            SSL_CTX_free((*record_ptr)->ctx);
        }
        CharVector_free(&(*record_ptr)->client_vector);
        CharVector_free(&(*record_ptr)->server_vector);
        TimesQueue_free(&(*record_ptr)->limiter);
        free(*record_ptr);
        *record_ptr = NULL;
    }
}

extern void free_records(ClientRecord_T records) {
    while (records != NULL) {
        ClientRecord_T next = records->next;
        ClientRecord_free(&records);
        records = next;
    }
}
