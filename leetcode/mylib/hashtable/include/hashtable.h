#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * A generic hash table library using open addressing (linear probing).
 * Supports string keys and void* values.
 */

/* Opaque handle */
typedef struct hashtable hashtable_t;

/* Iterator for traversing all entries */
typedef struct ht_iter {
	const char *key;
	void *value;
	/* private */
	hashtable_t *_ht;
	size_t _index;
} ht_iter_t;

/*
 * ht_create - Create a new hash table.
 * @capacity: initial number of buckets (0 for default 16).
 *
 * Returns a new hash table handle, or NULL on failure.
 */
hashtable_t *ht_create(size_t capacity);

/*
 * ht_destroy - Destroy a hash table and free all internal memory.
 * @ht: hash table handle.
 *
 * Note: does NOT free the stored values. Caller is responsible
 * for freeing values before calling ht_destroy if needed.
 */
void ht_destroy(hashtable_t *ht);

/*
 * ht_set - Insert or update a key-value pair.
 * @ht:    hash table handle.
 * @key:   null-terminated string key (a copy is stored internally).
 * @value: pointer to the value to store.
 *
 * Returns 0 on success, -1 on failure (allocation error).
 * If the key already exists, the old value is replaced.
 */
int ht_set(hashtable_t *ht, const char *key, void *value);

/*
 * ht_get - Look up a value by key.
 * @ht:  hash table handle.
 * @key: null-terminated string key.
 *
 * Returns the stored value pointer, or NULL if the key is not found.
 */
void *ht_get(hashtable_t *ht, const char *key);

/*
 * ht_remove - Remove a key-value pair.
 * @ht:  hash table handle.
 * @key: null-terminated string key.
 *
 * Returns the removed value pointer, or NULL if the key was not found.
 * Caller is responsible for freeing the returned value if needed.
 */
void *ht_remove(hashtable_t *ht, const char *key);

/*
 * ht_contains - Check whether a key exists.
 * @ht:  hash table handle.
 * @key: null-terminated string key.
 *
 * Returns 1 if found, 0 if not.
 */
int ht_contains(hashtable_t *ht, const char *key);

/*
 * ht_size - Get the number of key-value pairs stored.
 * @ht: hash table handle.
 *
 * Returns the number of entries.
 */
size_t ht_size(hashtable_t *ht);

/*
 * ht_iter_init - Initialize an iterator for traversal.
 * @ht:   hash table handle.
 * @iter: pointer to an ht_iter_t to initialize.
 */
void ht_iter_init(hashtable_t *ht, ht_iter_t *iter);

/*
 * ht_iter_next - Advance the iterator to the next entry.
 * @iter: pointer to an initialized ht_iter_t.
 *
 * Returns 1 if a next entry was found (iter->key and iter->value are valid),
 * or 0 if iteration is complete.
 */
int ht_iter_next(ht_iter_t *iter);

#ifdef __cplusplus
}
#endif

#endif /* HASHTABLE_H */
