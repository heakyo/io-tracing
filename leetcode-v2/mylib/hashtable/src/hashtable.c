#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

HashTable *hash_create(size_t capacity)
{
	HashTable *ht;

	if (capacity == 0)
		capacity = HT_DEF_CAP;

	ht = malloc(sizeof(*ht));
	if (!ht)
		return NULL;

	ht->buckets = calloc(capacity, sizeof(HashEntry *));
	if (!ht->buckets) {
		free(ht);
		return NULL;
	}

	ht->capacity = capacity;
	ht->size = 0;

	return ht;
}
