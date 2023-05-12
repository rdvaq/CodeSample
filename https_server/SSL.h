#ifndef _SSL_H
#define _SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>


SSL_CTX *create_new_context(char* hostname);

SSL_CTX *create_ps_context();

SSL* create_new_conn_obj(SSL_CTX *ctx, int fd);

int checked_SSL_write(SSL* ssl, char* buf, int len);

int checked_SSL_accept(SSL* ssl);

int checked_SSL_connect(SSL* ssl);

void SSL_free_sess(SSL* ssl);




#endif