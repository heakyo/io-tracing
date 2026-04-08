# Quick Sort

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

---

## Problem Statement

Given an unsorted array of integers, sort it in ascending order using the Quick Sort algorithm.

Example:

```
Input:  [1, 7, 3, 8, 4]
Output: [1, 3, 4, 7, 8]
```

---

## Core Idea

Imagine you have a messy deck of numbered cards and you want to sort them. You pick one card — the **pivot** — and split the rest into two piles: cards smaller than the pivot go left, cards larger go right. Then you repeat the process on each pile until every pile has just one card. Stack everything together, and it's sorted.

This code picks the **last element** as the pivot and uses a two-pointer approach to partition. Two pointers `i` and `j` start at opposite ends. `i` scans right looking for elements greater than the pivot, `j` scans left looking for elements less than the pivot. When each finds one, it swaps that element with the pivot. The pivot "walks" through the array until `i` and `j` meet — at that point, the pivot is in its final sorted position.

After partitioning, the function recurses on the left and right halves.

---

## Step-by-Step Walkthrough

Let's trace through `[1, 7, 3, 8, 4]`:

```
Initial: [1, 7, 3, 8, 4]   pivot_idx=4 (value 4), i=0, j=4

Scan i right: array[0]=1 <= 4, i++ -> i=1
              array[1]=7 > 4, stop
  i(1) < pivot(4): swap array[1] & array[4] -> [1, 4, 3, 8, 7]  pivot_idx=1

Scan j left:  array[1]=4 <= array[4]=7, j-- -> j=3
              array[1]=4 <= array[3]=8, j-- -> j=2
              array[1]=4 <= array[2]=3? No (4 > 3), stop
  pivot(1) < j(2): swap array[1] & array[2] -> [1, 3, 4, 8, 7]  pivot_idx=2

Check: i=1, j=2, i < j? Yes, continue outer loop.

Scan i right: i=1, array[1]=3 <= array[2]=4, i++ -> i=2
  i(2) == pivot(2): stop, i < pivot is false, no swap

Scan j left:  j=2, pivot(2) < j(2) is false, no scan, no swap

Check: i=2, j=2, i < j? No, exit loop. pivot_idx=2

Recurse left:  quick_sort([1, 3, 4, 8, 7], 0, 1)
Recurse right: quick_sort([1, 3, 4, 8, 7], 3, 4)
```

Left half `[1, 3]`: pivot=3 at index 1. 1 <= 3, pointers meet. Already sorted.

Right half `[8, 7]`: pivot=7 at index 4. 8 > 7, swap -> `[7, 8]`. Sorted.

Final result: `[1, 3, 4, 7, 8]`

---

## ASCII Flowchart

```
+----------------------------------+
| quick_sort(array, low, high)     |
+----------------------------------+
                |
                v
      +-------------------+
      | array == NULL ||  |
      | low >= high ?     |
      +-------------------+
       yes /        \ no
          v          v
     +--------+  +--------------------+
     | Return |  | pivot_idx = high   |
     +--------+  | i = low, j = high  |
                 +--------------------+
                          |
                          v
                 +--------------------+
             +-->|     i < j ?        |---no--+
             |   +--------------------+       |
             |          | yes                 |
             |          v                     |
             |   +--------------------+       |
             |   | Scan i -->         |       |
             |   | while arr[i] <=    |       |
             |   | arr[pivot_idx]     |       |
             |   +--------------------+       |
             |          |                     |
             |          v                     |
             |   +--------------------+       |
             |   | i < pivot_idx ?    |       |
             |   +----+----------+----+       |
             |    yes |          | no         |
             |        v          v            |
             |   +----------+ (skip)          |
             |   | swap i & |                 |
             |   | pivot_idx|                 |
             |   |pivot_idx |                 |
             |   |  = i     |                 |
             |   +----------+                 |
             |        |   |                   |
             |        v   v                   |
             |   +--------------------+       |
             |   | Scan j <--         |       |
             |   | while arr[pivot]   |       |
             |   | <= arr[j]          |       |
             |   +--------------------+       |
             |          |                     |
             |          v                     |
             |   +--------------------+       |
             |   | pivot_idx < j ?    |       |
             |   +----+----------+----+       |
             |    yes |          | no         |
             |        v          v            |
             |   +----------+ (skip)          |
             |   | swap piv |                 |
             |   | & j      |                 |
             |   |pivot_idx |                 |
             |   |  = j     |                 |
             |   +----------+                 |
             |        |   |                   |
             +--------+---+                   |
                                              |
                 +----------------------------+
                 |
                 v
        +------------------+
        | Recurse left:    |
        | sort(arr,0,piv-1)|
        +------------------+
                 |
                 v
        +------------------+
        | Recurse right:   |
        | sort(arr,piv+1,  |
        |          high)   |
        +------------------+
```

---

## Where It Gets Tricky

1. **The pivot "walks" through the array.** Unlike textbook Lomuto or Hoare partitioning, this code swaps the pivot itself with elements, so `pivot_idx` changes during the loop. You must track the pivot's current position — not just its value — because both pointers compare against `array[pivot_idx]`.

2. **Left recursion starts from 0 instead of `low`.** Line 44 calls `quick_sort(array, 0, pivot - 1)` rather than `quick_sort(array, low, pivot - 1)`. This doesn't break correctness — re-sorting an already-sorted prefix produces the same result — but it does redundant work. On every recursive call, the left partition starts from the beginning of the array rather than the current sub-range. This turns what should be O(n log n) average-case into O(n^2) in practice for many inputs.

3. **Pivot choice is always the last element.** Picking `high` as the pivot degrades to O(n^2) on already-sorted or reverse-sorted input, because every partition puts all elements on one side.

4. **Identical elements.** When all elements are equal, every scan crosses the entire sub-array before the pointers meet. The pivot ends up at one end, so each recursion level only reduces the problem by one element — O(n^2) behavior.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time (best/average)** | O(n log n) | Each partition divides the array roughly in half; log n levels of recursion, O(n) work per level |
| **Time (worst case)** | O(n^2) | Sorted input or all-equal elements cause maximally unbalanced partitions |
| **Space** | O(log n) average, O(n) worst | Recursion stack depth matches the partition balance |

Note: The `0` vs `low` issue on the left recursion (see "Where It Gets Tricky" #2) makes the practical performance worse than standard quick sort, even on random input.

---

## Summary

This Quick Sort implementation uses a "pivot-walking" partition scheme: the pivot starts at the last element and physically swaps into its final position as two pointers converge from both ends. Once the pivot is placed, the function recurses on both halves. It correctly sorts the array, but has a performance quirk — the left recursion always starts from index 0 instead of `low`, causing redundant re-sorting. Combined with a fixed last-element pivot choice, it degrades to O(n^2) on sorted or equal-element inputs. The optimized version in `optimized/` fixes these issues.
