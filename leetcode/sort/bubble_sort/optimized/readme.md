# Bubble Sort (Optimized)

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

The original bubble sort always runs all `n-1` passes regardless of whether the array is already sorted. For an already-sorted array of 1,000 elements, it still performs ~500,000 comparisons — all wasted.

The optimized version can detect a sorted array in a single pass (O(n)) and also skips already-sorted tail regions more aggressively by tracking the position of the last swap.

---

## Core Idea

Imagine the same line of people with number cards. The original version makes `n-1` full walks no matter what. The optimized version is smarter:

1. **If nobody swapped during your walk, the line is already sorted — stop immediately.** This is the "early termination" idea.

2. **Remember where the last swap happened.** Everything after that point is already in order, so next time you only need to walk up to that position — not the full remaining length.

In code, a single variable `bound` tracks how far to scan. After each pass, `bound` is updated to the position of the last swap (`new_bound`). If no swaps occurred, `new_bound` stays at 0, the while loop condition `bound > 0` fails, and we exit.

---

## Function Reference

| Function | Signature | Purpose |
|----------|-----------|---------|
| `swap` | `void swap(int *a, int *b)` | Swap two integers via a temporary variable. |
| `bubble_sort` | `void bubble_sort(int *array, int len)` | Sort `array[0..len-1]` using optimized bubble sort. Uses `bound` to track the unsorted region and `new_bound` to record the last swap position. Exits early when no swaps occur in a pass. |
| `print_array` | `void print_array(const char *label, int *array, int len)` | Print an array with a label prefix, e.g. `Input:  [1 7 3 8 4]`. |
| `check_sorted` | `int check_sorted(int *array, int len)` | Verify an array is sorted in non-decreasing order. Returns 1 if sorted, 0 otherwise. |
| `run_test` | `void run_test(const char *name, int *src, int len)` | Run a single test: copy input, sort it, verify with `check_sorted` and `assert`, print PASS. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 10 test cases (identical to the original) and prints "All tests passed!" on success. |

---

## ASCII Flowchart

```
+-------------------------------+
| bubble_sort(array, len)       |
+-------------------------------+
              |
              v
      +------------------+
      | bound = len - 1  |
      +------------------+
              |
              v
      +------------------+
  +-->| bound > 0 ?      |--no--> Return (sorted!)
  |   +------------------+
  |           | yes
  |           v
  |   +------------------+
  |   | new_bound = 0    |
  |   | j = 0            |
  |   +------------------+
  |           |
  |           v
  |   +------------------+
  |+->| j < bound ?      |--no--+
  ||  +------------------+       |
  ||          | yes              |
  ||          v                  |
  ||  +------------------+       |
  ||  | array[j] >       |       |
  ||  | array[j+1] ?     |       |
  ||  +------------------+       |
  ||   yes /       \ no         |
  ||      v         v            |
  || +----------+ (skip)         |
  || | swap j,  |   |            |
  || | j+1      |   |            |
  || | new_bound|   |            |
  || |  = j     |   |            |
  || +----------+   |            |
  ||      |         |            |
  ||      v         v            |
  ||  +------------------+       |
  ||  |     j++          |       |
  ||  +------------------+       |
  ||       |                     |
  |+-------+                     |
  |                              |
  |   +------------------+       |
  |   | bound = new_bound|<------+
  |   +------------------+
  |        |
  +--------+
```

---

## Step-by-Step Walkthrough

Let's trace `[1, 7, 3, 8, 4]`:

```
Initial: bound = 4

Pass 1 (scan j=0..3):
  j=0: 1 <= 7  no swap
  j=1: 7 > 3   swap -> [1, 3, 7, 8, 4]  new_bound=1
  j=2: 7 <= 8  no swap
  j=3: 8 > 4   swap -> [1, 3, 7, 4, 8]  new_bound=3
  bound = 3

Pass 2 (scan j=0..2):
  j=0: 1 <= 3  no swap
  j=1: 3 <= 7  no swap
  j=2: 7 > 4   swap -> [1, 3, 4, 7, 8]  new_bound=2
  bound = 2

Pass 3 (scan j=0..1):
  j=0: 1 <= 3  no swap
  j=1: 3 <= 4  no swap
  new_bound stays 0 -> bound = 0

bound == 0, exit. Result: [1, 3, 4, 7, 8]
```

Now trace the already-sorted case `[1, 2, 3, 4, 5]`:

```
Initial: bound = 4

Pass 1 (scan j=0..3):
  j=0: 1 <= 2  no swap
  j=1: 2 <= 3  no swap
  j=2: 3 <= 4  no swap
  j=3: 4 <= 5  no swap
  new_bound stays 0 -> bound = 0

bound == 0, exit after just ONE pass! Result: [1, 2, 3, 4, 5]
```

The original would have done 4 full passes here. The optimized version does 1.

---

## Where It Gets Tricky

1. **`new_bound` must start at 0, not `bound`.** If we initialize `new_bound = bound`, we never shrink the boundary. Setting it to 0 means "no swaps yet" — if it stays 0, we know the entire scanned region is sorted.

2. **`new_bound = j`, not `j + 1`.** We record the position of the left element in the swap pair. After the swap, `array[j+1]` is correct (larger), so the next pass only needs to scan up to `j`. If we set `new_bound = j + 1`, we'd do one unnecessary comparison per pass.

3. **Still O(n^2) worst case.** The early-exit and boundary-shrinking optimize best-case and average-case, but reverse-sorted input still requires `n-1` passes with swaps in every position. The fundamental O(n^2) nature of bubble sort remains.

4. **Single-element and empty arrays.** When `len <= 1`, `bound = len - 1 <= 0`, so the while loop never executes. This correctly handles edge cases without a special check.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time (best)** | O(n) | Already-sorted array: one pass, no swaps, immediate exit |
| **Time (average)** | O(n^2) | Swaps still occur in most passes for random input |
| **Time (worst)** | O(n^2) | Reverse-sorted: every pass does swaps, `bound` only shrinks by 1 |
| **Space** | O(1) | Only a few integer variables beyond the input array |

---

## Comparison with Original

| | Original | Optimized |
|--|----------|-----------|
| **Early exit** | No — always runs all n-1 passes | Yes — exits when a pass has no swaps |
| **Boundary tracking** | Shrinks by 1 each pass (`len-1-i`) | Shrinks to last swap position — can skip multiple elements |
| **Best-case time** | O(n^2) | O(n) |
| **Worst-case time** | O(n^2) | O(n^2) (unchanged) |
| **Loop structure** | Two nested for-loops with counter `i` | While-loop with `bound` + inner for-loop |

---

## Summary

The optimized bubble sort adds two improvements over the original. First, it tracks whether any swaps occurred during a pass — if none did, the array is sorted and we exit immediately, giving O(n) best-case on already-sorted input. Second, instead of shrinking the scan boundary by one position per pass, it jumps directly to the position of the last swap, skipping any tail elements that settled early. These changes don't improve the O(n^2) worst case, but they significantly reduce work for partially-sorted or nearly-sorted arrays — which are common in practice.
