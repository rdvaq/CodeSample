#include "client_utils.h"

#include <stdio.h>

#include "../char_vector.h"
#include "../http.h"
#include "../more_file.h"
#include "../more_math.h"
#include "../tcp.h"

HTTP_Response_T block_until_response(int socket) {
    char read_buf[DEFAULT_SERVER_READ_SIZE];
    HTTP_Response_T response = NULL;
    size_t response_progress = 0;
    int just_read_header = 0;
    CharVector_T vector = CharVector_new(DEFAULT_SERVER_READ_SIZE);
    int done = 0;
    while (!done) {
        size_t read_size =
            checked_read(socket, read_buf, DEFAULT_SERVER_READ_SIZE);
        if (response == NULL) {
            response_progress =
                CharVector_size(vector) > 3 ? CharVector_size(vector) - 3 : 0;
            CharVector_push(vector, read_buf, read_size);
            int read_end = read_header_end(vector, response_progress);
            if (read_end < 0) {
                continue;
            }
            response = parse_response(CharVector_asarray(vector), read_end);
            response_progress = read_end;
            just_read_header = 1;
        }
        if (response != NULL) {
            if (!just_read_header) {
                CharVector_push(vector, read_buf, read_size);
            }
            just_read_header = 0;
            if (CharVector_size(vector) <
                response_progress + response->content_len) {
                continue;
            }
            set_response_data(response, CharVector_asarray(vector),
                              response_progress,
                              CharVector_asarray(vector) + response_progress);
            done = 1;
        }
    }
    CharVector_free(&vector);
    return response;
}