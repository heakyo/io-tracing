# 015 - 3Sum

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

Given an integer array `nums`, return all unique triplets
`[nums[i], nums[j], nums[k]]` such that `i != j`, `i != k`, `j != k`, and
`nums[i] + nums[j] + nums[k] == 0`.

The solution set must not contain duplicate triplets. Two triplets are
considered duplicates if they contain the same three values in any order.

**Example**

Given `nums = [-1, 0, 1, 2, -1, -4]`:

The output should be `[[-1, -1, 2], [-1, 0, 1]]`.

Note that `[-1, 0, 1]` and `[0, 1, -1]` are the same triplet, so only one
appears. The order of the triplets in the output does not matter.

---

## Core Idea

The approach taken here is creative but unconventional. Rather than the
textbook method of fixing one element and sweeping the remaining two with a
two-pointer inward march, this algorithm treats the *outer* two elements as
the primary pair and hunts for a *middle* element between them.

Here is the mental model, step by step:

1. **Sort the array.** This is the standard first move -- it arranges the
   elements from most negative on the left to most positive on the right.

2. **Initialize two outer pointers.** `left` starts at index 0 (the smallest
   element) and `right` starts at index `n - 1` (the largest). These two
   pointers form the "bracket" of the current search window.

3. **Compute `val = nums[left] + nums[right]`.** This is the sum of the outer
   pair. For the triplet to sum to zero, we need a middle element `nums[k]`
   such that `nums[k] == -val`.

4. **Search for the middle element based on the sign of `val`:**

   - **`val < 0`** -- the outer pair is net negative, so we need a positive
     middle element to compensate. The code searches *right-to-left* starting
     from `k = right - 1`, stepping through elements while `nums[k] > 0`.

   - **`val > 0`** -- the outer pair is net positive, so we need a negative
     middle element. The code searches *left-to-right* starting from
     `k = left + 1`, stepping through elements while `nums[k] < 0`.

   - **`val == 0`** -- we need a middle element that is exactly zero. The code
     scans from `left + 1` to `right - 1` looking for a zero.

5. **Record the triplet** if a matching `nums[k]` is found.

6. **Advance one of the outer pointers** using heuristic logic. After the
   inner search finishes, the code examines the neighboring element to decide
   whether to move `left` forward or `right` backward. The goal is to skip
   past duplicate values and converge the window, but the decision is based on
   ad-hoc conditions rather than an exhaustive guarantee.

7. **Repeat** until `left >= right`.

Think of it like holding a rubber band at both ends (the outer pointers) and
asking: "Is there a bead somewhere in the middle that balances the tension?"
The algorithm reaches in from the appropriate end to look for that bead, then
shifts one hand inward and asks again.

---

## Step-by-Step Walkthrough

Input: `nums = [-1, 0, 1, 2, -1, -4]`

After sorting: `[-4, -1, -1, 0, 1, 2]`

| Index | 0  | 1  | 2  | 3 | 4 | 5 |
|-------|----|----|----|---|---|---|
| Value | -4 | -1 | -1 | 0 | 1 | 2 |

Initial state: `left = 0`, `right = 5`.

---

**Iteration 1** -- `left=0, right=5`

- `val = nums[0] + nums[5] = -4 + 2 = -2` (negative)
- Search right-to-left from `k=4` while `nums[k] > 0`:
  - `k=4`: `nums[4]=1 > 0`, check `val + 1 = -1 != 0`, `k--`
  - `k=3`: `nums[3]=0`, not `> 0` -- loop exits
- `found = 0` -- no triplet.
- Pointer advance (`val < 0`): `++k` gives `k=4`, `val + nums[4] = -2 + 1 = -1`,
  not `> 0`, so take the else branch: save `k = left = 0`, skip while
  `nums[left] == nums[0]` (-4): `left` advances to 1.
- **State:** `left=1, right=5`

---

**Iteration 2** -- `left=1, right=5`

- `val = nums[1] + nums[5] = -1 + 2 = 1` (positive)
- Search left-to-right from `k=2` while `nums[k] < 0`:
  - `k=2`: `nums[2]=-1 < 0`, check `val + (-1) = 0` -- **match!**
- `found = 1` -- record triplet: **[-1, -1, 2]**
- Pointer advance (`val > 0`): `--k` gives `k=1`, `val + nums[1] = 1 + (-1) = 0`,
  not `< 0`, so take the else branch: save `k = right = 5`, skip while
  `nums[right] == nums[5]` (2): `right` retreats to 4.
- **State:** `left=1, right=4`

---

**Iteration 3** -- `left=1, right=4`

- `val = nums[1] + nums[4] = -1 + 1 = 0`
- Search for a zero between indices 2 and 3:
  - `k=2`: `nums[2]=-1 != 0`
  - `k=3`: `nums[3]=0 == 0` -- **match!**
- `found = 1` -- record triplet: **[-1, 0, 1]**
- Pointer advance (`val == 0`): `k=3`, `(right - left)/2 = (4-1)/2 = 1`,
  since `k=3 >= 1`, take the else branch: save `k = left = 1`, skip while
  `nums[left] == nums[1]` (-1): `left` advances past index 2 (also -1) to 3.
- **State:** `left=3, right=4`

---

**Iteration 4** -- `left=3, right=4`

- `val = nums[3] + nums[4] = 0 + 1 = 1` (positive)
- Search left-to-right from `k=4` while `nums[k] < 0`:
  - `k=4`: `nums[4]=1`, not `< 0` -- loop exits immediately
- `found = 0` -- no triplet.
- Pointer advance (`val > 0`): `--k` gives `k=3`, `val + nums[3] = 1 + 0 = 1`,
  not `< 0`, so take the else branch: save `k = right = 4`, skip while
  `nums[right] == nums[4]` (1): `right` retreats to 3.
- **State:** `left=3, right=3` -- `left < right` is false, exit loop.

---

**Final output: `[[-1, -1, 2], [-1, 0, 1]]`** -- matches the expected result
for this particular input.

---

## ASCII Flowchart

```
                         +---------------------------+
                         |           START           |
                         +---------------------------+
                                     |
                                     v
                         +---------------------------+
                         |   qsort(nums, numsSize)   |
                         +---------------------------+
                                     |
                                     v
                         +---------------------------+
                         | left = 0                  |
                         | right = numsSize - 1      |
                         | returnSize = 0            |
                         +---------------------------+
                                     |
                                     v
                         +---------------------------+
                         |     left < right ?        |
                         +---------------------------+
                          /                         \
                        yes                          no
                        /                              \
                       v                                v
          +---------------------+              +----------------+
          |  val = nums[left]   |              | return output  |
          |      + nums[right]  |              +----------------+
          +---------------------+
                    |
                    v
          +---------------------+
          |     val < 0 ?       |----yes---+
          +---------------------+          |
                    |                      v
                   no            +-------------------------+
                    |            | k = right - 1           |
                    v            | search right-to-left    |
          +---------------------+| while nums[k] > 0:     |
          |     val > 0 ?       || if val+nums[k]==0 FOUND|
          +---------------------+| k--                    |
                    |            +-------------------------+
                   no                      |
                    |                      |
                    v                      |
          +---------------------+          |
          | val == 0            |          |
          | scan left+1..right-1|          |
          | for nums[k] == 0   |          |
          +---------------------+          |
                    |                      |
                    |     +----------------+
                    |     |
          +---------+     |     +-------------------------+
          |               |     | k = left + 1            |
          |               |     | search left-to-right    |
          |               |     | while nums[k] < 0:      |
          |               |     | if val+nums[k]==0 FOUND |
          |               |     | k++                     |
          |               |     +-------------------------+
          |               |                |
          v               v                v
          +-----------------------------------+
          |            found ?                |
          +-----------------------------------+
               /                     \
             yes                      no
             /                          \
            v                            |
  +---------------------------+          |
  | record triplet:           |          |
  |   [nums[left],            |          |
  |    nums[k],               |          |
  |    nums[right]]           |          |
  | returnSize++              |          |
  +---------------------------+          |
            \                           /
             \                         /
              v                       v
       +-------------------------------+
       |  advance pointer heuristic:   |
       |  based on val sign and        |
       |  neighboring element, move    |
       |  left++ or right-- (skipping  |
       |  duplicates)                  |
       +-------------------------------+
                      |
                      v
              (loop back to
               left < right ?)
```

---

## Where It Gets Tricky

### (a) Inner search is restricted by sign

When `val < 0`, the inner search loop runs `while (nums[k] > 0)`. It *only*
examines positive elements. Symmetrically, when `val > 0`, the loop runs
`while (nums[k] < 0)`, examining only negative elements.

This seems logical at first glance: if the outer pair sums to a negative
number, you need a positive number to cancel it out, right? Not necessarily.
The target value is `-val`, and while `-val` will indeed be positive when
`val < 0`, the loop termination condition is `nums[k] > 0`, not
`nums[k] == -val`. The loop *gives up* the moment it encounters a zero or
negative element, even if the correct `nums[k]` is further along.

In practice this works for many cases because the array is sorted and the
search sweeps from the correct end. But the sign-based guard is overly
restrictive: it stops the search early if there are zeros between the positive
elements and the outer pointer.

### (b) Pointer advancement is heuristic-based

This is the most serious bug. After finding (or not finding) a triplet, the
algorithm must decide which outer pointer to advance. In the textbook
two-pointer approach, the answer is simple: if the sum is too small, move
`left` forward; if too large, move `right` backward. But here, the pointers
are the *outer* pair, and the logic for advancing them is based on ad-hoc
conditions that inspect neighboring elements.

Consider the input `[-3, -1, 0, 1, 2, 4]` (already sorted). Here is what
happens:

| Iter | left | right | val | Inner search          | Found            | Advance                |
|------|------|-------|-----|-----------------------|------------------|------------------------|
| 1    | 0    | 5     | 1   | k=1: -1+1=0, match   | [-3, -1, 4]      | --k=0, val+nums[0]=-2<0 so left++ (skip -3) |
| 2    | 1    | 5     | 3   | k=2: nums[2]=0, exit  | none             | --k=1, val+nums[1]=2, not<0, so right-- |
| 3    | 1    | 4     | 1   | k=2: nums[2]=0, exit  | none             | --k=1, val+nums[1]=0, not<0, so right-- |
| 4    | 1    | 3     | 0   | k=2: nums[2]=0, match | [-1, 0, 1]       | k=2>=(3-1)/2=1, so left++ |
| 5    | 2    | 3     | 1   | k=3: nums[3]=1, exit  | none             | --k=2, val+nums[2]=1, not<0, so right-- |

**left=2, right=2 -- loop exits.**

Output: `[[-3, -1, 4], [-1, 0, 1]]`

Expected: `[[-3, -1, 4], [-3, 1, 2], [-1, 0, 1]]`

**Missing: `[-3, 1, 2]`.**

After iteration 1, the heuristic decides to advance `left` past -3 because
`val + nums[k-1]` happens to be negative. At that point, `left=0` (value -3)
is discarded forever. The pair `(left=0, right=4)` -- that is, `(-3, 2)` with
`val = -1` -- is never examined. The triplet `[-3, 1, 2]` requires `left`
to still be sitting on -3, but it has already been moved.

The fundamental issue is that a single `left` value can participate in
*multiple* valid triplets with different `right` values. The heuristic moves
`left` forward after finding *one* triplet, skipping all other `right` values
that could pair with that same `left`.

### (c) Early return inconsistency

The early-exit check at the top of the function is:

```c
if (nums[0] > 0 || nums[numsSize - 1] < 0) {
	*returnSize = 0;
	return NULL;
}
```

This returns `NULL` when no triplet is possible (all positive or all
negative). But the normal code path returns the `malloc`'d `output` array,
even when `returnSize` is 0. A caller that unconditionally calls `free()` on
the return value -- as the LeetCode contract suggests -- will crash with a
NULL pointer dereference on the early-return path, or at least behave
differently. The two paths should be consistent: either always return a valid
(possibly empty) allocated array, or always document that NULL means "no
results."

There is also a minor memory leak: `*returnColumnSizes` is allocated *twice*
when the early return is taken -- once before the check and once inside the
`if` block.

### (d) Missing bounds check on inner k search

The inner search loops rely on the sort order to self-terminate:

```c
k = right - 1;
while (nums[k] > 0) {
	...
	k--;
}
```

There is no explicit guard like `k > left`. If the array contains all positive
values from `left+1` through `right-1`, this loop will decrement `k` past
`left` and into the outer pointer's position, or even out of bounds. Granted,
the early return for all-positive arrays prevents the most obvious crash, but
there are mixed arrays where `k` can reach `left` itself, causing the same
element to be used twice in a triplet (violating `i != j != k`).

The `val > 0` branch has the same issue in the opposite direction: `k++` with
no upper bound check could reach `right`.

---

## Complexity Analysis

**Time: O(n^2) worst case**

The outer `while` loop runs at most O(n) iterations because each iteration
advances either `left` forward or `right` backward by at least one position,
and they start `n - 1` apart. Each inner search (the `while (nums[k] > 0)` or
`while (nums[k] < 0)` loop) can scan up to O(n) elements in the worst case.
Multiplying the two gives O(n^2).

In practice the inner searches tend to be short because the sign-based
termination stops them early, but the worst case is still quadratic.

**Space: O(1) extra** (ignoring output)

The algorithm uses a fixed number of integer variables (`left`, `right`, `k`,
`val`, `found`) regardless of input size. The output array and column sizes
array are allocated for the caller and are not counted as auxiliary space.

**Note:** Despite having reasonable time complexity, the algorithm does not
produce correct results for all inputs. An O(n^2) algorithm that actually
works (the standard fix-one-element + two-pointer approach) is strictly
preferable.

---

## Summary

The approach of fixing two outer elements and searching for a balancing middle
element is a creative way to think about 3Sum. It has an appealing symmetry:
the outer pointers form a bracket, and you reach inward to find the missing
piece.

Unfortunately, the implementation has fundamental correctness issues. The inner
search is gated by the sign of the candidate element, which can cause it to
stop prematurely. More critically, the heuristic pointer advancement logic
decides which outer pointer to move based on local conditions, and this causes
entire families of valid `(left, right)` pairs to be skipped. The result is
that triplets are silently missed -- as demonstrated by the input
`[-3, -1, 0, 1, 2, 4]` where `[-3, 1, 2]` is never found.

The standard approach to 3Sum avoids all of these pitfalls: sort the array,
then for each element `nums[i]`, use two pointers starting at `i+1` and
`n-1` to find pairs that sum to `-nums[i]`. Duplicate skipping is
straightforward (skip `nums[i]` if it equals `nums[i-1]`, and skip duplicate
pairs after a match). This gives a clean O(n^2) algorithm with a simple
correctness argument -- each `nums[i]` is exhaustively paired with all valid
`(left, right)` combinations, so nothing is missed.
