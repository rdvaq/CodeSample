#include "set.h"

#include <assert.h>
#include <stdlib.h>

#include "more_memory.h"

typedef struct Set_T *Set_T;

static const size_t EMPTY_SET_HINT = 100;

static const double LOAD_FACTOR_THRESHOLD = 0.5;

struct Set_T {
    size_t num_elements;
    size_t num_buckets;
    size_t mod_ops;
    int (*cmp)(const void *member1, const void *member2);
    size_t (*hash)(const void *member);
    struct set_node {
        struct set_node *next;
        void *member;
    } * *buckets;
};

// Checks if n is prime
static int is_prime(size_t n) {
    // Optimization: 2 is the only prime even number
    if (n % 2 == 0) return n == 2;
    // Only need to go to halfway point over odd numbers up to n
    // anything about halfway point yields quotient of 1
    size_t i;
    for (i = 3; i < (n / 2) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Finds the smallest prime number > n
static size_t next_prime(size_t n) {
    n += (n % 2 == 0);
    for (; !is_prime(n); n += 2)
        ;
    return n;
}

// hash into set
static size_t hash(Set_T set, void *member) {
    return set->hash(member) % set->num_buckets;
}

static struct set_node **find(Set_T set, void *member) {
    // Get pointer to set_node pointer containing member or pointer to
    // the next of the last set_node pointer in the bucket
    // for struct set_node **x, &x[h] = &(*(x+h)) = x + h
    // I just do x + h versus what's given in the text
    struct set_node **itr = set->buckets + hash(set, member);
    for (; *itr && set->cmp((*itr)->member, member) != 0; itr = &((*itr)->next))
        ;
    return itr;
}

// push set_node on top of a bucket
static void push_node(void *member, struct set_node **bucket) {
    struct set_node *new_node;
    new_node = checked_malloc(sizeof(*new_node));
    new_node->member = member;
    new_node->next = *bucket;
    *bucket = new_node;
}

// allocates buckets and inits to NULL
static struct set_node **allocate_buckets(size_t num_buckets) {
    struct set_node **buckets;
    buckets = checked_malloc(sizeof(buckets[0]) * num_buckets);
    size_t i;
    for (i = 0; i < num_buckets; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

static void auto_rehash(Set_T set) {
    if (set->num_elements < LOAD_FACTOR_THRESHOLD * set->num_buckets) return;

    struct set_node **cpy = set->buckets;
    // update set buckets so hash works properly
    unsigned old_num_buckets = set->num_buckets;
    set->num_buckets *= 2;
    set->buckets = allocate_buckets(set->num_buckets);
    struct set_node *next;
    size_t i;
    for (i = 0; i < old_num_buckets; i++)
        for (; cpy[i]; cpy[i] = next) {
            // push onto the appropriate bucket and free from old buckets
            push_node(cpy[i]->member, set->buckets + hash(set, cpy[i]->member));
            next = cpy[i]->next;
            free(cpy[i]);
        }
    free(cpy);
}

Set_T Set_new(size_t hint, int cmp(const void *x, const void *y),
              size_t hash(const void *x)) {
    assert(hint >= 0 && cmp != NULL && hash != NULL);
    size_t num_buckets = next_prime(hint);
    Set_T set;
    set = checked_malloc(sizeof(*set));
    set->cmp = cmp;
    set->hash = hash;
    set->num_elements = 0;
    set->num_buckets = num_buckets;
    set->mod_ops = 0;
    // buckets are not allocated as part of set struct allocation
    // because buckets must be freed during rehashing
    set->buckets = allocate_buckets(num_buckets);
    return set;
}

void Set_free(Set_T *set) {
    if (set == NULL || *set == NULL) {
        return;
    }
    struct set_node *next;
    size_t i;
    for (i = 0; i < (*set)->num_buckets; i++)
        for (; (*set)->buckets[i]; (*set)->buckets[i] = next) {
            next = (*set)->buckets[i]->next;
            free((*set)->buckets[i]);
        }
    free((*set)->buckets);
    free(*set);
    *set = NULL;
}

size_t Set_length(Set_T set) {
    assert(set);
    return set->num_elements;
}

int Set_member(Set_T set, void *member) {
    assert(set && member);
    return *find(set, member) != NULL;
}

void *Set_get(Set_T set, void *member) {
    assert(set && member);
    struct set_node *find_result = *find(set, member);
    return find_result == NULL ? NULL : find_result->member;
}

void *Set_put(Set_T set, void *member) {
    assert(set && member);
    auto_rehash(set);
    struct set_node **set_node = find(set, member);
    void *old = NULL;
    if (*set_node) {
        old = (*set_node)->member;
        (*set_node)->member = member;
    } else {
        // push onto bucket, note here to get the pointer
        // to the pointer that heads a bucket (so that bucket
        // head can be updated) we add to a set_node** to get
        // a set_node**
        push_node(member, set->buckets + hash(set, member));
        set->num_elements++;
    }
    set->mod_ops++;
    return old;
}

void *Set_remove(Set_T set, void *member) {
    assert(set && member);
    struct set_node **set_node = find(set, member);
    if (*set_node == NULL) return NULL;
    struct set_node *next = (*set_node)->next;
    void *out = (*set_node)->member;
    free(*set_node);
    *set_node = next;
    set->num_elements--;
    set->mod_ops++;
    return out;
}

void Set_map(Set_T set, void apply(const void *member, void *cl), void *cl) {
    assert(set && apply);
    unsigned start_mod_ops = set->mod_ops;
    size_t i;
    for (i = 0; i < set->num_buckets; i++)
        for (struct set_node *set_node = set->buckets[i]; set_node;
             set_node = set_node->next) {
            apply(set_node->member, cl);
            assert(set->mod_ops == start_mod_ops);
        }
}