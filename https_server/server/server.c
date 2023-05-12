// Simple GET server that services clients sequentially
// Files that can be sent back specified in records.txt along with their sizes, max ages
// Start server with ./startserver 
// Can curl it with: 
// curl http://localhost:9087/file01 

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../char_vector.h"
#include "../errors.h"
#include "../http.h"
#include "../more_file.h"
#include "../more_memory.h"
#include "../tcp.h"
#include "../more_string.h"
#include "../debugging.h"

typedef struct FileRecord_T *FileRecord_T;

struct FileRecord_T {
    char *name;
    size_t size;
    size_t max_age;
    FileRecord_T next;
};

static void free_records(FileRecord_T *records_p) {
    if (records_p != NULL) {
        FileRecord_T curr = *records_p;
        while (curr != NULL) {
            FileRecord_T next = curr->next;
            free(curr);
            curr = next;
        }
    }
}

static FileRecord_T make_record(char *name, size_t name_len, size_t size, size_t max_age) {
    FileRecord_T record;
    record = checked_malloc(sizeof(*record));
    record->name = create_starting_substr(name, name_len);
    record->size = size;
    record->max_age = max_age;
    return record;
}

static FileRecord_T push_record(FileRecord_T head, char *name, size_t name_len,
                                size_t size, size_t max_age) {
    FileRecord_T record = make_record(name, name_len, size, max_age);
    record->next = head;
    return record;
}

static FileRecord_T load_records() {
    FileRecord_T records = NULL;
    FILE *fp = fopen("records.txt", "r");
    if (fp == NULL) {
        custom_err("fopen failed to open for reading");
    }
    char *line = NULL;
    size_t allocated_size = 0;
    ssize_t read_size;
    while ((read_size = getline(&line, &allocated_size, fp)) != -1) {
        char *space = strchr(line, ' ');
        char *secondspace = strchr(space + 1, ' ');
        char *size_str = create_starting_substr(space + 1, secondspace - (space + 1));
        records = push_record(records, line, space - line, (size_t)atoi(size_str), (size_t)atoi(secondspace + 1));
        free(size_str);
    }
    free(line);
    fclose(fp);
    return records;
}

static void print_records(FileRecord_T records) {
    while (records != NULL) {
        debug("Name [%s] Size [%zu] Max-Age [%zu]", records->name, records->size, records->max_age);
        records = records->next;
    }
    debug("\n");
}

static FileRecord_T get_record(FileRecord_T records, char *filename) {
    while (records != NULL && strcmp(records->name, filename) != 0) {
        records = records->next;
    }
    return records;
}

static void build_and_send_response(FileRecord_T record, int client_socket) {
    char *content_len_str = size_t_to_str(record->size);
    char *max_age_str = size_t_to_str(record->max_age);
    char *status = "HTTP/1.1 200 OK\r\n";
    char *content_len_field = "Content-Length: ";
    char *max_age_field = "Cache-Control: max-age=";
    int status_len = strlen(status);
    int content_len_field_len = strlen(content_len_field);
    int content_len_str_len = strlen(content_len_str);
    int line_delimiter_len = strlen(LINE_DELIMITER);
    int max_age_field_len = strlen(max_age_field);
    int max_age_str_len = strlen(max_age_str);
    int header_end_len = strlen(HEADER_END);
    char *response = checked_malloc(
        status_len + content_len_field_len + content_len_str_len + line_delimiter_len +
        max_age_field_len + max_age_str_len + header_end_len + record->size
    );
    int offset = 0;
    strcpy(response + offset, status);
    offset += status_len;
    strcpy(response + offset, content_len_field);
    offset += content_len_field_len;
    strcpy(response + offset, content_len_str);
    offset += content_len_str_len;
    strcpy(response + offset, LINE_DELIMITER);
    offset += line_delimiter_len;
    strcpy(response + offset, max_age_field);
    offset += max_age_field_len;
    strcpy(response + offset, max_age_str);
    offset += max_age_str_len;
    strcpy(response + offset, HEADER_END);
    offset += header_end_len;
    char *file_read_buf = checked_malloc(record->size);
    char *folder = "files/";
    size_t folder_len = strlen(folder);
    char *path = checked_malloc(folder_len + strlen(record->name) + 1);
    strcpy(path, folder);
    strcpy(path + folder_len, record->name);
    int fd = checked_open_for_reading(path);
    read(fd, file_read_buf, record->size);
    close(fd);
    free(path);
    memcpy(response + offset, file_read_buf, record->size);
    offset += record->size;
    debug("Built %d byte response", offset);
    checked_write(client_socket, response, offset);
    debug("Response sent");
    free(content_len_str);
    free(max_age_str);
    free(file_read_buf);
    free(response);
}

static void service_clients_sequentially(int server_sd, FileRecord_T records, int delay) {
    char read_buf[DEFAULT_CLIENT_READ_SIZE];
    CharVector_T vector = CharVector_new(DEFAULT_CLIENT_READ_SIZE);
    while (1) {
        int client = get_incoming_client(server_sd);
        debug("Picked up client");
        size_t request_progress = 0;
        int read_end = -1;
        // Loop until header end received, then parse into request object
        while (read_end < 0) {
            size_t read_size = checked_read(client, read_buf, DEFAULT_CLIENT_READ_SIZE);
            request_progress = CharVector_size(vector) > 3 ? CharVector_size(vector) - 3 : 0;
            CharVector_push(vector, read_buf, read_size);
            read_end = read_header_end(vector, request_progress);
        }
        HTTP_Request_T request = parse_request_from_http_header(CharVector_asarray(vector), read_end, 1);
        char *resource = strrchr(request->resource, '/') + 1; 
        // Then build response from records 
        FileRecord_T record = get_record(records, resource);
        if (record == NULL) {
            custom_err("resource [%s] does not exist", resource);
        }
        if (delay > 0) {
            sleep(delay);
        }
        build_and_send_response(record, client);
        HTTP_Request_free(&request);
        CharVector_clear(vector, CharVector_size(vector) - 1);
        close(client);
    }
    CharVector_free(&vector);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./a.out <port> <delay>");
        return 1;
    }

    int delay = atoi(argv[2]);
    debug("\n\nSet response delay = %d s\n", delay);

    FileRecord_T records = load_records();
    debug("Loaded server records:\n");
    print_records(records);

    int server_sd = init_server_socket((unsigned short)atoi(argv[1]));
    listen(server_sd, 5);
    service_clients_sequentially(server_sd, records, delay);

    close(server_sd);
    free_records(&records);
    return 0;
}
