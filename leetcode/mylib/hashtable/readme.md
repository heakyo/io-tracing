# libhashtable — A C Hash Table Library

## Table of Contents

- [Overview](#overview)
- [Build Instructions](#build-instructions)
- [API Reference](#api-reference)
- [Usage Example](#usage-example)
- [Function Usage Q&A](#function-usage-qa)

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

---

## Function Usage Q&A

### ht_create

**Q: What does this function do?**
Create a new, empty hash table.

**Q: What are the parameters?**
- `capacity` (`size_t`) — Initial number of buckets. Pass `0` to use the default (16).

**Q: What does it return?**
A `hashtable_t *` handle on success, or `NULL` if memory allocation fails.

**Q: How do I use it?**
```c
hashtable_t *ht = ht_create(0);    /* default capacity */
hashtable_t *ht = ht_create(1024); /* pre-allocate 1024 buckets */
```

**Q: What should I watch out for?**
- Always check the return value for `NULL`.
- You must call `ht_destroy()` when done to avoid memory leaks.

---

### ht_destroy

**Q: What does this function do?**
Destroy the hash table and free all internal memory (keys and bucket array).

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle to destroy.

**Q: What does it return?**
Nothing (`void`).

**Q: How do I use it?**
```c
ht_destroy(ht);
ht = NULL; /* good practice */
```

**Q: What should I watch out for?**
- This does **not** free the values you stored. If your values are heap-allocated, free them first (e.g. iterate with `ht_iter` and free each value).
- Passing `NULL` is safe (no-op).

---

### ht_set

**Q: What does this function do?**
Insert a new key-value pair, or update the value if the key already exists.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.
- `key` (`const char *`) — A null-terminated string key. A copy is stored internally.
- `value` (`void *`) — Pointer to the value to store.

**Q: What does it return?**
`0` on success, `-1` on failure (memory allocation error).

**Q: How do I use it?**
```c
int score = 100;
ht_set(ht, "alice", &score);

/* Update existing key */
int new_score = 200;
ht_set(ht, "alice", &new_score);
```

**Q: What should I watch out for?**
- The key is copied internally, so you can modify or free your original string after calling `ht_set`.
- The value is **not** copied — only the pointer is stored. Make sure the pointed-to data stays valid for as long as it's in the table.
- When updating an existing key, the old value pointer is silently replaced. If the old value was heap-allocated, you should retrieve and free it first.

---

### ht_get

**Q: What does this function do?**
Look up a value by its key.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.
- `key` (`const char *`) — The key to search for.

**Q: What does it return?**
The stored `void *` value if the key is found, or `NULL` if the key does not exist.

**Q: How do I use it?**
```c
int *val = (int *)ht_get(ht, "alice");
if (val)
    printf("alice = %d\n", *val);
else
    printf("alice not found\n");
```

**Q: What should I watch out for?**
- If you stored `NULL` as a value, you cannot distinguish "key exists with NULL value" from "key not found". Use `ht_contains()` to check existence in that case.

---

### ht_remove

**Q: What does this function do?**
Remove a key-value pair from the hash table.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.
- `key` (`const char *`) — The key to remove.

**Q: What does it return?**
The removed `void *` value if the key was found, or `NULL` if the key did not exist.

**Q: How do I use it?**
```c
int *old = (int *)ht_remove(ht, "alice");
if (old)
    printf("removed alice, value was %d\n", *old);
```

**Q: What should I watch out for?**
- The returned value is your responsibility to free if it was heap-allocated.
- Internally uses a tombstone marker, so probe chains are not broken.

---

### ht_contains

**Q: What does this function do?**
Check whether a key exists in the hash table.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.
- `key` (`const char *`) — The key to check.

**Q: What does it return?**
`1` if the key exists, `0` if it does not.

**Q: How do I use it?**
```c
if (ht_contains(ht, "alice"))
    printf("alice is in the table\n");
```

**Q: What should I watch out for?**
- Internally calls `ht_get`, so if you stored `NULL` as a value, `ht_contains` will return `0` even if the key was inserted. This is a known limitation.

---

### ht_size

**Q: What does this function do?**
Return the number of key-value pairs currently stored.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.

**Q: What does it return?**
A `size_t` count of entries. Returns `0` if `ht` is `NULL`.

**Q: How do I use it?**
```c
printf("table has %zu entries\n", ht_size(ht));
```

**Q: What should I watch out for?**
- This is O(1) — the count is maintained internally.

---

### ht_iter_init

**Q: What does this function do?**
Initialize an iterator to traverse all entries in the hash table.

**Q: What are the parameters?**
- `ht` (`hashtable_t *`) — The hash table handle.
- `iter` (`ht_iter_t *`) — Pointer to an iterator struct to initialize.

**Q: What does it return?**
Nothing (`void`).

**Q: How do I use it?**
```c
ht_iter_t iter;
ht_iter_init(ht, &iter);
```

**Q: What should I watch out for?**
- You must call this before `ht_iter_next`.
- Do not modify the hash table (insert/remove) while iterating — behavior is undefined.

---

### ht_iter_next

**Q: What does this function do?**
Advance the iterator to the next entry.

**Q: What are the parameters?**
- `iter` (`ht_iter_t *`) — Pointer to an initialized iterator.

**Q: What does it return?**
`1` if a next entry was found (`iter->key` and `iter->value` are valid), `0` if iteration is complete.

**Q: How do I use it?**
```c
ht_iter_t iter;
ht_iter_init(ht, &iter);
while (ht_iter_next(&iter))
    printf("key=%s, value=%d\n", iter.key, *(int *)iter.value);
```

**Q: What should I watch out for?**
- Iteration order is **not** guaranteed (depends on hash bucket layout).
- Always cast `iter.value` to the correct type before dereferencing.
