#ifndef _MORE_STRING_H
#define _MORE_STRING_H

#include <stdlib.h>
#include <time.h>

// Removes whitespace from beginning and end of string and returns a new heap
// alloc'd string (string must be null terminated)
extern char *trim(char *str);

// Converts a string to lowercase in place (string must be null terminated)
extern void tolowercase(char *s);

// Creates a string array of the strings in str split on a provided delimiter
// (str must be null terminated). num_tokens is specified to the length of the
// resulting number of strings (note the array is heap alloc'd and should be
// freed with free_str_array)
extern char **strsplit(char *str, const char *delim, size_t *num_tokens);

// Converts nonnegative time_t to heap alloc'd string
extern char *nonneg_timet_to_str(time_t t);

// Converts size_t to heap alloc'd string
extern char *size_t_to_str(size_t s);

// Creates heap alloc'd substring of str that is the first len characters.
// Resulting string will be null terminated. Can also copy a string by doing
// create_starting_substr(s, strlen(s)).
extern char *create_starting_substr(char *str, size_t len);

// String hash function that matches header required by Table, Set. key must be
// a null terminated string.
extern size_t strhash(const void *key);

// String comparison function that matches header required by Table, Set. x, y
// must be null terminated strings.
extern int strhashcmp(const void *x, const void *y);

// String value free matching header required by Set
extern void strsetfree(const void *member, void *cl);

// Checks if a null terminated string starts with another null terminated string
extern int startswith(const char *str, const char *query);

// Checks if a null terminated string ends with another null terminated string
extern int endswith(const char *str, const char *query);

#endif