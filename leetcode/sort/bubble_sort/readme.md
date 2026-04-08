# Bubble Sort

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

Given an unsorted array of integers, sort it in ascending order using the Bubble Sort algorithm.

Example:

```
Input:  [1, 7, 3, 8, 4]
Output: [1, 3, 4, 7, 8]
```

---

## Core Idea

Imagine a row of people standing in line, each holding a numbered card. You walk along the line comparing each person with the one next to them. If a taller person is standing in front of a shorter one, you swap them. By the time you reach the end of the line, the tallest person has "bubbled up" to the back.

Now repeat: walk the line again, and the second-tallest bubbles to the second-to-last spot. Keep repeating until no swaps are needed — the line is sorted.

In code, two nested loops do the job:

- The **outer loop** (`i`) counts how many passes have been completed. After `i` passes, the last `i` elements are already in place.
- The **inner loop** (`j`) walks from the front to `len - 1 - i`, comparing adjacent pairs and swapping when out of order.

---

## Step-by-Step Walkthrough

Let's trace through `[1, 7, 3, 8, 4]`:

```
Pass i=0 (bubble the largest to the end):
  j=0: [1, 7, 3, 8, 4]  1 <= 7  no swap
  j=1: [1, 7, 3, 8, 4]  7 > 3   swap -> [1, 3, 7, 8, 4]
  j=2: [1, 3, 7, 8, 4]  7 <= 8  no swap
  j=3: [1, 3, 7, 8, 4]  8 > 4   swap -> [1, 3, 7, 4, 8]
  End of pass: 8 is in place.

Pass i=1 (bubble next largest):
  j=0: [1, 3, 7, 4, 8]  1 <= 3  no swap
  j=1: [1, 3, 7, 4, 8]  3 <= 7  no swap
  j=2: [1, 3, 7, 4, 8]  7 > 4   swap -> [1, 3, 4, 7, 8]
  End of pass: 7 is in place.

Pass i=2:
  j=0: [1, 3, 4, 7, 8]  1 <= 3  no swap
  j=1: [1, 3, 4, 7, 8]  3 <= 4  no swap
  End of pass: no swaps, but loop continues.

Pass i=3:
  j=0: [1, 3, 4, 7, 8]  1 <= 3  no swap
  End of pass: done.

Result: [1, 3, 4, 7, 8]
```

---

## ASCII Flowchart

```
+-------------------------------+
| bubble_sort(array, len)       |
+-------------------------------+
              |
              v
      +---------------+
      | i = 0         |
      +---------------+
              |
              v
      +---------------+
  +-->| i < len - 1 ? |--no--> Return
  |   +---------------+
  |           | yes
  |           v
  |   +---------------+
  |   | j = 0         |
  |   +---------------+
  |           |
  |           v
  |   +-------------------+
  |+->| j < len - 1 - i ? |--no--+
  ||  +-------------------+       |
  ||          | yes               |
  ||          v                   |
  ||  +-------------------+       |
  ||  | array[j] >        |       |
  ||  | array[j+1] ?      |       |
  ||  +-------------------+       |
  ||    yes /       \ no          |
  ||       v         v            |
  || +-----------+  (skip)        |
  || | swap j,   |    |           |
  || | j+1       |    |           |
  || +-----------+    |           |
  ||       |          |           |
  ||       v          v           |
  ||  +---------------+           |
  ||  |    j++        |           |
  ||  +---------------+           |
  ||       |                      |
  |+-------+                      |
  |                               |
  |   +---------------+           |
  |   |    i++        |<----------+
  |   +---------------+
  |        |
  +--------+
```

---

## Where It Gets Tricky

1. **Inner loop bound shrinks each pass.** The inner loop runs to `len - 1 - i`, not `len - 1`. After each pass, one more element is guaranteed to be in its final position at the tail. Forgetting the `- i` still produces a correct result but wastes comparisons.

2. **No early termination.** If the array becomes sorted before all passes complete (like the already-sorted test case), this implementation keeps looping anyway. It still does all `n*(n-1)/2` comparisons. The optimized version adds a `swapped` flag to break out early.

3. **Stability.** Bubble sort is **stable** — equal elements maintain their relative order because the comparison uses strict `>` (not `>=`). If you changed the condition to `>=`, equal elements would swap unnecessarily and stability would be lost.

4. **Off-by-one in the outer loop.** The outer loop runs `i` from `0` to `len - 2` (i.e., `i < len - 1`). After `len - 1` passes, only one unsorted element remains, and a single element is trivially sorted. If you wrote `i < len`, the inner loop would run zero iterations on the last pass — harmless but wasteful.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time (best)** | O(n^2) | No early termination — always runs all passes, even on sorted input |
| **Time (average/worst)** | O(n^2) | Two nested loops, each up to n iterations |
| **Space** | O(1) | Only a constant amount of extra memory (one temp variable for swap) |

Note: With the early-termination optimization (see `optimized/`), the best case drops to O(n) for already-sorted input.

---

## Summary

Bubble Sort repeatedly walks through the array comparing adjacent elements and swapping them if they're out of order. After each full pass, the next-largest element settles into its correct position at the tail. Two nested loops drive the process: the outer counts passes, the inner does comparisons. It's O(n^2) in all cases because there's no early exit — even if the array is already sorted, every pass still runs. The algorithm is simple, stable, and in-place, but not efficient for large inputs. The optimized version in `optimized/` adds a `swapped` flag to exit early when no swaps occur.
