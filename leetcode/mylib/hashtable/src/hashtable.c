#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define HT_DEFAULT_CAP	16
#define HT_LOAD_FACTOR	0.75
#define HT_GROWTH_FACTOR 2

typedef struct ht_entry {
	char *key;
	void *value;
	int occupied;	/* 1 = live entry, 0 = empty, -1 = tombstone */
} ht_entry_t;

struct hashtable {
	ht_entry_t *buckets;
	size_t capacity;
	size_t size;
};

/* FNV-1a hash */
static unsigned long ht_hash(const char *key, size_t cap)
{
	unsigned long h = 2166136261UL;
	const unsigned char *p = (const unsigned char *)key;

	while (*p) {
		h ^= *p++;
		h *= 16777619UL;
	}
	return h % cap;
}

static int ht_resize(hashtable_t *ht, size_t new_cap)
{
	ht_entry_t *old_buckets = ht->buckets;
	size_t old_cap = ht->capacity;
	size_t i;

	ht->buckets = calloc(new_cap, sizeof(ht_entry_t));
	if (!ht->buckets) {
		ht->buckets = old_buckets;
		return -1;
	}

	ht->capacity = new_cap;
	ht->size = 0;

	for (i = 0; i < old_cap; i++) {
		if (old_buckets[i].occupied == 1) {
			ht_set(ht, old_buckets[i].key, old_buckets[i].value);
			free(old_buckets[i].key);
		}
	}

	free(old_buckets);
	return 0;
}

hashtable_t *ht_create(size_t capacity)
{
	hashtable_t *ht;

	if (capacity == 0)
		capacity = HT_DEFAULT_CAP;

	ht = malloc(sizeof(*ht));
	if (!ht)
		return NULL;

	ht->buckets = calloc(capacity, sizeof(ht_entry_t));
	if (!ht->buckets) {
		free(ht);
		return NULL;
	}

	ht->capacity = capacity;
	ht->size = 0;
	return ht;
}

void ht_destroy(hashtable_t *ht)
{
	size_t i;

	if (!ht)
		return;

	for (i = 0; i < ht->capacity; i++) {
		if (ht->buckets[i].occupied == 1)
			free(ht->buckets[i].key);
	}

	free(ht->buckets);
	free(ht);
}

int ht_set(hashtable_t *ht, const char *key, void *value)
{
	unsigned long idx;
	size_t i;

	if (!ht || !key)
		return -1;

	/* Resize if load factor exceeded */
	if ((double)(ht->size + 1) / ht->capacity > HT_LOAD_FACTOR) {
		if (ht_resize(ht, ht->capacity * HT_GROWTH_FACTOR) < 0)
			return -1;
	}

	idx = ht_hash(key, ht->capacity);

	for (i = 0; i < ht->capacity; i++) {
		size_t pos = (idx + i) % ht->capacity;

		/* Update existing key */
		if (ht->buckets[pos].occupied == 1 &&
		    strcmp(ht->buckets[pos].key, key) == 0) {
			ht->buckets[pos].value = value;
			return 0;
		}

		/* Insert into empty or tombstone slot */
		if (ht->buckets[pos].occupied <= 0) {
			ht->buckets[pos].key = strdup(key);
			if (!ht->buckets[pos].key)
				return -1;
			ht->buckets[pos].value = value;
			ht->buckets[pos].occupied = 1;
			ht->size++;
			return 0;
		}
	}

	return -1;
}

void *ht_get(hashtable_t *ht, const char *key)
{
	unsigned long idx;
	size_t i;

	if (!ht || !key)
		return NULL;

	idx = ht_hash(key, ht->capacity);

	for (i = 0; i < ht->capacity; i++) {
		size_t pos = (idx + i) % ht->capacity;

		if (ht->buckets[pos].occupied == 0)
			return NULL;

		if (ht->buckets[pos].occupied == 1 &&
		    strcmp(ht->buckets[pos].key, key) == 0)
			return ht->buckets[pos].value;
	}

	return NULL;
}

void *ht_remove(hashtable_t *ht, const char *key)
{
	unsigned long idx;
	size_t i;
	void *val;

	if (!ht || !key)
		return NULL;

	idx = ht_hash(key, ht->capacity);

	for (i = 0; i < ht->capacity; i++) {
		size_t pos = (idx + i) % ht->capacity;

		if (ht->buckets[pos].occupied == 0)
			return NULL;

		if (ht->buckets[pos].occupied == 1 &&
		    strcmp(ht->buckets[pos].key, key) == 0) {
			val = ht->buckets[pos].value;
			free(ht->buckets[pos].key);
			ht->buckets[pos].key = NULL;
			ht->buckets[pos].value = NULL;
			ht->buckets[pos].occupied = -1; /* tombstone */
			ht->size--;
			return val;
		}
	}

	return NULL;
}

int ht_contains(hashtable_t *ht, const char *key)
{
	return ht_get(ht, key) != NULL ? 1 : 0;
}

size_t ht_size(hashtable_t *ht)
{
	if (!ht)
		return 0;
	return ht->size;
}

void ht_iter_init(hashtable_t *ht, ht_iter_t *iter)
{
	if (!iter)
		return;

	iter->key = NULL;
	iter->value = NULL;
	iter->_ht = ht;
	iter->_index = 0;
}

int ht_iter_next(ht_iter_t *iter)
{
	hashtable_t *ht;

	if (!iter || !iter->_ht)
		return 0;

	ht = iter->_ht;

	while (iter->_index < ht->capacity) {
		size_t i = iter->_index++;

		if (ht->buckets[i].occupied == 1) {
			iter->key = ht->buckets[i].key;
			iter->value = ht->buckets[i].value;
			return 1;
		}
	}

	return 0;
}
