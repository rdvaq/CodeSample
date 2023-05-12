#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SSL.h"


void configure_context(SSL_CTX *ctx, char* keypath, char* certpath) {
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, certpath, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keypath, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }    
}

static inline void cert_generation() {
    system("sh sign_cert.sh");
}

void make_sign_config(char* hostname) {
    char text[] = "authorityKeyIdentifier=keyid,issuer\nbasicConstraints=CA:FALSE\nkeyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment\nsubjectAltName = @alt_names\n\n[alt_names]\nDNS.1 = ";
    int hostname_len = strlen(hostname);
    int text_len = strlen(text);
    char* output = calloc(hostname_len+text_len, 1);
    memcpy(output, text, text_len);
    memcpy(output+text_len, hostname, hostname_len);

    int fd = open("./proxy_key_cert/cert_config.ext", O_WRONLY | O_CREAT, 0666);
    int n = write(fd, output, hostname_len+text_len);
    close(fd);
}

/*create a new ssl_ctx with new cert cooresponding to the hostname passed in*/
SSL_CTX *create_new_context(char* hostname) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_method();

    ctx =  SSL_CTX_new(method);

    if (ctx == NULL) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    make_sign_config(hostname);
    cert_generation();
    configure_context(ctx, "./proxy_key_cert/key.pem", "./proxy_key_cert/cert.pem");
    system("rm ./proxy_key_cert/cert.pem");
    system("rm ./proxy_key_cert/cert_config.ext");

    return ctx;
}

SSL_CTX *create_ps_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_client_method();

    ctx =  SSL_CTX_new(method);

    if (ctx == NULL) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    configure_context(ctx, "./proxy_key_cert/key.pem", "./proxy_key_cert/def_cert.pem");

    return ctx;
}

SSL* create_new_conn_obj(SSL_CTX *ctx, int fd) {
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, fd);
    return ssl;
}

int checked_SSL_accept(SSL* ssl) {
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    } 
    return 1;
}

int checked_SSL_write(SSL* ssl, char* buf, int len) {
    if (SSL_write(ssl, buf, len) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    return 1;
}

int checked_SSL_connect(SSL* ssl) {
    if (SSL_connect(ssl) <= 0) {  /* perform the connection */  
        ERR_print_errors_fp(stderr);
        return -1;
    }
    return 1;
}

void SSL_free_sess(SSL* ssl) {
    if (ssl == NULL) {
        return;
    }
    int ret = SSL_shutdown(ssl);
    SSL_free(ssl);
}

