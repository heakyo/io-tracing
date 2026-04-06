# libhashtable — A C Hash Table Library

## Table of Contents

- [Overview](#overview)
- [Build Instructions](#build-instructions)
- [API Reference](#api-reference)
- [Usage Example](#usage-example)

---

## Overview

`libhashtable` is a generic hash table library written in C, built as a dynamic shared library (`.so`). It uses **open addressing with linear probing** and **FNV-1a hashing** for string keys, storing arbitrary `void*` values.

Features:
- String keys, `void*` values
- Automatic resizing when load factor exceeds 75%
- Tombstone-based deletion (safe for probe chains)
- Iterator for traversing all entries
- No external dependencies — pure C99

---

## Build Instructions

```bash
# Build the shared library
cd mylib/hashtable
make

# Build and run the demo
cd demo
make
./demo
```

---

## API Reference

### Types

| Type | Description |
|------|-------------|
| `hashtable_t` | Opaque hash table handle. Created by `ht_create`, freed by `ht_destroy`. |
| `ht_iter_t` | Iterator struct. Contains `key` and `value` fields after each `ht_iter_next` call. |

### Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| `ht_create` | `hashtable_t *ht_create(size_t capacity)` | Create a new hash table. Pass `0` for default capacity (16). Returns `NULL` on failure. |
| `ht_destroy` | `void ht_destroy(hashtable_t *ht)` | Destroy the hash table and free internal memory. Does **not** free stored values. |
| `ht_set` | `int ht_set(hashtable_t *ht, const char *key, void *value)` | Insert or update a key-value pair. The key is copied internally. Returns `0` on success, `-1` on failure. |
| `ht_get` | `void *ht_get(hashtable_t *ht, const char *key)` | Look up a value by key. Returns the value pointer, or `NULL` if not found. |
| `ht_remove` | `void *ht_remove(hashtable_t *ht, const char *key)` | Remove a key-value pair. Returns the removed value, or `NULL` if not found. |
| `ht_contains` | `int ht_contains(hashtable_t *ht, const char *key)` | Check if a key exists. Returns `1` if found, `0` otherwise. |
| `ht_size` | `size_t ht_size(hashtable_t *ht)` | Return the number of stored entries. |
| `ht_iter_init` | `void ht_iter_init(hashtable_t *ht, ht_iter_t *iter)` | Initialize an iterator for the hash table. |
| `ht_iter_next` | `int ht_iter_next(ht_iter_t *iter)` | Advance to the next entry. Returns `1` if valid, `0` if done. Access `iter->key` and `iter->value`. |

---

## Usage Example

```c
#include <stdio.h>
#include "hashtable.h"

int main(void)
{
    hashtable_t *ht = ht_create(0);
    int val = 42;

    ht_set(ht, "answer", &val);

    int *result = (int *)ht_get(ht, "answer");
    if (result)
        printf("answer = %d\n", *result);

    ht_destroy(ht);
    return 0;
}
```

Compile with:

```bash
gcc -I/path/to/include -L/path/to/lib -lhashtable -o myapp myapp.c
```
