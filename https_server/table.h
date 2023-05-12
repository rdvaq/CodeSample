#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED

#include <stddef.h>

typedef struct Table_T *Table_T;

// Constructs a table with a hinted size, comparison function, and hash
// function. All that is required of the comparison function is that it supports
// equality (0 return) and inequality (non 0 return)
extern Table_T Table_new(size_t hint, int cmp(const void *x, const void *y),
                         size_t hash(const void *key));

// Free function
extern void Table_free(Table_T *table);

// Number of elements in table
extern size_t Table_length(Table_T table);

// Puts a member into the table if it does not already exist. If it already
// exists, existing value is overwritten with this value. The old key is returned.
extern void *Table_put(Table_T table, void *key, void *value);

// Checks if key is in table - safer alternative to unchecked GET
extern int Table_contains(Table_T table, void *key);

// Gets value corresponding to key. Program aborts if it doesn't exist
extern void *Table_get(Table_T table, void *key);

// Gets key in table that matches provided key on cmp. NULL is returned if it doesn't exist
extern void *Table_key(Table_T table, void *key);

// Removes key, value from table - value returned. Program aborts if it doesn't
// exist
extern void *Table_remove(Table_T table, void *key);

// Maps an apply function to each element of table. cl are additional parameters
// that are passed to mapped function
extern void Table_map(Table_T table,
                      void apply(void *key, void **value, void *cl), void *cl);

#endif