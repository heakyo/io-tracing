#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdio.h>

#define HT_DEF_CAP 16

typedef struct hash_entry {
	int key;
	int value;
	struct hash_entry *next;
} HashEntry;

typedef struct hash_table {
	HashEntry **buckets;
	size_t size;
	size_t capacity;
} HashTable;

/*
 * Create a hash table.
 *
 * capacity: number of buckets
 *
 * Return:
 *     success -> pointer to HashTable
 *     failure -> NULL
 */
HashTable *hash_create(size_t capacity);

/*
 * Insert a key/value pair.
 *
 * ht:    hash table
 * key:   key
 * value: value associated with key
 *
 * Return:
 *      0 -> success
 *     -1 -> failure
 *
 * If key already exists, update its value.
 */
int hash_insert(HashTable *ht, int key, int value);

/*
 * Find a key.
 *
 * ht:     hash table
 * key:    key to search
 * value:  output parameter; receives associated value
 *
 * Return:
 *      1 -> found
 *      0 -> not found
 */
int hash_find(const HashTable *ht, int key, int *value);

/*
 * Destroy hash table and release all allocated memory.
 */
void hash_destroy(HashTable *ht);

#endif /* _HASHTABLE_H_ */
