# 001 - Two Sum

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

---

## Problem Statement

Given an array of integers `nums` and an integer `target`, return **indices of the two numbers** such that they add up to `target`.

You may assume that each input would have **exactly one solution**, and you may not use the same element twice.

Example:

```
Input:  nums = [2, 7, 11, 15], target = 9
Output: [0, 1]
Explanation: nums[0] + nums[1] = 2 + 7 = 9
```

---

## Core Idea

Imagine you're standing in a room full of people, each person holding a number card. Someone shouts out a target number. You need to find **two people** whose cards add up to that target.

The simplest approach (brute force) is: pick up every person one by one, then look at every other person to check if their cards add up to the target. This is exactly what the code does — **two nested loops**, trying every possible pair.

The outer loop picks one number, the inner loop scans all numbers after it. If a matching pair is found, return their positions immediately.

---

## Step-by-Step Walkthrough

Let's trace through `nums = [3, 2, 3]`, `target = 6`:

```
Step 1: i=0, pick nums[0]=3
        j=1: nums[0]+nums[1] = 3+2 = 5 != 6  -> skip
        j=2: nums[0]+nums[2] = 3+3 = 6 == 6  -> found!
        Return [0, 2]
```

Another example: `nums = [2, 7, 11, 15]`, `target = 9`:

```
Step 1: i=0, pick nums[0]=2
        j=1: nums[0]+nums[1] = 2+7 = 9 == 9  -> found!
        Return [0, 1]
```

One more: `nums = [3, 2, 4]`, `target = 6`:

```
Step 1: i=0, pick nums[0]=3
        j=1: 3+2 = 5 != 6  -> skip
        j=2: 3+4 = 7 != 6  -> skip

Step 2: i=1, pick nums[1]=2
        j=2: 2+4 = 6 == 6  -> found!
        Return [1, 2]
```

---

## Where It Gets Tricky

1. **Don't use the same element twice.** The inner loop starts from `j = i + 1`, not `j = 0`. This ensures we never pair an element with itself.

2. **Early return.** Once a pair is found, we return immediately. The problem guarantees exactly one solution, so there's no need to keep searching.

3. **No solution found path.** If no pair matches (shouldn't happen per the problem constraints), the function returns `[0, 0]` — the memset-initialized default. In production, this edge case should be handled more explicitly.

4. **The variable `k`.** In the code, `k = i` is assigned then used as `nums[k]` and `k + 1`. This is equivalent to using `i` directly. It doesn't introduce a bug, but it's unnecessary indirection.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time** | O(n^2) | Two nested loops, each iterating up to n elements |
| **Space** | O(1) | Only a fixed-size (2-element) array is allocated for the result |

This brute-force approach is the simplest to understand. A more optimal solution uses a **hash table** to achieve O(n) time by storing each number's index as we scan — for each number, we check if `target - nums[i]` already exists in the table.

---

## Summary

Two Sum is the classic "find a pair" problem. The brute-force solution here tries every pair (i, j) with two nested loops and returns the first pair whose sum matches the target. It's O(n^2) time and O(1) space — simple and correct, just not the fastest. The key is to start the inner loop at `j = i + 1` so you never pair an element with itself, and to return immediately once a match is found.
