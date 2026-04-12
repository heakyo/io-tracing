# 003 - Longest Substring Without Repeating Characters (Optimized)

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

---

## Why Optimize?

The original solution is correct and has O(n) amortized time, but it carries unnecessary overhead:

1. **Memset per window start.** Every time `p1` advances, the original resets all 256 entries of the `struct mark` array (2048 bytes). This constant factor adds up.
2. **Nested loop structure.** The original has an outer `while(*p1)` and inner `while(*p2)`, which obscures the single-pass nature of the algorithm.
3. **Unused struct field.** The `cnt` field only ever holds 0 or 1 — a boolean check masquerading as a counter.

The optimized version replaces all of this with a single `for` loop over the string and a `last[256]` array storing the most recent index of each character. One `memset` at the start, zero inside the loop.

---

## Core Idea

Sliding window with a "last-seen position" lookup table.

Maintain a window `[left, right]` that always contains unique characters. As `right` advances one character at a time:

- Look up `last[ch]` — the most recent index where character `ch` appeared.
- If `last[ch] >= left`, the character is inside the current window. Shrink the window by moving `left` to `last[ch] + 1`.
- Record `last[ch] = right`.
- Update `max_len` if the current window is the longest so far.

The key insight: we never scan backward or reset the lookup table. The `>= left` check filters out stale entries automatically.

---

## Function Reference

| Function | Signature | Purpose |
|----------|-----------|---------|
| `lengthOfLongestSubstring` | `int lengthOfLongestSubstring(char *s)` | Find the length of the longest substring without repeating characters. Uses a `last[256]` array initialized to -1 (via `memset(last, 0xff, ...)`). Single pass with `right` from 0 to len-1; adjusts `left` when a duplicate is inside the window. Returns `max_len`. |
| `run_test` | `void run_test(const char *name, char *s, int expected)` | Run a single test case: print input, call `lengthOfLongestSubstring`, print and assert the result matches `expected`. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 10 test cases (identical to the original) and prints "All tests passed!" on success. |

---

## ASCII Flowchart

```
+-----------------------------------+
| lengthOfLongestSubstring(s)       |
+-----------------------------------+
                |
                v
   +---------------------------+
   | memset(last, -1, 256)     |
   | left = 0, max_len = 0    |
   | len = strlen(s)           |
   +---------------------------+
                |
                v
   +---------------------------+
+->| right < len ?             |--no--> return max_len
|  +---------------------------+
|              | yes
|              v
|  +---------------------------+
|  | ch = s[right]             |
|  +---------------------------+
|              |
|              v
|  +---------------------------+
|  | last[ch] >= left ?        |
|  +---------------------------+
|     yes /          \ no
|        v            |
|  +--------------+   |
|  | left =       |   |
|  | last[ch] + 1 |   |
|  +--------------+   |
|        |            |
|        v            v
|  +---------------------------+
|  | last[ch] = right          |
|  +---------------------------+
|              |
|              v
|  +---------------------------+
|  | right - left + 1 >       |
|  | max_len ?                 |
|  +---------------------------+
|     yes /          \ no
|        v            |
|  +--------------+   |
|  | max_len =    |   |
|  | right-left+1 |   |
|  +--------------+   |
|        |            |
|        v            v
|  +---------------------------+
|  | right++                   |
|  +---------------------------+
|              |
+--------------+
```

---

## Step-by-Step Walkthrough

### Example 1: `"abcabcbb"` (expected: 3)

```
last[] initialized to all -1.  left=0, max_len=0

right=0, ch='a': last['a']=-1 < 0  -> no move
  last['a']=0, window=[0,0], size=1, max_len=1

right=1, ch='b': last['b']=-1 < 0  -> no move
  last['b']=1, window=[0,1], size=2, max_len=2

right=2, ch='c': last['c']=-1 < 0  -> no move
  last['c']=2, window=[0,2], size=3, max_len=3

right=3, ch='a': last['a']=0 >= left=0  -> left=1
  last['a']=3, window=[1,3], size=3, max_len=3

right=4, ch='b': last['b']=1 >= left=1  -> left=2
  last['b']=4, window=[2,4], size=3, max_len=3

right=5, ch='c': last['c']=2 >= left=2  -> left=3
  last['c']=5, window=[3,5], size=3, max_len=3

right=6, ch='b': last['b']=4 >= left=3  -> left=5
  last['b']=6, window=[5,6], size=2, max_len=3

right=7, ch='b': last['b']=6 >= left=5  -> left=7
  last['b']=7, window=[7,7], size=1, max_len=3

Result: 3
```

### Example 2: `"dvdf"` (expected: 3)

```
last[] initialized to all -1.  left=0, max_len=0

right=0, ch='d': last['d']=-1 < 0  -> no move
  last['d']=0, window=[0,0], size=1, max_len=1

right=1, ch='v': last['v']=-1 < 0  -> no move
  last['v']=1, window=[0,1], size=2, max_len=2

right=2, ch='d': last['d']=0 >= left=0  -> left=1
  last['d']=2, window=[1,2], size=2, max_len=2

right=3, ch='f': last['f']=-1 < 0  -> no move
  last['f']=3, window=[1,3], size=3, max_len=3

Result: 3
```

---

## Where It Gets Tricky

1. **Initializing to -1 via `memset(last, 0xff, ...)`.**  For a 32-bit `int`, `0xFFFFFFFF` is -1 in two's complement. This is a common C idiom but relies on two's complement representation (guaranteed by C23 and universal on all modern hardware). A loop setting `last[i] = -1` is the "safe" alternative.

2. **`last[ch] >= left` filters stale entries.** A character may have `last[ch]` set from a previous window position that is now behind `left`. The `>= left` check ignores it — no need to clear old entries. This eliminates the per-window memset that the original requires.

3. **`unsigned char ch` avoids negative indexing.** If `char` is signed, characters with values 128-255 would produce negative array indices. Casting to `unsigned char` ensures valid 0-255 indexing. The original code doesn't do this (it uses `*p2` directly), which would be undefined behavior for extended ASCII.

4. **No special case for end-of-string.** The original needs a separate `*p2 == '\0'` check after the inner loop. With a single `for` loop bounded by `len`, the empty-string case (len=0, loop body never executes, returns 0) and end-of-string are handled naturally.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time** | O(n) | Single pass — `right` visits each character exactly once. `left` only moves forward. |
| **Space** | O(1) | Fixed `last[256]` array (1 KB), independent of input size. |

---

## Comparison with Original

| | Original | Optimized |
|--|----------|-----------|
| **Algorithm** | Two-pointer with mark array | Sliding window with last-seen array |
| **Time** | O(n) amortized | O(n) strict single pass |
| **Space** | O(1) (256 structs = 2 KB) | O(1) (256 ints = 1 KB) |
| **Memset calls** | One per window start position | One total (at initialization) |
| **Loop structure** | Nested `while`/`while` | Single `for` loop |
| **Signed char safety** | No (`*p2` used as index directly) | Yes (`unsigned char` cast) |
| **Lines of code** | 39 (function body) | 16 (function body) |

The optimized version is simpler, has lower constant overhead (no per-position memset), uses half the memory for the lookup table, and handles extended ASCII safely.

---

## Summary

The optimized solution uses a classic sliding window with a "last-seen position" array. A single `for` loop advances `right` through the string. When the character at `right` was last seen at or after `left`, we shrink the window by moving `left` past that position. The `last[256]` array is initialized once to -1 and never reset — stale entries are filtered by the `>= left` check. This eliminates the per-window `memset` of the original, reduces the struct to a plain int array, and collapses the nested loop into a single pass. The result is O(n) time with lower constant overhead and cleaner code.
