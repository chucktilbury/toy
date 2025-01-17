/*
 * Public interface for hash tables.
 */
#ifndef _HASH_H_
#define _HASH_H_

#include <stdlib.h> // size_t

typedef struct {
    const char* key;
    void* data;
} _hash_node_t;

/*
 * If a node's key is NULL, but the bucket pointer in the table
 * is not NULL, then the bucket is a tombstone.
 */
typedef struct {
    _hash_node_t** table;
    int cap;
    int count;
    int tombstones;
} hash_table_t;


hash_table_t* create_hashtable(void);
void destroy_hashtable(hash_table_t* table);
void insert_hashtable(hash_table_t* table, const char* key, void* data);
void* find_hashtable(hash_table_t* tab, const char* key);
void remove_hashtable(hash_table_t* tab, const char* key);

void dump_hashtable(hash_table_t* tab);

#endif
