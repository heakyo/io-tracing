# 392 - Is Subsequence (Optimized)

## Table of Contents

- [Why Optimize?](#why-optimize)
- [Core Idea](#core-idea)
- [Function Reference](#function-reference)
- [ASCII Flowchart](#ascii-flowchart)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Comparison with Original](#comparison-with-original)
- [Summary](#summary)

## Why Optimize?

The original solution works correctly, but it uses nested loops with a `found` flag,
making the logic more verbose than necessary. The outer `while` iterates over each
character in `s`, and for each one, the inner `while` scans forward through `t` looking
for a match. If no match is found, it returns false immediately. This requires a boolean
`found` flag, a conditional break, an early-return check after each inner loop, and two
explicit pointer variables `p1` and `p2`.

The optimized version inverts the perspective entirely. Instead of asking "for each
character in `s`, scan `t` until we find it", it asks "scan `t` once from start to end,
and whenever `t`'s current character matches `s`'s current character, advance `s`". This
eliminates the `found` flag, the nested loop structure, and the early-return check --
collapsing the entire function body to 3 lines of code.

## Core Idea

Single pass through `t`.

Walk `t` from start to end. Each time `t`'s current character matches `s`'s current
character, advance `s` by one position. After `t` is exhausted, check whether `s` has
been fully consumed: if `!*s` is true (i.e., `s` points to the null terminator), then
every character in `s` was matched in order, so `s` is a subsequence of `t`.

The pointer `s` acts as a progress marker. It only advances on matches, which
automatically preserves the ordering requirement: we never skip a character in `s`, and
we never look backward in `t`.

## Function Reference

| Function | Signature | Purpose |
|---|---|---|
| `isSubsequence` | `bool isSubsequence(char *s, char *t)` | Single-pass scan through `t`; advances `s` pointer on each match. Returns `true` if `s` is fully consumed. |
| `run_test` | `void run_test(const char *name, char *s, char *t, bool expected)` | Runs a single test case, prints input/output, asserts result matches expected. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 12 test cases (identical to original) and prints "All tests passed!" on success. |

## ASCII Flowchart

```
              +-------+
              | START |
              +---+---+
                  |
                  v
          +-------+--------+
          | *t != '\0' ?   |<-----------+
          +---+--------+---+            |
              |        |                |
             yes       no               |
              |        |                |
              v        v                |
      +-------+---+ +--+------------+  |
      | *t == *s? | | return !*s    |  |
      +--+-----+--+ +---------------+  |
         |     |                        |
        yes    no                       |
         |     |                        |
         v     |                        |
      +--+--+  |                        |
      | s++ |  |                        |
      +--+--+  |                        |
         |     |                        |
         +--+--+                        |
            |                           |
            v                           |
        +---+---+                       |
        |  t++  |   (implicit in for)   |
        +---+---+                       |
            |                           |
            +---------------------------+
```

## Step-by-Step Walkthrough

### Example 1: s = "abc", t = "ahbgdc" -- returns true

We walk through every character of `t`. The pointer `s` starts at `'a'`.

| Iteration | `*t` | `*s` | `*t == *s`? | Action | `s` after |
|---|---|---|---|---|---|
| 1 | `'a'` | `'a'` | yes | `s++` | points to `'b'` |
| 2 | `'h'` | `'b'` | no | -- | points to `'b'` |
| 3 | `'b'` | `'b'` | yes | `s++` | points to `'c'` |
| 4 | `'g'` | `'c'` | no | -- | points to `'c'` |
| 5 | `'d'` | `'c'` | no | -- | points to `'c'` |
| 6 | `'c'` | `'c'` | yes | `s++` | points to `'\0'` |

Loop ends (`*t` is `'\0'` after `t++`). Now `*s` is `'\0'`, so `!*s` is `true`.
All three characters of `s` were matched in order. Result: **true**.

### Example 2: s = "axc", t = "ahbgdc" -- returns false

The pointer `s` starts at `'a'`.

| Iteration | `*t` | `*s` | `*t == *s`? | Action | `s` after |
|---|---|---|---|---|---|
| 1 | `'a'` | `'a'` | yes | `s++` | points to `'x'` |
| 2 | `'h'` | `'x'` | no | -- | points to `'x'` |
| 3 | `'b'` | `'x'` | no | -- | points to `'x'` |
| 4 | `'g'` | `'x'` | no | -- | points to `'x'` |
| 5 | `'d'` | `'x'` | no | -- | points to `'x'` |
| 6 | `'c'` | `'x'` | no | -- | points to `'x'` |

Loop ends. Now `*s` is `'x'` (not `'\0'`), so `!*s` is `false`.
The character `'x'` was never found in `t`, so `s` got stuck. Result: **false**.

## Where It Gets Tricky

**(a) s advances only on match.**
The pointer `s` acts as a cursor into the subsequence we are trying to match. It only
moves forward when we find the next needed character in `t`. Because `t` is scanned
left-to-right and `s` only moves forward, the ordering constraint of a subsequence is
automatically maintained -- we never match characters out of order.

**(b) The `!*s` return.**
If `s` points to `'\0'` at the end, it means `s` was advanced past every character in
the original string, so all characters were matched. If `s` does not point to `'\0'`,
some characters remain unmatched, and the answer is false. This single expression
replaces what was a `found` flag and an early-return check in the original.

**(c) Empty s is handled naturally.**
If `s` is the empty string, then `*s` is `'\0'` from the start. The for-loop may or may
not execute (depending on `t`), but it does not matter: `s` never needs to advance.
At the end, `!*s` is `true` immediately, which is correct -- the empty string is a
subsequence of any string.

**(d) No explicit "not found" check.**
The original algorithm needs a `found` flag and an early `return false` after each inner
loop iteration to detect when a character in `s` has no match in `t`. Here, failure is
implicit: if a character in `s` has no match, `s` simply does not advance. At the end,
`!*s` evaluates to `false` because `s` still points to that unmatched character (or a
later one). No special-case logic is needed.

## Complexity Analysis

| Metric | Value | Explanation |
|---|---|---|
| Time | O(\|t\|) | Single pass through `t`. Each character of `t` is examined exactly once. |
| Space | O(1) | No extra data structures. Only the two pointers `s` and `t` are used, and both are parameters modified in place. |

## Comparison with Original

| | Original | Optimized |
|---|---|---|
| Algorithm | Nested two-pointer with found flag | Single for-loop scan |
| Time | O(\|s\| + \|t\|) | O(\|t\|) |
| Space | O(1) | O(1) |
| Loop structure | Nested while/while | Single for |
| Lines of code | 17 (function body) | 4 (function body) |
| Extra variables | p1, p2, found | none (modifies s, t directly) |

## Summary

The optimized solution replaces the original's nested-loop-with-flag approach with a
single linear scan through `t`. By inverting the control flow -- iterating over `t` and
advancing `s` only on a match -- the algorithm eliminates all auxiliary variables and
collapses the logic into a three-line for-loop plus a return statement. The correctness
argument is straightforward: `s` acts as a progress marker that only moves forward on
matches, preserving subsequence ordering, and the final `!*s` check captures whether
every character in `s` was consumed. Edge cases (empty `s`, empty `t`, no match) are all
handled implicitly without any special-case code.
