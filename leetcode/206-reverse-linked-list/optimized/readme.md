# 206 - Reverse Linked List (Optimized)

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Function Reference](#function-reference)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

---

## Problem Statement

Given the head of a singly linked list, reverse the list, and return the
reversed list.

For example, given `[1, 2, 3, 4, 5]`, return `[5, 4, 3, 2, 1]`.

---

## Core Idea

Imagine you have a chain of paper clips linked together in a line. You want
to reverse the whole chain. You can't pick the chain up and flip it — you
have to re-hook each clip, one at a time, onto a new chain that grows in
the opposite direction.

That's exactly what this algorithm does. It walks through the original list
one node at a time and re-points each node's `next` link backward onto a
"reversed so far" chain.

The optimized version uses the same O(n) iterative reversal as the original
but with cleaner variable naming. There are only three players, and each
one has exactly one job:

- **`prev`** — The previously-built reversed chain. Starts as `NULL`
  (nothing reversed yet) and ends as the new head.
- **`next`** — A temporary that saves the next node *before* we overwrite
  the link. Without it, we'd lose the rest of the list.
- **`head`** — Used directly as the cursor that walks through the original
  list. No extra pointer `p` needed.

The loop body is four lines, and they always run in the same order:

```c
next = head->next;	/* 1. save next         */
head->next = prev;	/* 2. reverse link      */
prev = head;		/* 3. advance prev      */
head = next;		/* 4. advance head      */
```

Each variable has exactly one clear purpose — no double duty, no confusion.

---

## Step-by-Step Walkthrough

Let's trace through the input `[1, 2, 3, 4, 5]`.

### Initial state

```
prev = NULL
head -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
```

### Iteration 1 (head points to node 1)

| Variable | Before | After |
|----------|--------|-------|
| `next`   | —      | `-> [2]` |
| `head->next` | `-> [2]` | `NULL` (prev) |
| `prev`   | `NULL` | `-> [1] -> NULL` |
| `head`   | `-> [1]` | `-> [2]` (next) |

```
prev -> [1] -> NULL
head -> [2] -> [3] -> [4] -> [5] -> NULL
```

### Iteration 2 (head points to node 2)

| Variable | Before | After |
|----------|--------|-------|
| `next`   | `-> [2]` | `-> [3]` |
| `head->next` | `-> [3]` | `-> [1]` (prev) |
| `prev`   | `-> [1]` | `-> [2] -> [1] -> NULL` |
| `head`   | `-> [2]` | `-> [3]` (next) |

```
prev -> [2] -> [1] -> NULL
head -> [3] -> [4] -> [5] -> NULL
```

### Iteration 3 (head points to node 3)

| Variable | Before | After |
|----------|--------|-------|
| `next`   | `-> [3]` | `-> [4]` |
| `head->next` | `-> [4]` | `-> [2]` (prev) |
| `prev`   | `-> [2]` | `-> [3] -> [2] -> [1] -> NULL` |
| `head`   | `-> [3]` | `-> [4]` (next) |

```
prev -> [3] -> [2] -> [1] -> NULL
head -> [4] -> [5] -> NULL
```

### Iteration 4 (head points to node 4)

| Variable | Before | After |
|----------|--------|-------|
| `next`   | `-> [4]` | `-> [5]` |
| `head->next` | `-> [5]` | `-> [3]` (prev) |
| `prev`   | `-> [3]` | `-> [4] -> [3] -> [2] -> [1] -> NULL` |
| `head`   | `-> [4]` | `-> [5]` (next) |

```
prev -> [4] -> [3] -> [2] -> [1] -> NULL
head -> [5] -> NULL
```

### Iteration 5 (head points to node 5)

| Variable | Before | After |
|----------|--------|-------|
| `next`   | `-> [5]` | `NULL` |
| `head->next` | `NULL` | `-> [4]` (prev) |
| `prev`   | `-> [4]` | `-> [5] -> [4] -> [3] -> [2] -> [1] -> NULL` |
| `head`   | `-> [5]` | `NULL` (next) |

```
prev -> [5] -> [4] -> [3] -> [2] -> [1] -> NULL
head = NULL  (loop ends)
```

### Result

`head` is `NULL`, so the `while` loop exits. We return `prev`, which
points to `[5] -> [4] -> [3] -> [2] -> [1] -> NULL` — the reversed list.

---

## ASCII Flowchart

```
            +-------+
            | START |
            +---+---+
                |
                v
        +--------------+
        | prev = NULL  |
        +--------------+
                |
                v
       +----------------+
       | head != NULL ? |<-----------+
       +-------+--------+            |
               |                     |
          yes  |    no               |
               v     |               |
   +-------------------+             |
   | next = head->next |             |
   +-------------------+             |
               |                     |
               v                     |
   +-------------------+             |
   | head->next = prev |             |
   +-------------------+             |
               |                     |
               v                     |
   +-------------------+             |
   |   prev = head     |             |
   +-------------------+             |
               |                     |
               v                     |
   +-------------------+             |
   |   head = next     +-------------+
   +-------------------+
               |
          no   | (back to condition)
               v
       +---------------+
       |  return prev  |
       +-------+-------+
               |
               v
           +-------+
           |  END  |
           +-------+
```

---

## Function Reference

All functions are defined in `optimized/main.c`.

| Function | Description |
|----------|-------------|
| `struct ListNode *insert_list(int *nums, int size)` | Build a linked list from an array of integers. Allocates nodes from back to front so the list order matches the array order. |
| `void show_list(char *type, struct ListNode *head)` | Print every element in the list, prefixed with a label string `type`. |
| `struct ListNode *reverseList(struct ListNode *head)` | **Core algorithm.** Reverse the linked list in-place. O(n) time, O(1) space. Returns the new head. |
| `static void free_list(struct ListNode *head)` | Walk the list and `free()` every node. |
| `static int check_list(struct ListNode *head, int *expected, int len)` | Verify that the list matches an expected integer array. Returns 1 on match, 0 on mismatch. |
| `static void run_test(const char *name, int *nums, int len, int *expected, int expected_len)` | Run a single test case: build list, reverse it, print results, assert correctness, then free memory. |

---

## Where It Gets Tricky

### (a) Why `head` can be the cursor

In C, function parameters are passed by value. That means `head` inside
`reverseList` is a **local copy** of the pointer the caller passed in.
Modifying it (advancing it through the list) doesn't affect the caller's
variable at all. The caller gets the reversed list through the **return
value**, not through the original pointer. So there's no harm in reusing
`head` as our walking cursor — it's ours to overwrite.

The original version introduced an extra pointer `p` to serve as the
cursor, leaving `head` "available" — but then immediately repurposed
`head` as a temporary anyway. The optimized version skips the middleman:
`head` *is* the cursor, and we use a clearly-named `next` for the
temporary.

### (b) Order still matters

The four lines inside the loop **must** execute in exactly this order:

```c
next = head->next;	/* MUST be first: save before we overwrite */
head->next = prev;	/* now safe to reverse the link             */
prev = head;		/* grow the reversed chain                  */
head = next;		/* advance to the saved next node           */
```

If you swap lines 1 and 2, `head->next` would already point to `prev`
by the time you try to save it — you'd lose the rest of the list.

### (c) Variable naming

Compare the two versions side by side:

| Optimized | Original | Role |
|-----------|----------|------|
| `prev`    | `rh`     | The reversed chain built so far |
| `next`    | `head` (reused) | Temporary: saves next node |
| `head`    | `p`      | Cursor walking the original list |

`prev` immediately tells you "this is what came before." `rh` makes you
stop and think "reverse head?" `next` immediately tells you "this is the
next node." Reusing `head` as a temporary makes you wonder if the caller's
pointer is affected (it isn't — see point (a) — but you have to think
about it). Good names eliminate unnecessary thinking.

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time** | O(n) | We visit each of the n nodes exactly once. |
| **Space** | O(1) | Only two extra pointers (`prev`, `next`) regardless of list size. |

The performance is identical to the original version. The optimization is
purely in **code clarity**, not in runtime or memory. Cleaner variable
names and one fewer named pointer make the code easier to read, review,
and maintain — with zero cost to performance.

---

## Summary

The optimized version achieves the same O(n) time / O(1) space performance
as the original but with cleaner code: 2 named variables instead of 3,
each with a clear single purpose. Using `head` directly as the cursor
eliminates the redundant `p` pointer, and using `next` instead of
repurposing the `head` parameter makes the code's intent immediately clear.

The algorithm is still the same "peel one node off the front, stick it on
the reversed chain" loop. What changed is how we talk about it in code.
And when you come back to this function six months from now, that's what
matters — not clever tricks with fewer characters, but names that tell
you exactly what's happening at a glance.
