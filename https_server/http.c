#include "http.h"

#include <stdlib.h>

#include "debugging.h"
#include "more_memory.h"
#include "more_string.h"
#include "more_time.h"
#include "tcp.h"

// Default port

const char HEADER_END[] = "\r\n\r\n";
const char LINE_DELIMITER[] = "\r\n";
const char PROXY_SHUTDOWN_SIGNAL[] = "HALT";

static const unsigned short DEFAULT_PORT = 80;
static const size_t DEFAULT_MAX_AGE = 3600;

#define LINE_DELIMITER_LEN strlen(LINE_DELIMITER)

static HTTP_Request_T init_request() {
    HTTP_Request_T request;
    request = checked_malloc(sizeof(*request));
    request->hostname = NULL;
    request->portno = 0;
    request->resource = NULL;
    request->type = TYPE_UNSET;
    return request;
}

static int is_valid_request(HTTP_Request_T request) {
    return (request->type == TYPE_GET || request->type == TYPE_CONNECT) &&
           request->hostname != NULL && request->resource != NULL &&
           request->portno > 0;
}

static void parse_host(HTTP_Request_T request, char *host) {
    char *trimmed = trim(host);
    char *rcolon = strrchr(trimmed, ':');
    if (rcolon != NULL && strcmp(rcolon + 1, "0") == 0) {
        free(trimmed);
        return;
    }

    if (rcolon == NULL) {
        request->hostname = checked_malloc(strlen(trimmed) + 1);
        strcpy(request->hostname, trimmed);
        request->portno = DEFAULT_PORT;
    } else {
        int conv = atoi(rcolon + 1);
        if (conv == 0) {
            request->hostname = checked_malloc(strlen(trimmed) + 1);
            strcpy(request->hostname, trimmed);
            request->portno = DEFAULT_PORT;
        } else {
            request->hostname =
                create_starting_substr(trimmed, rcolon - trimmed);
            request->portno = (unsigned short)conv;
        }
    }
    free(trimmed);
}

static void parse_resource(HTTP_Request_T request, char *method_line) {
    char *resource_start = strchr(method_line, ' ');
    char *trimmed = trim(resource_start);
    char *resource_end = strchr(trimmed, ' ');
    size_t len =
        (resource_end == NULL ? strlen(trimmed) : resource_end - trimmed) + 1;
    request->resource = create_starting_substr(trimmed, len - 1);
    free(trimmed);
}

static int get_field_len(char *str) {
    char *colon = strchr(str, ':');
    return colon == NULL ? -1 : colon - str;
}

static HTTP_Response_T init_response() {
    HTTP_Response_T response;
    response = checked_malloc(sizeof(*response));
    // if no content length can be parsed, content length is set 0 (see
    // parse_content_length)
    response->content_len = 0;
    response->max_age = -1;
    response->header = NULL;
    response->header_len = 0;
    response->body = NULL;
    response->type = UN;
    return response;
}

static void parse_encoding(HTTP_Response_T response, char *encoding) {
    char *trimmed = trim(encoding);
    if (strcmp(trimmed, "chunked") == 0) {
        response->type = CHUNKED;
    } else {
        response->type = STREAM;
    }
    free(trimmed);
}

static void parse_content_length(HTTP_Response_T response, char *content_len) {
    char *trimmed = trim(content_len);
    response->content_len = atoi(trimmed);
    free(trimmed);
}

static void parse_max_age(HTTP_Response_T response, char *cache_control) {
    char *trimmed = trim(cache_control);
    char *max_age = strstr(trimmed, "max-age");
    if (max_age == NULL) {
        response->max_age = DEFAULT_MAX_AGE;
    } else {
        char *equals = strstr(max_age, "=");
        if (equals == NULL) {
            response->max_age = DEFAULT_MAX_AGE;
        } else {
            char *trimmed_age = trim(equals + 1);
            if (strcmp(trimmed_age, "0") == 0) {
                response->max_age = 0;
            } else {
                int conv = atoi(trimmed_age);
                if (conv <= 0) {
                    response->max_age = DEFAULT_MAX_AGE;
                } else {
                    response->max_age = conv;
                }
            }
            free(trimmed_age);
        }
    }
    free(trimmed);
}

HTTP_Request_T parse_request_from_http_header(char *header, size_t header_len,
                                              int http) {
    HTTP_Request_T request = init_request();

#ifdef DEBUGSTATUS
    if (header_len >= strlen(PROXY_SHUTDOWN_SIGNAL) &&
        memcmp(header, PROXY_SHUTDOWN_SIGNAL, strlen(PROXY_SHUTDOWN_SIGNAL)) ==
            0) {
        return request;
    }
#endif
    // for(int i = 0; i < header_len; i++) {
    //     printf("%c", header[i]);
    // }
    char *header_cpy = create_starting_substr(header, header_len);
    tolowercase(header_cpy);

    size_t num_lines;
    char **lines = strsplit(header_cpy, LINE_DELIMITER, &num_lines);
    size_t i;
    for (i = 0; i < num_lines; i++) {
        char *trimmed = trim(lines[i]);
        int field_len = get_field_len(trimmed);
        int is_get = strncmp(trimmed, "get ", 4) == 0;
        int is_connect = strncmp(trimmed, "connect ", 8) == 0;
        if (is_get || is_connect) {
            if (request->type == TYPE_UNSET) {
                request->type = is_get ? TYPE_GET : TYPE_CONNECT;
                parse_resource(request, trimmed);
            } else {
                request->type = TYPE_UNSET;
            }
        } else if (field_len > 0 && strncmp(trimmed, "host", 4) == 0) {
            parse_host(request, trimmed + field_len + 1);
        }  // could add further request parsing here
        free(trimmed);
    }
    free(header_cpy);
    free_str_array(lines, num_lines);
    if (http && !is_valid_request(request)) {  // only check this in http mode
        HTTP_Request_free(&request);
        return NULL;
    }
    return request;
}

static char *build_age_str(struct timeval *last_put_time) {
    struct timeval curr_time = get_current_time();
    struct timeval time_diff = subtract_times(&curr_time, last_put_time);
    char *time_str = nonneg_timet_to_str(time_diff.tv_sec);
    size_t ndigits = strlen(time_str);
    char *age_str = checked_malloc(ndigits + 8);
    strcpy(age_str, "Age: ");
    strcpy(age_str + 5, time_str);
    strcpy(age_str + 5 + ndigits, LINE_DELIMITER);
    free(time_str);
    return age_str;
}

extern void HTTP_Request_free(HTTP_Request_T *request_ptr) {
    if (request_ptr == NULL || *request_ptr == NULL) {
        return;
    }
    if ((*request_ptr)->hostname != NULL) {
        free((*request_ptr)->hostname);
    }
    if ((*request_ptr)->resource != NULL) {
        free((*request_ptr)->resource);
    }
    free(*request_ptr);
    *request_ptr = NULL;
}

HTTP_Response_T parse_response(char *header, size_t header_len) {
    char *header_cpy = create_starting_substr(header, header_len);
    HTTP_Response_T response = init_response();
    response->header_len = header_len;
    tolowercase(header_cpy);
    size_t num_lines;
    char **lines = strsplit(header_cpy, LINE_DELIMITER, &num_lines);
    size_t i;
    for (i = 0; i < num_lines; i++) {
        char *trimmed = trim(lines[i]);
        int field_len = get_field_len(trimmed);
        if (field_len >= 0 && strncmp(trimmed, "content-length", 14) == 0) {
            parse_content_length(response, trimmed + field_len + 1);
        } else if (field_len >= 0 &&
                   strncmp(trimmed, "cache-control", 13) == 0) {
            parse_max_age(response, trimmed + field_len + 1);
        } else if (field_len >= 0 &&
                   strncmp(trimmed, "transfer-encoding", 17) == 0) {
            parse_encoding(response, trimmed + field_len + 1);
        }
        free(trimmed);
    }
    if (response->max_age < 0) {
        response->max_age = DEFAULT_MAX_AGE;
    }
    free_str_array(lines, num_lines);
    free(header_cpy);
    return response;
}

extern void set_response_data(HTTP_Response_T response, char *header,
                              size_t header_len, char *body) {
    if (response->header != NULL) {
        free(response->header);
    }
    if (response->body != NULL) {
        free(response->body);
    }
    response->header = checked_malloc(header_len);
    memcpy(response->header, header, header_len);
    if (response->content_len > 0) {
        response->body = checked_malloc(response->content_len);
        memcpy(response->body, body, response->content_len);
    } else {
        response->body = NULL;
    }
    response->header_len = header_len;
}

extern void send_response(HTTP_Response_T response,
                          struct timeval *last_put_time, int socket,
                          void *ssl) {
    size_t age_len = 0;
    char *age_str = NULL;
    if (last_put_time != NULL) {
        age_str = build_age_str(last_put_time);
        age_len = strlen(age_str);
    }
    size_t send_len = response->header_len + response->content_len + age_len;
    char *send_data = checked_malloc(send_len);
    memcpy(send_data, response->header, response->header_len - 2);
    if (last_put_time != NULL) {
        memcpy(send_data + response->header_len - 2, age_str, age_len);
        debug("Added age field!");
    }
    memcpy(send_data + response->header_len - 2 + age_len, LINE_DELIMITER, 2);
    memcpy(send_data + response->header_len + age_len, response->body,
           response->content_len);
    if (ssl != NULL) {
        checked_SSL_write((SSL *)ssl, send_data, send_len);
    } else {
        checked_write(socket, send_data, send_len);
    }
    if (last_put_time != NULL) {
        free(age_str);
    }
    free(send_data);
}

extern void HTTP_Response_free(HTTP_Response_T *response_ptr) {
    if (response_ptr == NULL || *response_ptr == NULL) {
        return;
    }
    if ((*response_ptr)->header != NULL) {
        free((*response_ptr)->header);
    }
    if ((*response_ptr)->body != NULL) {
        free((*response_ptr)->body);
    }
    free(*response_ptr);
    *response_ptr = NULL;
}

extern void send_bad_request_response(int socket, SSL *ssl) {
    char message[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
    if (ssl) {
        checked_SSL_write(ssl, message, strlen(message));
    } else {
        checked_write(socket, message, strlen(message));
    }
}

extern void send_too_many_requests_response(int socket, SSL *ssl) {
    char message[] = "HTTP/1.1 429 Too Many Requests\r\n\r\n";
    if (ssl) {
        checked_SSL_write(ssl, message, strlen(message));
    } else {
        checked_write(socket, message, strlen(message));
    }
}

extern void send_ok_response(int socket) {
    char message[] = "HTTP/1.1 200 OK\r\n\r\n";
    checked_write(socket, message, strlen(message));
}

extern void send_forbidden_response(int socket, SSL *ssl) {
    char message[] = "HTTP/1.1 403 Forbidden\r\n\r\n";
    if (ssl) {
        checked_SSL_write(ssl, message, strlen(message));
    } else {
        checked_write(socket, message, strlen(message));
    }
}

extern void send_internal_error_response(int socket) {
    char message[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    checked_write(socket, message, strlen(message));
}

extern void send_server_closed_error_response(int socket) {
    char message[] =
        "HTTP/1.1 500 Internal Server Error\r\nContent-Length: "
        "33\r\n\r\nServer closed connection to proxy";
    checked_write(socket, message, strlen(message));
}

// Reads over vector from search_start onwards looking for header end delimiter
// (\r\n\r\n) and returns the index after the last character in the delimiter.
// If the header end delimiter is not found in
// [search_start,CharVector_size(vector)), return -1
extern int read_header_end(CharVector_T vector, int search_start) {
    int i;
    for (i = search_start; i < (int)CharVector_size(vector) - 3; i++) {
        if (strncmp(CharVector_asarray(vector) + i, HEADER_END, 4) == 0) {
            return i + 4;
        }
    }
    return -1;
}

extern int http_hostname_equals(char *hostname1, char *hostname2) {
    char *optional_host_start = "http://";
    size_t optional_host_start_len = strlen(optional_host_start);
    char *host_cmp1 = startswith(hostname1, optional_host_start)
                          ? hostname1 + optional_host_start_len
                          : hostname1;
    char *host_cmp2 = startswith(hostname2, optional_host_start)
                          ? hostname2 + optional_host_start_len
                          : hostname2;
    return strcmp(host_cmp1, host_cmp2) == 0;
}

extern int HTTP_Request_equals(HTTP_Request_T request1,
                               HTTP_Request_T request2) {
    return http_hostname_equals(request1->hostname, request2->hostname) &&
           strcmp(request1->resource, request2->resource) == 0 &&
           request1->portno == request2->portno;
}

extern size_t http_hostname_hash(char *hostname) {
    return strhash(startswith(hostname, "http://")
                       ? hostname + strlen("http://")
                       : hostname);
}

HTTP_Request_T parse_request_from_url(char *url, int short_resource) {
    HTTP_Request_T request = init_request();
    request->type = TYPE_GET;
    size_t url_len = strlen(url);
    char *url_cpy = create_starting_substr(url, url_len);
    tolowercase(url_cpy);
    char *first_slash = strchr(url_cpy, '/');
    if (first_slash == NULL) {
        free(url_cpy);
        HTTP_Request_free(&request);
        return NULL;
    }
    char *first_slash_slash = strstr(url_cpy, "//");
    if (first_slash == first_slash_slash) {
        first_slash = strchr(first_slash_slash + 2, '/');
        // href to a website (not a particular resource) e.g
        // http://www.nigms.nih.gov
        if (first_slash == NULL) {
            free(url_cpy);
            HTTP_Request_free(&request);
            return NULL;
        }
    }
    size_t first_slash_idx = first_slash - url_cpy;
    if (short_resource) {
        request->resource =
            create_starting_substr(first_slash, url_len - first_slash_idx);
    } else {
        request->resource = url_cpy;
    }
    char *full_host = create_starting_substr(url_cpy, first_slash_idx);
    parse_host(request, full_host);
    if (short_resource) {
        free(url_cpy);
    }
    free(full_host);
    return request;
}

extern int got_all_chunks(char *data, int len) {
    // printf("len is %d\n", len);
    char *copy = checked_malloc(len + 1);
    char *safe = copy;
    memcpy(copy, data, len);
    copy[len] = '\0';
    char *ext_len = strtok_r(copy, LINE_DELIMITER, &copy);
    int offset = 0;
    while (ext_len != NULL) {
        // fprintf(stderr, "chunk len: %s\n", ext_len);
        int chunk_len = strtol(ext_len, NULL, 16);
        // fprintf(stderr, "chunk len: %d\n", chunk_len);
        if (chunk_len == 0) {
            free(safe);
            return 1;
        }
        offset = chunk_len + 3;
        if ((copy + offset - safe) >= len) {
            free(safe);
            return 0;
        }
        ext_len = strtok_r(copy + offset, LINE_DELIMITER, &copy);
    }
    free(safe);
    return 0;
}

char *make_full_url(char *url, HTTP_Request_T request) {
    char *portstr = size_t_to_str((size_t)request->portno);
    // start from 0 if doesn't start with ./, otherwise from 1
    size_t copypoint = startswith(url, "./");
    const char *start = "http://";
    size_t startlen = strlen(start);
    size_t hostlen = strlen(request->hostname);
    size_t portlen = strlen(portstr);
    size_t urllen = strlen(url) - copypoint;
    size_t offset = 0;
    // +2 = +1 for the colon +1 for null terminator
    char *full_url = checked_malloc(startlen + hostlen + portlen + urllen + 2);
    strcpy(full_url, start);
    offset += startlen;
    strcpy(full_url + offset, request->hostname);
    offset += hostlen;
    strcpy(full_url + offset, ":");
    offset++;
    strcpy(full_url + offset, portstr);
    offset += portlen;
    strcpy(full_url + offset, url + copypoint);
    offset += urllen;
    full_url[offset] = '\0';
    free(portstr);
    tolowercase(full_url);
    return full_url;
}

char *append_eol(char *line, size_t *newline_len) {
    size_t line_len = strlen(line);
    *newline_len = line_len + LINE_DELIMITER_LEN;
    char *newline = checked_malloc(*newline_len);
    strcpy(newline, line);
    memcpy(newline + line_len, LINE_DELIMITER, LINE_DELIMITER_LEN);
    return newline;
}

CharVector_T modify_contentlen(char *header, size_t header_len,
                               size_t content_len) {
    CharVector_T newheader = CharVector_new(header_len);

    char *header_cpy = create_starting_substr(header, header_len);

    size_t num_lines;
    // strsplit only works on null-terminated
    char **lines = strsplit(header_cpy, LINE_DELIMITER, &num_lines);

    const char *content_len_field = "Content-Length: ";
    size_t content_len_field_len = strlen(content_len_field);

    char *content_len_str = size_t_to_str(content_len);
    size_t content_len_str_len = strlen(content_len_str);

    size_t field_str_len =
        content_len_field_len + content_len_str_len + LINE_DELIMITER_LEN;
    char *field_str = checked_malloc(field_str_len);
    strcpy(field_str, content_len_field);
    strcpy(field_str + content_len_field_len, content_len_str);
    memcpy(field_str + content_len_field_len + content_len_str_len,
           LINE_DELIMITER, LINE_DELIMITER_LEN);
    free(content_len_str);

    size_t newline_len = 0;
    char *newline = NULL;

    size_t i;
    for (i = 0; i < num_lines; i++) {
        char *trimmed = trim(lines[i]);
        tolowercase(trimmed);
        int field_len = get_field_len(trimmed);
        if (field_len >= 0 && strncmp(trimmed, "content-length", 14) == 0) {
            CharVector_push(newheader, field_str, field_str_len);
        } else {
            newline = append_eol(lines[i], &newline_len);
            CharVector_push(newheader, newline, newline_len);
            free(newline);
        }
        free(trimmed);
    }

    CharVector_push(newheader, (char *)LINE_DELIMITER, LINE_DELIMITER_LEN);

    free(field_str);
    free_str_array(lines, num_lines);
    free(header_cpy);
    return newheader;
}