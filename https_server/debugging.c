#include "debugging.h"

#include <stdarg.h>
#include <stdio.h>

extern void debug(const char *format, ...) {
#ifdef DEBUGSTATUS
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
#endif
}

static int is_printable_byte(char byte) {
    return byte == 9 || byte == 10 || (byte >= 32 && byte <= 126);
}

extern void debug_bytes(char *bytes, size_t nbytes) {
#ifdef DEBUGSTATUS
    while (nbytes > 0) {
        if (is_printable_byte(*bytes)) {
            putc(*bytes, stderr);
        } else {
            fprintf(stderr, "{%d}", (int)*bytes);
        }
        bytes++;
        nbytes--;
    }
#endif
}

extern void debug_null(const char *name, void *ptr) {
#ifdef DEBUGSTATUS
    debug(ptr == NULL ? "%s is NULL" : "%s is not NULL", name);
#endif
}