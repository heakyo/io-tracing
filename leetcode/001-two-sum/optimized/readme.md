# 001 - Two Sum (Optimized: Hash Table)

## Table of Contents

- [Why Optimize?](#why-optimize)
- [Core Idea](#core-idea)
- [Function Reference](#function-reference)
- [ASCII Flowchart](#ascii-flowchart)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Comparison with Original](#comparison-with-original)
- [Summary](#summary)

---

## Why Optimize?

The original brute-force solution uses two nested loops to try every pair, resulting in O(n^2) time. When the array is large (e.g. 10,000 elements), that means up to 50 million comparisons. We can do much better by trading a bit of memory for speed.

---

## Core Idea

Imagine the same room full of people holding number cards. Instead of comparing every pair, you set up a **bulletin board** (hash table). As each person walks in:

1. Calculate what number you **need** to pair with them: `complement = target - my_number`.
2. Check the bulletin board — is the complement already posted?
3. If yes, you found the pair. If no, post your own number and index on the board, then move on.

This way, each person only needs to glance at the board once. One pass through the room, and you're done.

In code, the "bulletin board" is a hash table that maps `number -> index`. For each element, we compute `complement = target - nums[i]`, look it up in the hash table, and either return the answer or insert the current element.

---

## Function Reference

| Function | Signature | Purpose |
|----------|-----------|---------|
| `ht_init` | `void ht_init(struct ht_entry *ht)` | Initialize the hash table — zero out all slots, marking them as unused. |
| `ht_hash` | `unsigned int ht_hash(int key)` | Compute a hash index for a given integer key. Uses bit-mixing (multiply-shift) and casts to `unsigned int` to handle negatives. Returns a value in `[0, HT_SIZE)`. |
| `ht_get` | `int ht_get(struct ht_entry *ht, int key, int *val)` | Look up `key` in the hash table. On hit, writes the stored value (array index) into `*val` and returns `1`. On miss, returns `0`. Uses linear probing to resolve collisions. |
| `ht_put` | `void ht_put(struct ht_entry *ht, int key, int val)` | Insert a key-value pair into the hash table. `key` is the number from `nums`, `val` is its index. Uses linear probing to find the first empty slot. |
| `twoSum` | `int *twoSum(int *nums, int numsSize, int target, int *returnSize)` | Main algorithm. Allocates a 2-element result array, creates a hash table, then iterates through `nums`. For each element, checks if `target - nums[i]` exists in the table; if yes returns the pair of indices, otherwise inserts the current element. Caller must `free()` the returned array. |
| `print_array` | `void print_array(const char *label, int *array, int len)` | Helper to print an array with a label prefix, e.g. `Input:  [3 2 3]`. Used for test output. |
| `run_test` | `void run_test(const char *name, int *nums, int numsSize, int target, int exp0, int exp1)` | Run a single test case: print input, call `twoSum`, verify result with `assert`, print PASS. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 10 test cases and prints "All tests passed!" on success. |

---

## ASCII Flowchart

```
         +---------------------+
         |       Start         |
         +---------------------+
                   |
                   v
         +---------------------+
         | alloc result[2]     |
         | ht = ht_create()    |
         | i = 0               |
         +---------------------+
                   |
                   v
         +---------------------+
         |   i < numsSize ?    |
         +---------------------+
          /  yes          \  no
         v                 v
 +------------------+   +------------------+
 | complement =     |   | ht_destroy(ht)   |
 | target - nums[i] |   | return result    |
 +------------------+   | (no match found) |
         |              +------------------+
         v
 +------------------+
 | ht_get(ht,       |
 |   complement)    |
 +------------------+
    /  found    \  not found
   v             v
 +-------------+  +------------------+
 | result[0] = |  | ht_set(ht,       |
 |   saved idx |  |   nums[i], i+1)  |
 | result[1] = |  +------------------+
 |   i         |           |
 +-------------+           v
       |           +------------------+
       v           |     i++          |
 +-------------+   +------------------+
 | ht_destroy  |           |
 | return      |           |
 | result      |    (loop back to
 +-------------+     "i < numsSize ?")
```

---

## Step-by-Step Walkthrough

`nums = [3, 2, 3]`, `target = 6`:

```
Hash table: {}

i=0: nums[0]=3, complement=6-3=3
     Look up 3 in hash table -> not found
     Insert {3: 0}

i=1: nums[1]=2, complement=6-2=4
     Look up 4 in hash table -> not found
     Insert {3: 0, 2: 1}

i=2: nums[2]=3, complement=6-3=3
     Look up 3 in hash table -> found! index=0
     Return [0, 2]
```

Another example: `nums = [2, 7, 11, 15]`, `target = 9`:

```
Hash table: {}

i=0: nums[0]=2, complement=9-2=7
     Look up 7 -> not found
     Insert {2: 0}

i=1: nums[1]=7, complement=9-7=2
     Look up 2 -> found! index=0
     Return [0, 1]
```

---

## Where It Gets Tricky

1. **Insert after lookup, not before.** If we insert `nums[i]` before checking the complement, we might match an element with itself. For example, `nums = [3, 3]`, `target = 6`: if we insert first, `i=0` would find itself in the table. By looking up first and inserting after, the first `3` won't find a match, but the second `3` will correctly find the first one.

2. **Hash collisions.** In C, we don't have a built-in hash map, so we implement one with open addressing (linear probing). The hash table size must be large enough to minimize collisions. We use 4096 slots, which is more than sufficient for typical LeetCode inputs.

3. **Negative keys.** The hash function must handle negative integers correctly. We cast to `unsigned int` before hashing to avoid undefined behavior with negative modulo.

4. **Same result order.** The hash table approach naturally returns `[smaller_index, larger_index]` because we always find the earlier-inserted element first.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time** | O(n) | Single pass through the array; each hash table lookup/insert is O(1) amortized |
| **Space** | O(n) | Hash table stores up to n elements |

---

## Comparison with Original

| | Original (Brute Force) | Optimized (Hash Table) |
|--|------------------------|------------------------|
| **Time** | O(n^2) | O(n) |
| **Space** | O(1) | O(n) |
| **Approach** | Try every pair with nested loops | One-pass lookup with hash table |
| **Code complexity** | Very simple | Needs hash table implementation in C |

The trade-off is classic: **spend memory to save time**. For this problem, the hash table approach is the standard optimal solution.

---

## Summary

The optimized solution replaces the O(n^2) brute-force with a single-pass hash table approach. For each element, we compute `target - nums[i]` and check if it's already in the hash table. If found, we have our answer; if not, we store the current element. This brings the time down from O(n^2) to O(n) at the cost of O(n) extra space — the classic time-space trade-off. In C, we implement the hash table manually with open addressing and a simple integer hash function.
