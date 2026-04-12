# 088 - Merge Sorted Array (Optimized)

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

The original solution is already algorithmically optimal — O(m+n) time and O(1) space. The "optimization" here is about **code clarity and hygiene**:

1. **Unused `swap` function removed.** The original defines `swap` but never calls it — dead code that adds confusion.
2. **Index-based access instead of pointer arithmetic.** Using `i`, `j`, `k` as integer indices is easier to reason about than `p1`, `p2`, `p` as pointers with comparisons like `nums1 <= p1`. The boundary checks become simple `i >= 0` instead of address comparisons.
3. **Same algorithm, cleaner implementation.** The backward-merge logic is identical.

---

## Core Idea

Same approach as the original: fill `nums1` from the back. Three indices — `i` (end of `nums1` data), `j` (end of `nums2`), `k` (write position) — start at the tail and move forward. At each step, the larger tail element goes to position `k`. After the main loop, copy any remaining `nums2` elements.

Using integer indices makes the logic transparent:

```c
int i = m - 1;    // last element in nums1's data
int j = n - 1;    // last element in nums2
int k = m + n - 1; // last slot in nums1
```

The loop condition `i >= 0 && j >= 0` is immediately clear — we stop when either array is exhausted.

---

## Function Reference

| Function | Signature | Purpose |
|----------|-----------|---------|
| `merge` | `void merge(int *nums1, int nums1Size, int m, int *nums2, int nums2Size, int n)` | Merge `nums2` into `nums1` in-place. Uses three indices (`i`, `j`, `k`) to fill `nums1` from the back. Compares tail elements, places the larger at position `k`, and decrements. After the main loop, copies remaining `nums2` elements. O(m+n) time, O(1) space. |
| `print_array` | `void print_array(const char *label, int *array, int len)` | Print an array with a label prefix, e.g. `nums1:    [1 2 3]`. |
| `run_test` | `void run_test(const char *name, int *nums1_src, int m, int *nums2_src, int n, int *expected)` | Run a single test: allocate `nums1` with m+n slots, copy source data, call `merge`, verify output matches `expected` element-by-element with `assert`. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 10 test cases (identical to the original) and prints "All tests passed!" on success. |

---

## ASCII Flowchart

```
+--------------------------------------+
| merge(nums1, nums1Size, m,           |
|       nums2, nums2Size, n)           |
+--------------------------------------+
                  |
                  v
        +-------------------+
        | i = m - 1         |
        | j = n - 1         |
        | k = m + n - 1     |
        +-------------------+
                  |
                  v
        +-------------------+
   +--->| i >= 0 && j >= 0? |--no--+
   |    +-------------------+      |
   |           | yes               |
   |           v                   |
   |    +-------------------+      |
   |    | nums2[j] >        |      |
   |    | nums1[i] ?        |      |
   |    +-------------------+      |
   |     yes /       \ no         |
   |        v         v            |
   |  +-----------+ +-----------+  |
   |  | nums1[k]= | | nums1[k]= | |
   |  | nums2[j]  | | nums1[i]  | |
   |  | j--       | | i--       | |
   |  +-----------+ +-----------+  |
   |       |           |           |
   |       v           v           |
   |    +-------------------+      |
   |    |      k--          |      |
   |    +-------------------+      |
   |           |                   |
   +-----------+                   |
                                   |
        +--------------------------+
        |
        v
   +-------------------+
+->| j >= 0 ?          |--no--> Return
|  +-------------------+
|         | yes
|         v
|  +-------------------+
|  | nums1[k] = nums2[j]
|  | j--, k--          |
|  +-------------------+
|         |
+---------+
```

---

## Step-by-Step Walkthrough

`nums1 = [3, 4, 0, 0]` (m=2), `nums2 = [1, 2]` (n=2):

```
i=1 (val 4), j=1 (val 2), k=3

Step 1: nums2[1]=2 < nums1[1]=4 -> nums1[3]=4, i=0, k=2
  nums1: [3, 4, 0, 4]

Step 2: nums2[1]=2 < nums1[0]=3 -> nums1[2]=3, i=-1, k=1
  nums1: [3, 4, 3, 4]

Main loop exits (i < 0).

Copy remaining nums2: nums1[1]=2, nums1[0]=1
  nums1: [1, 2, 3, 4]
```

`nums1 = [1, 2, 3, 0, 0, 0]` (m=3), `nums2 = [2, 5, 6]` (n=3):

```
i=2 (val 3), j=2 (val 6), k=5

Step 1: 6 > 3 -> nums1[5]=6, j=1, k=4
Step 2: 5 > 3 -> nums1[4]=5, j=0, k=3
Step 3: 2 < 3 -> nums1[3]=3, i=1, k=2
Step 4: 2 = 2 -> nums1[2]=2 (from nums1), i=0, k=1
Step 5: 2 > 1 -> nums1[1]=2, j=-1, k=0

Main loop exits (j < 0). No remaining nums2 elements.
Result: [1, 2, 2, 3, 5, 6]
```

---

## Where It Gets Tricky

1. **Index -1 is the "empty" sentinel.** When `i` drops to -1, `nums1`'s data is exhausted. When `j` drops to -1, `nums2` is exhausted. The simple `>= 0` check makes this clear. The original's pointer comparison `nums1 <= p1` achieves the same thing but is less intuitive.

2. **Only `nums2` leftover needs copying.** If `i >= 0` but `j < 0` after the main loop, the remaining `nums1` elements are already in positions `0..i` — exactly where they need to be. No copy needed. This is the key insight that makes the algorithm O(1) space.

3. **Equal elements go from `nums1`.** When `nums2[j] == nums1[i]`, the else branch takes the element from `nums1`. This maintains stability (original `nums1` elements come first among equals), though stability isn't required by the problem.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time** | O(m + n) | Each of the m+n elements is placed exactly once |
| **Space** | O(1) | Three integer variables; merge is in-place |

---

## Comparison with Original

| | Original | Optimized |
|--|----------|-----------|
| **Algorithm** | Backward merge | Backward merge (identical) |
| **Time** | O(m + n) | O(m + n) |
| **Space** | O(1) | O(1) |
| **Loop bounds** | Pointer comparison (`nums1 <= p1`) | Index comparison (`i >= 0`) |
| **Dead code** | Unused `swap()` function | Removed |
| **Readability** | Pointer arithmetic requires mental address tracking | Integer indices are immediately clear |

Both are algorithmically identical. The optimized version is a cleanup, not a complexity improvement.

---

## Summary

The optimized version replaces pointer arithmetic with integer index variables (`i`, `j`, `k`), making the backward-merge logic easier to follow. The unused `swap` function is removed. The algorithm is unchanged — fill `nums1` from the back by comparing the largest remaining elements from each array. The complexity stays at O(m+n) time and O(1) space, which is already optimal for this problem.
