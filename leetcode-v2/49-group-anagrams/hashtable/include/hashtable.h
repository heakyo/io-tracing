#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdio.h>

#define HT_DEF_CAP 16

typedef struct hash_entry {
	int signature[26];
	int grp_index;
	struct hash_entry *next;
} HashEntry;

typedef struct hash_table {
	HashEntry **buckets;
	size_t size;
	size_t capacity;
} HashTable;

size_t hash_index(const int signature[26], size_t capacity);
HashTable *hash_create(size_t capacity);
int hash_insert(HashTable *ht, const int signature[26], int grp_index);
int hash_find(const HashTable *ht, const int signature[26], int *grp_index);
void hash_destroy(HashTable *ht);

#endif /* _HASHTABLE_H_ */
