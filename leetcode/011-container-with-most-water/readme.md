# 011 - Container With Most Water

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

You are given an array of `n` non-negative integers `height[0..n-1]`. Each
element `height[i]` represents a vertical line drawn at horizontal position `i`
whose top end is at coordinate `(i, height[i])` and whose bottom end sits on the
x-axis at `(i, 0)`.

Pick any two of these lines. Together with the x-axis, they form a
U-shaped container. The amount of water that container can hold is determined by
two things:

- **Height** -- the water level cannot rise above the *shorter* of the two
  lines, so the effective height is `min(height[left], height[right])`.
- **Width** -- the horizontal distance between the two lines, which is simply
  `right - left`.

Therefore:

```
water = min(height[left], height[right]) * (right - left)
```

The task is to return the maximum water over all possible pairs of lines.

**Example**

Given `height = [1, 8, 6, 2, 5, 4, 8, 3, 7]`:

| Position | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|----------|---|---|---|---|---|---|---|---|---|
| Height   | 1 | 8 | 6 | 2 | 5 | 4 | 8 | 3 | 7 |

The best pair is positions 1 and 8 (heights 8 and 7). The container they form
has effective height `min(8, 7) = 7` and width `8 - 1 = 7`, giving an area of
`7 * 7 = 49`.

---

## Core Idea

The brute-force approach checks every pair of lines -- that is O(n^2). We can do
much better with **two pointers**.

Start with the widest possible container: place one pointer `left` at position 0
and another pointer `right` at position `n - 1`. Compute the area and record it
if it beats the current maximum. Then make a decision: **move the pointer that
points to the shorter line inward by one position**.

Why does this greedy choice work?

Think about it this way. The area formula is `min(h[left], h[right]) * width`.
Suppose `h[left] < h[right]`. The bottleneck is the left line -- it is the
shorter one, so it alone determines the water level. Now consider what happens if
you move the *taller* pointer (`right`) inward instead:

- The width decreases by 1.
- The min height can only stay the same or decrease (because `h[left]` is still
  the shorter side, and the new `h[right]` might be even shorter).
- Therefore the area can only decrease or stay the same. You will never find a
  better solution by moving the taller side inward while keeping the shorter side
  fixed.

On the other hand, if you move the *shorter* pointer (`left`) inward:

- The width decreases by 1, yes.
- But the new `h[left]` might be much taller, which could raise the min height
  enough to more than compensate for the lost width.

So the only direction that has any chance of improving the area is to move the
shorter side. This greedy reasoning guarantees that we never skip over the
optimal pair, and we converge in at most `n - 1` steps.

---

## Step-by-Step Walkthrough

Input: `height = [1, 8, 6, 2, 5, 4, 8, 3, 7]` (n = 9)

| Iter | left | right | h[left] | h[right] | min | width | area | max\_area | Move         |
|------|------|-------|---------|----------|-----|-------|------|-----------|--------------|
| 1    | 0    | 8     | 1       | 7        | 1   | 8     | 8    | 8         | left++ (1<7) |
| 2    | 1    | 8     | 8       | 7        | 7   | 7     | 49   | 49        | right-- (8>=7) |
| 3    | 1    | 7     | 8       | 3        | 3   | 6     | 18   | 49        | right-- (8>=3) |
| 4    | 1    | 6     | 8       | 8        | 8   | 5     | 40   | 49        | right-- (8>=8) |
| 5    | 1    | 5     | 8       | 4        | 4   | 4     | 16   | 49        | right-- (8>=4) |
| 6    | 1    | 4     | 8       | 5        | 5   | 3     | 15   | 49        | right-- (8>=5) |
| 7    | 1    | 3     | 8       | 2        | 2   | 2     | 4    | 49        | right-- (8>=2) |
| 8    | 1    | 2     | 8       | 6        | 6   | 1     | 6    | 49        | right-- (8>=6) |

After iteration 8, `left = 1` and `right = 1`. The condition `left < right` is
false, so the loop exits.

**Result: 49**

---

## ASCII Flowchart

```
                      +-------------------------+
                      |          START          |
                      +-------------------------+
                                  |
                                  v
                      +-------------------------+
                      |  left = 0               |
                      |  right = heightSize - 1 |
                      |  max_area = 0           |
                      +-------------------------+
                                  |
                                  v
                      +-------------------------+
                      |    left < right ?       |
                      +-------------------------+
                       /                       \
                     yes                        no
                     /                            \
                    v                              v
     +-------------------------------+   +-------------------+
     | area = MIN(h[left], h[right]) |   | return max_area   |
     |        * (right - left)       |   +-------------------+
     +-------------------------------+
                    |
                    v
             +--------------+
             | area >       |
             | max_area ?   |
             +--------------+
              /           \
            yes            no
            /               \
           v                 |
  +------------------+       |
  | max_area = area  |       |
  +------------------+       |
           \                 |
            \               /
             v             v
      +------------------------+
      | h[left] < h[right] ?  |
      +------------------------+
           /            \
         yes             no
         /                \
        v                  v
  +-----------+     +------------+
  |  left++   |     |  right--   |
  +-----------+     +------------+
        \                /
         \              /
          v            v
          +------------+
          | (loop back |
          |  to check) |
          +------------+
                |
                v
      +-------------------------+
      |    left < right ?       |
      +-------------------------+
              (as above)
```

---

## Where It Gets Tricky

### (a) Why move the shorter side?

This is the heart of the algorithm's correctness and the part most people find
non-obvious. Here is the argument stated precisely.

Suppose at some step we have pointers at `left` and `right` with
`h[left] < h[right]`. The current area is `h[left] * (right - left)`. Now
consider every pair `(left, r)` where `left < r <= right`. For all such pairs,
the effective height is at most `h[left]` (since `min(h[left], h[r]) <= h[left]`
for any `h[r]`), and the width `r - left` is at most `right - left`. So no pair
involving `left` with any index between `left` and `right` can produce an area
larger than the one we just computed. We have already recorded it. That means
`left` can be safely discarded -- it will never be part of a better solution.
Hence, `left++`.

The symmetric argument applies when `h[right] < h[left]`.

### (b) Equal heights

When `h[left] == h[right]`, the ternary expression
`(height[left] < height[right]) ? left++ : right--` evaluates the condition as
false, so `right--` is executed.

Does it matter which one we move? No. When both heights are equal, the same
argument applies in both directions. The current area is
`h[left] * (right - left)`. Any pair `(left, r)` with `r < right` has width
strictly less than `right - left` and min-height at most `h[left]`, so it cannot
beat the current area. Symmetrically for `(l, right)` with `l > left`. So both
`left` and `right` can be discarded. Moving either one (or even both
simultaneously) is correct.

### (c) The MIN macro and double evaluation

The macro is defined as:

```c
#define MIN(a, b) ((a) < (b) ? (a) : (b))
```

This evaluates one of its arguments twice. In this code, the arguments are
`height[left]` and `height[right]` -- simple array index operations with no side
effects, so double evaluation is harmless. But it is worth noting: if `MIN` were
ever called with expressions that have side effects (e.g., `MIN(a++, b++)`), the
behavior would be incorrect. A safer alternative in modern C would be a
`static inline` function.

### (d) Single element edge case

If `heightSize` is 1, then `left = 0` and `right = 0`. The while condition
`left < right` is immediately false. The function returns `max_area`, which is
still 0. This is correct: you need at least two lines to form a container.

---

## Complexity Analysis

**Time: O(n)**

Each iteration of the while loop advances either `left` forward or `right`
backward by exactly one position. The two pointers start a total distance of
`n - 1` apart and meet when `left == right`. Therefore the loop runs at most
`n - 1` times. Each iteration does O(1) work (one comparison, one
multiplication, one conditional update). Total: O(n).

**Space: O(1)**

The algorithm uses a fixed number of integer variables (`left`, `right`, `area`,
`max_area`) regardless of input size. No auxiliary data structures are allocated.

---

## Summary

The two-pointer technique transforms a naive O(n^2) search over all pairs into a
single linear scan. By starting with the widest possible container and always
discarding the shorter line, we guarantee that no optimal pair is ever skipped.
The key insight is that the shorter line is the bottleneck: pairing it with any
closer line can only yield a smaller width and an equal-or-smaller height, so its
best partnership has already been evaluated. The result is an elegant O(n) time,
O(1) space algorithm that computes the maximum container area in a single pass
through the array.
