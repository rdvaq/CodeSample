#ifndef _MORE_MEMORY_H
#define _MORE_MEMORY_H

#include <stdlib.h>

// Checks that malloc doesn't fail, if it does program aborts
extern void *checked_malloc(size_t size);

// Frees an array of strings
extern void free_str_array(char **str_array, size_t len);

#endif