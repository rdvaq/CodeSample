#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void custom_err(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void errno_err(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    perror("Errno Error");
    exit(1);
}