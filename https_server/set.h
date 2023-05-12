#ifndef SET_H
#define SET_H

#include <stddef.h>

typedef struct Set_T *Set_T;

// Constructs a set with a hinted size, comparison function, and hash function
extern Set_T Set_new(size_t hint, int cmp(const void *x, const void *y),
                     size_t hash(const void *x));

// Free function
extern void Set_free(Set_T *set);

// Number of elements in set
extern size_t Set_length(Set_T set);

// Checks if a member is an element of a set
extern int Set_member(Set_T set, void *member);

// Returns set member that matches provided member that's in the set if one
// exists. NULL if one does not.
extern void *Set_get(Set_T set, void *member);

// Puts a member into the set if it does not already exist. If it already
// exists, existing member is overwritten with this one (careful for memory
// leak!). If this caused an overwrite, the old member is returned so it can be
// freed. If the put did not overwrite, NULL is returned
extern void *Set_put(Set_T set, void *member);

// Removes member from set
extern void *Set_remove(Set_T set, void *member);

// Maps an apply function to each element of set. cl are additional parameters
// that are passed to mapped function
extern void Set_map(Set_T set, void apply(const void *member, void *cl),
                    void *cl);

#endif