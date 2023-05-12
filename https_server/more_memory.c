#include "errors.h"
#include "more_memory.h"

void *checked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        errno_err("MALLOC for %zu bytes failed", size);
    }
    return ptr;
}

extern void free_str_array(char **str_array, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        free(str_array[i]);
    }
    free(str_array);
}