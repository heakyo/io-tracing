#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "../include/hashtable.h"

/* Function declaration */
static void test_hash_create(void);
static void test_hash_insert(void);
static void test_hash_find(void);
static void test_hash_destroy(void);

int
main(void)
{
    //test_hash_create();
    //test_hash_insert();
    //test_hash_find();
    test_hash_destroy();

    return (0);
}

static void
test_hash_create(void)
{
    size_t capacity = 16;
    HashTable *ht;

    printf("Running test_hash_create...\n");

    ht = hash_create(capacity);

    /* Verify that the hash table was created successfully */
    assert(ht != NULL);

    /* Verify that the capacity is correct */
    assert(ht->capacity == capacity);

    /* A newly created hash table should contain no entries */
    assert(ht->size == 0);

    /* Verify that the bucket array was allocated */
    assert(ht->buckets != NULL);

    /* All buckets should initially be NULL */
    for (size_t i = 0; i < capacity; i++) {
        assert(ht->buckets[i] == NULL);
    }

    ht = hash_create(0);

    assert(ht != NULL);
    assert(ht->capacity == HT_DEF_CAP);
    assert(ht->size == 0);

    /*
     * hash_destroy() has not been implemented yet,
     * so release the allocated memory manually.
     */
    free(ht->buckets);
    free(ht);

    printf("test_hash_create PASSED\n");
}

static void
test_hash_insert(void)
{
        HashTable *ht;
        HashEntry *entry;
        size_t index;
        int ret;
        int found_2;
        int found_10;

        printf("Running test_hash_insert...\n");

        /*
         * Use a small capacity to make collisions easy to test.
         */
        ht = hash_create(8);
        assert(ht != NULL);

        /*
         * Test 1: Insert the first entry.
         */
        ret = hash_insert(ht, 2, 100);

        assert(ret == 0);
        assert(ht->size == 1);

        index = hash_index(2, ht->capacity);
        entry = ht->buckets[index];

        assert(entry != NULL);
        assert(entry->key == 2);
        assert(entry->value == 100);

        /*
         * Test 2: Insert another entry into a different bucket.
         */
        ret = hash_insert(ht, 3, 200);

        assert(ret == 0);
        assert(ht->size == 2);

        index = hash_index(3, ht->capacity);
        entry = ht->buckets[index];

        assert(entry != NULL);
        assert(entry->key == 3);
        assert(entry->value == 200);

        /*
         * Test 3: Insert an entry that collides with key 2.
         *
         * With a capacity of 8:
         *
         *     2  % 8 = 2
         *     10 % 8 = 2
         *
         * Both entries should be stored in bucket 2.
         */
        ret = hash_insert(ht, 10, 300);

        assert(ret == 0);
        assert(ht->size == 3);

        index = hash_index(2, ht->capacity);
        entry = ht->buckets[index];

        found_2 = 0;
        found_10 = 0;

        /*
         * Do not assume whether new entries are inserted at the
         * head or tail of the collision chain.
         */
        while (entry != NULL) {
                if (entry->key == 2 && entry->value == 100)
                        found_2 = 1;

                if (entry->key == 10 && entry->value == 300)
                        found_10 = 1;

                entry = entry->next;
        }

        assert(found_2);
        assert(found_10);

        /*
         * Test 4: Insert a negative key.
         *
         * With the current hash function, -10 maps to the same
         * bucket as 2 and 10, which also tests collision handling.
         */
        ret = hash_insert(ht, -10, 400);

        assert(ret == 0);
        assert(ht->size == 4);

        index = hash_index(-10, ht->capacity);
        entry = ht->buckets[index];

        while (entry != NULL && entry->key != -10)
                entry = entry->next;

        assert(entry != NULL);
        assert(entry->key == -10);
        assert(entry->value == 400);

        /*
         * Test 5: Insert INT_MIN.
         *
         * INT_MIN is a special case because abs(INT_MIN) cannot
         * be represented by an int.
         */
        ret = hash_insert(ht, INT_MIN, 500);

        assert(ret == 0);
        assert(ht->size == 5);

        index = hash_index(INT_MIN, ht->capacity);
        entry = ht->buckets[index];

        while (entry != NULL && entry->key != INT_MIN)
                entry = entry->next;

        assert(entry != NULL);
        assert(entry->key == INT_MIN);
        assert(entry->value == 500);

        /*
         * Test 6: Update an existing key.
         *
         * Updating an existing key must not increase the number
         * of entries in the hash table.
         */
        ret = hash_insert(ht, 2, 600);

        assert(ret == 0);
        assert(ht->size == 5);

        index = hash_index(2, ht->capacity);
        entry = ht->buckets[index];

        found_2 = 0;

        while (entry != NULL) {
                if (entry->key == 2) {
                        assert(entry->value == 600);
                        found_2++;
                }

                entry = entry->next;
        }

        /*
         * There must be exactly one entry for key 2.
         */
        assert(found_2 == 1);

        /*
         * Release all entries manually until hash_destroy()
         * is implemented.
         */
        for (size_t i = 0; i < ht->capacity; i++) {
                entry = ht->buckets[i];

                while (entry != NULL) {
                        HashEntry *next;

                        next = entry->next;
                        free(entry);
                        entry = next;
                }
        }

        free(ht->buckets);
        free(ht);

        printf("test_hash_insert PASSED\n");
}

static void
test_hash_find(void)
{
        HashTable *ht;
        int value;
        int ret;

        printf("Running test_hash_find...\n");

        ht = hash_create(8);
        assert(ht != NULL);

        /*
         * Insert test entries.
         *
         * Keys 2, 10, and -10 map to the same bucket when
         * the hash table capacity is 8.
         */
        assert(hash_insert(ht, 2, 100) == 0);
        assert(hash_insert(ht, 3, 200) == 0);
        assert(hash_insert(ht, 10, 300) == 0);
        assert(hash_insert(ht, -10, 400) == 0);
        assert(hash_insert(ht, INT_MIN, 500) == 0);

        assert(ht->size == 5);

        /*
         * Test 1: Find an entry in a bucket without collision.
         */
        value = 0;
        ret = hash_find(ht, 3, &value);

        assert(ret == 1);
        assert(value == 200);

        /*
         * Test 2: Find the first entry in a collision chain.
         */
        value = 0;
        ret = hash_find(ht, 2, &value);

        assert(ret == 1);
        assert(value == 100);

        /*
         * Test 3: Find another entry in the same collision chain.
         */
        value = 0;
        ret = hash_find(ht, 10, &value);

        assert(ret == 1);
        assert(value == 300);

        /*
         * Test 4: Find a negative key.
         */
        value = 0;
        ret = hash_find(ht, -10, &value);

        assert(ret == 1);
        assert(value == 400);

        /*
         * Test 5: Find INT_MIN.
         */
        value = 0;
        ret = hash_find(ht, INT_MIN, &value);

        assert(ret == 1);
        assert(value == 500);

        /*
         * Test 6: Search for a key that does not exist.
         *
         * The output value should not be modified when the
         * requested key is not found.
         */
        value = 12345;
        ret = hash_find(ht, 999, &value);

        assert(ret == 0);
        assert(value == 12345);

        /*
         * Test 7: Update an existing key and verify that
         * hash_find() returns the new value.
         */
        assert(hash_insert(ht, 10, 600) == 0);
        assert(ht->size == 5);

        value = 0;
        ret = hash_find(ht, 10, &value);

        assert(ret == 1);
        assert(value == 600);

        /*
         * Test 8: A NULL output pointer should be allowed when
         * the caller only wants to check whether the key exists.
         */
        ret = hash_find(ht, 10, NULL);
        assert(ret == 1);

        /*
         * Test 9: Searching for a missing key with a NULL output
         * pointer should still return zero.
         */
        ret = hash_find(ht, 999, NULL);
        assert(ret == 0);

        /*
         * Release all entries manually until hash_destroy()
         * is implemented.
         */
        for (size_t i = 0; i < ht->capacity; i++) {
                HashEntry *entry;

                entry = ht->buckets[i];

                while (entry != NULL) {
                        HashEntry *next;

                        next = entry->next;
                        free(entry);
                        entry = next;
                }
        }

        free(ht->buckets);
        free(ht);

        printf("test_hash_find PASSED\n");
}

static void
test_hash_destroy(void)
{
        HashTable *ht;

        printf("Running test_hash_destroy...\n");

        /*
         * Test 1: Destroy an empty hash table.
         */
        ht = hash_create(8);
        assert(ht != NULL);
        assert(ht->size == 0);

        hash_destroy(ht);

        /*
         * Test 2: Destroy a hash table containing entries
         * in different buckets.
         */
        ht = hash_create(8);
        assert(ht != NULL);

        assert(hash_insert(ht, 2, 100) == 0);
        assert(hash_insert(ht, 3, 200) == 0);
        assert(hash_insert(ht, 4, 300) == 0);

        assert(ht->size == 3);

        hash_destroy(ht);

        /*
         * Test 3: Destroy a hash table containing a collision
         * chain.
         *
         * With a capacity of 8, keys 2, 10, 18, and -10
         * map to the same bucket with the current hash function.
         */
        ht = hash_create(8);
        assert(ht != NULL);

        assert(hash_insert(ht, 2, 100) == 0);
        assert(hash_insert(ht, 10, 200) == 0);
        assert(hash_insert(ht, 18, 300) == 0);
        assert(hash_insert(ht, -10, 400) == 0);

        assert(ht->size == 4);

        hash_destroy(ht);

        /*
         * Test 4: Destroy a hash table containing INT_MIN.
         */
        ht = hash_create(8);
        assert(ht != NULL);

        assert(hash_insert(ht, INT_MIN, 500) == 0);
        assert(hash_insert(ht, 1, 600) == 0);

        assert(ht->size == 2);

        hash_destroy(ht);

        /*
         * Test 5: Destroying a NULL hash table should be safe.
         */
        hash_destroy(NULL);

        printf("test_hash_destroy PASSED\n");
}
