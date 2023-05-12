#include "proxy.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "SSL.h"
#include "cache.h"
#include "char_vector.h"
#include "client_record.h"
#include "debugging.h"
#include "errors.h"
#include "http.h"
#include "more_math.h"
#include "more_memory.h"
#include "more_string.h"
#include "more_time.h"
#include "set.h"
#include "socket_record.h"
#include "tcp.h"

#define SSL_PORT 443

struct Proxy_T {
    int server_sd;
    fd_set master_set;
    Cache_T cache;
    int socket_max;
    ClientRecord_T records;
    Set_T banned_hosts;
    size_t total_bytes;
    struct timeval rate_time;
    int modify_responses;
    ServerSocketTable_T unused_server_sockets;
    int http_mode;
    SSL_CTX *proxy_ctx;
};

enum ProxyAction_T { PROXY_SHUTDOWN, REMOVE_CLIENT, KEEP_CLIENT };

static void handle_new_client(Proxy_T proxy) {
    // Pick up current client
    int curr_client_sd = get_incoming_client(proxy->server_sd);
    debug("\nPicked up client, assigned socket %d", curr_client_sd);
    ClientRecord_T record =
        ClientRecord_new(curr_client_sd, proxy->total_bytes, &proxy->rate_time);
    record->next = proxy->records;
    // record->mode = UNKNOWN;
    proxy->records = record;
    proxy->socket_max = max(proxy->socket_max, curr_client_sd);
    FD_SET(curr_client_sd, &proxy->master_set);
}

static void auto_update_socket_max(Proxy_T proxy, int socket_to_remove) {
    if (socket_to_remove == proxy->socket_max) {
        proxy->socket_max = proxy->server_sd;
        ClientRecord_T record;
        for (record = proxy->records; record != NULL; record = record->next) {
            proxy->socket_max = max(proxy->socket_max,
                                    max(record->client_sd, record->server_sd));
        }
    }
}

static void release_server_socket(Proxy_T proxy, ClientRecord_T record) {
    if (record->request != NULL && record->server_sd >= 0) {
        // Why do we have these checks? Note after handling a response, the
        // request is set to NULL in client record (as the server socket is
        // released at that stage). Consider another scenario: a record was
        // never given a server socket (as it was serviced from the proxy
        // cache). Again, we have no socket to add to our table. The server
        // socket >= 0 is needed in the case the request is not NULL but client
        // was serviced from cache. In this case, no socket to remove / add to
        // table
        auto_update_socket_max(proxy, record->server_sd);
        FD_CLR(record->server_sd, &proxy->master_set);
        debug("Saving server socket %d", record->server_sd);
        int bumped = ServerSocketTable_add(
            proxy->unused_server_sockets, record->request->hostname,
            record->request->portno, record->server_sd);
        if (bumped >= 0) {
            debug("Bumped unused server socket %d", bumped);
            clean_close(bumped, NULL);
        }
    }
}

static int unused_server_socket_closed(int socket) {
    char temp;
    fd_set temp_set;
    FD_ZERO(&temp_set);
    FD_SET(socket, &temp_set);
    struct timeval short_wait = {0, 1000};
    select(socket + 1, &temp_set, NULL, NULL, &short_wait);
    if (FD_ISSET(socket, &temp_set) && read(socket, &temp, 1) <= 0) {
        debug("Detected unused server socket %d closed / reporting an error",
              socket);
        clean_close(socket, NULL);
        return 1;
    }
    // Assumed read( ) > 0 won't happen - would be a malicious server
    return 0;
}

static void connect_proxy_to_server(Proxy_T proxy, ClientRecord_T record,
                                    HTTP_Request_T request) {
    record->server_sd = ServerSocketTable_remove(
        proxy->unused_server_sockets, request->hostname, request->portno);
    if (record->server_sd >= 0 &&
        !unused_server_socket_closed(record->server_sd)) {
        debug("Using an already open socket %d", record->server_sd);
    } else {
        // debug("request portno: %d\n", request->portno);
        int portno = proxy->http_mode
                         ? request->portno
                         : SSL_PORT;  // can't use port 80 to establish ssl conn
        // debug("used portno: %d\n", portno);
        record->server_sd = connect_to_server(request->hostname, portno);
        debug("Created proxy->server socket %d", record->server_sd);
    }
    FD_SET(record->server_sd, &proxy->master_set);
    proxy->socket_max = max(proxy->socket_max, record->server_sd);
}

static CharVector_T get_modified_body(Cache_T cache,
                                      HTTP_Request_T response_request,
                                      HTTP_Response_T response) {
    char *link_query = "<a href=\"";
    size_t link_query_len = strlen(link_query);
    char *modification = " style=\"color:green;\"";
    size_t modification_len = strlen(modification);
    CharVector_T modified_body = CharVector_new(response->content_len);
    char *body_cpy =
        create_starting_substr(response->body, response->content_len);
    tolowercase(body_cpy);
    // Use an index just so we can search lowercased version, but add from the
    // original
    size_t search_start = 0;
    char *link = strstr(body_cpy + search_start, link_query);
    while (link != NULL) {
        char *url_start = link + link_query_len;
        char *end_link = strchr(url_start, '\"');
        size_t offset = end_link - (body_cpy + search_start) + 1;
        CharVector_push(modified_body, response->body + search_start, offset);
        char *url = create_starting_substr(url_start, end_link - url_start);
        // Request may be identified by its "short" version or its full version
        // in the cache. Search for both (O(1) anyway)
        HTTP_Request_T request0 = parse_request_from_url(url, 0);
        HTTP_Request_T request1 = parse_request_from_url(url, 1);
        char *full_url = make_full_url(url, response_request);
        HTTP_Request_T request2 = parse_request_from_url(full_url, 0);
        HTTP_Request_T request3 = parse_request_from_url(full_url, 1);
        if (request0 != NULL && (Cache_contains(cache, request0) ||
                                 Cache_contains(cache, request1) ||
                                 Cache_contains(cache, request2) ||
                                 Cache_contains(cache, request3))) {
            CharVector_push(modified_body, modification, modification_len);
        }
        search_start += offset;
        link = strstr(body_cpy + search_start, link_query);
        free(url);
        free(full_url);
        if (request0 != NULL) {
            HTTP_Request_free(&request0);
            HTTP_Request_free(&request1);
            HTTP_Request_free(&request2);
            HTTP_Request_free(&request3);
        }
    }
    free(body_cpy);
    CharVector_push(modified_body, response->body + search_start,
                    response->content_len - search_start);
    return modified_body;
}

static enum ProxyAction_T check_client_vector(Proxy_T proxy,
                                              ClientRecord_T record) {
    int read_end =
        read_header_end(record->client_vector, record->request_progress);
    if (read_end < 0) {
        record->request_progress =
            CharVector_size(record->client_vector) > 3
                ? CharVector_size(record->client_vector) - 3
                : 0;
        return KEEP_CLIENT;
    }

    HTTP_Request_T request = parse_request_from_http_header(
        CharVector_asarray(record->client_vector), read_end, proxy->http_mode);
    record->request_progress = 0;

    if (request == NULL) {
        debug("Parsed invalid request");
        send_bad_request_response(record->client_sd, record->ssl_c);
        HTTP_Request_free(&request);
        CharVector_clear(record->client_vector, read_end - 1);
        return check_client_vector(proxy, record);  // changed
    } else if (proxy->http_mode &&
               request->type ==
                   TYPE_UNSET) {  // only run this check in http mode
        HTTP_Request_free(&request);
        return PROXY_SHUTDOWN;
    } else if (Set_member(proxy->banned_hosts, request->hostname)) {
        debug("Client trying to access banned host %s", request->hostname);
        send_forbidden_response(record->client_sd, record->ssl_c);
        CharVector_clear(record->client_vector, read_end - 1);
        HTTP_Request_free(&request);
        return check_client_vector(proxy, record);  // changed
    }

    record->request = request;
    if (request->type == TYPE_GET) {  // check if request is get or connect
        if (record->mode == UNKNOWN) {
            record->mode = GET_MODE;
        }
        struct timeval last_put_time;
        // Don't free this because it's in cache
        HTTP_Response_T response =
            Cache_get(proxy->cache, request, &last_put_time);
        if (response != NULL) {
            debug("Responding from cache for host [%s] resource [%s]",
                  request->hostname, request->resource);
            if (endswith(request->resource, ".html") &&
                proxy->modify_responses) {
                debug("Modifying resource: %s", request->resource);
                char *old_header = response->header, *old_body = response->body;
                size_t old_header_len = response->header_len,
                       old_body_len = response->content_len;
                CharVector_T modified_body =
                    get_modified_body(proxy->cache, record->request, response);
                response->body = CharVector_asarray(modified_body);
                response->content_len = CharVector_size(modified_body);
                CharVector_T modified_header = modify_contentlen(
                    old_header, old_header_len, response->content_len);
                response->header = CharVector_asarray(modified_header);
                response->header_len = CharVector_size(modified_header);
                send_response(
                    response, &last_put_time, record->client_sd,
                    (record->mode == SSL_MODE) ? record->ssl_c : NULL);
                debug(
                    "\nPerformed (potentially modified) %zu byte (header: %zu, "
                    "body: %zu) write on proxy->client socket %d (original "
                    "size: %zu)",
                    response->header_len + response->content_len,
                    response->header_len, response->content_len,
                    record->client_sd, old_header_len + old_body_len);
                response->header = old_header;
                response->header_len = old_header_len;
                response->body = old_body;
                response->content_len = old_body_len;
                CharVector_free(&modified_body);
                CharVector_free(&modified_header);
            } else {
                send_response(
                    response, &last_put_time, record->client_sd,
                    (record->mode == SSL_MODE) ? record->ssl_c : NULL);
                debug("\nPerformed %zu byte write on proxy->client socket %d",
                      response->header_len + response->content_len,
                      record->client_sd);
            }
            HTTP_Request_free(&request);
        } else {
            if (record->mode == SSL_MODE) {
                connect_proxy_to_server(
                    proxy, record, request); /*tcp to server with port 443*/
                record->ssl_s =
                    create_new_conn_obj(proxy->proxy_ctx, record->server_sd);
                if (checked_SSL_connect(record->ssl_s) < 0) {
                    debug("[SSL] error connecting to server %s on socket %d",
                          request->hostname, record->server_sd);
                    return REMOVE_CLIENT;
                }
                debug("[SSL] ------ connected proxy to %s -------", request->hostname);
                checked_SSL_write(record->ssl_s,
                                  CharVector_asarray(record->client_vector),
                                  read_end);
                debug("[SSL] sending %d bytes proxy->server %d", read_end,
                      record->server_sd);
                record->waiting_to_forward_get_response = 1;
            } else {
                connect_proxy_to_server(proxy, record, request);
                debug("[GET] connected proxy to host %s", request->hostname);
                checked_write(record->server_sd,
                              CharVector_asarray(record->client_vector),
                              read_end);
                record->waiting_to_forward_get_response = 1;
            }
        }
        CharVector_clear(record->client_vector, read_end - 1);
    } else {  // connect request
        if (!proxy->http_mode) {
            send_ok_response(record->client_sd);
            // first whip up a certificate with the host name and configure ctx
            SSL_CTX *ctx = create_new_context(request->hostname);
            // then create new conn obj and bind socket to it
            SSL *ssl_client = create_new_conn_obj(ctx, record->client_sd);
            if (checked_SSL_accept(ssl_client) < 0) {
                debug("[SSL] error accepting ssl client %d", record->client_sd);
                return REMOVE_CLIENT;
            }
            // debug("[SSL] sucessful ssl handshake with client %d",
                //   record->client_sd);
            record->mode = SSL_MODE;
            record->ctx = ctx;
            record->ssl_c = ssl_client;
            // connect_proxy_to_server(proxy, record, request); /*tcp to
            // server*/ record->ssl_s = create_new_conn_obj(ctx,
            // record->server_sd); if (checked_SSL_connect(record->ssl_s) < 0) {
            //     debug("[SSL] error connecting to server %s on socket %d",
            //     request->hostname, record->server_sd);
            //     return REMOVE_CLIENT;
            // }
            // debug("[SSL] connected proxy to %s", request->hostname);
        } else {
            record->mode = CONNECT_MODE;
            connect_proxy_to_server(proxy, record, request);
            send_ok_response(record->client_sd);
            debug("[CONNECT] connected proxy to host %s", request->hostname);
            checked_write(record->server_sd,
                          CharVector_asarray(record->client_vector) + read_end,
                          CharVector_size(record->client_vector) -
                              read_end);  // should not happen
        }
        CharVector_clear(record->client_vector,
                         CharVector_size(record->client_vector) - 1);
    }
    return KEEP_CLIENT;
}

static enum ProxyAction_T handle_client_sd(Proxy_T proxy,
                                           ClientRecord_T record) {
    char read_buf[DEFAULT_CLIENT_READ_SIZE];
    int read_size;

    if (record->mode == SSL_MODE) {
        read_size = SSL_read(record->ssl_c, read_buf, DEFAULT_CLIENT_READ_SIZE);
    } else {
        read_size = read(record->client_sd, read_buf, DEFAULT_CLIENT_READ_SIZE);
    }

    if (read_size <= 0) {
        debug("Detected early close by client %d", record->client_sd);
        return REMOVE_CLIENT;  // keep this here - client leaves, we remove
                               // record
    }

    // Moved this from out of check_client_vector and in the if statement below
    // as this should be applied at a byte level (regardless of reading mode)
    if (!TimesQueue_push(record->limiter, read_size)) {
        debug("Rate limit violated by socket %d", record->client_sd);
        send_too_many_requests_response(record->client_sd, NULL);
        return KEEP_CLIENT;
    }

    if (record->mode == CONNECT_MODE) {
        checked_write(record->server_sd, read_buf, read_size);
        debug("[CONNECT] sending %d bytes proxy->server", read_size);

        return KEEP_CLIENT;
    }

    CharVector_push(record->client_vector, read_buf, read_size);
    if (record->waiting_to_forward_get_response) {
        debug("Staging bytes for later (waiting to forward GET response)");
        return KEEP_CLIENT;
    }
    return check_client_vector(proxy, record);
}

static enum ProxyAction_T handle_server_sd(Proxy_T proxy,
                                           ClientRecord_T record) {
    char read_buf[DEFAULT_SERVER_READ_SIZE];
    int read_size;

    if (record->mode == SSL_MODE) {
        read_size = SSL_read(record->ssl_s, read_buf, DEFAULT_SERVER_READ_SIZE);
    } else {
        read_size = read(record->server_sd, read_buf, DEFAULT_SERVER_READ_SIZE);
    }

    if (read_size <= 0) {
        if (read_size == 0) {
            debug("Server->proxy socket %d closed by server",
                  record->server_sd);
            send_server_closed_error_response(record->client_sd);
        } else {
            debug("Server socket %d failed", record->server_sd);
            send_internal_error_response(record->client_sd);
        }
        // If server closed socket / failed when client is in connect mode,
        // client will only be sending to this server so they should be notified
        // immediately that something went wrong. If in GET mode, client can
        // always connect to other servers.
        return record->mode == GET_MODE  // matt adds: minor change here to
                                         // accomate ssl mode
                   ? check_client_vector(proxy, record)
                   : REMOVE_CLIENT;
    }

    if (record->mode == CONNECT_MODE) {
        checked_write(record->client_sd, read_buf, read_size);
        debug("[CONNECT] sending %d bytes proxy->client", read_size);
        return KEEP_CLIENT;
    }

    int just_finished_header = 0;
    if (record->response == NULL) {
        CharVector_push(record->server_vector, read_buf, read_size);
        int read_end =
            read_header_end(record->server_vector, record->response_progress);
        if (read_end < 0) {
            record->response_progress =
                CharVector_size(record->server_vector) > 3
                    ? CharVector_size(record->server_vector) - 3
                    : 0;
            return KEEP_CLIENT;
        }

        record->response_progress = read_end;
        HTTP_Response_T response =
            parse_response(CharVector_asarray(record->server_vector), read_end);
        record->response = response;
        // debug(
        //     "response to client %d response type %d response header size %d"
        //     " response content len %d response max-age %d",
        //     record->client_sd, record->response->type, read_end,
        //     record->response->content_len, record->response->max_age);
        just_finished_header = 1;
    }
    if (record->response != NULL) {
        if (!just_finished_header) {
            CharVector_push(record->server_vector, read_buf, read_size);
        }

        if (record->response->type == CHUNKED) {
            int header_len = record->response->header_len;
            // printf("header_len: %d\n", header_len);
            char *chunks =
                CharVector_asarray(record->server_vector) + header_len;
            int chunks_size =
                CharVector_size(record->server_vector) - header_len;
            if (chunks_size > 0 && got_all_chunks(chunks, chunks_size)) {
                checked_SSL_write(record->ssl_c,
                                  CharVector_asarray(record->server_vector),
                                  CharVector_size(record->server_vector));
                debug(
                    "[SSL-Chunked] Performed %zu byte write on proxy->client "
                    "socket %d",
                    CharVector_size(record->server_vector), record->client_sd);
                record->response->content_len = chunks_size;
                set_response_data(record->response,
                                  CharVector_asarray(record->server_vector),
                                  header_len, chunks);
                Cache_put(proxy->cache, record->request, record->response);
                return REMOVE_CLIENT;
            } else {
                return KEEP_CLIENT;
            }
        }

        if (CharVector_size(record->server_vector) <
            record->response_progress + record->response->content_len) {
            return KEEP_CLIENT;
        }
        set_response_data(record->response,
                          CharVector_asarray(record->server_vector),
                          record->response_progress,
                          CharVector_asarray(record->server_vector) +
                              record->response_progress);
        Cache_put(proxy->cache, record->request, record->response);
        if (endswith(record->request->resource, ".html") &&
            proxy->modify_responses) {
            debug("Modifying resource: %s", record->request->resource);
            char *old_header = record->response->header,
                 *old_body = record->response->body;
            size_t old_header_len = record->response->header_len,
                   old_body_len = record->response->content_len;
            CharVector_T modified_body = get_modified_body(
                proxy->cache, record->request, record->response);
            record->response->body = CharVector_asarray(modified_body);
            record->response->content_len = CharVector_size(modified_body);
            CharVector_T modified_header = modify_contentlen(
                old_header, old_header_len, record->response->content_len);
            record->response->header = CharVector_asarray(modified_header);
            record->response->header_len = CharVector_size(modified_header);
            send_response(record->response, NULL, record->client_sd,
                          (record->mode == SSL_MODE) ? record->ssl_c : NULL);
            debug(
                "\nPerformed (potentially modified) %zu byte (header: %zu, "
                "body: %zu) write on proxy->client socket %d (original size: "
                "%zu)",
                record->response->header_len + record->response->content_len,
                record->response->header_len, record->response->content_len,
                record->client_sd, old_header_len + old_body_len);
            record->response->header = old_header;
            record->response->header_len = old_header_len;
            record->response->body = old_body;
            record->response->content_len = old_body_len;
            CharVector_free(&modified_body);
            CharVector_free(&modified_header);
        } else {
            send_response(record->response, NULL, record->client_sd,
                          (record->mode == SSL_MODE) ? record->ssl_c : NULL);
            debug("\nPerformed %zu byte write on proxy->client socket %d",
                  record->response->header_len + record->response->content_len,
                  record->client_sd);
        }

        if (record->mode == SSL_MODE) {  // for some reason, removing this
                                         // stalls the ssl connections
            return REMOVE_CLIENT;
        }
        release_server_socket(proxy, record);

        // should be clearing the entire server buffer, because we may read
        // beyond response content length and we don't know what that is..
        CharVector_clear(record->server_vector,
                         CharVector_size(record->server_vector) - 1);
        record->waiting_to_forward_get_response = 0;
        record->response_progress = 0;
        record->response = NULL;
        record->request = NULL;
        debug("Finished response - calling check_client_vector");
        return check_client_vector(proxy, record);
    }
}

static void delete_record_from_proxy(Proxy_T proxy, ClientRecord_T prev,
                                     ClientRecord_T curr) {
    //debug("Deleting client record with client socket %d", curr->client_sd);
    if (prev == NULL) {
        proxy->records = proxy->records->next;
    } else {
        prev->next = curr->next;
    }
    clean_close(curr->client_sd, &proxy->master_set);

    // can be -1 if responding to client from cache close necessary for closing
    // CONNECT server connection
    if (curr->server_sd >= 0) {
        if (curr->mode == SSL_MODE) {
            auto_update_socket_max(proxy, curr->server_sd);
            clean_close(curr->server_sd, &proxy->master_set);
        } else {
            release_server_socket(proxy, curr);
        }
    }
    auto_update_socket_max(proxy, curr->client_sd);
    ClientRecord_free(&curr);
}

static size_t load_banned_hosts(Proxy_T proxy, const char *banned_hosts) {
    proxy->banned_hosts = Set_new(100, strhashcmp, strhash);
    FILE *fp = fopen(banned_hosts, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    if (fp == NULL) {
        custom_err("Could not open banned_hosts file %s", banned_hosts);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        char *hostname = create_starting_substr(line, read - 1);
        tolowercase(hostname);
        if (Set_put(proxy->banned_hosts, hostname) != NULL) {
            free(hostname);
        }
    }
    if (line != NULL) {
        free(line);
    }
    fclose(fp);
    return Set_length(proxy->banned_hosts);
}

Proxy_T Proxy_new(unsigned short portno, size_t cache_ram_capacity,
                  const char *banned_hosts, size_t total_bytes,
                  time_t rate_time, int modify_responses,
                  size_t unused_server_socket_capacity, int http_mode) {
    Proxy_T proxy;
    proxy = checked_malloc(sizeof(*proxy));
    proxy->server_sd = init_server_socket(portno);
    proxy->cache = Cache_new(cache_ram_capacity);
    proxy->socket_max = proxy->server_sd;
    FD_ZERO(&proxy->master_set);
    FD_SET(proxy->server_sd, &proxy->master_set);
    proxy->records = NULL;
    size_t num_banned_hosts = load_banned_hosts(proxy, banned_hosts);
    proxy->total_bytes = total_bytes;
    proxy->rate_time.tv_sec = rate_time;
    proxy->rate_time.tv_usec = 0;
    proxy->modify_responses = modify_responses;
    proxy->unused_server_sockets =
        ServerSocketTable_new(unused_server_socket_capacity);
    proxy->http_mode = http_mode;
    if (http_mode) {
        proxy->proxy_ctx = NULL;
    } else {
        proxy->proxy_ctx = create_ps_context();
    }
    debug(
        "\nCreated proxy with configuration:\n\n\tport %hu\n\tcache RAM "
        "capacity "
        "%zu\n\t%zu banned hosts\n\trate limit %zu bytes/%d s\n\tmodify "
        "responses %d\n\tmaximum unused server sockets %zu\n\thttp mode %d\n",
        portno, cache_ram_capacity, num_banned_hosts, total_bytes, rate_time,
        modify_responses, unused_server_socket_capacity, http_mode);
    return proxy;
}

void Proxy_start(Proxy_T proxy) {
    // Begin listening for client connections (cannot fail)
    listen(proxy->server_sd, 5);
    debug("Started proxy - listening for connections");

    int keep_alive = 1;
    while (keep_alive) {
        fd_set curr_ready = proxy->master_set;
        select(proxy->socket_max + 1, &curr_ready, NULL, NULL, NULL);
        if (FD_ISSET(proxy->server_sd, &curr_ready)) {
            handle_new_client(proxy);
        }
        ClientRecord_T prev = NULL;
        ClientRecord_T curr = proxy->records;
        while (curr != NULL) {
            ClientRecord_T next = curr->next;
            int client_removed = 0;
            if (FD_ISSET(curr->client_sd, &curr_ready)) {
                enum ProxyAction_T result = handle_client_sd(proxy, curr);
                if (result == PROXY_SHUTDOWN) {
                    keep_alive = 0;
                    break;
                }
                if (result == REMOVE_CLIENT) {
                    delete_record_from_proxy(proxy, prev, curr);
                    client_removed = 1;
                }
            }
            if (!client_removed && curr->server_sd >= 0 &&
                FD_ISSET(curr->server_sd, &curr_ready)) {
                enum ProxyAction_T result = handle_server_sd(proxy, curr);
                if (result == PROXY_SHUTDOWN) {
                    keep_alive = 0;
                    break;
                }
                if (result == REMOVE_CLIENT) {
                    delete_record_from_proxy(proxy, prev, curr);
                    client_removed = 1;
                }
            }

            if (!client_removed) {
                prev = curr;
            }
            curr = next;
        }
    }
    debug("Proxy shutting down");
}

void Proxy_free(Proxy_T *proxy_p) {
    if (proxy_p != NULL && (*proxy_p) != NULL) {
        free_records((*proxy_p)->records);
        Set_map((*proxy_p)->banned_hosts, strsetfree, NULL);
        Set_free(&((*proxy_p)->banned_hosts));
        ServerSocketTable_free(&((*proxy_p)->unused_server_sockets));
        Cache_free(&((*proxy_p)->cache));
        if (!(*proxy_p)->http_mode) {
            SSL_CTX_free((*proxy_p)->proxy_ctx);
        }
        free(*proxy_p);
        *proxy_p = NULL;
    }
}