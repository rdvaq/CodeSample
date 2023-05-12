#include "table.h"

#include <assert.h>
#include <stdlib.h>

#include "more_memory.h"

static const double LOAD_FACTOR_THRESHOLD = 0.5;

typedef struct Table_T *Table_T;

struct Table_T {
    size_t num_elements;
    size_t num_buckets;
    size_t mod_ops;
    int (*cmp)(const void *key1, const void *key2);
    size_t (*hash)(const void *key);
    struct table_node {
        struct table_node *next;
        // Not marking a member as const since this table_node type is
        // hidden from client via opaque pointer, can't find
        // anything online about modifying const variables in struct
        // did find that this is considered bad practice:
        // https://softwareengineering.stackexchange.com/a/222461
        void *key;
        void *value;
    } * *buckets;
};

// Checks if n is prime
static int is_prime(int n) {
    // Optimization: 2 is the only prime even number
    if (n % 2 == 0) return n == 2;
    // Only need to go to halfway point over odd numbers up to n
    // anything about halfway point yields quotient of 1
    for (int i = 3; i < (n / 2) + 1; i += 2)
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

// hash into table
static size_t hash(Table_T table, void *key) {
    return table->hash(key) % table->num_buckets;
}

static struct table_node **find(Table_T table, void *key) {
    // Get pointer to table_node pointer containing key or pointer to
    // the next of the last table_node pointer in the bucket
    // for struct table_node **x, &x[h] = &(*(x+h)) = x + h
    // I just do x + h versus what's given in the text
    struct table_node **itr = table->buckets + hash(table, key);
    for (; *itr && table->cmp((*itr)->key, key) != 0; itr = &((*itr)->next))
        ;
    return itr;
}

// push table_node on top of a bucket
static void push_node(void *key, void *value, struct table_node **bucket) {
    struct table_node *new_node;
    new_node = checked_malloc(sizeof(*new_node));
    new_node->key = key;
    new_node->value = value;
    new_node->next = *bucket;
    *bucket = new_node;
}

// allocates buckets and inits to NULL
static struct table_node **allocate_buckets(size_t num_buckets) {
    struct table_node **buckets;
    buckets = checked_malloc(sizeof(buckets[0]) * num_buckets);
    size_t i;
    for (i = 0; i < num_buckets; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

static void auto_rehash(Table_T table) {
    if (table->num_elements < LOAD_FACTOR_THRESHOLD * table->num_buckets)
        return;

    struct table_node **cpy = table->buckets;
    // update table buckets so hash works properly
    size_t old_num_buckets = table->num_buckets;
    table->num_buckets *= 2;
    table->buckets = allocate_buckets(table->num_buckets);
    struct table_node *next;
    size_t i;
    for (i = 0; i < old_num_buckets; i++)
        for (; cpy[i]; cpy[i] = next) {
            // push onto the appropriate bucket and free from old buckets
            push_node(cpy[i]->key, cpy[i]->value,
                      table->buckets + hash(table, cpy[i]->key));
            next = cpy[i]->next;
            free(cpy[i]);
        }
    free(cpy);
}

Table_T Table_new(size_t hint, int cmp(const void *x, const void *y),
                  size_t hash(const void *key)) {
    assert(hint >= 0 && cmp != NULL && hash != NULL);
    size_t num_buckets = next_prime(hint);
    Table_T table;
    table = checked_malloc(sizeof(*table));
    table->cmp = cmp;
    table->hash = hash;
    table->num_elements = 0;
    table->num_buckets = num_buckets;
    table->mod_ops = 0;
    // buckets are not allocated as part of table struct allocation
    // because buckets must be freed during rehashing
    table->buckets = allocate_buckets(num_buckets);
    return table;
}

void Table_free(Table_T *table) {
    if (table == NULL || *table == NULL) {
        return;
    }
    struct table_node *next;
    size_t i;
    for (i = 0; i < (*table)->num_buckets; i++)
        for (; (*table)->buckets[i]; (*table)->buckets[i] = next) {
            next = (*table)->buckets[i]->next;
            free((*table)->buckets[i]);
        }
    free((*table)->buckets);
    free(*table);
    *table = NULL;
}

size_t Table_length(Table_T table) {
    assert(table);
    return table->num_elements;
}

void *Table_put(Table_T table, void *key, void *value) {
    assert(table);
    auto_rehash(table);
    // will store bucket to push onto
    struct table_node **table_node = find(table, key);
    // overwrite if find yielded result
    void *out = NULL;
    if (*table_node) {
        out = (*table_node)->key;
        (*table_node)->key = key;
        (*table_node)->value = value;
    } else {
        // push onto bucket, note here to get the pointer
        // to the pointer that heads a bucket (so that bucket
        // head can be updated) we add to a table_node** to get
        // a table_node**
        push_node(key, value, table->buckets + hash(table, key));
        table->num_elements++;
    }
    table->mod_ops++;
    return out;
}

void *Table_get(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    assert(*table_node != NULL);
    return (*table_node)->value;
}

void *Table_key(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    return *table_node == NULL ? NULL : (*table_node)->key;
}

int Table_contains(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    return *table_node != NULL;
}

void *Table_remove(Table_T table, void *key) {
    assert(table);
    struct table_node **table_node = find(table, key);
    assert(*table_node != NULL);
    struct table_node *next = (*table_node)->next;
    void *value = (*table_node)->value;
    free(*table_node);
    *table_node = next;
    table->num_elements--;
    table->mod_ops++;
    return value;
}

void Table_map(Table_T table, void apply(void *key, void **value, void *cl),
               void *cl) {
    assert(table && apply);
    unsigned start_mod_ops = table->mod_ops;
    size_t i;
    for (i = 0; i < table->num_buckets; i++)
        for (struct table_node *table_node = table->buckets[i]; table_node;
             table_node = table_node->next) {
            apply(table_node->key, &table_node->value, cl);
            assert(table->mod_ops == start_mod_ops);
        }
}
