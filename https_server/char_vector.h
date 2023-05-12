#ifndef _CHAR_VECTOR_H
#define _CHAR_VECTOR_H

#include <stdlib.h>

// Character vector type - clients should use this. This is an expanding
// character array.
typedef struct CharVector_T *CharVector_T;

// Initializes array with some initial capacity
extern CharVector_T CharVector_new(size_t initial_capacity);

// Pushes arr_len characters from arr onto end of vector
extern void CharVector_push(CharVector_T vector, char *arr, size_t arr_len);

// Number of elements in vector
extern size_t CharVector_size(CharVector_T vector);

// Clears up to and including the index characters in vector
extern void CharVector_clear(CharVector_T vector, int index);

// Frees vector
extern void CharVector_free(CharVector_T *vector_p);

// Gets pointer to underlying array in vector - may become invalidated due to
// push resize
extern char *CharVector_asarray(CharVector_T vector);

#endif