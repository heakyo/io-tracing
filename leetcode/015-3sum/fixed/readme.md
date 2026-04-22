# 015 - 3Sum (Fixed)

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Function Reference](#function-reference)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

---

## Problem Statement

Given an integer array `nums`, return all unique triplets
`[nums[i], nums[j], nums[k]]` such that:

- `i != j`, `i != k`, and `j != k`
- `nums[i] + nums[j] + nums[k] == 0`

The result must not contain duplicate triplets. The order of the output does
not matter.

Think of it this way: you have a bag of positive and negative numbers, and you
want to find every group of three that perfectly cancel each other out to zero.
The catch is you cannot list the same group twice, even if the same value
appears multiple times in the array.

---

## Core Idea

The standard approach is **sort + fix-one-element + two-pointer**. Here is the
mental model, explained as simply as possible.

**Step 1 — Sort the array.**
Sorting unlocks two things: (a) the two-pointer technique works only on sorted
data, and (b) duplicate values sit next to each other, making them trivial to
skip.

**Step 2 — For each element `nums[i]` (i from 0 to n-3), treat it as the
"anchor."**

- **Early break:** if `nums[i] > 0`, stop. The array is sorted, so every
  element to the right of `i` is also positive. Three positive numbers can
  never sum to zero.
- **Skip duplicate anchors:** if `nums[i] == nums[i-1]`, skip this `i`. We
  already explored every triplet that starts with this value.

**Step 3 — Two pointers for the remaining pair.**
Set `left = i + 1` and `right = n - 1`. Now slide them inward:

- Compute `sum = nums[i] + nums[left] + nums[right]`.
- If `sum == 0`: record the triplet. Then skip duplicate values on both the
  left and right sides, and move both pointers inward.
- If `sum < 0`: the total is too small, so move `left++` to get a larger
  value.
- If `sum > 0`: the total is too large, so move `right--` to get a smaller
  value.

This systematically explores all valid triplets without missing any. Every
anchor is tried exactly once, and the two-pointer sweep covers every valid pair
for that anchor.

---

## Step-by-Step Walkthrough

**Input:** `[-1, 0, 1, 2, -1, -4]`

**After sorting:** `[-4, -1, -1, 0, 1, 2]`

Indices:
```
index:  0    1    2    3   4   5
value: -4   -1   -1    0   1   2
```

### i = 0, nums[0] = -4

| left | right | sum              | action     |
|------|-------|------------------|------------|
| 1    | 5     | -4 + (-1) + 2 = -3 | sum < 0, left++ |
| 2    | 5     | -4 + (-1) + 2 = -3 | sum < 0, left++ |
| 3    | 5     | -4 + 0 + 2 = -2    | sum < 0, left++ |
| 4    | 5     | -4 + 1 + 2 = -1    | sum < 0, left++ |
| 5    | 5     | left >= right, done | —          |

No triplet found with anchor -4.

### i = 1, nums[1] = -1

| left | right | sum              | action     |
|------|-------|------------------|------------|
| 2    | 5     | -1 + (-1) + 2 = 0  | **Record [-1, -1, 2]**. Skip dups: left lands on 3, right lands on 4. |
| 3    | 4     | -1 + 0 + 1 = 0     | **Record [-1, 0, 1]**. Skip dups: left=4, right=3. |
| 4    | 3     | left >= right, done | —          |

Two triplets found.

### i = 2, nums[2] = -1

`nums[2] == nums[1]` (-1 == -1), so **skip** — we already explored every
triplet starting with -1.

### i = 3, nums[3] = 0

`nums[3] > 0`? No (0 is not > 0), so we proceed.

| left | right | sum              | action     |
|------|-------|------------------|------------|
| 4    | 5     | 0 + 1 + 2 = 3      | sum > 0, right-- |
| 4    | 4     | left >= right, done | —          |

No triplet found.

### i = 4

`i < n - 2` means `i < 4`, so the loop ends at i = 3.

**Result:** `[[-1, -1, 2], [-1, 0, 1]]`

---

## ASCII Flowchart

```
                          +-------+
                          | START |
                          +---+---+
                              |
                              v
                      +---------------+
                      | sort(nums)    |
                      +-------+-------+
                              |
                              v
                          +-------+
                          | i = 0 |
                          +---+---+
                              |
                              v
                    +-------------------+
              +---->| i < n - 2 ?       |----NO----> RETURN result
              |     +-------------------+
              |             | YES
              |             v
              |     +-------------------+
              |     | nums[i] > 0 ?     |----YES---> BREAK -> RETURN result
              |     +-------------------+
              |             | NO
              |             v
              |     +-----------------------------+
              |     | i>0 && nums[i]==nums[i-1] ? |----YES---> i++ ---+
              |     +-----------------------------+                   |
              |             | NO                                      |
              |             v                                         |
              |     +-------------------------+                       |
              |     | left = i+1              |                       |
              |     | right = n-1             |                       |
              |     +------------+------------+                       |
              |                  |                                    |
              |                  v                                    |
              |        +------------------+                           |
              |   +--->| left < right ?   |----NO----> i++ ----------+
              |   |    +------------------+                           |
              |   |            | YES                                  |
              |   |            v                                      |
              |   |    +-------------------------------+              |
              |   |    | sum = nums[i]+nums[l]+nums[r] |              |
              |   |    +-------------------------------+              |
              |   |            |                                      |
              |   |      +-----+------+                               |
              |   |      |     |      |                               |
              |   |   sum<0  sum==0  sum>0                            |
              |   |      |     |      |                               |
              |   |      v     v      v                               |
              |   |   left++ record right--                           |
              |   |      |   triplet  |                               |
              |   |      |     |      |                               |
              |   |      |     v      |                               |
              |   |      |  skip dups |                               |
              |   |      |  left++    |                               |
              |   |      |  right--   |                               |
              |   |      |     |      |                               |
              |   +------+-----+------+                               |
              |                                                       |
              +-------------------------------------------------------+
```

---

## Function Reference

All functions are defined in `fixed/main.c`.

### `void swap(int *a, int *b)`

**Purpose:** Swap two integers in place.

| Parameter | Description |
|-----------|-------------|
| `a`       | Pointer to the first integer  |
| `b`       | Pointer to the second integer |

**Returns:** Nothing. Modifies values at the given pointers.

### `void show_array(char *type, int *a, int len)`

**Purpose:** Print a one-dimensional integer array with a label.

| Parameter | Description |
|-----------|-------------|
| `type`    | Label string printed before the array |
| `a`       | Pointer to the integer array          |
| `len`     | Number of elements to print           |

**Returns:** Nothing. Output goes to stdout.

### `void show_array_dint(char *type, int **a, int size, int *a_column_size)`

**Purpose:** Print a two-dimensional integer array (array of arrays) with a
label.

| Parameter       | Description |
|-----------------|-------------|
| `type`          | Label string printed before the array           |
| `a`             | Array of int pointers (the 2D data)              |
| `size`          | Number of rows                                   |
| `a_column_size` | Array holding the column count for each row      |

**Returns:** Nothing. Output goes to stdout.

### `int compare(const void *a, const void *b)`

**Purpose:** Comparator function for `qsort`. Sorts integers in ascending
order.

| Parameter | Description |
|-----------|-------------|
| `a`       | Pointer to the first element (cast to `int*`)  |
| `b`       | Pointer to the second element (cast to `int*`) |

**Returns:** Negative if `*a < *b`, zero if equal, positive if `*a > *b`.

### `int** threeSum(int *nums, int numsSize, int *returnSize, int **returnColumnSizes)`

**Purpose:** Main algorithm. Sorts the input array, then uses the
fix-one-element + two-pointer approach to find all unique triplets that sum to
zero.

| Parameter            | Description |
|----------------------|-------------|
| `nums`               | Input integer array (modified in place by sort)           |
| `numsSize`           | Number of elements in `nums`                              |
| `returnSize`         | Output: set to the number of triplets found               |
| `returnColumnSizes`  | Output: allocated array where each entry is 3             |

**Returns:** A `malloc`-ed array of `int*`, each pointing to a triplet of
three integers. Caller is responsible for freeing all memory.

### `static int compare_triplet(const void *a, const void *b)`

**Purpose:** Comparator for sorting an array of `int*` triplets
lexicographically. Used in test validation to put the actual results in a
canonical order before comparing with expected output.

| Parameter | Description |
|-----------|-------------|
| `a`       | Pointer to first `int*` triplet  |
| `b`       | Pointer to second `int*` triplet |

**Returns:** Negative, zero, or positive based on lexicographic comparison of
the two triplets (first by element 0, then 1, then 2).

### `static int compare_flat(const void *a, const void *b)`

**Purpose:** Comparator for sorting `int[][3]` triplets (flat, stack-allocated
arrays) lexicographically. Used to sort the expected-result arrays in test
validation.

| Parameter | Description |
|-----------|-------------|
| `a`       | Pointer to first flat triplet (`int[3]`)  |
| `b`       | Pointer to second flat triplet (`int[3]`) |

**Returns:** Negative, zero, or positive based on lexicographic comparison.

### `static void run_test(const char *name, int *nums, int numsSize, int expected[][3], int expected_size)`

**Purpose:** Run a single test case. Copies the input (so the original is
preserved), calls `threeSum`, sorts both the actual and expected results into
canonical order, compares them element by element, and prints PASS or FAIL.

| Parameter       | Description |
|-----------------|-------------|
| `name`          | Human-readable test name for the output      |
| `nums`          | Input array for this test case                |
| `numsSize`      | Length of `nums`                              |
| `expected`      | 2D array of expected triplets (`int[][3]`)    |
| `expected_size` | Number of expected triplets                   |

**Returns:** Nothing. Prints the result to stdout and frees all allocated
memory.

---

## Where It Gets Tricky

### (a) Duplicate skipping at three levels

There are three distinct places where duplicates must be skipped, and **all
three are necessary**:

1. **Duplicate anchor:** `if (i > 0 && nums[i] == nums[i-1]) continue;`
   Without this, you would generate the same set of triplets for every
   repeated value of `nums[i]`.
2. **Duplicate left:** `while (left < right && nums[left] == nums[left+1]) left++;`
   After finding a valid triplet, if the next left value is the same, it would
   produce the exact same triplet again.
3. **Duplicate right:** `while (left < right && nums[right] == nums[right-1]) right--;`
   Same reasoning as above, but for the right pointer.

Miss any one of these and you get duplicate triplets in your output.

### (b) Early termination when nums[i] > 0

```c
	if (nums[i] > 0)
		break;
```

This is not just an optimization — it is a correctness shortcut. Since the
array is sorted, `nums[left] >= nums[i]` and `nums[right] >= nums[i]`. So:

```
nums[i] + nums[left] + nums[right] >= 3 * nums[i] > 0
```

Every remaining sum is strictly positive. No more valid triplets exist, so we
break out of the loop entirely (not just `continue`).

### (c) Moving BOTH pointers after recording a triplet

After recording a triplet where `sum == 0`, the code does:

```c
	left++;
	right--;
```

If you only moved one pointer, say `left++`, then `nums[i] + nums[left_new] + nums[right]` would not be zero (it would be too large), so you would just end
up decrementing `right` on the next iteration anyway — wasting a step. Worse,
if you forgot to move either pointer at all, you would loop forever on the
same triplet.

Moving both is correct because: if you increase `left`, the sum grows, so you
must decrease `right` to have any chance of hitting zero again.

### (d) Duplicate-skip loops run BEFORE the final pointer increment

Look at the exact sequence:

```c
	while (left < right && nums[left] == nums[left + 1])
		left++;
	while (left < right && nums[right] == nums[right - 1])
		right--;
	left++;
	right--;
```

The `while` loops skip past all copies of the current value, landing on the
**last** copy. Then `left++` and `right--` move to a genuinely new value. If
you reversed the order (increment first, then skip), you would land one
position too far and potentially miss valid triplets.

---

## Complexity Analysis

**Time: O(n^2)**

- Sorting costs O(n log n).
- The outer loop runs O(n) times (one iteration per anchor).
- For each anchor, the two-pointer sweep is O(n) in the worst case — each
  pointer moves at most n positions total.
- Combined: O(n log n) + O(n) * O(n) = O(n^2).

**Space: O(1) extra**

- The sort is in-place (`qsort` uses O(log n) stack space).
- The two pointers and loop variables use constant space.
- The output array is not counted as extra space (it is part of the required
  return value).
- Total extra space: O(log n) for the sort stack, which is effectively O(1).

---

## Summary

The standard **sort + anchor + two-pointer** approach guarantees that all valid
triplets are found. By fixing one element and using two pointers for the
remaining two, every valid combination is explored in O(n) time per anchor.
Duplicate skipping at all three positions — the anchor, the left pointer, and
the right pointer — ensures no duplicate triplets appear in the result. The
early break when `nums[i] > 0` prunes impossible branches. The total running
time is **O(n^2)** with **O(1) extra space**, which is optimal for this
problem since there can be O(n^2) valid triplets in the worst case.
