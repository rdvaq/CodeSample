#ifndef _DEBUGGING_H
#define _DEBUGGING_H

#include <stdlib.h>

#define DEBUGSTATUS 1

// Like printf, but only to stderr when DEBUGSTATUS is defined
extern void debug(const char *format, ...);

// Like print for a string, but prints a certain number of bytes only trying to
// display those that are deemed printable and for others just displaying their
// ASCII code
extern void debug_bytes(char *bytes, size_t nbytes);

// Reports if pointer is NULL
extern void debug_null(const char *name, void *ptr);

#endif