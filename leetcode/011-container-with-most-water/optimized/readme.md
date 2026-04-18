# 011 - Container With Most Water (Optimized)

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

The original two-pointer solution moves the shorter pointer inward by exactly
one position each iteration. That works, but think about what happens when the
next line over is the same height or shorter. The width just shrank by one, and
the minimum height stayed the same or got worse. The area *must* be smaller.
We computed it for nothing.

The optimized version notices this and refuses to do pointless work. After
picking the shorter side and computing its area, it runs an inner `while` loop
that skips forward past every consecutive line on that side whose height is `<=`
the current `h`. It only stops when it finds a strictly taller line -- the first
line that could possibly produce a larger area.

This has two concrete payoffs:

- **Fewer outer-loop iterations.** Each outer iteration now covers a whole
  plateau of equal-or-shorter lines, not just one line at a time.
- **Fewer area computations.** We only compute `h * (right - left)` once per
  unique height level we actually land on, instead of once per single step.

There is also a small structural win: the original uses a `MIN(a, b)` macro to
figure out the limiting height. The optimized version does not need it. Because
the `if/else` branch already tells us which side is shorter, we just set
`h = height[left]` or `h = height[right]` directly. The shorter side *is* the
min. No comparison required.

Both versions are O(n) in the worst case -- each pointer can move at most n
positions total regardless of skipping. But in practice the optimized version
burns through far fewer iterations on inputs with repeated or gradually
changing heights.

---

## Core Idea

Same two-pointer setup as the original: `left` starts at 0, `right` starts at
the end, and they walk toward each other.

The twist is what happens after we compute the area for the shorter side.
Instead of nudging that pointer by one, we enter an inner `while` loop:

```
while (left < right && height[left] <= h)
    left++;
```

(or the mirror for the right side). This loop eats through every line that is
no taller than `h`. It only breaks when it finds a line strictly taller than
`h`, or when the pointers meet.

Why is this safe? Consider any position we skip. Its height is `<= h`. The
width is strictly less than it was when we computed the area for `h` (because
we moved inward). So the area at any skipped position is `<= h * (something
smaller)`, which is `<= area we already recorded`. We cannot miss the maximum
by jumping over these positions.

The key insight, stated as simply as possible: *if the height did not increase,
the area cannot increase, because the width strictly decreased.* So skip
forward until the height actually increases.

---

## Function Reference

| Function | Signature | Purpose |
|---|---|---|
| `maxArea` | `int maxArea(int *height, int heightSize)` | Two-pointer with skip optimization. After computing area for the shorter side, skips all lines <= current height. |
| `print_array` | `static void print_array(const char *label, int *array, int len)` | Prints array contents with a label prefix in bracket notation. |
| `run_test` | `static void run_test(const char *name, int *height, int len, int expected)` | Runs a single test case: prints input, calls `maxArea`, asserts result matches expected. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs 10 test cases covering edge cases and typical inputs. |

---

## ASCII Flowchart

```
                        +-------+
                        | START |
                        +---+---+
                            |
                            v
                    +---------------+
                    | left < right? |
                    +-------+-------+
                       /         \
                     no           yes
                     /             \
                    v               v
             +-----------+   +---------------------+
             | return    |   | h[left] < h[right]? |
             | max_area  |   +---------+-----------+
             +-----------+        /           \
                                yes            no
                               /                \
                              v                  v
                  +------------------+   +------------------+
                  | h = h[left]      |   | h = h[right]     |
                  | area=h*(right-   |   | area=h*(right-   |
                  |       left)      |   |       left)      |
                  | update max_area  |   | update max_area  |
                  +--------+---------+   +--------+---------+
                           |                      |
                           v                      v
                  +------------------+   +------------------+
                  | skip while       |   | skip while       |
                  | left < right &&  |   | left < right &&  |
                  | h[left] <= h     |   | h[right] <= h    |
                  |   left++         |   |   right--        |
                  +--------+---------+   +--------+---------+
                           |                      |
                           +----------+-----------+
                                      |
                                      v
                              (loop back to top)
```

---

## Step-by-Step Walkthrough

Input: `[1, 8, 6, 2, 5, 4, 8, 3, 7]` (indices 0 through 8).

**Iteration 1:**

```
left=0 (h=1)   right=8 (h=7)
height[left]=1 < height[right]=7, so take the left branch.
h = height[0] = 1
area = 1 * (8 - 0) = 8
max_area = 8

Skip loop: height[0]=1 <= 1, so left++. Now left=1.
           height[1]=8 > 1, stop.
```

Left jumped from 0 to 1 in one skip step. Nothing dramatic yet.

**Iteration 2:**

```
left=1 (h=8)   right=8 (h=7)
height[left]=8 >= height[right]=7, so take the right branch.
h = height[8] = 7
area = 7 * (8 - 1) = 49
max_area = 49

Skip loop: height[8]=7 <= 7, so right--. Now right=7.
           height[7]=3 <= 7, so right--. Now right=6.
           height[6]=8 > 7, stop.
```

Right jumped from 8 to 6, skipping indices 8 and 7 (heights 7 and 3). Two
positions consumed in one outer iteration.

**Iteration 3:**

```
left=1 (h=8)   right=6 (h=8)
height[left]=8 >= height[right]=8, so take the right branch.
h = height[6] = 8
area = 8 * (6 - 1) = 40
max_area = 49  (no update, 40 < 49)

Skip loop: height[6]=8 <= 8, so right--. Now right=5.
           height[5]=4 <= 8, so right--. Now right=4.
           height[4]=5 <= 8, so right--. Now right=3.
           height[3]=2 <= 8, so right--. Now right=2.
           height[2]=6 <= 8, so right--. Now right=1.
           Now left=1 == right=1, condition left < right fails. Stop.
```

Right plowed through indices 6, 5, 4, 3, 2 all the way down to 1. Five
positions consumed in one outer iteration.

**Outer loop check:** `left=1`, `right=1`. `left < right` is false. Exit loop.

**Return 49.**

The original algorithm would have taken 8 outer iterations (one `left++` or
`right--` per step) to walk the pointers together. The optimized version did it
in **3 outer iterations**. Same answer, less busywork.

---

## Where It Gets Tricky

**(a) The inner skip loop must check `left < right`.**

Without that guard, the skip loop could drive `left` past `right` (or `right`
below `left`). The pointers would cross, and we would read garbage positions or
loop forever. Every inner `while` starts with `left < right &&` before checking
the height condition.

**(b) The skip uses `<=`, not `<`.**

This is subtle. If the next line is the *exact same* height as `h`, should we
skip it? Yes. The width decreased by at least one, the min height is at best
the same, so the area is strictly less. Equal-height lines are dead weight --
skip them. Using `<` instead of `<=` would cause us to stop on a same-height
line, compute an area we already know is worse, and waste an outer iteration.

**(c) No MIN macro needed.**

The original computes `MIN(height[left], height[right])` every iteration
because it does not know which side is shorter until it evaluates the macro.
The optimized version already branches on `height[left] < height[right]`, so
inside each branch we know exactly which side is shorter. We just set `h` to
that side's height. The `h` variable *is* the min. This eliminates one
conditional evaluation per iteration.

**(d) Correctness argument: we cannot skip the optimal pair.**

Suppose we skip position `i` on the left side because `height[i] <= h`. Could
position `i` have been part of the maximum-area pair? For any right pointer `r`
currently in play, the area using position `i` would be
`min(height[i], height[r]) * (r - i)`. Since `height[i] <= h` and `r - i <
right - left` (we moved inward), this area is at most `h * (right - left)`,
which is exactly the area we already computed and compared against `max_area`.
So skipping `i` is safe -- it cannot beat what we already recorded.

---

## Complexity Analysis

| Metric | Value |
|---|---|
| Time complexity | O(n) worst case. Each pointer moves at most n positions total across all iterations. The inner skip loops do not add extra traversals -- they just batch pointer movement that the original would have done one-at-a-time. |
| Space complexity | O(1). Only a fixed number of integer variables (`left`, `right`, `h`, `max_area`) regardless of input size. |

---

## Comparison with Original

| Aspect | Original | Optimized |
|---|---|---|
| Pointer movement | +1 per iteration | Skip all lines <= current h |
| Area computations | One per pointer step | One per unique height level |
| MIN macro | Yes | No (h known from branch) |
| Worst-case time | O(n) | O(n) |
| Best-case iterations | n-1 | Potentially much fewer |
| Space | O(1) | O(1) |

---

## Summary

The optimized version keeps the same two-pointer skeleton but adds an inner
skip loop after each area computation. Once we know the shorter side has height
`h`, we jump past every line on that side with height `<= h`, because none of
them can produce a larger area (the width shrank and the min height did not
grow). This batches what the original does one step at a time into a single
leap. It also eliminates the MIN macro since the `if/else` branch already
identifies the shorter side. The worst-case complexity stays O(n) -- both
versions move each pointer at most n positions total -- but the optimized
version completes in fewer outer iterations whenever the input contains runs of
equal or decreasing heights, which is most real inputs.
