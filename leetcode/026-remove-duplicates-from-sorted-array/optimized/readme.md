# 026 - Remove Duplicates from Sorted Array (Optimized)

## Table of Contents

1. [Why Optimize?](#why-optimize)
2. [Core Idea](#core-idea)
3. [Function Reference](#function-reference)
4. [ASCII Flowchart](#ascii-flowchart)
5. [Step-by-Step Walkthrough](#step-by-step-walkthrough)
6. [Where It Gets Tricky](#where-it-gets-tricky)
7. [Complexity Analysis](#complexity-analysis)
8. [Comparison with Original](#comparison-with-original)
9. [Summary](#summary)

---

## Why Optimize?

The original algorithm uses a `swap()` helper to move each unique element into
place. Every swap performs three assignments: `tmp = a`, `a = b`, `b = tmp`.
That is three writes per unique element discovered.

But think about what we are actually doing. Position `k` and everything beyond
it is territory that LeetCode will never inspect -- it is garbage. The problem
statement says: "It does not matter what you leave beyond the returned k." So
there is no reason to carefully preserve the value sitting at `nums[k]` by
swapping it somewhere else. We can just overwrite it with a single assignment:
`nums[k] = nums[i]`. One write instead of three.

The optimized version makes three additional improvements:

- It eliminates the separate `swap` function entirely, since it is no longer
  needed.
- It handles `numsSize == 0` with an early return. The original skips this
  check and returns 1 for an empty array, which is incorrect.
- It folds the `k++` increment directly into the assignment expression
  (`nums[k++] = nums[i]`), producing more compact code without sacrificing
  clarity.

---

## Core Idea

The two-pointer logic is identical to the original. Pointer `k` marks the
write position -- the end of the "unique prefix" that we are building at the
front of the array. Pointer `i` scans forward through every element.

Whenever `nums[i] != nums[k-1]` (that is, the scanner finds a value different
from the last unique value we kept), we copy `nums[i]` into `nums[k]` and
advance `k`. That is the whole algorithm: one pass, one comparison, one
conditional copy.

The only structural differences from the original are:

- Direct assignment (`nums[k] = nums[i]`) replaces the three-assignment swap.
- An early return for the empty array case (`numsSize == 0` returns 0).

---

## Function Reference

| Function            | Signature                                                                              | Purpose                                                                                              |
|---------------------|----------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------|
| `removeDuplicates`  | `int removeDuplicates(int *nums, int numsSize)`                                        | Removes duplicates in-place using direct assignment. Returns count of unique elements.                |
| `print_array`       | `static void print_array(const char *label, int *array, int len)`                      | Prints array with label. Handles empty arrays.                                                       |
| `run_test`          | `static void run_test(const char *name, int *src, int len, int expected_k, int *expected_nums)` | Runs a test case: copies input, calls algorithm, asserts k and first k elements match expected.       |
| `main`              | `int main(int argc, char *argv[])`                                                     | Entry point. Runs all 10 test cases (identical to original) and prints "All tests passed!".           |

---

## ASCII Flowchart

```
                    +-------+
                    | START |
                    +---+---+
                        |
                        v
               +------------------+
               | numsSize == 0 ?  |
               +--------+---------+
                yes /        \ no
                   v          v
            +----------+   +------------+
            | return 0 |   | i=1, k=1   |
            +----------+   +-----+------+
                                 |
                                 v
                        +----------------+
                   +--->| i < numsSize ? |
                   |    +-------+--------+
                   |       yes /    \ no
                   |          v      v
                   |  +-----------------+   +----------+
                   |  | nums[i] !=      |   | return k |
                   |  | nums[k-1] ?     |   +----------+
                   |  +------+----------+
                   |    yes /    \ no
                   |       v      |
                   | +-----------+|
                   | | nums[k++] ||
                   | | = nums[i] ||
                   | +-----+-----+|
                   |       |      |
                   |       v      v
                   |      +------+
                   |      | i++  |
                   |      +--+---+
                   |         |
                   +---------+
```

---

## Step-by-Step Walkthrough

### Example 1: [1, 1, 2]

Starting state: `k = 1`, `i = 1`.

| Step | `i` | `nums[i]` | `nums[k-1]` | Different? | Action              | `k` after | Array state   |
|------|-----|-----------|-------------|------------|---------------------|-----------|---------------|
| 1    | 1   | 1         | 1           | No         | Skip                | 1         | [1, 1, 2]     |
| 2    | 2   | 2         | 1           | Yes        | `nums[1] = 2`, k++  | 2         | [1, 2, 2]     |

Loop ends (`i = 3`, not < 3). Return `k = 2`.

Result: first 2 elements are `[1, 2]`.

### Example 2: [0, 0, 1, 1, 1, 2, 2, 3, 3, 4]

Starting state: `k = 1`, `i = 1`.

| Step | `i` | `nums[i]` | `nums[k-1]` | Different? | Action              | `k` after | Array state (first 5)   |
|------|-----|-----------|-------------|------------|---------------------|-----------|-------------------------|
| 1    | 1   | 0         | 0           | No         | Skip                | 1         | [0, 0, 1, 1, 1, ...]    |
| 2    | 2   | 1         | 0           | Yes        | `nums[1] = 1`, k++  | 2         | [0, 1, 1, 1, 1, ...]    |
| 3    | 3   | 1         | 1           | No         | Skip                | 2         | [0, 1, 1, 1, 1, ...]    |
| 4    | 4   | 1         | 1           | No         | Skip                | 2         | [0, 1, 1, 1, 1, ...]    |
| 5    | 5   | 2         | 1           | Yes        | `nums[2] = 2`, k++  | 3         | [0, 1, 2, 1, 1, ...]    |
| 6    | 6   | 2         | 2           | No         | Skip                | 3         | [0, 1, 2, 1, 1, ...]    |
| 7    | 7   | 3         | 2           | Yes        | `nums[3] = 3`, k++  | 4         | [0, 1, 2, 3, 1, ...]    |
| 8    | 8   | 3         | 3           | No         | Skip                | 4         | [0, 1, 2, 3, 1, ...]    |
| 9    | 9   | 4         | 3           | Yes        | `nums[4] = 4`, k++  | 5         | [0, 1, 2, 3, 4, ...]    |

Loop ends (`i = 10`, not < 10). Return `k = 5`.

Result: first 5 elements are `[0, 1, 2, 3, 4]`.

---

## Where It Gets Tricky

**(a) Assignment vs swap: why it is safe to overwrite.**

The value sitting at `nums[k]` when we write to it is always a duplicate. Here
is why: `k <= i` at all times (they start equal at 1, and `k` only increments
when `i` does, but not every time `i` does). If `k < i`, then position `k` has
already been "passed over" by the unique-prefix builder -- it holds a leftover
value that is either a duplicate of something already in `nums[0..k-1]` or a
previously copied value we no longer need. Either way, overwriting it with one
assignment is safe and saves two of the three writes that swap would perform.

**(b) `nums[k++] = nums[i]` combines write and advance.**

This is a standard C idiom. It is exactly equivalent to writing:

```c
nums[k] = nums[i];
k++;
```

The post-increment `k++` evaluates to the current value of `k` (used as the
array index), then increments `k` afterward. Compact, but no hidden magic.

**(c) Empty array guard.**

The original algorithm starts with `k = 1` and returns `k` directly. If
`numsSize` is 0, the loop body never executes, and it returns 1 -- claiming
there is one unique element in an empty array. That is wrong. The optimized
version adds an explicit check at the top: if `numsSize == 0`, return 0.

**(d) Self-copy when there are no duplicates yet.**

When `k == i` (which happens at the start, and continues as long as every
element is unique), the assignment `nums[k] = nums[i]` copies a value onto
itself. This is a harmless no-op -- one wasted write, but no incorrect
behavior. Adding a `k != i` guard would save that write at the cost of an
extra branch on every iteration, which is not worth it.

---

## Complexity Analysis

| Metric | Value | Explanation                                                       |
|--------|-------|-------------------------------------------------------------------|
| Time   | O(n)  | Single pass through the array. Each element is visited exactly once. |
| Space  | O(1)  | Only two integer variables (`i`, `k`). No extra allocation.        |

---

## Comparison with Original

|                   | Original                        | Optimized                          |
|-------------------|---------------------------------|------------------------------------|
| Move operation    | swap (3 assignments)            | Direct assignment (1 assignment)   |
| Extra function    | `swap()` helper                 | None                               |
| Empty array       | Returns 1 (wrong)               | Returns 0 (correct)               |
| Lines of code     | 14 (swap + function body)       | 8 (function body)                  |
| Time              | O(n)                            | O(n)                               |
| Space             | O(1)                            | O(1)                               |

---

## Summary

The optimized version keeps the same two-pointer skeleton as the original --
`k` tracks the unique prefix, `i` scans ahead, and a comparison decides
whether to keep each element -- but replaces the three-assignment swap with a
single direct assignment, since values beyond position `k` are garbage that
LeetCode never inspects. It also removes the standalone `swap` helper, fixes
the empty-array edge case by returning 0 instead of 1, and folds the `k`
increment into the assignment expression. The result is shorter, slightly
faster in practice (fewer writes per unique element), and correct on all
inputs including the empty array.
