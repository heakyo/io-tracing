# 088 - Merge Sorted Array

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

You are given two integer arrays `nums1` and `nums2`, sorted in non-decreasing order. You are also given two integers `m` and `n`, representing the number of elements in `nums1` and `nums2` respectively.

Merge `nums2` into `nums1` so that `nums1` becomes a single sorted array. `nums1` has a total length of `m + n` — the first `m` elements are the actual data, and the remaining `n` slots are filled with zeros (placeholders for the merged result).

Example:

```
Input:  nums1 = [1, 2, 3, 0, 0, 0], m = 3
        nums2 = [2, 5, 6],           n = 3
Output: nums1 = [1, 2, 2, 3, 5, 6]
```

---

## Core Idea

Imagine you have two sorted stacks of exam papers and one long desk with exactly enough room for all of them. If you start placing papers from the **front** of the desk, you risk overwriting papers from the first stack that you haven't placed yet.

The trick: **start from the back**. Compare the largest paper from each stack, place the bigger one at the end of the desk, and work your way forward. Since `nums1` has empty slots at the tail, filling from the back never overwrites unprocessed elements.

The code uses three pointers:
- `p1` — points to the last real element in `nums1` (index `m - 1`)
- `p2` — points to the last element in `nums2` (index `n - 1`)
- `p` — points to the last slot in `nums1` (index `m + n - 1`)

At each step, compare `*p1` and `*p2`, copy the larger to `*p`, and move the corresponding pointers backward. After the main loop, if `nums2` has remaining elements, copy them over. No need to copy remaining `nums1` elements — they're already in place.

---

## Step-by-Step Walkthrough

Let's trace `nums1 = [4, 0, 0, 0, 0, 0]` (m=1) and `nums2 = [1, 2, 3, 5, 6]` (n=5):

```
Initial state:
  nums1: [4, 0, 0, 0, 0, 0]
  p1=0 (value 4), p2=4 (value 6), p=5

Step 1: *p2=6 > *p1=4 -> nums1[5]=6, p2=3, p=4
  nums1: [4, 0, 0, 0, 0, 6]

Step 2: *p2=5 > *p1=4 -> nums1[4]=5, p2=2, p=3
  nums1: [4, 0, 0, 0, 5, 6]

Step 3: *p2=3 < *p1=4 -> nums1[3]=4, p1 goes before nums1, p=2
  nums1: [4, 0, 0, 4, 5, 6]

Main loop exits (p1 before nums1).

Copy remaining nums2: 3, 2, 1
  nums1[2]=3, nums1[1]=2, nums1[0]=1

Result: [1, 2, 3, 4, 5, 6]
```

Another example: `nums1 = [1, 2, 3, 0, 0, 0]` (m=3), `nums2 = [2, 5, 6]` (n=3):

```
p1=2 (value 3), p2=2 (value 6), p=5

Step 1: 6 > 3 -> nums1[5]=6, p2=1, p=4
Step 2: 5 > 3 -> nums1[4]=5, p2=0, p=3
Step 3: 2 < 3 -> nums1[3]=3, p1=1, p=2
Step 4: 2 = 2 -> nums1[2]=2 (from p1, since else branch), p1=0, p=1
Step 5: 2 > 1 -> nums1[1]=2, p2 before nums2, p=0

Main loop exits (p2 before nums2). No remaining nums2 elements.

Result: [1, 2, 2, 3, 5, 6]
```

---

## ASCII Flowchart

```
+-------------------------------------------+
| merge(nums1, nums1Size, m, nums2,         |
|       nums2Size, n)                        |
+-------------------------------------------+
                    |
                    v
         +--------------------+
         | p1 = nums1 + m - 1|
         | p2 = nums2 + n - 1|
         | p  = nums1+m+n-1  |
         +--------------------+
                    |
                    v
         +--------------------+
    +--->| p1 >= nums1 &&    |--no--+
    |    | p2 >= nums2 ?     |      |
    |    +--------------------+      |
    |           | yes                |
    |           v                    |
    |    +--------------------+      |
    |    | *p2 > *p1 ?       |      |
    |    +--------------------+      |
    |     yes /        \ no         |
    |        v          v            |
    |   +----------+ +----------+   |
    |   | *p = *p2 | | *p = *p1 |   |
    |   | p2--     | | p1--     |   |
    |   +----------+ +----------+   |
    |        |          |            |
    |        v          v            |
    |    +--------------------+      |
    |    |       p--          |      |
    |    +--------------------+      |
    |           |                    |
    +-----------+                    |
                                     |
         +---------------------------+
         |
         v
    +--------------------+
+-->| p2 >= nums2 ?     |--no--> Return
|   +--------------------+
|          | yes
|          v
|   +--------------------+
|   | *p = *p2           |
|   | p2--, p--          |
|   +--------------------+
|          |
+----------+
```

---

## Where It Gets Tricky

1. **Merging from the back, not the front.** If you merge from the front (index 0 upward), each insertion shifts all remaining `nums1` elements right, turning O(m+n) into O(m*n). Merging from the back avoids this entirely because the tail slots are empty placeholders.

2. **Only `nums2` needs a leftover copy loop.** After the main loop, if `nums2` has remaining elements, they must be copied. But if `nums1` has remaining elements, they're already in their correct positions (they were there from the start and nothing has overwritten them).

3. **Pointer comparison for loop bounds.** The code uses `nums1 <= p1` and `nums2 <= p2` instead of index-based comparisons. When `m=0`, `p1 = nums1 - 1` which is before `nums1`, so `nums1 <= p1` is immediately false. Same logic for `n=0`. This elegantly handles edge cases without special-casing.

4. **The `swap` function is unused.** The code defines `swap` but `merge` doesn't call it. The merge is done by direct pointer assignment (`*p-- = *p2--`), which is more efficient than swapping.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time** | O(m + n) | Each element is visited and placed exactly once |
| **Space** | O(1) | Only three pointer variables; merge is done in-place within `nums1` |

This is already the optimal solution for this problem — you can't do better than O(m+n) time since every element must be examined at least once.

---

## Summary

Merge Sorted Array merges two sorted arrays by filling `nums1` from the back. Three pointers track the end of each array's data and the write position. At each step, the larger of the two tail elements gets placed at the write position, and both the source and write pointers move backward. After the main loop, any remaining `nums2` elements are copied. This avoids the element-shifting problem of front-to-back merging and achieves O(m+n) time with O(1) extra space — already optimal.
