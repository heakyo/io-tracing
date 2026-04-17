# 026 - Remove Duplicates from Sorted Array

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Core Idea](#core-idea)
3. [Step-by-Step Walkthrough](#step-by-step-walkthrough)
4. [ASCII Flowchart](#ascii-flowchart)
5. [Where It Gets Tricky](#where-it-gets-tricky)
6. [Complexity Analysis](#complexity-analysis)
7. [Summary](#summary)

---

## Problem Statement

You are given an integer array `nums` that is already sorted in non-decreasing
order. Your job is to remove the duplicates *in-place* so that each unique value
appears exactly once. After you are done, the first `k` elements of the array
must hold all the unique values in their original order, and you return `k` --
the count of unique elements. Whatever sits beyond position `k-1` does not
matter.

The critical constraint is **in-place**: you cannot allocate a second array and
copy things over. You have to rearrange the elements inside the same array you
were given.

| Input array                  | k | First k elements     |
| ---------------------------- | - | -------------------- |
| `[1, 1, 2]`                 | 2 | `[1, 2]`             |
| `[0, 0, 1, 1, 1, 2, 2, 3, 3, 4]` | 5 | `[0, 1, 2, 3, 4]` |

---

## Core Idea

The algorithm uses a **two-pointer approach**.

- `k` is the **write pointer**. It tracks the length of the "unique prefix" --
  the region at the front of the array where every element is distinct and in
  order. It also points to the next slot that is available for writing.
- `i` is the **read pointer**. It scans forward through the entire array, one
  element at a time.

For each position `i`, the algorithm asks a single question:

> Is `nums[i]` different from `nums[k-1]` (the last element placed in the
> unique prefix)?

If **yes**, this is a new unique value. Swap `nums[i]` into position `nums[k]`
and advance `k` by one. If **no**, this value is a duplicate of something
already in the prefix -- skip it and move on.

Why is comparing with `nums[k-1]` enough? Because the array is sorted. All
copies of a given value sit in a consecutive run. Once you have placed one copy
into the unique prefix and moved past the run, you will never encounter that
value again. So you only ever need to check against the most recently placed
unique element.

The swap preserves both elements (the old value at position `k` gets moved to
position `i`), but we do not actually need the swapped-out value -- everything
beyond the unique prefix is "don't care" territory. A simple assignment would
work just as well. More on that below.

After the loop finishes, positions `0` through `k-1` hold every unique value
exactly once, in the same relative order they appeared in the original array.
The function returns `k`.

---

## Step-by-Step Walkthrough

### Example 1: `[1, 1, 2]`

Initial state: `k = 1`, `i = 1`.

**Iteration 1** -- `i = 1`:

- Compare `nums[1]` (1) with `nums[k-1]` = `nums[0]` (1).
- They are equal, so this is a duplicate. Do nothing.
- Array: `[1, 1, 2]`, `k = 1`.

**Iteration 2** -- `i = 2`:

- Compare `nums[2]` (2) with `nums[k-1]` = `nums[0]` (1).
- They differ. Swap `nums[1]` with `nums[2]`, then `k++`.
- Array: `[1, 2, 1]`, `k = 2`.

Loop ends (`i = 3` is not less than `numsSize = 3`). Return `k = 2`.
First 2 elements: `[1, 2]`. Correct.

---

### Example 2: `[0, 0, 1, 1, 1, 2, 2, 3, 3, 4]`

Initial state: `k = 1`, `i = 1`. `numsSize = 10`.

| i | nums[i] | nums[k-1] | Equal? | Action             | k after | Array after                        |
|---|---------|-----------|--------|--------------------|---------|------------------------------------|
| 1 | 0       | 0         | yes    | skip               | 1       | `[0, 0, 1, 1, 1, 2, 2, 3, 3, 4]` |
| 2 | 1       | 0         | no     | swap [1]<->[2], k++ | 2       | `[0, 1, 0, 1, 1, 2, 2, 3, 3, 4]` |
| 3 | 1       | 1         | yes    | skip               | 2       | `[0, 1, 0, 1, 1, 2, 2, 3, 3, 4]` |
| 4 | 1       | 1         | yes    | skip               | 2       | `[0, 1, 0, 1, 1, 2, 2, 3, 3, 4]` |
| 5 | 2       | 1         | no     | swap [2]<->[5], k++ | 3       | `[0, 1, 2, 1, 1, 0, 2, 3, 3, 4]` |
| 6 | 2       | 2         | yes    | skip               | 3       | `[0, 1, 2, 1, 1, 0, 2, 3, 3, 4]` |
| 7 | 3       | 2         | no     | swap [3]<->[7], k++ | 4       | `[0, 1, 2, 3, 1, 0, 2, 1, 3, 4]` |
| 8 | 3       | 3         | yes    | skip               | 4       | `[0, 1, 2, 3, 1, 0, 2, 1, 3, 4]` |
| 9 | 4       | 3         | no     | swap [4]<->[9], k++ | 5       | `[0, 1, 2, 3, 4, 0, 2, 1, 3, 1]` |

Loop ends. Return `k = 5`. First 5 elements: `[0, 1, 2, 3, 4]`. Correct.

Notice how the tail of the array (`[0, 2, 1, 3, 1]`) is a jumble of leftover
duplicates. That is fine -- the problem says we can ignore everything past
position `k-1`.

---

## ASCII Flowchart

```
+---------------------+
|  START              |
|  k = 1,  i = 1     |
+---------------------+
          |
          v
+---------------------+
|  i < numsSize ?     |
+---------------------+
    |            |
    | no         | yes
    v            v
+--------+   +-----------------------------+
| return |   | nums[i] != nums[k-1] ?      |
|   k    |   +-----------------------------+
+--------+       |                |
                 | no              | yes
                 |                 v
                 |     +-------------------------+
                 |     | swap(nums[k], nums[i])  |
                 |     | k++                     |
                 |     +-------------------------+
                 |                 |
                 v                 v
              +---------------------+
              |       i++           |
              +---------------------+
                       |
                       +------> (back to "i < numsSize ?")
```

---

## Where It Gets Tricky

### (a) `swap` vs simple assignment

The code calls `swap(&nums[k], &nums[i])`, which exchanges the two values.
A plain assignment `nums[k] = nums[i]` would also produce a correct result,
because the value being overwritten at `nums[k]` is a duplicate that has
already been accounted for (or, when `k == i`, it is the element itself). The
positions beyond the unique prefix are "don't care" -- the problem explicitly
says their contents are irrelevant.

So the swap is unnecessary overhead: it does an extra read, an extra write, and
uses a temporary variable, all to preserve a value nobody will ever look at.
It does not hurt correctness, but a simple assignment would be leaner.

### (b) `k` starts at 1, not 0

In a non-empty sorted array, the very first element is always part of the
unique prefix -- there is nothing before it for it to be a duplicate of.
So the unique prefix begins with length 1, and both `k` and `i` start at 1.
The loop never touches index 0; it is already in its correct place.

### (c) Comparing with `nums[k-1]`, not `nums[i-1]`

This is the most subtle point. The comparison is between `nums[i]` and
`nums[k-1]` -- the last element written into the unique prefix -- **not**
between `nums[i]` and `nums[i-1]`.

Why does this matter? Once swaps start happening, the element at `nums[i-1]`
may no longer be the value that was originally there. It could be a leftover
that was swapped in from the unique prefix region. If you compared against
`nums[i-1]`, you might see two different values and incorrectly conclude that
`nums[i]` is unique, when in fact it is a duplicate of something already placed.

By comparing against `nums[k-1]`, you always check against the true last unique
value, regardless of what swaps have done to the rest of the array.

### (d) Does not handle `numsSize == 0`

The function initializes `k = 1` and immediately returns `k` if the loop body
never executes (which it will not when `numsSize` is 0 or 1). For
`numsSize == 1` this is correct -- a single-element array has exactly one
unique value. For `numsSize == 0` it returns 1, which is wrong: an empty array
has zero unique elements.

LeetCode guarantees `1 <= nums.length`, so this edge case never arises in
practice. But if you were writing a general-purpose library function, you would
want a guard like:

```c
	if (numsSize == 0)
		return 0;
```

---

## Complexity Analysis

- **Time: O(n)** -- The read pointer `i` visits every element exactly once in a
  single pass from index 1 to `numsSize - 1`. Each iteration does at most one
  comparison and one swap, both O(1). Total work is linear in the array length.

- **Space: O(1)** -- The algorithm operates in-place. The only extra storage is
  a handful of integer variables (`i`, `k`, and the `tmp` inside `swap`),
  independent of the input size.

---

## Summary

The algorithm maintains two pointers over a sorted array: a read pointer `i`
that scans every element left to right, and a write pointer `k` that marks the
boundary of the unique prefix built at the front of the array. Whenever `i`
lands on a value different from the last unique element (`nums[k-1]`), that
value is swapped into position `k` and the prefix grows by one. Because the
input is sorted, duplicates are always consecutive, so a single comparison
against the tail of the prefix is enough to detect them. The result is a single
O(n) pass using O(1) extra space, leaving the first `k` positions of the array
holding every distinct value exactly once in their original order.
