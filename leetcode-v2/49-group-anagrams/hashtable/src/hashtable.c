#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/hashtable.h"

size_t
hash_index(const int signature[26], size_t capacity)
{
	size_t hash = 0;
	int i;

	for (i = 0; i < 26; i++)
		hash = hash * 31 + (size_t)signature[i];

	return (hash % capacity);
}

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

int hash_insert(HashTable *ht, const int signature[26], int grp_index)
{
	HashEntry **p, *new;
	size_t idx;

	idx = hash_index(signature, ht->capacity);
	p = &ht->buckets[idx];

	while (*p) {
		if (!memcmp((*p)->signature, signature, 26*sizeof(int))) 
			return 0;

		p = &(*p)->next;
	}

	new = malloc(sizeof(HashEntry));
	if (!new)
		return -1;

	memcpy(new->signature, signature, 26*sizeof(int));
	new->grp_index = grp_index;
	new->next = NULL;
	*p = new;

	ht->size++;

	return 0;
}

int hash_find(const HashTable *ht, const int signature[26], int *grp_index)
{
	HashEntry *he;
	size_t idx;

	idx = hash_index(signature, ht->capacity);
	he = ht->buckets[idx];

	while (he) {
		if (!memcmp(he->signature, signature, 26*sizeof(int))) {
			if (grp_index)
				*grp_index = he->grp_index;
			return 1;
		}

		he = he->next;
	}

	return 0;
}

void hash_destroy(HashTable *ht)
{
	HashEntry *he, *tmp;
	size_t i;

	if (!ht)
		return;

	for (i = 0; i < ht->capacity; i++) {

		he = ht->buckets[i];
		while (he) {
			tmp = he;
			he = he->next;

			free(tmp);
		}
	}
	
	free(ht->buckets);
	free(ht);
}
