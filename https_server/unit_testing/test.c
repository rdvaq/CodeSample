#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DEBUGSTATUS 1

#define MAX_AGE_SIZE 11
const size_t DEFAULT_MAX_AGE = 3600;

static void errno_err(char *msg) {
    perror(msg);
    exit(1);
}

void custom_err(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static void *checked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        errno_err("MALLOC failed");
    } else {
        return ptr;
    }
}

static void free_str_array(char **str_array, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        free(str_array[i]);
    }
    free(str_array);
}

///////////////////////////// more_string.h //////////////////////////////

// Allocates and loads new string with the contents of line with leading and
// trailing whitespace removed. The length of the new string is returned.
static char *trim(char *str) {
    size_t slen = strlen(str);
    char *trimmed = NULL;
    // Edge case necessary otherwise end is initialized to a potential out of
    // bounds string
    if (slen == 0) {
        trimmed = checked_malloc(1);
        trimmed[0] = '\0';
        return trimmed;
    }
    char *start = str;
    while (isspace(*start) && *start != '\0') {
        start++;
    }
    char *end = str + slen - 1;
    while (isspace(*end) && end >= start) {
        end--;
    }
    size_t len = end - start + 2;
    trimmed = checked_malloc(len);
    memcpy(trimmed, start, len - 1);
    trimmed[len - 1] = '\0';
    return trimmed;
}

static void tolowercase(char *s) {
    for (; *s != '\0'; s++) {
        *s = tolower(*s);
    }
}

static char **strsplit(char *str, const char *delim, size_t *num_tokens) {
    char *str_cpy = checked_malloc(strlen(str) + 1);
    strcpy(str_cpy, str);
    *num_tokens = 0;
    char *token = strtok(str_cpy, delim);
    while (token != NULL) {
        (*num_tokens)++;
        token = strtok(NULL, delim);
    }
    char **tokens = checked_malloc(sizeof(char *) * (*num_tokens));
    strcpy(str_cpy, str);
    size_t i;
    token = strtok(str_cpy, delim);
    for (i = 0; i < *num_tokens; i++) {
        size_t token_len = strlen(token);
        tokens[i] = checked_malloc(token_len + 1);
        strcpy(tokens[i], token);
        tokens[i][token_len] = '\0';
        token = strtok(NULL, delim);
    }
    free(str_cpy);
    return tokens;
}

static char *nonneg_timet_to_str(time_t t) {
    time_t t_cpy = t;
    // Can't be size_t looping down causes infinite loop
    int num_digits = 0;
    do {
        num_digits++;
        t /= 10;
    } while (t > 0);
    char *tstr = checked_malloc(num_digits + 1);
    int i;
    for (i = num_digits - 1; i >= 0; i--) {
        tstr[i] = '0' + (t_cpy % 10);
        t_cpy /= 10;
    }
    tstr[num_digits] = '\0';
    return tstr;
}

static char *size_t_to_str(size_t s) {
    size_t s_cpy = s;
    // Can't be size_t looping down causes infinite loop
    int num_digits = 0;
    do {
        num_digits++;
        s /= 10;
    } while (s > 0);
    char *sstr = checked_malloc(num_digits + 1);
    int i;
    for (i = num_digits - 1; i >= 0; i--) {
        sstr[i] = '0' + (s_cpy % 10);
        s_cpy /= 10;
    }
    sstr[num_digits] = '\0';
    return sstr;
}

char *create_starting_substr(char *str, size_t len) {
    char *substr = checked_malloc(len + 1);
    memcpy(substr, str, len);
    substr[len] = '\0';
    return substr;
}

int startswith(const char *str, const char *query) {
    return strncmp(str, query, strlen(query)) == 0;
}

int endswith(const char *str, const char *query) {
    size_t slen = strlen(str);
    size_t qlen = strlen(query);
    if (slen < qlen) {
        return 0;
    }
    return strcmp(str + slen - qlen, query) == 0;
}

void test_start() {
    char *a = "abcd";
    assert(startswith(a, "ab"));
    assert(!startswith(a, "bc"));
    assert(!startswith(a, "abcde"));
}

void test_end() {
    char *a = "abcd";
    assert(endswith(a, "cd"));
    assert(!endswith(a, "bc"));
    assert(!endswith(a, "abcde"));
}

void test_trim() {
    char *line = "  a b c    ";
    char *trimmed_line = trim(line);
    assert(strcmp(trimmed_line, "a b c") == 0);
    free(trimmed_line);
    line = "d  e  f";
    trimmed_line = trim(line);
    assert(strcmp(trimmed_line, "d  e  f") == 0);
    free(trimmed_line);
    line = "";
    trimmed_line = trim(line);
    assert(strcmp(trimmed_line, "") == 0);
    free(trimmed_line);
}

void test_tolowercase() {
    char a[] = "ABCD";
    tolowercase(a);
    assert(strcmp(a, "abcd") == 0);
    char b[] = "abcd";
    tolowercase(b);
    assert(strcmp(b, "abcd") == 0);
}

void test_strsplit() {
    char *line = "this is my fate";
    size_t num_tokens;
    char **split;

    split = strsplit(line, " ", &num_tokens);
    assert(num_tokens == 4);
    assert(strcmp(split[0], "this") == 0);
    assert(strcmp(split[1], "is") == 0);
    assert(strcmp(split[2], "my") == 0);
    assert(strcmp(split[3], "fate") == 0);
    free_str_array(split, 4);

    char *line2 = "this";
    split = strsplit(line2, "|", &num_tokens);
    assert(num_tokens == 1);
    assert(strcmp(split[0], "this") == 0);
    free_str_array(split, 1);

    char *line3 = "this|is|my|";
    split = strsplit(line3, "|", &num_tokens);
    assert(num_tokens == 3);
    assert(strcmp(split[0], "this") == 0);
    assert(strcmp(split[1], "is") == 0);
    assert(strcmp(split[2], "my") == 0);
    free_str_array(split, 3);
}

void test_nonneg_time_to_str() {
    time_t t = 100;
    char *s = nonneg_timet_to_str(t);
    assert(strcmp(s, "100") == 0);
    free(s);

    t = 123989842;
    s = nonneg_timet_to_str(t);
    assert(strcmp(s, "123989842") == 0);
    free(s);
}

void test_size_t_to_str() {
    size_t t = 100;
    char *s = size_t_to_str(t);
    assert(strcmp(s, "100") == 0);
    free(s);

    t = 123989842;
    s = size_t_to_str(t);
    assert(strcmp(s, "123989842") == 0);
    free(s);
}

void test_create_starting_substr() {
    char *s = "chami lamelas";
    char *t = create_starting_substr(s, 5);
    assert(strlen(t) == 5);
    assert(strcmp(t, "chami") == 0);
    free(t);
}

///////////////////////// more_time.h ////////////////////////////////////////

static const suseconds_t MICROSEC_PER_SEC = 1000000;

extern struct timeval get_current_time() {
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    return curr_time;
}

// 0 if same, ><0 otherwise
extern int compare_times(struct timeval *time1, struct timeval *time2) {
    // If seconds match, compare on microseconds, otherwise compare seconds
    return (time1->tv_sec == time2->tv_sec) ? time1->tv_usec - time2->tv_usec
                                            : time1->tv_sec - time2->tv_sec;
}

extern struct timeval subtract_times(struct timeval *time1,
                                     struct timeval *time2) {
    struct timeval difference;
    if (compare_times(time2, time1) > 0) {
        // time2 > time1 - 0 timeval
        difference.tv_sec = 0;
        difference.tv_usec = 0;
    } else if (time1->tv_usec >= time2->tv_usec) {
        // Both time1 members bigger => difference is subtraction of both
        // members
        difference.tv_usec = time1->tv_usec - time2->tv_usec;
        difference.tv_sec = time1->tv_sec - time2->tv_sec;
    } else {
        // time1 bigger but time1 microseconds < time2 microseconds, we do
        // "carry" by adding MICROSEC_PER_SEC to time1 microseconds then
        // subtract microseconds. Then, subtract 1 from seconds difference
        difference.tv_usec = MICROSEC_PER_SEC + time1->tv_usec - time2->tv_usec;
        difference.tv_sec = time1->tv_sec - time2->tv_sec - 1;
    }
    return difference;
}

void test_subtract_time() {
    struct timeval t1;
    t1.tv_sec = 10;
    t1.tv_usec = 12345;
    struct timeval t2;
    t2.tv_sec = 5;
    t2.tv_usec = 1234;
    struct timeval t3;
    t3.tv_sec = 8;
    t3.tv_usec = 12346;

    struct timeval d = subtract_times(&t1, &t2);
    assert(d.tv_sec == 5);
    assert(d.tv_usec == 11111);

    d = subtract_times(&t2, &t1);
    assert(d.tv_sec == 0);
    assert(d.tv_usec == 0);

    d = subtract_times(&t1, &t3);
    assert(d.tv_sec == 1);
    assert(d.tv_usec == 999999);
}

////////////////////////// http ///////////////////////////////////

typedef struct HTTP_Request_T *HTTP_Request_T;
typedef struct HTTP_Response_T *HTTP_Response_T;

enum Request_T { TYPE_UNSET, TYPE_GET, TYPE_CONNECT };

extern const char HEADER_END[];

struct HTTP_Request_T {
    char *hostname;
    unsigned short portno;
    char *resource;
    enum Request_T type;
};

struct HTTP_Response_T {
    size_t content_len;
    int max_age;
    char *header;
    char *body;
    size_t header_len;
};

static const unsigned short DEFAULT_PORT = 80;
static const char *LINE_DELIMITER = "\r\n";

const char HEADER_END[] = "\r\n\r\n";
const char PROXY_SHUTDOWN_SIGNAL[] = "HALT";

static HTTP_Request_T init_request() {
    HTTP_Request_T request;
    request = checked_malloc(sizeof(*request));
    request->hostname = NULL;
    request->portno = 0;
    request->resource = NULL;
    request->type = 0;
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
            request->hostname = checked_malloc(rcolon - trimmed + 1);
            memcpy(request->hostname, trimmed, rcolon - trimmed);
            request->hostname[rcolon - trimmed] = '\0';
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
    request->resource = checked_malloc(len);
    memcpy(request->resource, trimmed, len - 1);
    request->resource[len - 1] = '\0';
    free(trimmed);
}

static int get_field_len(char *str) {
    char *colon = strchr(str, ':');
    return colon == NULL ? -1 : colon - str;
}

static HTTP_Response_T HTTP_Response_new() {
    HTTP_Response_T response;
    response = checked_malloc(sizeof(*response));
    response->content_len = 0;
    response->max_age = -1;
    response->header = NULL;
    response->header_len = 0;
    response->body = NULL;
    return response;
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
static void HTTP_Request_free(HTTP_Request_T *request_ptr) {
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

static void HTTP_Response_free(HTTP_Response_T *response_ptr) {
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

HTTP_Request_T parse_request(char *header, size_t header_len) {
    HTTP_Request_T request = init_request();

#ifdef DEBUGSTATUS
    if (header_len >= strlen(PROXY_SHUTDOWN_SIGNAL) &&
        memcmp(header, PROXY_SHUTDOWN_SIGNAL, strlen(PROXY_SHUTDOWN_SIGNAL)) ==
            0) {
        return request;
    }
#endif

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
            if (request->type == 0) {
                request->type = is_get ? TYPE_GET : TYPE_CONNECT;
                parse_resource(request, trimmed);
            } else {
                request->type = 0;
            }
        } else if (field_len > 0 && strncmp(trimmed, "host", 4) == 0) {
            parse_host(request, trimmed + field_len + 1);
        }  // could add further request parsing here
        free(trimmed);
    }
    free(header_cpy);
    free_str_array(lines, num_lines);
    if (!is_valid_request(request)) {
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

HTTP_Response_T parse_response(char *header, size_t header_len) {
    char *header_cpy = checked_malloc(header_len + 1);
    memcpy(header_cpy, header, header_len);
    header_cpy[header_len] = '\0';
    HTTP_Response_T response = HTTP_Response_new();
    tolowercase(header_cpy);
    size_t num_lines;
    char **lines = strsplit(header_cpy, "\r\n", &num_lines);
    size_t i;
    for (i = 0; i < num_lines; i++) {
        char *trimmed = trim(lines[i]);
        int field_len = get_field_len(trimmed);
        if (field_len >= 0 && strncmp(trimmed, "content-length", 14) == 0) {
            parse_content_length(response, trimmed + field_len + 1);
        } else if (field_len >= 0 &&
                   strncmp(trimmed, "cache-control", 13) == 0) {
            parse_max_age(response, trimmed + field_len + 1);
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

static void set_response_data(HTTP_Response_T response, char *header,
                              size_t header_len, char *body) {
    if (response->header != NULL) {
        free(response->header);
    }
    if (response->body != NULL) {
        free(response->body);
    }
    response->header = checked_malloc(header_len);
    response->body = checked_malloc(response->content_len);
    memcpy(response->header, header, header_len);
    memcpy(response->body, body, response->content_len);
    response->header_len = header_len;
}

static void send_response(HTTP_Response_T response,
                          struct timeval *last_put_time, int socket) {
    char *age_str = build_age_str(last_put_time);
    size_t age_len = strlen(age_str);
    size_t send_len = response->header_len + response->content_len + age_len;
    char *send_data = checked_malloc(send_len);
    memcpy(send_data, response->header, response->header_len - 2);
    memcpy(send_data + response->header_len - 2, age_str, age_len);
    memcpy(send_data + response->header_len - 2 + age_len, LINE_DELIMITER, 2);
    memcpy(send_data + response->header_len + age_len, response->body,
           response->content_len);
    // checked_write(socket, send_data, send_len);
    free(age_str);
}

static void send_bad_request_response(int socket) {
    char message[] = "400 Bad Request\r\n";
    // checked_write(socket, message, strlen(message));
}

//// tests ///
static void test_parse_request() {
    char *header =
        "GET /~prs/bio.html HTTP/1.1\r\nHost: www.cs.cmu.edu\r\nUser-Agent: "
        "curl/7.55.1\r\nAccept: */*\r\n\r\n";
    HTTP_Request_T r = parse_request(header, strlen(header));
    assert(strcmp(r->hostname, "www.cs.cmu.edu") == 0);
    assert(r->portno == 80);
    assert(strcmp(r->resource, "/~prs/bio.html") == 0);
    assert(r->type == TYPE_GET);

    char *header2 =
        "GET /~prs/bio.html HTTP/1.1\r\nHost: "
        "www.cs.cmu.edu:123\r\nUser-Agent: curl/7.55.1\r\nAccept: */*\r\n\r\n";
    HTTP_Request_T r2 = parse_request(header2, strlen(header2));
    assert(strcmp(r2->hostname, "www.cs.cmu.edu") == 0);
    assert(r2->portno == 123);
    assert(strcmp(r2->resource, "/~prs/bio.html") == 0);
    assert(r2->type == TYPE_GET);

    char *header3 =
        "GET /~prs/bio.html HTTP/1.1\r\nHaost: "
        "www.cs.cmu.edu:123\r\nUser-Agent: curl/7.55.1\r\nAccept: */*\r\n\r\n";
    HTTP_Request_T r3 = parse_request(header3, strlen(header3));
    assert(r3 == NULL);

    char *header4 =
        "GEqwT /~prs/bio.html HTTP/1.1\r\nHost: "
        "www.cs.cmu.edu:123\r\nUser-Agent: curl/7.55.1\r\nAccept: */*\r\n\r\n";
    HTTP_Request_T r4 = parse_request(header4, strlen(header4));
    assert(r4 == NULL);

    char *header5 =
        "CONNECT server.example.com:81 HTTP/1.1\r\nHost: "
        "server.example.com:81\r\nProxy-Authorization: basic "
        "aGVsbG86d29ybGQ=\r\n\r\n";
    HTTP_Request_T r5 = parse_request(header5, strlen(header5));
    assert(strcmp(r5->hostname, "server.example.com") == 0);
    assert(r5->portno == 81);
    assert(strcmp(r5->resource, "server.example.com:81") == 0);
    assert(r5->type == TYPE_CONNECT);

    char *header6 = "HALT\r\n\r\n";
    HTTP_Request_T r6 = parse_request(header6, strlen(header6));
    assert(r6->type == TYPE_UNSET);

    HTTP_Request_free(&r);
    HTTP_Request_free(&r2);
    HTTP_Request_free(&r3);
    HTTP_Request_free(&r4);
    HTTP_Request_free(&r5);
    HTTP_Request_free(&r6);
}

static void test_parse_response() {
    // only testing for get requests
    char *response1 =
        "HTTP/1.1 200 OK\r\nDate: Sat, 12 Nov 2022 20:50:09 GMT\r\n \
                       Server: Apache/2.4.18 (Ubuntu)\r\n \
                       Set-Cookie: SHIBLOCATION=tilde; path=/; domain=.cs.cmu.edu\r\n \
                       Accept-Ranges: bytes\r\nVary: Accept-Encoding\r\n \
                       Content-Length: 5\r\nContent-Type: text/html\r\n\r\n";
    HTTP_Response_T r1 = parse_response(response1, strlen(response1));
    assert(r1->content_len == 5);
    // case of no cache control field, max age should be set to 3600
    // printf("%d\n", r1->max_age);
    assert(r1->max_age == DEFAULT_MAX_AGE);

    char *response2 =
        "HTTP/1.1 200 OK\r\nDate: Sat, 12 Nov 2022 20:50:09 GMT\r\n \
                       Server: Apache/2.4.18 (Ubuntu)\r\ncache-control: Max-Age=61\r\n \
                       Set-Cookie: SHIBLOCATION=tilde; path=/; domain=.cs.cmu.edu\r\n \
                       Accept-Ranges: bytes\r\nVary: Accept-Encoding\r\n \
                       Content-Length: 8458\r\nContent-Type: text/html\r\n\r\n";

    HTTP_Response_T r2 = parse_response(response2, strlen(response2));
    assert(r2->content_len == 8458);
    assert(r2->max_age == 61);

    char *response3 =
        "HTTP/1.1 200 OK\r\nDate: Sat, 12 Nov 2022 20:50:09 GMT\r\n \
                       Server: Apache/2.4.18 (Ubuntu)\r\ncache-control: Max-Age=0\r\n \
                       Set-Cookie: SHIBLOCATION=tilde; path=/; domain=.cs.cmu.edu\r\n \
                       Accept-Ranges: bytes\r\nVary: Accept-Encoding\r\n \
                       Content-Length: 8458\r\nContent-Type: text/html\r\n\r\n";

    HTTP_Response_T r3 = parse_response(response3, strlen(response3));
    assert(r3->content_len == 8458);
    assert(r3->max_age == 0);

    HTTP_Response_free(&r1);
    HTTP_Response_free(&r2);
    HTTP_Response_free(&r3);
}

////////////////////////////////// CharVector ////////////////////////////////

typedef struct CharVector_T *CharVector_T;

// Class that represents resizable vector for chars that pushes back, retrieves
// everywhere,
struct CharVector_T {
    size_t capacity;
    size_t num_elems;
    char *buf;
};

CharVector_T CharVector_new(size_t initial_capacity) {
    CharVector_T vector;
    vector = checked_malloc(sizeof(*vector));
    vector->capacity = initial_capacity;
    vector->num_elems = 0;
    vector->buf = checked_malloc(vector->capacity);
    return vector;
}

extern void CharVector_push(CharVector_T vector, char *arr, size_t arr_len) {
    size_t i;
    if (vector->capacity < vector->num_elems + arr_len) {
        vector->capacity = 2 * vector->num_elems + arr_len;
        char *tmp_buf = checked_malloc(vector->capacity);
        for (i = 0; i < vector->num_elems; i++) {
            tmp_buf[i] = vector->buf[i];
        }
        free(vector->buf);
        vector->buf = tmp_buf;
    }
    for (i = 0; i < arr_len; i++) {
        vector->buf[vector->num_elems++] = arr[i];
    }
}

char CharVector_get(CharVector_T vector, int index) {
    if (index < 0 || index >= vector->num_elems) {
        custom_err("CharVector_get - Index out of bounds");
    }
    return vector->buf[index];
}

size_t CharVector_size(CharVector_T vector) { return vector->num_elems; }

void CharVector_clear(CharVector_T vector, int index) {
    if (index < 0 || index >= vector->num_elems) {
        custom_err("CharVector_clear - Index out of bounds");
    }
    if (index == vector->num_elems - 1) {
        vector->num_elems = 0;
    } else {
        vector->num_elems -= index + 1;
        size_t i;
        for (i = 0; i < vector->num_elems; i++) {
            vector->buf[i] = vector->buf[index + i + 1];
        }
    }
}

void CharVector_free(CharVector_T *vector_p) {
    if (vector_p == NULL || *vector_p == NULL) {
        custom_err("Trying to free NULL CharVector_T");
    }

    free((*vector_p)->buf);
    free(*vector_p);
    *vector_p = NULL;
}

extern char *CharVector_asarray(CharVector_T vector) { return vector->buf; }

void test_CharVector_new() {
    CharVector_T vector = CharVector_new(5);
    assert(vector->capacity == 5);
    assert(vector->num_elems == 0);
    assert(CharVector_size(vector) == 0);
    CharVector_free(&vector);
    assert(vector == NULL);
}

void test_CharVector_push() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    assert(vector->num_elems == 3);
    assert(CharVector_size(vector) == 3);
    assert(vector->buf[0] == 'a');
    assert(vector->buf[1] == 'b');
    assert(vector->buf[2] == 'c');
    CharVector_free(&vector);
    assert(vector == NULL);
}

void test_CharVector_get() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    assert(vector->num_elems == 3);
    assert(CharVector_size(vector) == 3);
    assert(CharVector_get(vector, 0) == 'a');
    assert(CharVector_get(vector, 1) == 'b');
    assert(CharVector_get(vector, 2) == 'c');
    CharVector_free(&vector);
    assert(vector == NULL);
}

void test_CharVector_clear() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    CharVector_clear(vector, 2);
    assert(vector->num_elems == 0);
    assert(CharVector_size(vector) == 0);
    CharVector_free(&vector);
    assert(vector == NULL);
}

void test_CharVector_clear_partial() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    CharVector_clear(vector, 1);
    assert(vector->num_elems == 1);
    assert(CharVector_size(vector) == 1);
    assert(CharVector_get(vector, 0) == 'c');
    CharVector_free(&vector);
    assert(vector == NULL);
}

void test_CharVector_get_error1() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    CharVector_get(vector, 3);
}

void test_CharVector_get_error2() {
    CharVector_T vector = CharVector_new(5);
    char abc[] = {'a', 'b', 'c'};
    CharVector_push(vector, abc, 3);
    CharVector_get(vector, -1);
}

void test_CharVector_stress_put() {
    CharVector_T vector = CharVector_new(5);
    char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                       'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                       's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    CharVector_push(vector, alphabet, 26);
    assert(vector->num_elems == 26);
    assert(CharVector_size(vector) == 26);
    for (int i = 0; i < 26; i++) {
        assert(vector->buf[i] == 'a' + i);
        assert(CharVector_get(vector, i) == 'a' + i);
    }
    CharVector_free(&vector);
}

////////////////////////////////// TimesQueue ////////////////////////////////

typedef struct TimesQueue_T *TimesQueue_T;

struct node {
    struct timeval time;
    size_t bytes;
    struct node *next;
};

struct TimesQueue_T {
    struct node *head;
    struct node *tail;
    size_t curr_bytes;
    size_t byte_limit;
    struct timeval time_limit;
};
TimesQueue_T TimesQueue_new(size_t byte_limit, struct timeval *time_limit) {
    TimesQueue_T queue;
    queue = checked_malloc(sizeof(*queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->curr_bytes = 0;
    queue->byte_limit = byte_limit;
    queue->time_limit = *time_limit;
    return queue;
}

static struct node *make_node(struct timeval *currtime, size_t bytes) {
    struct node *node;
    node = checked_malloc(sizeof(*node));
    node->next = NULL;
    node->time = *currtime;
    node->bytes = bytes;
    return node;
}

// Pre-condition: head is not NULL
static void pop_head(TimesQueue_T queue) {
    queue->curr_bytes -= queue->head->bytes;
    struct node *next = queue->head->next;
    free(queue->head);
    queue->head = next;
    if (next == NULL) {
        queue->tail = NULL;
    }
}

static void push(TimesQueue_T queue, struct timeval *currtime, size_t bytes) {
    struct node *node = make_node(currtime, bytes);
    if (queue->tail == NULL) {
        queue->head = node;
    } else {
        queue->tail->next = node;
    }
    queue->tail = node;
    queue->curr_bytes += bytes;
}

int TimesQueue_push(TimesQueue_T queue, size_t bytes) {
    if (queue->time_limit.tv_sec == 0 && queue->time_limit.tv_usec == 0) {
        return 1;
    }

    struct timeval currtime = get_current_time();
    int too_old = 1;
    while (queue->head != NULL && too_old) {
        struct timeval diff = subtract_times(&currtime, &(queue->head->time));
        too_old = compare_times(&diff, &(queue->time_limit)) > 0;
        if (too_old) {
            pop_head(queue);
        }
    }
    if (queue->curr_bytes + bytes > queue->byte_limit) {
        return 0;
    } else {
        push(queue, &currtime, bytes);
        return 1;
    }
}

void TimesQueue_free(TimesQueue_T *queue_p) {
    if (queue_p == NULL || *queue_p == NULL) {
        return;
    }

    struct node *curr = (*queue_p)->head;
    while (curr != NULL) {
        struct node *next = curr->next;
        free(curr);
        curr = next;
    }

    free(*queue_p);
    *queue_p = NULL;
}

void test_TimesQueue_new_free() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    TimesQueue_free(&queue);
    assert(queue == NULL);
}

void test_TimesQueue_push() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    assert(TimesQueue_push(queue, 10) == 1);
    TimesQueue_free(&queue);
}

void test_TimesQueue_push_causing_remove() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    assert(TimesQueue_push(queue, 10) == 1);
    sleep(2);
    assert(TimesQueue_push(queue, 10) == 1);
    TimesQueue_free(&queue);
}

void test_TimesQueue_push_not_causing_remove() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    assert(TimesQueue_push(queue, 10) == 1);
    assert(TimesQueue_push(queue, 10) == 1);
    TimesQueue_free(&queue);
}

void test_TimesQueue_one_push_over_limit() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    assert(TimesQueue_push(queue, 101) == 0);
    TimesQueue_free(&queue);
}

void test_TimesQueue_push_over_limit() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    assert(TimesQueue_push(queue, 50) == 1);
    assert(TimesQueue_push(queue, 51) == 0);
    TimesQueue_free(&queue);
}

void test_TimesQueue_slowly_push_over_limit() {
    struct timeval limit = {1, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    int i;
    for (i = 0; i < 10; i++) {
        assert(TimesQueue_push(queue, 10) == 1);
    }
    sleep(1);
    for (i = 0; i < 9; i++) {
        assert(TimesQueue_push(queue, 10) == 1);
    }
    assert(TimesQueue_push(queue, 11) == 0);
    TimesQueue_free(&queue);
}

void test_TimesQueue_unlimited() {
    struct timeval limit = {0, 0};
    TimesQueue_T queue = TimesQueue_new(100, &limit);
    int i;
    for (i = 0; i < 20; i++) {
        assert(TimesQueue_push(queue, 10) == 1);
    }
    TimesQueue_free(&queue);
}

//////////////////////////////////// Table and Set
/////////////////////////////////////////////

// Taken from text - unsure how to replicate this because RAND_MAX
// seems much smaller
static size_t scatter[256] = {
    2078917053, 143302914,  1027100827, 1953210302, 755253631,  2002600785,
    1405390230, 45248011,   1099951567, 433832350,  2018585307, 438263339,
    813528929,  1703199216, 618906479,  573714703,  766270699,  275680090,
    1510320440, 1583583926, 1723401032, 1965443329, 1098183682, 1636505764,
    980071615,  1011597961, 643279273,  1315461275, 157584038,  1069844923,
    471560540,  89017443,   1213147837, 1498661368, 2042227746, 1968401469,
    1353778505, 1300134328, 2013649480, 306246424,  1733966678, 1884751139,
    744509763,  400011959,  1440466707, 1363416242, 973726663,  59253759,
    1639096332, 336563455,  1642837685, 1215013716, 154523136,  593537720,
    704035832,  1134594751, 1605135681, 1347315106, 302572379,  1762719719,
    269676381,  774132919,  1851737163, 1482824219, 125310639,  1746481261,
    1303742040, 1479089144, 899131941,  1169907872, 1785335569, 485614972,
    907175364,  382361684,  885626931,  200158423,  1745777927, 1859353594,
    259412182,  1237390611, 48433401,   1902249868, 304920680,  202956538,
    348303940,  1008956512, 1337551289, 1953439621, 208787970,  1640123668,
    1568675693, 478464352,  266772940,  1272929208, 1961288571, 392083579,
    871926821,  1117546963, 1871172724, 1771058762, 139971187,  1509024645,
    109190086,  1047146551, 1891386329, 994817018,  1247304975, 1489680608,
    706686964,  1506717157, 579587572,  755120366,  1261483377, 884508252,
    958076904,  1609787317, 1893464764, 148144545,  1415743291, 2102252735,
    1788268214, 836935336,  433233439,  2055041154, 2109864544, 247038362,
    299641085,  834307717,  1364585325, 23330161,   457882831,  1504556512,
    1532354806, 567072918,  404219416,  1276257488, 1561889936, 1651524391,
    618454448,  121093252,  1010757900, 1198042020, 876213618,  124757630,
    2082550272, 1834290522, 1734544947, 1828531389, 1982435068, 1002804590,
    1783300476, 1623219634, 1839739926, 69050267,   1530777140, 1802120822,
    316088629,  1830418225, 488944891,  1680673954, 1853748387, 946827723,
    1037746818, 1238619545, 1513900641, 1441966234, 367393385,  928306929,
    946006977,  985847834,  1049400181, 1956764878, 36406206,   1925613800,
    2081522508, 2118956479, 1612420674, 1668583807, 1800004220, 1447372094,
    523904750,  1435821048, 923108080,  216161028,  1504871315, 306401572,
    2018281851, 1820959944, 2136819798, 359743094,  1354150250, 1843084537,
    1306570817, 244413420,  934220434,  672987810,  1686379655, 1301613820,
    1601294739, 484902984,  139978006,  503211273,  294184214,  176384212,
    281341425,  228223074,  147857043,  1893762099, 1896806882, 1947861263,
    1193650546, 273227984,  1236198663, 2116758626, 489389012,  593586330,
    275676551,  360187215,  267062626,  265012701,  719930310,  1621212876,
    2108097238, 2026501127, 1865626297, 894834024,  552005290,  1404522304,
    48964196,   5816381,    1889425288, 188942202,  509027654,  36125855,
    365326415,  790369079,  264348929,  513183458,  536647531,  13672163,
    313561074,  1730298077, 286900147,  1549759737, 1699573055, 776289160,
    2143346068, 1975249606, 1136476375, 262925046,  92778659,   1856406685,
    1884137923, 53392249,   1735424165, 1602280572};

static size_t strhashhelper(const char *str, size_t len) {
    size_t h = 0;
    for (size_t i = 0; i < len; i++)
        h = (h >> 1) + scatter[(unsigned char)str[i]];
    return h;
}

size_t strhash(const void *key) {
    const char *ckey = (const char *)key;
    return strhashhelper(ckey, strlen(ckey));
}

size_t good_hash(const void *key) {
    const char *ckey = (const char *)key;
    return strhash(ckey);
}

int cmp(const void *x, const void *y) {
    return strcmp((const char *)x, (const char *)y);
}

size_t bad_hash(const void *key) { return 1; }

int int_cmp(const void *x, const void *y) { return *((int *)x) != *((int *)y); }

// table start

static const double LOAD_FACTOR_THRESHOLD = 0.5;

typedef struct Table_T *Table_T;

struct Table_T {
    size_t num_elements;
    size_t num_buckets;
    size_t mod_ops;
    int (*cmp)(const void *key1, const void *key2);
    size_t (*hash)(const void *key);
    struct table_node {
        struct table_node *next;
        // Not marking a member as const since this table_node type is
        // hidden from client via opaque pointer, can't find
        // anything online about modifying const variables in struct
        // did find that this is considered bad practice:
        // https://softwareengineering.stackexchange.com/a/222461
        void *key;
        void *value;
    } **buckets;
};

// Checks if n is prime
static int is_prime(int n) {
    // Optimization: 2 is the only prime even number
    if (n % 2 == 0) return n == 2;
    // Only need to go to halfway point over odd numbers up to n
    // anything about halfway point yields quotient of 1
    for (int i = 3; i < (n / 2) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Finds the smallest prime number > n
static size_t next_prime(size_t n) {
    n += (n % 2 == 0);
    for (; !is_prime(n); n += 2)
        ;
    return n;
}

// hash into table
static size_t hash(Table_T table, void *key) {
    return table->hash(key) % table->num_buckets;
}

static struct table_node **find(Table_T table, void *key) {
    // Get pointer to table_node pointer containing key or pointer to
    // the next of the last table_node pointer in the bucket
    // for struct table_node **x, &x[h] = &(*(x+h)) = x + h
    // I just do x + h versus what's given in the text
    struct table_node **itr = table->buckets + hash(table, key);
    for (; *itr && table->cmp((*itr)->key, key) != 0; itr = &((*itr)->next))
        ;
    return itr;
}

// push table_node on top of a bucket
static void push_node(void *key, void *value, struct table_node **bucket) {
    struct table_node *new_node;
    new_node = checked_malloc(sizeof(*new_node));
    new_node->key = key;
    new_node->value = value;
    new_node->next = *bucket;
    *bucket = new_node;
}

// allocates buckets and inits to NULL
static struct table_node **allocate_buckets(size_t num_buckets) {
    struct table_node **buckets;
    buckets = checked_malloc(sizeof(buckets[0]) * num_buckets);
    size_t i;
    for (i = 0; i < num_buckets; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

static void auto_rehash(Table_T table) {
    if (table->num_elements < LOAD_FACTOR_THRESHOLD * table->num_buckets)
        return;

    struct table_node **cpy = table->buckets;
    // update table buckets so hash works properly
    size_t old_num_buckets = table->num_buckets;
    table->num_buckets *= 2;
    table->buckets = allocate_buckets(table->num_buckets);
    struct table_node *next;
    size_t i;
    for (i = 0; i < old_num_buckets; i++)
        for (; cpy[i]; cpy[i] = next) {
            // push onto the appropriate bucket and free from old buckets
            push_node(cpy[i]->key, cpy[i]->value,
                      table->buckets + hash(table, cpy[i]->key));
            next = cpy[i]->next;
            free(cpy[i]);
        }
    free(cpy);
}

Table_T Table_new(size_t hint, int cmp(const void *x, const void *y),
                  size_t hash(const void *key)) {
    assert(hint >= 0 && cmp != NULL && hash != NULL);
    size_t num_buckets = next_prime(hint);
    Table_T table;
    table = checked_malloc(sizeof(*table));
    table->cmp = cmp;
    table->hash = hash;
    table->num_elements = 0;
    table->num_buckets = num_buckets;
    table->mod_ops = 0;
    // buckets are not allocated as part of table struct allocation
    // because buckets must be freed during rehashing
    table->buckets = allocate_buckets(num_buckets);
    return table;
}

void Table_free(Table_T *table) {
    if (table == NULL || *table == NULL) {
        return;
    }
    struct table_node *next;
    size_t i;
    for (i = 0; i < (*table)->num_buckets; i++)
        for (; (*table)->buckets[i]; (*table)->buckets[i] = next) {
            next = (*table)->buckets[i]->next;
            free((*table)->buckets[i]);
        }
    free((*table)->buckets);
    free(*table);
    *table = NULL;
}

size_t Table_length(Table_T table) {
    assert(table);
    return table->num_elements;
}

void *Table_put(Table_T table, void *key, void *value) {
    assert(table);
    auto_rehash(table);
    // will store bucket to push onto
    struct table_node **table_node = find(table, key);
    // overwrite if find yielded result
    void *out = NULL;
    if (*table_node) {
        out = (*table_node)->key;
        (*table_node)->key = key;
        (*table_node)->value = value;
    } else {
        // push onto bucket, note here to get the pointer
        // to the pointer that heads a bucket (so that bucket
        // head can be updated) we add to a table_node** to get
        // a table_node**
        push_node(key, value, table->buckets + hash(table, key));
        table->num_elements++;
    }
    table->mod_ops++;
    return out;
}

void *Table_get(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    assert(*table_node != NULL);
    return (*table_node)->value;
}

int Table_contains(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    return *table_node != NULL;
}

void *Table_remove(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    assert(*table_node != NULL);
    struct table_node *next = (*table_node)->next;
    void *value = (*table_node)->value;
    free(*table_node);
    *table_node = next;
    table->num_elements--;
    table->mod_ops++;
    return value;
}

void Table_map(Table_T table, void apply(void *key, void **value, void *cl),
               void *cl) {
    assert(table && apply);
    unsigned start_mod_ops = table->mod_ops;
    size_t i;
    for (i = 0; i < table->num_buckets; i++)
        for (struct table_node *table_node = table->buckets[i]; table_node;
             table_node = table_node->next) {
            apply(table_node->key, &table_node->value, cl);
            assert(table->mod_ops == start_mod_ops);
        }
}

// table end

void table_apply1(void *key, void **value, void *cl) {
    *((int *)cl) += **(int **)value;
}

void table_apply2(void *key, void **value, void *cl) {
    *(int **)value = (int *)cl;
}

void Table_test(void) {
    const char *key_data[5] = {"a", "b", "c", "d", "e"};
    int value_data[5] = {1, 2, 3, 4, 5};
    char *k[5];
    int *v[5];
    for (int i = 0; i < 5; i++) {
        k[i] = (char *)key_data[i];
        v[i] = &value_data[i];
    }
    puts("TABLE EMPTY BEHAVIOR");
    Table_T t = Table_new(20, cmp, good_hash);
    assert(Table_length(t) == 0);
    assert(Table_length(t) == 0);
    puts("TABLE ADD 1");
    assert(Table_put(t, k[0], v[0]) == NULL);
    assert(Table_length(t) == 1);
    assert(Table_get(t, k[0]) == v[0]);
    puts("TABLE OVERWRITE");
    assert(Table_put(t, k[0], v[1]) == k[0]);
    assert(Table_length(t) == 1);
    assert(Table_get(t, k[0]) == v[1]);
    puts("TABLE REMOVE");
    assert(Table_remove(t, k[0]) == v[1]);
    assert(Table_length(t) == 0);
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE COLLISION");
    t = Table_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) {
        assert(Table_put(t, k[i], v[i]) == NULL);
        assert(Table_get(t, k[i]) == v[i]);
        assert(Table_length(t) == i + 1);
    }
    for (int i = 0; i < 3; i++) {
        assert(Table_put(t, k[i], v[3]) == k[i]);
        assert(Table_get(t, k[i]) == v[3]);
        assert(Table_length(t) == 3);
    }
    for (int i = 0; i < 3; i++) {
        assert(Table_remove(t, k[i]) == v[3]);
        assert(Table_length(t) == 2 - i);
    }
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE MAP");
    t = Table_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) Table_put(t, k[i], v[i]);
    int sum = 0;
    Table_map(t, table_apply1, &sum);
    assert(sum == 6);
    Table_map(t, table_apply2, v[3]);
    for (int i = 0; i < 3; i++) assert(Table_get(t, k[i]) == v[3]);
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE REHASH");
    t = Table_new(3, cmp, good_hash);
    for (int i = 0; i < 5; i++) {
        assert(Table_put(t, k[i], v[i]) == NULL);
        assert(Table_get(t, k[i]) == v[i]);
        assert(Table_length(t) == i + 1);
    }
    Table_free(&t);
    assert(t == NULL);
}

// typedef struct Set_T *Set_T;

// static const size_t EMPTY_SET_HINT = 100;

// struct Set_T {
//     size_t num_elements;
//     size_t num_buckets;
//     size_t mod_ops;
//     int (*cmp)(const void *member1, const void *member2);
//     size_t (*hash)(const void *member);
//     struct set_node {
//         struct set_node *next;
//         void *member;
//     } * *buckets;
// };

// // Checks if n is prime
// static int is_prime(size_t n) {
//     // Optimization: 2 is the only prime even number
//     if (n % 2 == 0) return n == 2;
//     // Only need to go to halfway point over odd numbers up to n
//     // anything about halfway point yields quotient of 1
//     size_t i;
//     for (i = 3; i < (n / 2) + 1; i += 2)
//         if (n % i == 0) return 0;
//     return 1;
// }

// // Finds the smallest prime number > n
// static size_t next_prime(size_t n) {
//     n += (n % 2 == 0);
//     for (; !is_prime(n); n += 2)
//         ;
//     return n;
// }

// // hash into set
// static size_t hash(Set_T set, void *member) {
//     return set->hash(member) % set->num_buckets;
// }

// static struct set_node **find(Set_T set, void *member) {
//     // Get pointer to set_node pointer containing member or pointer to
//     // the next of the last set_node pointer in the bucket
//     // for struct set_node **x, &x[h] = &(*(x+h)) = x + h
//     // I just do x + h versus what's given in the text
//     struct set_node **itr = set->buckets + hash(set, member);
//     for (; *itr && set->cmp((*itr)->member, member) != 0; itr =
//     &((*itr)->next))
//         ;
//     return itr;
// }

// // push set_node on top of a bucket
// static void push_node(void *member, struct set_node **bucket) {
//     struct set_node *new_node;
//     new_node = checked_malloc(sizeof(*new_node));
//     new_node->member = member;
//     new_node->next = *bucket;
//     *bucket = new_node;
// }

// // allocates buckets and inits to NULL
// static struct set_node **allocate_buckets(size_t num_buckets) {
//     struct set_node **buckets;
//     buckets = checked_malloc(sizeof(buckets[0]) * num_buckets);
//     size_t i;
//     for (i = 0; i < num_buckets; i++) {
//         buckets[i] = NULL;
//     }
//     return buckets;
// }

// static void auto_rehash(Set_T set) {
//     if (set->num_elements < LOAD_FACTOR_THRESHOLD * set->num_buckets) return;

//     struct set_node **cpy = set->buckets;
//     // update set buckets so hash works properly
//     unsigned old_num_buckets = set->num_buckets;
//     set->num_buckets *= 2;
//     set->buckets = allocate_buckets(set->num_buckets);
//     struct set_node *next;
//     size_t i;
//     for (i = 0; i < old_num_buckets; i++)
//         for (; cpy[i]; cpy[i] = next) {
//             // push onto the appropriate bucket and free from old buckets
//             push_node(cpy[i]->member, set->buckets + hash(set,
//             cpy[i]->member)); next = cpy[i]->next; free(cpy[i]);
//         }
//     free(cpy);
// }

// Set_T Set_new(size_t hint, int cmp(const void *x, const void *y),
//               size_t hash(const void *x)) {
//     assert(hint >= 0 && cmp != NULL && hash != NULL);
//     size_t num_buckets = next_prime(hint);
//     Set_T set;
//     set = checked_malloc(sizeof(*set));
//     set->cmp = cmp;
//     set->hash = hash;
//     set->num_elements = 0;
//     set->num_buckets = num_buckets;
//     set->mod_ops = 0;
//     // buckets are not allocated as part of set struct allocation
//     // because buckets must be freed during rehashing
//     set->buckets = allocate_buckets(num_buckets);
//     return set;
// }

// void Set_free(Set_T *set) {
//     if (set == NULL || *set == NULL) return;
//     struct set_node *next;
//     size_t i;
//     for (i = 0; i < (*set)->num_buckets; i++)
//         for (; (*set)->buckets[i]; (*set)->buckets[i] = next) {
//             next = (*set)->buckets[i]->next;
//             free((*set)->buckets[i]);
//         }
//     free((*set)->buckets);
//     free(*set);
//     *set = NULL;
// }

// size_t Set_length(Set_T set) {
//     assert(set);
//     return set->num_elements;
// }

// int Set_member(Set_T set, void *member) {
//     assert(set && member);
//     return *find(set, member) != NULL;
// }

// void *Set_get(Set_T set, void *member) {
//     assert(set && member);
//     struct set_node *find_result = *find(set, member);
//     return find_result == NULL ? NULL : find_result->member;
// }

// void *Set_put(Set_T set, void *member) {
//     assert(set && member);
//     auto_rehash(set);
//     struct set_node **set_node = find(set, member);
//     void *old = NULL;
//     if (*set_node) {
//         old = (*set_node)->member;
//         (*set_node)->member = member;
//     } else {
//         // push onto bucket, note here to get the pointer
//         // to the pointer that heads a bucket (so that bucket
//         // head can be updated) we add to a set_node** to get
//         // a set_node**
//         push_node(member, set->buckets + hash(set, member));
//         set->num_elements++;
//     }
//     set->mod_ops++;
//     return old;
// }

// void *Set_remove(Set_T set, void *member) {
//     assert(set && member);
//     struct set_node **set_node = find(set, member);
//     if (*set_node == NULL) return NULL;
//     struct set_node *next = (*set_node)->next;
//     void *out = (*set_node)->member;
//     free(*set_node);
//     *set_node = next;
//     set->num_elements--;
//     set->mod_ops++;
//     return out;
// }

// void Set_map(Set_T set, void apply(const void *member, void *cl), void *cl) {
//     assert(set && apply);
//     unsigned start_mod_ops = set->mod_ops;
//     size_t i;
//     for (i = 0; i < set->num_buckets; i++)
//         for (struct set_node *set_node = set->buckets[i]; set_node;
//              set_node = set_node->next) {
//             apply(set_node->member, cl);
//             assert(set->mod_ops == start_mod_ops);
//         }
// }

// void set_apply1(const void *member, void *cl) {
//     *((int *)cl) += strlen(member);
// }

// void Set_test(void) {
//     const char *member_data[5] = {"a", "b", "c", "d", "e"};
//     char *m[5];
//     for (int i = 0; i < 5; i++) {
//         m[i] = (char *)member_data[i];
//     }
//     char *n[5];
//     for (int i = 0; i < 5; i++) {
//         n[i] = (char *)member_data[i];
//     }
//     Set_T s = Set_new(20, cmp, good_hash);
//     assert(Set_length(s) == 0);
//     assert(!Set_member(s, m[0]));
//     assert(Set_get(s, m[0]) == NULL);
//     assert(Set_remove(s, m[0]) == NULL);
//     assert(Set_length(s) == 0);
//     assert(Set_put(s, m[0]) == NULL);
//     assert(Set_get(s, m[0]) == m[0]);
//     assert(Set_get(s, n[0]) == m[0]);
//     assert(Set_length(s) == 1);
//     assert(Set_member(s, m[0]));
//     assert(Set_put(s, m[0]) == m[0]);
//     assert(Set_length(s) == 1);
//     assert(Set_member(s, m[0]));
//     assert(Set_remove(s, m[0]) == m[0]);
//     assert(Set_length(s) == 0);
//     assert(!Set_member(s, m[0]));
//     Set_free(&s);
//     assert(s == NULL);
//     s = Set_new(20, int_cmp, bad_hash);
//     for (int i = 0; i < 3; i++) {
//         assert(Set_put(s, m[i]) == NULL);
//         assert(Set_member(s, m[i]));
//         assert(Set_get(s, m[i]) == m[i]);
//         assert(Set_get(s, n[i]) == m[i]);
//         assert(Set_length(s) == i + 1);
//     }
//     for (int i = 0; i < 3; i++) {
//         assert(Set_put(s, m[i]) == m[i]);
//         assert(Set_member(s, m[i]));
//         assert(Set_length(s) == 3);
//     }
//     for (int i = 0; i < 3; i++) {
//         assert(Set_remove(s, m[i]) == m[i]);
//         assert(!Set_member(s, m[i]));
//         assert(Set_get(s, m[i]) == NULL);
//         assert(Set_get(s, n[i]) == NULL);
//         assert(Set_length(s) == 2 - i);
//     }
//     Set_free(&s);
//     assert(s == NULL);
//     s = Set_new(20, int_cmp, bad_hash);
//     for (int i = 0; i < 3; i++) assert(Set_put(s, m[i]) == NULL);
//     int sum = 0;
//     Set_map(s, set_apply1, &sum);
//     assert(sum == 3);
//     Set_free(&s);
//     assert(s == NULL);
//     s = Set_new(3, cmp, good_hash);
//     for (int i = 0; i < 5; i++) {
//         assert(Set_put(s, m[i]) == NULL);
//         assert(Set_member(s, m[i]));
//         assert(Set_length(s) == i + 1);
//     }
//     Set_free(&s);
//     assert(s == NULL);
//     int x = 1;
//     int y = 1;
//     int *onep1 = &x;
//     int *onep2 = &y;
//     Set_T s2 = Set_new(20, int_cmp, bad_hash);
//     assert(Set_put(s2, onep1) == NULL);
//     assert(Set_member(s2, onep2));
//     Set_free(&s2);
//     s2 = Set_new(20, int_cmp, bad_hash);
//     assert(Set_put(s2, onep1) == NULL);
//     assert(Set_put(s2, onep2) == onep1);
//     assert(Set_length(s2) == 1);
//     assert(Set_remove(s2, onep2) == onep2);
//     Set_free(&s2);
//     // Uncomment to see assertion failures
//     // s = Set_new(-1, NULL, NULL);
//     // s = Set_new(20, int_cmp, bad_hash);
//     // Set_free(NULL);
//     // Set_T t2 = NULL;
//     // Set_free(&t2);
//     // Set_member(NULL, m[0]);
//     // Set_member(s, NULL);
//     // Set_length(NULL);
//     // Set_put(NULL, m[0]);
//     // Set_put(s, NULL);
//     // Set_remove(NULL, m[0]);
//     // Set_remove(s, NULL);
//     // Set_map(NULL, NULL, NULL);
// }

///// PQueue

// typedef struct FastPQueue_T *FastPQueue_T;

// #define parent(index) (index - 1) / 2
// #define left(index) (index * 2) + 1
// #define right(index) (index * 2) + 2

// struct FastPQueue_T {
//     void **heap;
//     Table_T table;
//     size_t num_elements;
//     size_t capacity;
//     int (*priority_cmp)(const void *key1, const void *key2);
// };

// FastPQueue_T FastPQueue_new(size_t hint,
//                             int priority_cmp(const void *x, const void *y),
//                             int eq_cmp(const void *x, const void *y),
//                             size_t hash(const void *key)) {
//     assert(priority_cmp != NULL && eq_cmp != NULL && hash != NULL);
//     FastPQueue_T queue;
//     queue = checked_malloc(sizeof(*queue));
//     // Assume load factor threshold >= 0.5. We know this priority queue will
//     // store up to hint entries maybe, so preempt this with giving a hash
//     table
//     // initial capacity of at least 2*hint (to avoid 1 rehash)
//     queue->table = Table_new(2 * hint, eq_cmp, hash);
//     queue->num_elements = 0;
//     queue->capacity = hint;
//     queue->heap = checked_malloc(sizeof(queue->heap[0]) * hint);
//     queue->priority_cmp = priority_cmp;
//     return queue;
// }

// static void auto_resize(FastPQueue_T queue) {
//     if (queue->num_elements == queue->capacity) {
//         queue->capacity = 2 * queue->capacity + 1;
//         void **new_heap = checked_malloc(sizeof(new_heap[0]) *
//         queue->capacity); size_t i; for (i = 0; i < queue->num_elements; i++)
//         {
//             new_heap[i] = queue->heap[i];
//         }
//         free(queue->heap);
//         queue->heap = new_heap;
//     }
// }

// static void swap(FastPQueue_T queue, size_t index1, size_t index2) {
//     void *tmp = queue->heap[index1];
//     queue->heap[index1] = queue->heap[index2];
//     queue->heap[index2] = tmp;
//     Table_put(queue->table, queue->heap[index2], index2);
//     Table_put(queue->table, queue->heap[index1], index1);
// }

// static void heapify_up(FastPQueue_T queue, size_t index) {
//     while (index > 0 && queue->priority_cmp(queue->heap[index],
//                                             queue->heap[parent(index)]) < 0)
//                                             {
//         swap(queue, index, parent(index));
//         index = parent(index);
//     }
// }

// static void heapify_down(FastPQueue_T queue, size_t index) {
//     int done = 0;
//     while (!done) {
//         if (left(index) >= queue->num_elements) {
//             done = 1;
//         } else {
//             size_t min_index = left(index);
//             if (right(index) < queue->num_elements &&
//                 queue->priority_cmp(queue->heap[right(index)],
//                                     queue->heap[min_index]) < 0) {
//                 min_index = right(index);
//             }
//             if (queue->priority_cmp(queue->heap[index],
//                                     queue->heap[min_index]) < 0) {
//                 done = 1;
//             } else {
//                 swap(queue, index, min_index);
//                 index = min_index;
//             }
//         }
//     }
// }

// void *FastPQueue_push(FastPQueue_T queue, void *data) {
//     if (Table_contains(queue->table, data)) {
//         size_t index = Table_get(queue->table, data);
//         void *old = queue->heap[index];
//         queue->heap[index] = data;
//         Table_put(queue->table, queue->heap[index], index);
//         if (queue->priority_cmp(data, old) < 0) {
//             heapify_up(queue, index);
//         } else {
//             heapify_down(queue, index);
//         }
//         return old;
//     } else {
//         auto_resize(queue);
//         queue->heap[queue->num_elements] = data;
//         Table_put(queue->table, data, queue->num_elements);
//         heapify_up(queue, queue->num_elements);
//         queue->num_elements++;
//         return NULL;
//     }
// }

// static void *remove_from_index(FastPQueue_T queue, size_t index) {
//     void *out = queue->heap[index];
//     swap(queue, index, queue->num_elements - 1);
//     queue->num_elements--;
//     heapify_down(queue, index);
//     Table_remove(queue->table, out);
//     return out;
// }

// void *FastPQueue_pop(FastPQueue_T queue) {
//     return queue->num_elements == 0 ? NULL : remove_from_index(queue, 0);
// }

// int FastPQueue_contains(FastPQueue_T queue, void *data) {
//     return Table_contains(queue->table, data);
// }

// void FastPQueue_free(FastPQueue_T *queue_p) {
//     if (queue_p == NULL || *queue_p == NULL) {
//         return;
//     }
//     Table_free(&((*queue_p)->table));
//     free((*queue_p)->heap);
//     free(*queue_p);
//     *queue_p = NULL;
// }

// size_t FastPQueue_length(FastPQueue_T queue) { return queue->num_elements; }

// void *FastPQueue_get(FastPQueue_T queue, void *data) {
//     return Table_contains(queue->table, data)
//                ? queue->heap[Table_get(queue->table, data)]
//                : NULL;
// }

// void *FastPQueue_remove(FastPQueue_T queue, void *data) {
//     return Table_contains(queue->table, data)
//                ? remove_from_index(queue, Table_get(queue->table, data))
//                : NULL;
// }

// struct data_element {
//     char *key;
//     int priority;
// };

// int data_element_priority_cmp(const void *x, const void *y) {
//     return ((const struct data_element *)x)->priority -
//            ((const struct data_element *)y)->priority;
// }

// int data_element_eq_cmp(const void *x, const void *y) {
//     return strcmp(((const struct data_element *)x)->key,
//                   ((const struct data_element *)y)->key);
// }

// size_t data_element_hash(const void *key) {
//     const char *key_str = ((const struct data_element *)key)->key;
//     return strhash(key_str);
// }

// void PQueue_new_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     assert(FastPQueue_length(queue) == 0);
//     FastPQueue_free(&queue);
//     assert(queue == NULL);
// }

// void PQueue_push_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element e = {"a", 1};
//     assert(FastPQueue_push(queue, &e) == NULL);
//     assert(FastPQueue_length(queue) == 1);
//     FastPQueue_free(&queue);
// }

// void PQueue_push_pop_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element e = {"a", 1};
//     assert(FastPQueue_push(queue, &e) == NULL);
//     struct data_element *f = FastPQueue_pop(queue);
//     assert(strcmp(f->key, "a") == 0);
//     assert(f->priority == 1);
//     assert(FastPQueue_length(queue) == 0);
//     FastPQueue_free(&queue);
// }

// void PQueue_push_contains_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element e = {"a", 1};
//     assert(FastPQueue_push(queue, &e) == NULL);
//     assert(FastPQueue_contains(queue, &e));
//     FastPQueue_free(&queue);
// }

// void PQueue_push_pop_contains_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element e = {"a", 1};
//     assert(FastPQueue_push(queue, &e) == NULL);
//     struct data_element *f = FastPQueue_pop(queue);
//     assert(!FastPQueue_contains(queue, &e));
//     FastPQueue_free(&queue);
// }

// void PQueue_resize_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element a = {"a", 1};
//     struct data_element b = {"b", 2};
//     struct data_element c = {"c", 3};
//     struct data_element d = {"d", 4};
//     struct data_element e = {"e", 5};
//     struct data_element f = {"f", 6};
//     assert(FastPQueue_push(queue, &a) == NULL);
//     assert(FastPQueue_push(queue, &b) == NULL);
//     assert(FastPQueue_push(queue, &c) == NULL);
//     assert(FastPQueue_push(queue, &d) == NULL);
//     assert(FastPQueue_push(queue, &e) == NULL);
//     assert(FastPQueue_push(queue, &f) == NULL);
//     assert(FastPQueue_length(queue) == 6);
//     assert(FastPQueue_contains(queue, &a));
//     assert(FastPQueue_contains(queue, &b));
//     assert(FastPQueue_contains(queue, &c));
//     assert(FastPQueue_contains(queue, &d));
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 5);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(FastPQueue_contains(queue, &b));
//     assert(FastPQueue_contains(queue, &c));
//     assert(FastPQueue_contains(queue, &d));
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 4);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     assert(FastPQueue_contains(queue, &c));
//     assert(FastPQueue_contains(queue, &d));
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 3);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     assert(!FastPQueue_contains(queue, &c));
//     assert(FastPQueue_contains(queue, &d));
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 2);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     assert(!FastPQueue_contains(queue, &c));
//     assert(!FastPQueue_contains(queue, &d));
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 1);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     assert(!FastPQueue_contains(queue, &c));
//     assert(!FastPQueue_contains(queue, &d));
//     assert(!FastPQueue_contains(queue, &e));
//     assert(FastPQueue_contains(queue, &f));
//     FastPQueue_pop(queue);
//     assert(FastPQueue_length(queue) == 0);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     assert(!FastPQueue_contains(queue, &c));
//     assert(!FastPQueue_contains(queue, &d));
//     assert(!FastPQueue_contains(queue, &e));
//     assert(!FastPQueue_contains(queue, &f));
//     FastPQueue_free(&queue);
// }

// void PQueue_fifo_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element a = {"a", 1};
//     struct data_element b = {"b", 1};
//     assert(FastPQueue_push(queue, &a) == NULL);
//     assert(FastPQueue_push(queue, &b) == NULL);
//     FastPQueue_pop(queue);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(FastPQueue_contains(queue, &b));
//     FastPQueue_pop(queue);
//     assert(!FastPQueue_contains(queue, &a));
//     assert(!FastPQueue_contains(queue, &b));
//     FastPQueue_free(&queue);
// }

// void PQueue_increase_priority_test() {
//     char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i",
//                     "j", "k", "l", "m", "n", "o", "p", "q", "r",
//                     "s", "t", "u", "v", "w", "x", "y", "z"};
//     int priorities[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
//                         14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
//     struct data_element data[26];
//     int i;
//     for (i = 0; i < 26; i++) {
//         struct data_element tmp = {keys[i], priorities[i]};
//         data[i] = tmp;
//     }
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     for (i = 25; i >= 0; i--) {
//         FastPQueue_push(queue, &data[i]);
//     }
//     struct data_element new_data = {"z", 0};
//     struct data_element *old_data = FastPQueue_push(queue, &new_data);
//     assert(FastPQueue_length(queue) == 26);
//     assert(strcmp(old_data->key, "z") == 0);
//     assert(old_data->priority == 26);
//     for (i = 0; i < 26; i++) {
//         assert(FastPQueue_contains(queue, &data[i]));
//     }
//     struct data_element *new_top = FastPQueue_pop(queue);
//     assert(strcmp(new_top->key, "z") == 0);
//     assert(new_top->priority == 0);
//     for (i = 0; i < 25; i++) {
//         FastPQueue_pop(queue);
//         assert(!FastPQueue_contains(queue, &data[i]));
//         int j;
//         for (j = i + 1; j < 25; j++) {
//             assert(FastPQueue_contains(queue, &data[j]));
//         }
//     }
//     FastPQueue_free(&queue);
// }

// void PQueue_decrease_priority_test() {
//     char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i",
//                     "j", "k", "l", "m", "n", "o", "p", "q", "r",
//                     "s", "t", "u", "v", "w", "x", "y", "z"};
//     int priorities[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
//                         14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
//     struct data_element data[26];
//     int i;
//     for (i = 0; i < 26; i++) {
//         struct data_element tmp = {keys[i], priorities[i]};
//         data[i] = tmp;
//     }
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     for (i = 25; i >= 0; i--) {
//         FastPQueue_push(queue, &data[i]);
//     }
//     struct data_element new_data = {"a", 27};
//     struct data_element *old_data = FastPQueue_push(queue, &new_data);
//     assert(FastPQueue_length(queue) == 26);
//     assert(strcmp(old_data->key, "a") == 0);
//     assert(old_data->priority == 1);
//     for (i = 0; i < 26; i++) {
//         assert(FastPQueue_contains(queue, &data[i]));
//     }
//     for (i = 0; i < 25; i++) {
//         FastPQueue_pop(queue);
//         assert(!FastPQueue_contains(queue, &data[i + 1]));
//         int j;
//         for (j = i + 2; j < 26; j++) {
//             assert(FastPQueue_contains(queue, &data[j]));
//         }
//     }
//     struct data_element *new_bottom = FastPQueue_pop(queue);
//     assert(strcmp(new_bottom->key, "a") == 0);
//     assert(new_bottom->priority == 27);
//     FastPQueue_free(&queue);
// }

// void PQueue_same_priority_test() {
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     struct data_element e = {"a", 1};
//     assert(FastPQueue_push(queue, &e) == NULL);
//     struct data_element *f = FastPQueue_push(queue, &e);
//     assert(strcmp(f->key, "a") == 0);
//     assert(f->priority == 1);
//     assert(FastPQueue_contains(queue, &e));
//     assert(FastPQueue_length(queue) == 1);
//     FastPQueue_free(&queue);
// }

// void PQueue_multiple_resize_test() {
//     char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i",
//                     "j", "k", "l", "m", "n", "o", "p", "q", "r",
//                     "s", "t", "u", "v", "w", "x", "y", "z"};
//     int priorities[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
//                         14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
//     struct data_element data[26];
//     int i;
//     for (i = 0; i < 26; i++) {
//         struct data_element tmp = {keys[i], priorities[i]};
//         data[i] = tmp;
//     }
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     for (i = 25; i >= 0; i--) {
//         FastPQueue_push(queue, &data[i]);
//     }
//     assert(FastPQueue_length(queue) == 26);
//     for (i = 0; i < 26; i++) {
//         assert(FastPQueue_contains(queue, &data[i]));
//     }
//     for (i = 0; i < 26; i++) {
//         FastPQueue_pop(queue);
//         assert(!FastPQueue_contains(queue, &data[i]));
//         int j;
//         for (j = i + 1; j < 26; j++) {
//             assert(FastPQueue_contains(queue, &data[j]));
//         }
//     }

//     FastPQueue_free(&queue);
// }

// void PQueue_random_remove_test() {
//     char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i",
//                     "j", "k", "l", "m", "n", "o", "p", "q", "r",
//                     "s", "t", "u", "v", "w", "x", "y", "z"};
//     int priorities[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
//                         14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
//     struct data_element data[26];
//     int i;
//     for (i = 0; i < 26; i++) {
//         struct data_element tmp = {keys[i], priorities[i]};
//         data[i] = tmp;
//     }
//     FastPQueue_T queue = FastPQueue_new(5, data_element_priority_cmp,
//                                         data_element_eq_cmp,
//                                         data_element_hash);
//     for (i = 25; i >= 0; i--) {
//         FastPQueue_push(queue, &data[i]);
//     }
//     assert(FastPQueue_length(queue) == 26);
//     for (i = 0; i < 26; i++) {
//         assert(FastPQueue_contains(queue, &data[i]));
//     }
//     int removals[] = { 12, 1, 5, 22, 15, 0 };
//     for (i = 0; i < 6; i++) {
//         struct data_element *r = FastPQueue_remove(queue, &data[i]);
//         assert(r->key == keys[i]);
//         assert(r->priority == priorities[i]);
//         assert(FastPQueue_length(queue) == 26 - (i + 1));
//     }
//     FastPQueue_free(&queue);
// }

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

HTTP_Request_T parse_request_form_url(char *url, int short_resource) {
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

void test_make_full_url() {
    HTTP_Request_T request = NULL;
    char *full_url = NULL;

    HTTP_Request_T response_request = init_request();
    response_request->hostname =
        create_starting_substr("www.google.com", strlen("www.google.com"));
    response_request->portno = 80;

    full_url = make_full_url("/files/index.html", response_request);
    assert(strcmp(full_url, "http://www.google.com:80/files/index.html") == 0);
    request = parse_request_form_url(full_url, 0);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname, "http://www.google.com") == 0);
    assert(strcmp(request->resource, full_url) == 0);
    HTTP_Request_free(&request);
    request = parse_request_form_url(full_url, 1);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname, "http://www.google.com") == 0);
    assert(strcmp(request->resource, "/files/index.html") == 0);
    HTTP_Request_free(&request);
    free(full_url);

    full_url = make_full_url("./files/index.html", response_request);
    assert(strcmp(full_url, "http://www.google.com:80/files/index.html") == 0);
    request = parse_request_form_url(full_url, 0);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname, "http://www.google.com") == 0);
    assert(strcmp(request->resource, full_url) == 0);
    HTTP_Request_free(&request);
    request = parse_request_form_url(full_url, 1);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname, "http://www.google.com") == 0);
    assert(strcmp(request->resource, "/files/index.html") == 0);
    HTTP_Request_free(&request);
    free(full_url);

    full_url =
        make_full_url("www.google.com:80/files/index.html", response_request);
    assert(
        strcmp(full_url,
               "http://www.google.com:80www.google.com:80/files/index.html") ==
        0);
    request = parse_request_form_url(full_url, 0);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname,
                  "http://www.google.com:80www.google.com") == 0);
    assert(strcmp(request->resource, full_url) == 0);
    HTTP_Request_free(&request);
    request = parse_request_form_url(full_url, 1);
    assert(request->type == TYPE_GET);
    assert(request->portno == 80);
    assert(strcmp(request->hostname,
                  "http://www.google.com:80www.google.com") == 0);
    assert(strcmp(request->resource, "/files/index.html") == 0);
    HTTP_Request_free(&request);
    free(full_url);

    HTTP_Request_free(&response_request);
}

void test_parse_request_from_url() {
    char *url1 = "http://www.example.com:100/index.html";
    char *url2 = "http://www.example.com/index.html";
    char *url3 = "http://www.example.com/files/index.html";
    char *url4 = "http://www.example.com:100/files/index.html";
    char *url5 = "www.example.com:100/index.html";
    char *url6 = "www.example.com:100/files/index.html";
    char *url7 = "www.example.com/files/index.html";
    char *url8 = "www.example.com/index.html";
    char *url9 = "mailto:fahad@cs.tufts.edu";
    char *url10 = "http://www.nigms.nih.gov";
    HTTP_Request_T req10 = parse_request_form_url(url1, 0);
    assert(req10->type == TYPE_GET);
    assert(req10->portno == 100);
    assert(strcmp(req10->hostname, "http://www.example.com") == 0);
    assert(strcmp(req10->resource, url1) == 0);
    HTTP_Request_T req11 = parse_request_form_url(url1, 1);
    assert(req11->type == TYPE_GET);
    assert(req11->portno == 100);
    assert(strcmp(req11->hostname, "http://www.example.com") == 0);
    assert(strcmp(req11->resource, "/index.html") == 0);
    HTTP_Request_T req20 = parse_request_form_url(url2, 0);
    assert(req20->type == TYPE_GET);
    assert(req20->portno == 80);
    assert(strcmp(req20->hostname, "http://www.example.com") == 0);
    assert(strcmp(req20->resource, url2) == 0);
    HTTP_Request_T req21 = parse_request_form_url(url2, 1);
    assert(req21->type == TYPE_GET);
    assert(req21->portno == 80);
    assert(strcmp(req21->hostname, "http://www.example.com") == 0);
    assert(strcmp(req21->resource, "/index.html") == 0);
    HTTP_Request_T req30 = parse_request_form_url(url3, 0);
    assert(req30->type == TYPE_GET);
    assert(req30->portno == 80);
    assert(strcmp(req30->hostname, "http://www.example.com") == 0);
    assert(strcmp(req30->resource, url3) == 0);
    HTTP_Request_T req31 = parse_request_form_url(url3, 1);
    assert(req31->type == TYPE_GET);
    assert(req31->portno == 80);
    assert(strcmp(req31->hostname, "http://www.example.com") == 0);
    assert(strcmp(req31->resource, "/files/index.html") == 0);
    HTTP_Request_T req40 = parse_request_form_url(url4, 0);
    assert(req40->type == TYPE_GET);
    assert(req40->portno == 100);
    assert(strcmp(req40->hostname, "http://www.example.com") == 0);
    assert(strcmp(req40->resource, url4) == 0);
    HTTP_Request_T req41 = parse_request_form_url(url4, 1);
    assert(req41->type == TYPE_GET);
    assert(req41->portno == 100);
    assert(strcmp(req41->hostname, "http://www.example.com") == 0);
    assert(strcmp(req41->resource, "/files/index.html") == 0);
    HTTP_Request_T req50 = parse_request_form_url(url5, 0);
    assert(req50->type == TYPE_GET);
    assert(req50->portno == 100);
    assert(strcmp(req50->hostname, "www.example.com") == 0);
    assert(strcmp(req50->resource, url5) == 0);
    HTTP_Request_T req51 = parse_request_form_url(url5, 1);
    assert(req51->type == TYPE_GET);
    assert(req51->portno == 100);
    assert(strcmp(req51->hostname, "www.example.com") == 0);
    assert(strcmp(req51->resource, "/index.html") == 0);
    HTTP_Request_T req60 = parse_request_form_url(url6, 0);
    assert(req60->type == TYPE_GET);
    assert(req60->portno == 100);
    assert(strcmp(req60->hostname, "www.example.com") == 0);
    assert(strcmp(req60->resource, url6) == 0);
    HTTP_Request_T req61 = parse_request_form_url(url6, 1);
    assert(req61->type == TYPE_GET);
    assert(req61->portno == 100);
    assert(strcmp(req61->hostname, "www.example.com") == 0);
    assert(strcmp(req61->resource, "/files/index.html") == 0);
    HTTP_Request_T req70 = parse_request_form_url(url7, 0);
    assert(req70->type == TYPE_GET);
    assert(req70->portno == 80);
    assert(strcmp(req70->hostname, "www.example.com") == 0);
    assert(strcmp(req70->resource, url7) == 0);
    HTTP_Request_T req71 = parse_request_form_url(url7, 1);
    assert(req71->type == TYPE_GET);
    assert(req71->portno == 80);
    assert(strcmp(req71->hostname, "www.example.com") == 0);
    assert(strcmp(req71->resource, "/files/index.html") == 0);
    HTTP_Request_T req80 = parse_request_form_url(url8, 0);
    assert(req80->type == TYPE_GET);
    assert(req80->portno == 80);
    assert(strcmp(req80->hostname, "www.example.com") == 0);
    assert(strcmp(req80->resource, url8) == 0);
    HTTP_Request_T req81 = parse_request_form_url(url8, 1);
    assert(req81->type == TYPE_GET);
    assert(req81->portno == 80);
    assert(strcmp(req81->hostname, "www.example.com") == 0);
    assert(strcmp(req81->resource, "/index.html") == 0);
    HTTP_Request_T req90 = parse_request_form_url(url9, 0);
    assert(req90 == NULL);
    HTTP_Request_T req91 = parse_request_form_url(url9, 1);
    assert(req91 == NULL);
    HTTP_Request_T req100 = parse_request_form_url(url10, 0);
    assert(req100 == NULL);
    HTTP_Request_T req101 = parse_request_form_url(url10, 1);
    assert(req101 == NULL);
    HTTP_Request_free(&req10);
    HTTP_Request_free(&req11);
    HTTP_Request_free(&req20);
    HTTP_Request_free(&req21);
    HTTP_Request_free(&req30);
    HTTP_Request_free(&req31);
    HTTP_Request_free(&req40);
    HTTP_Request_free(&req41);
    HTTP_Request_free(&req50);
    HTTP_Request_free(&req51);
    HTTP_Request_free(&req60);
    HTTP_Request_free(&req61);
    HTTP_Request_free(&req70);
    HTTP_Request_free(&req71);
    HTTP_Request_free(&req80);
    HTTP_Request_free(&req81);
}

char *append_eol(char *line, size_t *newline_len) {
    size_t line_len = strlen(line);
    *newline_len = line_len + strlen(LINE_DELIMITER);
    char *newline = checked_malloc(*newline_len);
    strcpy(newline, line);
    memcpy(newline + line_len, LINE_DELIMITER, strlen(LINE_DELIMITER));
    return newline;
}

CharVector_T modify_contentlen(char *header, size_t header_len,
                               size_t content_len) {
    CharVector_T newheader = CharVector_new(header_len);

    char *header_cpy = create_starting_substr(header, header_len);

    size_t num_lines;
    char **lines = strsplit(header_cpy, LINE_DELIMITER, &num_lines);

    const char *content_len_field = "Content-Length: ";
    size_t content_len_field_len = strlen(content_len_field);

    char *content_len_str = size_t_to_str(content_len);
    size_t content_len_str_len = strlen(content_len_str);

    size_t field_str_len =
        content_len_field_len + content_len_str_len + strlen(LINE_DELIMITER);
    char *field_str = checked_malloc(field_str_len);
    strcpy(field_str, content_len_field);
    strcpy(field_str + content_len_field_len, content_len_str);
    memcpy(field_str + content_len_field_len + content_len_str_len,
           LINE_DELIMITER, strlen(LINE_DELIMITER));
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

    CharVector_push(newheader, (char *)LINE_DELIMITER, strlen(LINE_DELIMITER));

    free(field_str);
    free_str_array(lines, num_lines);
    free(header_cpy);
    return newheader;
}

static int is_printable_byte(char byte) {
    return byte == 9 || byte == 10 || (byte >= 32 && byte <= 126);
}

extern void debug_bytes(char *bytes, size_t nbytes) {
    while (nbytes > 0) {
        if (is_printable_byte(*bytes)) {
            putc(*bytes, stderr);
        } else {
            fprintf(stderr, "{%d}", (int)*bytes);
        }
        bytes++;
        nbytes--;
    }
}

void test_modify_contentlen() {
    char *header =
        "HTTP/1.1 200 OK\r\nDate: Sun, 11 Dec 2022 01:52:16 "
        "GMT\r\nContent-Type: text/html\r\nContent-Length: "
        "28634\r\nConnection: keep-alive\r\nLast-Modified: Fri, 08 Apr 2022 "
        "16:54:52 GMT\r\nAccept-Ranges: bytes\r\nETag: "
        "\"0d6835d694bd81:0\"\r\nServer: Microsoft-IIS/10.0\r\nX-Powered-By: "
        "ASP.NET\r\nSN: EC2AMAZ-BSL60ON\r\nAge: 34\r\n\r\n";
    char *newheader =
        "HTTP/1.1 200 OK\r\nDate: Sun, 11 Dec 2022 01:52:16 "
        "GMT\r\nContent-Type: text/html\r\nContent-Length: 1\r\nConnection: "
        "keep-alive\r\nLast-Modified: Fri, 08 Apr 2022 16:54:52 "
        "GMT\r\nAccept-Ranges: bytes\r\nETag: \"0d6835d694bd81:0\"\r\nServer: "
        "Microsoft-IIS/10.0\r\nX-Powered-By: ASP.NET\r\nSN: "
        "EC2AMAZ-BSL60ON\r\nAge: 34\r\n\r\n";
    char *newheader2 =
        "HTTP/1.1 200 OK\r\nDate: Sun, 11 Dec 2022 01:52:16 "
        "GMT\r\nContent-Type: text/html\r\nContent-Length: "
        "128634\r\nConnection: keep-alive\r\nLast-Modified: Fri, 08 Apr 2022 "
        "16:54:52 GMT\r\nAccept-Ranges: bytes\r\nETag: "
        "\"0d6835d694bd81:0\"\r\nServer: Microsoft-IIS/10.0\r\nX-Powered-By: "
        "ASP.NET\r\nSN: EC2AMAZ-BSL60ON\r\nAge: 34\r\n\r\n";
    CharVector_T vector = NULL;
    vector = modify_contentlen(header, strlen(header), 1);
    // debug_bytes(CharVector_asarray(vector), CharVector_size(vector));
    assert(memcmp(CharVector_asarray(vector), newheader,
                  CharVector_size(vector)) == 0);
    CharVector_free(&vector);
    vector = modify_contentlen(header, strlen(header), 128634);
    // debug_bytes(CharVector_asarray(vector), CharVector_size(vector));
    assert(memcmp(CharVector_asarray(vector), newheader2,
                  CharVector_size(vector)) == 0);
    CharVector_free(&vector);
}

int main() {
    // test_tolowercase();
    // test_trim();
    // test_start();
    // test_end();
    // test_strsplit();
    // test_nonneg_time_to_str();
    // test_size_t_to_str();
    // test_create_starting_substr();
    // test_subtract_time();
    // test_parse_request();
    // test_parse_response();
    // test_CharVector_new();
    // test_CharVector_push();
    // test_CharVector_get();
    // test_CharVector_clear();
    // test_CharVector_clear_partial();
    // test_CharVector_stress_put();
    // // Uncomment to check if errors occur
    // // test_CharVector_get_error1();
    // // test_CharVector_get_error2();
    // test_TimesQueue_new_free();
    // test_TimesQueue_push();
    // test_TimesQueue_push_causing_remove();
    // test_TimesQueue_push_not_causing_remove();
    // test_TimesQueue_one_push_over_limit();
    // test_TimesQueue_push_over_limit();
    // test_TimesQueue_slowly_push_over_limit();
    // test_TimesQueue_unlimited();
    // Table_test();
    // Set_test();
    // PQueue_new_test();
    // PQueue_push_test();
    // PQueue_push_pop_test();
    // PQueue_push_contains_test();
    // PQueue_push_pop_contains_test();
    // PQueue_resize_test();
    // PQueue_fifo_test();
    // PQueue_increase_priority_test();
    // PQueue_decrease_priority_test();
    // PQueue_multiple_resize_test();
    // PQueue_random_remove_test();
    // test_parse_request_from_url();
    // test_make_full_url();
    test_modify_contentlen();
    return 0;
}