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

- `cnt` — has this character appeared in the current window?
- `offset` — where in the current window (relative to `p1`) did it appear?

For each window starting at `p1`, the inner pointer `p2` scans forward one character at a time. When `p2` lands on a character that has already been seen (`m[*p2].cnt > 0`), the algorithm does not just inch `p1` forward by one. Instead, it looks up the **stored offset** of the first occurrence of that duplicate character and **jumps** `p1` past it:

```c
p1 += m[*p2].offset + 1;
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

p2 = 0: *p2 = 'a'  → m['a'].cnt == 0, so mark it:  m['a'] = {cnt:1, offset:0}
p2 = 1: *p2 = 'b'  → m['b'].cnt == 0, so mark it:  m['b'] = {cnt:1, offset:1}
p2 = 2: *p2 = 'c'  → m['c'].cnt == 0, so mark it:  m['c'] = {cnt:1, offset:2}
p2 = 3: *p2 = 'a'  → m['a'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 3 - 0 = 3
  max = max(0, 3) = 3

  Jump: p1 += m['a'].offset + 1 = 0 + 1 = 1
  → p1 is now 1
```

**Pass 2 — p1 = 1 (`'b'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 1: *p2 = 'b'  → mark it:  m['b'] = {cnt:1, offset:0}
p2 = 2: *p2 = 'c'  → mark it:  m['c'] = {cnt:1, offset:1}
p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {cnt:1, offset:2}
p2 = 4: *p2 = 'b'  → m['b'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 4 - 1 = 3
  max = max(3, 3) = 3

  Jump: p1 += m['b'].offset + 1 = 0 + 1 = 1
  → p1 is now 2
```

**Pass 3 — p1 = 2 (`'c'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 2: *p2 = 'c'  → mark it:  m['c'] = {cnt:1, offset:0}
p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {cnt:1, offset:1}
p2 = 4: *p2 = 'b'  → mark it:  m['b'] = {cnt:1, offset:2}
p2 = 5: *p2 = 'c'  → m['c'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 5 - 2 = 3
  max = max(3, 3) = 3

  Jump: p1 += m['c'].offset + 1 = 0 + 1 = 1
  → p1 is now 3
```

**Pass 4 — p1 = 3 (`'a'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 3: *p2 = 'a'  → mark it:  m['a'] = {cnt:1, offset:0}
p2 = 4: *p2 = 'b'  → mark it:  m['b'] = {cnt:1, offset:1}
p2 = 5: *p2 = 'c'  → mark it:  m['c'] = {cnt:1, offset:2}
p2 = 6: *p2 = 'b'  → m['b'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 6 - 3 = 3
  max = max(3, 3) = 3

  Jump: p1 += m['b'].offset + 1 = 1 + 1 = 2
  → p1 is now 5
```

**Pass 5 — p1 = 5 (`'c'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 5: *p2 = 'c'  → mark it:  m['c'] = {cnt:1, offset:0}
p2 = 6: *p2 = 'b'  → mark it:  m['b'] = {cnt:1, offset:1}
p2 = 7: *p2 = 'b'  → m['b'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 7 - 5 = 2
  max = max(3, 2) = 3

  Jump: p1 += m['b'].offset + 1 = 1 + 1 = 2
  → p1 is now 7
```

**Pass 6 — p1 = 7 (`'b'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 7: *p2 = 'b'  → mark it:  m['b'] = {cnt:1, offset:0}
p2 = 8: *p2 = '\0' → end of string

  End-of-string check: p2 - p1 = 8 - 7 = 1
  max = max(3, 1) = 3
  Break out of outer loop.
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

p2 = 0: *p2 = 'd'  → mark it:  m['d'] = {cnt:1, offset:0}
p2 = 1: *p2 = 'v'  → mark it:  m['v'] = {cnt:1, offset:1}
p2 = 2: *p2 = 'd'  → m['d'].cnt == 1 → DUPLICATE FOUND!

  Current window length: p2 - p1 = 2 - 0 = 2
  max = max(0, 2) = 2

  Jump: p1 += m['d'].offset + 1 = 0 + 1 = 1
  → p1 is now 1
```

**Pass 2 — p1 = 1 (`'v'`)**

```
memset(m, 0) — clear all 256 entries

p2 = 1: *p2 = 'v'  → mark it:  m['v'] = {cnt:1, offset:0}
p2 = 2: *p2 = 'd'  → mark it:  m['d'] = {cnt:1, offset:1}
p2 = 3: *p2 = 'f'  → mark it:  m['f'] = {cnt:1, offset:2}
p2 = 4: *p2 = '\0' → end of string

  End-of-string check: p2 - p1 = 4 - 1 = 3
  max = max(2, 3) = 3
  Break out of outer loop.
```

**Result: `max = 3`** (the substring `"vdf"`)

---

## ASCII Flowchart

```
                        ┌──────────────┐
                        │    START     │
                        │  max = 0     │
                        │  p1 = s      │
                        └──────┬───────┘
                               │
                               ▼
                     ┌─────────────────────┐
                 ┌───│     *p1 != '\0' ?   │
                 │   └─────────────────────┘
                 │ No          │ Yes
                 │             ▼
                 │   ┌─────────────────────┐
                 │   │  memset(m, 0, ...)  │
                 │   │  p2 = p1            │
                 │   └─────────┬───────────┘
                 │             │
                 │             ▼
                 │   ┌─────────────────────┐
                 │   │    *p2 != '\0' ?    │──── No ───┐
                 │   └─────────────────────┘           │
                 │             │ Yes                    │
                 │             ▼                        │
                 │   ┌─────────────────────┐           │
                 │   │  m[*p2].cnt > 0 ?   │           │
                 │   └─────────────────────┘           │
                 │      │ Yes        │ No              │
                 │      ▼            ▼                  │
                 │   ┌──────────┐ ┌──────────────┐     │
                 │   │ DUPLICATE│ │ m[*p2].cnt++ │     │
                 │   │ FOUND    │ │ m[*p2].offset│     │
                 │   │          │ │  = p2 - p1   │     │
                 │   │ update   │ │ p2++         │     │
                 │   │ max if   │ └──────┬───────┘     │
                 │   │ p2-p1 >  │        │             │
                 │   │ max      │        │ (loop back  │
                 │   └────┬─────┘        │  to *p2?)   │
                 │        │              └──────────────┤
                 │        │                             │
                 │        │                             ▼
                 │        │               ┌─────────────────────────┐
                 │        │               │ *p2 == '\0' &&          │
                 │        │               │ p2 - p1 > max ?         │
                 │        │               └─────────────────────────┘
                 │        │                 │ Yes              │ No
                 │        │                 ▼                  │
                 │        │          ┌─────────────┐          │
                 │        │          │ max = p2-p1 │          │
                 │        │          │ BREAK outer │          │
                 │        │          └──────┬──────┘          │
                 │        │                 │                  │
                 │        ▼                 │                  │
                 │   ┌────────────────────┐ │                  │
                 │   │ p1 += m[*p2].offset│ │                  │
                 │   │       + 1          │◄┼──────────────────┘
                 │   │ (jump past the     │ │
                 │   │  first duplicate)  │ │
                 │   └────────┬───────────┘ │
                 │            │             │
                 │            └──── (loop back to *p1?) ───►
                 │                          │
                 ▼                          ▼
          ┌──────────────┐          ┌──────────────┐
          │  return max  │          │  return max  │
          └──────────────┘          └──────────────┘
```

---

## Where It Gets Tricky

### (a) The `memset` resets the entire 256-entry array every pass

Every time the outer loop starts a new window (a new `p1` position), it calls:

```c
memset(m, 0x0, sizeof(struct mark) * 256);
```

This wipes the entire 256-entry mark array — roughly 2 KB of data — even though only a handful of entries were actually used. This is a constant-time operation (256 entries is fixed), so it does not hurt asymptotic complexity, but it is worth noticing because a more refined implementation might only clear the entries that were actually set. The upside is simplicity: you never have stale data from a previous pass leaking into the current one.

### (b) The offset-based jump is the key optimization

The line:

```c
p1 += m[*p2].offset + 1;
```

is what separates this from a naive O(n²) approach. When a duplicate character is detected at `p2`, the algorithm does not simply do `p1++`. Instead, it asks: "Where did I first see this character within the current window?" and jumps `p1` to **one position past** that first occurrence. Every starting position between the old `p1` and the new `p1` would have also contained that same duplicate, so they are guaranteed to produce shorter or equal windows. Skipping them is safe.

### (c) The end-of-string check breaks the outer loop early

```c
if (*p2 == '\0' && p2 - p1 > max) {
    max = p2 - p1;
    break;
}
```

When the inner loop exits because `p2` reached the null terminator (not because of a duplicate), it means the substring from `p1` to the end of the string is entirely unique. There is no point in trying any later starting position — they would all produce shorter substrings. So the algorithm updates `max` and immediately breaks out of the outer loop. This is a clean early-exit optimization.

### (d) The `m[*p2].cnt = 0` reset before the break is defensive

```c
if (m[*p2].cnt > 0) {
    m[*p2].cnt = 0;          // ← this line
    if (p2 - p1 > max)
        max = p2 - p1;
    break;
}
```

Right before breaking out of the inner loop, the code sets `m[*p2].cnt = 0`. This is **defensive but not strictly necessary** — the very next thing the outer loop does is `memset` the entire array to zero anyway. However, the **offset** stored in `m[*p2].offset` is still valid and is read immediately after the break to compute the jump. The reset of `cnt` does not interfere with the offset field, so the jump calculation remains correct.

---

## Complexity Analysis

### Time: O(n) amortized

At first glance this might look like O(n²) because of the nested loops, but look at what `p1` does: it **only moves forward**, and it often jumps forward by more than one position. Each character in the string is visited by `p2` at most a constant number of times across all passes, because when `p1` jumps forward, the next inner scan starts from the new `p1`, not from the beginning. The total work done by `p2` across all iterations of the outer loop is proportional to `n`. The `memset` call is O(256) = O(1) per outer iteration, and the number of outer iterations is at most `n`. So the total time is **O(n)** amortized.

### Space: O(1)

The mark array `m[256]` has a fixed size of 256 entries (one per possible ASCII byte value), regardless of the input string length. The only other variables are two pointers and an integer. Total extra space: **O(1)**.

---

## Summary

This algorithm finds the longest substring without repeating characters by sliding two pointers across the string. The clever part is the **mark array**: it records not just *whether* a character has been seen, but *where* in the current window it was seen. When a duplicate is detected, the algorithm uses that stored offset to jump the left pointer directly past the first occurrence of the duplicate, skipping all the starting positions that would be doomed to produce shorter results. Combined with an early exit when the inner scan reaches the end of the string, this gives O(n) amortized time in O(1) space — a clean, efficient solution built from nothing more than two pointers and a 256-slot lookup table.
