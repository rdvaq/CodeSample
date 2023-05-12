#include "socket_record.h"

#include "debugging.h"
#include "http.h"
#include "more_memory.h"
#include "more_string.h"
#include "table.h"

static const int NO_SOCKET = -1;

typedef struct ServerSocketRecord_T *ServerSocketRecord_T;

struct ServerSocketRecord_T {
    char *host;
    unsigned short portno;
    int socket;
};

struct dll_node {
    ServerSocketRecord_T record;
    struct dll_node *next;
    struct dll_node *prev;
};

struct ServerSocketTable_T {
    struct dll_node *qhead;
    struct dll_node *qtail;
    Table_T node_map;
    size_t capacity;
};

static ServerSocketRecord_T ServerSocketRecord_new(char *host,
                                                   unsigned short portno,
                                                   int socket) {
    ServerSocketRecord_T record;
    record = checked_malloc(sizeof(*record));
    record->host = create_starting_substr(host, strlen(host));
    record->portno = portno;
    record->socket = socket;
    return record;
}

static void ServerSocketRecord_free(ServerSocketRecord_T *record_ptr) {
    if (record_ptr == NULL || *record_ptr == NULL) {
        return;
    }
    free((*record_ptr)->host);
    free(*record_ptr);
    *record_ptr = NULL;
}

static int ServerSocketRecord_cmp(const void *x, const void *y) {
    const ServerSocketRecord_T x_record = (const ServerSocketRecord_T)x;
    const ServerSocketRecord_T y_record = (const ServerSocketRecord_T)y;
    return !(http_hostname_equals(x_record->host, y_record->host) &&
             x_record->portno == y_record->portno);
}

static size_t ServerSocketRecord_hash(const void *key) {
    const ServerSocketRecord_T record = (const ServerSocketRecord_T)key;
    return http_hostname_hash(record->host) + 13 * record->portno;
}

ServerSocketTable_T ServerSocketTable_new(size_t capacity) {
    ServerSocketTable_T table;
    table = checked_malloc(sizeof(*table));
    table->node_map =
        Table_new(capacity, ServerSocketRecord_cmp, ServerSocketRecord_hash);
    table->qhead = NULL;
    table->qtail = NULL;
    table->capacity = capacity;
    return table;
}

static struct dll_node *new_dll_node(ServerSocketRecord_T record) {
    struct dll_node *node;
    node = checked_malloc(sizeof(*node));
    node->next = NULL;
    node->prev = NULL;
    node->record = record;
    return node;
}

static void delete_node(ServerSocketTable_T table, struct dll_node *node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        table->qhead = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        table->qtail = node->prev;
    }
    free(node);
}

static void push_node(ServerSocketTable_T table, struct dll_node *node) {
    if (table->qtail == NULL) {
        table->qhead = node;
        table->qtail = node;
    } else {
        table->qtail->next = node;
        node->prev = table->qtail;
        table->qtail = node;
    }
}

int ServerSocketTable_add(ServerSocketTable_T table, char *host,
                          unsigned short portno, int socket) {
    int out_socket = NO_SOCKET;
    if (table->capacity > 0) {
        if (Table_length(table->node_map) == table->capacity) {
            ServerSocketRecord_T to_remove = table->qhead->record;
            Table_remove(table->node_map, to_remove);
            out_socket = to_remove->socket;
            ServerSocketRecord_free(&to_remove);
            delete_node(table, table->qhead);
        }
        ServerSocketRecord_T new_record =
            ServerSocketRecord_new(host, portno, socket);
        struct dll_node *new_node = new_dll_node(new_record);
        Table_put(table->node_map, new_record, new_node);
        push_node(table, new_node);
    }
    return out_socket;
}

int ServerSocketTable_remove(ServerSocketTable_T table, char *host,
                             unsigned short portno) {
    if (table->capacity == 0) {
        return NO_SOCKET;
    }
    ServerSocketRecord_T search_dummy =
        ServerSocketRecord_new(host, portno, NO_SOCKET);
    int out = NO_SOCKET;
    ServerSocketRecord_T record = Table_key(table->node_map, search_dummy);
    if (record != NULL) {
        struct dll_node *node =
            (struct dll_node *)Table_remove(table->node_map, search_dummy);
        out = record->socket;
        ServerSocketRecord_free(&record);
        delete_node(table, node);
    }
    ServerSocketRecord_free(&search_dummy);
    return out;
}

void ServerSocketTable_free(ServerSocketTable_T *table_ptr) {
    if (table_ptr == NULL || *table_ptr == NULL) {
        return;
    }
    Table_free(&((*table_ptr)->node_map));
    struct dll_node *curr = (*table_ptr)->qhead;
    while (curr != NULL) {
        struct dll_node *next = curr->next;
        ServerSocketRecord_free(&curr->record);
        free(curr);
        curr = next;
    }
    free(*table_ptr);
    *table_ptr = NULL;
}