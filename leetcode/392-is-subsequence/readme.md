# 392 - Is Subsequence

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

## Problem Statement

LeetCode 392: Given two strings `s` and `t`, return `true` if `s` is a
subsequence of `t`, or `false` otherwise.

A subsequence is formed by deleting some (or no) characters from `t` without
changing the relative order of the remaining characters. In other words, you
can strike out characters from `t` and if what remains can spell `s` (in
order), then `s` is a subsequence of `t`.

| Example | `s`   | `t`        | Result  | Reason                                            |
|---------|-------|------------|---------|---------------------------------------------------|
| 1       | "abc" | "ahbgdc"   | `true`  | a...b...c can be found in order within "ahbgdc"   |
| 2       | "axc" | "ahbgdc"   | `false` | 'x' does not appear in "ahbgdc" after 'a'        |
| 3       | ""    | "ahbgdc"   | `true`  | The empty string is a subsequence of everything   |

## Core Idea

The algorithm uses two pointers, `p1` and `p2`. Think of `p1` as a finger
pointing at the character in `s` we are currently trying to match, and `p2` as
a finger scanning through `t` looking for that character.

For each character `*p1` in `s`, we sweep `p2` forward through `t` until we
either find a match or run out of characters. If we find it, we advance `p1`
to the next character of `s` and keep `p2` right where it is (one past the
match, thanks to the post-increment). If `t` runs out before we find a match,
we know `s` cannot be a subsequence, so we return `false`. If we get through
every character of `s` without failing, we return `true`.

The key insight: `p2` is never reset. It retains its position between
iterations of the outer loop. This is exactly what enforces the ordering
constraint -- each successive character of `s` must be found *after* (not at
or before) the position where the previous character was matched. Resetting
`p2` to the start of `t` each time would allow out-of-order matches and break
correctness.

## Step-by-Step Walkthrough

### Example 1: s = "abc", t = "ahbgdc" -- result: `true`

**Outer iteration 1** -- `*p1 = 'a'`, `p2` points to `t[0]` ('a')

	Inner loop: compare *p2 ('a') with *p1 ('a'). Match!
	p2 advances past the match (post-increment) to t[1] ('h').
	found = true, break out of inner loop.
	p1 advances to s[1] ('b').

**Outer iteration 2** -- `*p1 = 'b'`, `p2` points to `t[1]` ('h')

	Inner loop: compare *p2 ('h') with *p1 ('b'). No match. p2 -> t[2].
	Inner loop: compare *p2 ('b') with *p1 ('b'). Match!
	p2 advances to t[3] ('g').
	found = true, break out of inner loop.
	p1 advances to s[2] ('c').

**Outer iteration 3** -- `*p1 = 'c'`, `p2` points to `t[3]` ('g')

	Inner loop: compare *p2 ('g') with *p1 ('c'). No match. p2 -> t[4].
	Inner loop: compare *p2 ('d') with *p1 ('c'). No match. p2 -> t[5].
	Inner loop: compare *p2 ('c') with *p1 ('c'). Match!
	p2 advances to t[6] ('\0').
	found = true, break out of inner loop.
	p1 advances to s[3] ('\0').

Outer `while (*p1)` check: `*p1` is `'\0'`, so we exit. Return `true`.

### Example 2: s = "axc", t = "ahbgdc" -- result: `false`

**Outer iteration 1** -- `*p1 = 'a'`, `p2` points to `t[0]` ('a')

	Inner loop: compare *p2 ('a') with *p1 ('a'). Match!
	p2 advances to t[1] ('h').
	found = true, break out of inner loop.
	p1 advances to s[1] ('x').

**Outer iteration 2** -- `*p1 = 'x'`, `p2` points to `t[1]` ('h')

	Inner loop: compare 'h' with 'x'. No match. p2 -> t[2].
	Inner loop: compare 'b' with 'x'. No match. p2 -> t[3].
	Inner loop: compare 'g' with 'x'. No match. p2 -> t[4].
	Inner loop: compare 'd' with 'x'. No match. p2 -> t[5].
	Inner loop: compare 'c' with 'x'. No match. p2 -> t[6].
	Inner loop: *p2 is '\0', exit inner while.
	found is still false.
	Check: !found is true. Return false.

We never reached 'c' in `s` because we exhausted all of `t` trying to find
'x'. The algorithm correctly reports that "axc" is not a subsequence of
"ahbgdc".

## ASCII Flowchart

```
                    +-------+
                    | START |
                    +---+---+
                        |
                        v
                +-------+--------+
                | p1 = s, p2 = t |
                +-------+--------+
                        |
                        v
                  +-----+------+
             +--->| *p1 != 0 ? |
             |    +-----+------+
             |      yes |    | no
             |          v    +----------> RETURN TRUE
             |   +------+-------+
             |   | found = false |
             |   +------+-------+
             |          |
             |          v
             |    +-----+------+
             | +->| *p2 != 0 ? |
             | |  +-----+------+
             | |    yes |    | no
             | |        v    +----+
             | | +------+------+  |
             | | | *p2++ == *p1 ? |  |
             | | +---+------+--+  |
             | |  yes |      | no |
             | |      v      |    |
             | | +----+----+ |    |
             | | | found =  | |    |
             | | | true     | |    |
             | | +----+----+ |    |
             | |      |      |    |
             | |      v      |    |
             | | +----+----+ |    |
             | | |  break  | |    |
             | | +----+----+ |    |
             | |      |      |    |
             | |      |   +--+    |
             | |      |   |       |
             | +------+---+       |
             |        |           |
             |        +<----------+
             |        |
             |  +-----+------+
             |  | !found ?   |
             |  +-----+------+
             |    yes |    | no
             |        v    |
             |  +-----+------+
             |  |RETURN FALSE|
             |  +------------+
             |             |
             |         +---+---+
             |         | p1++  |
             |         +---+---+
             |             |
             +-------------+
```

## Where It Gets Tricky

### (a) The post-increment `*p2++`

The expression `*p2++` dereferences `p2` and then advances it, regardless of
whether the comparison succeeded. This means that when we find a match and
`break`, `p2` is already pointing one past the matched character. That is
exactly correct: the next character of `s` must appear strictly after the
current match in `t`. If we used a pre-increment or advanced `p2` only on
mismatch, we would risk matching the same position in `t` twice for two
different characters of `s`.

### (b) The `found` flag

The inner `while (*p2)` loop can exit for two reasons: either we hit a match
and executed `break`, or `p2` reached the null terminator of `t`. In both
cases, control falls to the same point after the inner loop. Without the
`found` flag, we would have no way to distinguish "we found the character" from
"we exhausted `t` without finding it." The flag makes the exit reason explicit,
so the subsequent `if (!found) return false;` check works correctly.

### (c) `p2` is never reset

Across all iterations of the outer loop, `p2` only moves forward through `t`.
It is initialized once to point at `t[0]` and is never set back. This is the
mechanism that preserves the relative ordering constraint. If `p2` were reset
to `t` at the start of each outer iteration, you could match characters of `s`
out of order -- for instance, matching 'c' before 'a' -- which would violate
the definition of a subsequence.

## Complexity Analysis

**Time: O(|s| + |t|)**

Each character of `t` is visited at most once by `p2` across all iterations of
the outer loop, because `p2` never goes backward. The outer loop runs at most
`|s|` times. Therefore, the total work is bounded by `|s| + |t|`.

**Space: O(1)**

The algorithm uses only two pointers (`p1`, `p2`) and a boolean (`found`).
No auxiliary data structures are allocated.

## Summary

This algorithm determines whether `s` is a subsequence of `t` using a simple
two-pointer scan. Pointer `p1` steps through each character of `s` one at a
time, and for each one, pointer `p2` sweeps forward through `t` looking for a
match. Because `p2` never resets -- it remembers where the last match occurred
-- the relative ordering of characters is automatically preserved. The
post-increment on `p2` ensures that no position in `t` is matched twice, and
the `found` flag disambiguates the two exit conditions of the inner loop. The
result is a clean O(|s| + |t|) time, O(1) space solution that touches each
character of each string at most once.
