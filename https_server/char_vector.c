#include "char_vector.h"

#include "errors.h"
#include "more_memory.h"

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