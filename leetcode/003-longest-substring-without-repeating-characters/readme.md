# 003 - Longest Substring Without Repeating Characters

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

**LeetCode 3:** Given a string `s`, find the length of the longest substring without repeating characters.

For example:

| Input        | Output | Longest Substring |
|--------------|--------|-------------------|
| `"abcabcbb"` | 3      | `"abc"`           |
| `"bbbbb"`    | 1      | `"b"`             |
| `"dvdf"`     | 3      | `"vdf"`           |

---

## Core Idea

Imagine you are reading a string character by character and you want to find the longest stretch where no letter appears twice. The brute-force way is: start at position 0, scan forward until you hit a duplicate, record the length, then move your start to position 1 and do it all over again. That is slow because you are re-examining characters you already looked at.

This algorithm is smarter. It uses **two pointers** (`p1` and `p2`) together with a **256-entry mark array** that tracks two things for every ASCII character:

- `seen` — has this character appeared in the current window?
- `len` — where in the current window (relative to `p1`) did it appear?

For each window starting at `p1`, the inner pointer `p2` scans forward one character at a time. When `p2` lands on a character that has already been seen (`m[*p2].seen`), the inner loop breaks. Then the algorithm checks if the current window (`p2 - p1`) is the longest found so far, and uses the stored position to **jump** `p1` past the first occurrence of the duplicate:

```c
p1 += m[*p2].len + 1;
```

This single line is the key insight. By jumping `p1` directly to one position after the earlier duplicate, the algorithm skips all the starting positions that would inevitably hit the same duplicate again. After the jump, it resets the mark array and starts a fresh scan from the new `p1`.

---

## Step-by-Step Walkthrough

### Example 1: `"abcabcbb"`

```
String:   a   b   c   a   b   c   b   b
Index:    0   1   2   3   4   5   6   7
```

**Pass 1 — p1 = 0 (`'a'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 0: *p2 = 'a'  → not seen, mark it:  m['a'] = {seen:1, len:0}
p2 = 1: *p2 = 'b'  → not seen, mark it:  m['b'] = {seen:1, len:1}
p2 = 2: *p2 = 'c'  → not seen, mark it:  m['c'] = {seen:1, len:2}
p2 = 3: *p2 = 'a'  → m['a'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 3 - 0 = 3
  max = max(0, 3) = 3

  Jump: p1 += m['a'].len + 1 = 0 + 1 = 1
  → p1 is now 1
```

**Pass 2 — p1 = 1 (`'b'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 1: *p2 = 'b'  → mark it:  m['b'] = {seen:1, len:0}
p2 = 2: *p2 = 'c'  → mark it:  m['c'] = {seen:1, len:1}
p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {seen:1, len:2}
p2 = 4: *p2 = 'b'  → m['b'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 4 - 1 = 3, not > max(3), no update

  Jump: p1 += m['b'].len + 1 = 0 + 1 = 1
  → p1 is now 2
```

**Pass 3 — p1 = 2 (`'c'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 2: *p2 = 'c'  → mark it:  m['c'] = {seen:1, len:0}
p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {seen:1, len:1}
p2 = 4: *p2 = 'b'  → mark it:  m['b'] = {seen:1, len:2}
p2 = 5: *p2 = 'c'  → m['c'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 5 - 2 = 3, not > max(3), no update

  Jump: p1 += m['c'].len + 1 = 0 + 1 = 1
  → p1 is now 3
```

**Pass 4 — p1 = 3 (`'a'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {seen:1, len:0}
p2 = 4: *p2 = 'b'  → mark it:  m['b'] = {seen:1, len:1}
p2 = 5: *p2 = 'c'  → mark it:  m['c'] = {seen:1, len:2}
p2 = 6: *p2 = 'b'  → m['b'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 6 - 3 = 3, not > max(3), no update

  Jump: p1 += m['b'].len + 1 = 1 + 1 = 2
  → p1 is now 5
```

**Pass 5 — p1 = 5 (`'c'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 5: *p2 = 'c'  → mark it:  m['c'] = {seen:1, len:0}
p2 = 6: *p2 = 'b'  → mark it:  m['b'] = {seen:1, len:1}
p2 = 7: *p2 = 'b'  → m['b'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 7 - 5 = 2, not > max(3), no update

  Jump: p1 += m['b'].len + 1 = 1 + 1 = 2
  → p1 is now 7
```

**Pass 6 — p1 = 7 (`'b'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 7: *p2 = 'b'  → mark it:  m['b'] = {seen:1, len:0}
p2 = 8: *p2 = '\0' → inner while exits

  Window length: p2 - p1 = 8 - 7 = 1, not > max(3), no update

  p1 += m['\0'].len + 1 = 0 + 1 = 1
  → p1 is now 8, *p1 == '\0', outer while exits
```

**Result: `max = 3`**

---

### Example 2: `"dvdf"`

```
String:   d   v   d   f
Index:    0   1   2   3
```

**Pass 1 — p1 = 0 (`'d'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 0: *p2 = 'd'  → mark it:  m['d'] = {seen:1, len:0}
p2 = 1: *p2 = 'v'  → mark it:  m['v'] = {seen:1, len:1}
p2 = 2: *p2 = 'd'  → m['d'].seen == 1 → DUPLICATE → break

  Window length: p2 - p1 = 2 - 0 = 2
  max = max(0, 2) = 2

  Jump: p1 += m['d'].len + 1 = 0 + 1 = 1
  → p1 is now 1
```

**Pass 2 — p1 = 1 (`'v'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 1: *p2 = 'v'  → mark it:  m['v'] = {seen:1, len:0}
p2 = 2: *p2 = 'd'  → mark it:  m['d'] = {seen:1, len:1}
p2 = 3: *p2 = 'f'  → mark it:  m['f'] = {seen:1, len:2}
p2 = 4: *p2 = '\0' → inner while exits

  Window length: p2 - p1 = 4 - 1 = 3
  max = max(2, 3) = 3

  p1 += m['\0'].len + 1 = 0 + 1 = 1
  → p1 is now 2
```

**Pass 3 — p1 = 2 (`'d'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 2: *p2 = 'd'  → mark it:  m['d'] = {seen:1, len:0}
p2 = 3: *p2 = 'f'  → mark it:  m['f'] = {seen:1, len:1}
p2 = 4: *p2 = '\0' → inner while exits

  Window length: p2 - p1 = 4 - 2 = 2, not > max(3), no update

  p1 += m['\0'].len + 1 = 0 + 1 = 1
  → p1 is now 3
```

**Pass 4 — p1 = 3 (`'f'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 3: *p2 = 'f'  → mark it:  m['f'] = {seen:1, len:0}
p2 = 4: *p2 = '\0' → inner while exits

  Window length: p2 - p1 = 4 - 3 = 1, not > max(3), no update

  p1 += m['\0'].len + 1 = 0 + 1 = 1
  → p1 is now 4, *p1 == '\0', outer while exits
```

**Result: `max = 3`** (the substring `"vdf"`)

> **Note:** Unlike the previous version of this code, the current algorithm does not break out of the outer loop early when `p2` reaches the end of string. It continues checking remaining positions until `*p1 == '\0'`. This is simpler code at the cost of a few extra (harmless) iterations.

---

## ASCII Flowchart

```
                    +------------------+
                    |     START        |
                    |  max = 0         |
                    |  p1 = s          |
                    +--------+---------+
                             |
                             v
                   +-------------------+
               +-->| *p1 != '\0' ?     |--no--> return max
               |   +-------------------+
               |            | yes
               |            v
               |   +-------------------+
               |   | memset(m, 0)      |
               |   | p2 = p1           |
               |   +--------+----------+
               |            |
               |            v
               |   +-------------------+
               |   | *p2 != '\0' ?     |--no--+
               |   +-------------------+      |
               |            | yes             |
               |            v                 |
               |   +-------------------+      |
               |   | m[*p2].seen ?     |      |
               |   +-------------------+      |
               |    yes /       \ no          |
               |       v         v            |
               |  +--------+ +------------+   |
               |  | break  | | mark seen  |   |
               |  +---+----+ | store len  |   |
               |      |      | p2++       |   |
               |      |      +-----+------+   |
               |      |            |          |
               |      |            +-->(loop) |
               |      |                       |
               |      +----------+------------+
               |                 |
               |                 v
               |      +-------------------+
               |      | p2 - p1 > max ?   |
               |      +-------------------+
               |       yes /       \ no
               |          v         |
               |   +------------+   |
               |   | max =      |   |
               |   | p2 - p1    |   |
               |   +-----+------+   |
               |         |          |
               |         v          v
               |      +-------------------+
               |      | p1 += m[*p2].len  |
               |      |        + 1        |
               |      +--------+----------+
               |               |
               +---------------+
```

---

## Where It Gets Tricky

### (a) The `memset` resets the entire 256-entry array every pass

Every time the outer loop starts a new window (a new `p1` position), it calls:

```c
memset(m, 0x0, sizeof(m));
```

This wipes the entire 256-entry mark array — roughly 2 KB of data — even though only a handful of entries were actually used. This is a constant-time operation (256 entries is fixed), so it does not hurt asymptotic complexity, but it is worth noticing because a more refined implementation might only clear the entries that were actually set. The upside is simplicity: you never have stale data from a previous pass leaking into the current one.

### (b) The `len`-based jump is the key optimization

The line:

```c
p1 += m[*p2].len + 1;
```

is what separates this from a naive O(n^2) approach. When a duplicate character is detected at `p2`, `m[*p2].len` records the position (relative to `p1`) where that character was first seen in the current window. Adding 1 jumps `p1` to **one position past** that first occurrence. Every starting position between the old `p1` and the new `p1` would have also contained that same duplicate, so they are guaranteed to produce shorter or equal windows. Skipping them is safe.

### (c) Unified max update after the inner loop

```c
if (p2 - p1 > max)
    max = p2 - p1;
```

The current code updates `max` in one place — after the inner loop exits, regardless of whether it exited due to a duplicate or end-of-string. This is cleaner than the previous version which had separate max updates inside the duplicate check and inside the end-of-string check. One update point means fewer chances for bugs.

### (d) No early exit from the outer loop

When `p2` reaches `'\0'` (end of string), the code does not `break` out of the outer loop. Instead, it falls through to `p1 += m[*p2].len + 1`. Since `'\0'` was never marked, `m[0].len` is 0 (from memset), so `p1` advances by 1. The outer loop continues until `*p1 == '\0'`. This trades a few extra (harmless) iterations for simpler code — no special-case branch for end-of-string.

---

## Complexity Analysis

### Time: O(n) amortized

At first glance this might look like O(n^2) because of the nested loops, but look at what `p1` does: it **only moves forward**, and it often jumps forward by more than one position. Each character in the string is visited by `p2` at most a constant number of times across all passes, because when `p1` jumps forward, the next inner scan starts from the new `p1`, not from the beginning. The total work done by `p2` across all iterations of the outer loop is proportional to `n`. The `memset` call is O(256) = O(1) per outer iteration, and the number of outer iterations is at most `n`. So the total time is **O(n)** amortized.

### Space: O(1)

The mark array `m[256]` has a fixed size of 256 entries (one per possible ASCII byte value), regardless of the input string length. The only other variables are two pointers and an integer. Total extra space: **O(1)**.

---

## Summary

This algorithm finds the longest substring without repeating characters by sliding two pointers across the string. The clever part is the **mark array**: it records not just *whether* a character has been seen (`seen`), but *where* in the current window it was seen (`len`). When a duplicate is detected, the algorithm uses that stored position to jump the left pointer directly past the first occurrence of the duplicate, skipping all the starting positions that would be doomed to produce shorter results. The max is updated once after each inner scan in a single unified check. This gives O(n) amortized time in O(1) space — a clean, efficient solution built from nothing more than two pointers and a 256-slot lookup table.
