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

HashTable *hash_create(size_t capacity);

#endif /* _HASHTABLE_H_ */
