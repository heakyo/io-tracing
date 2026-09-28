#include <stdio.h>
#include <stdlib.h>

#include "../include/hashtable.h"

size_t hash_index(int key, size_t capacity)
{
	long long k;

	k = key;

	if (k < 0)
		k = -k;

	return ((size_t)k % capacity);
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

int hash_insert(HashTable *ht, int key, int value)
{
	HashEntry **p, *new;
	size_t idx;

	idx = hash_index(key, ht->capacity);
	p = &ht->buckets[idx];

	while (*p) {
		if ((*p)->key == key) {
			(*p)->value = value;
			return 0;
		}

		p = &(*p)->next;
	}

	new = malloc(sizeof(HashEntry));
	if (!new)
		return -1;

	new->key = key;
	new->value = value;
	new->next = NULL;
	*p = new;

	ht->size++;

	return 0;
}

int hash_find(const HashTable *ht, int key, int *value)
{
	HashEntry *he;
	size_t idx;

	idx = hash_index(key, ht->capacity);
	he = ht->buckets[idx];

	while (he) {
		if (he->key == key) {
			if (value)
				*value = he->value;
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
