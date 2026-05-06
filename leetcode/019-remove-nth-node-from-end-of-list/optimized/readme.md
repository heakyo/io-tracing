# 19 - Remove Nth Node From End of List (Optimized)

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Comparison with Original](#comparison-with-original)
- [Complexity Analysis](#complexity-analysis)
- [Function Reference](#function-reference)
- [Summary](#summary)

---

## Problem Statement

Given the `head` of a linked list, remove the nth node from the **end** of
the list and return its head.

For example, given `[1, 2, 3, 4, 5]` and `n = 2`, the 2nd node from the
end is `4`. Remove it and return `[1, 2, 3, 5]`.

---

## Core Idea

The classic two-pointer technique: advance `fast` by `n` steps so that it
leads `slow` by exactly `n` nodes. Then move both forward in lockstep.
When `fast` hits `NULL`, `slow` is at the right position.

The optimized version makes two key improvements over the original:

**1. `slow` starts from `&dummy`, not from `head`.** This single change
eliminates the need for a separate `prev` pointer entirely. Think about
why: if `slow` starts one step behind `head` (at the dummy node), then
when the loop ends, `slow` is naturally the node **before** the target
-- exactly where you need to be to unlink the target. The original version
started `slow` at `head`, so when the loop ended `slow` *was* the target
itself, meaning you needed a separate `prev` trailing behind to do the
unlinking.

**2. The removed node is freed.** The original version just rewired the
`prev->next` pointer around the target node, leaving the target allocated
but unreachable -- a memory leak. The optimized version saves the target
in a temporary pointer, unlinks it, and calls `free()`.

---

## Step-by-Step Walkthrough

Let's trace through the input `[1, 2, 3, 4, 5]` with `n = 2`.

### Phase 1: Setup

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
fast = head (points to [1])
```

### Phase 2: Advance `fast` by n = 2 steps

**for loop, i = 0:**

```
fast moves from [1] to [2]
```

**for loop, i = 1:**

```
fast moves from [2] to [3]
```

After the for loop:

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                        ^
                       fast
slow = &dummy
```

`fast` is 2 steps ahead of `head`. Now `slow` starts at `&dummy`, which
is one position before `head`.

### Phase 3: Move both until `fast` hits NULL

**While iteration 1:**

| Variable | Before | After |
|----------|--------|-------|
| `slow`   | `-> dummy` | `-> [1]` |
| `fast`   | `-> [3]` | `-> [4]` |

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
          ^                    ^
         slow                 fast
```

**While iteration 2:**

| Variable | Before | After |
|----------|--------|-------|
| `slow`   | `-> [1]` | `-> [2]` |
| `fast`   | `-> [4]` | `-> [5]` |

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                 ^                    ^
                slow                 fast
```

**While iteration 3:**

| Variable | Before | After |
|----------|--------|-------|
| `slow`   | `-> [2]` | `-> [3]` |
| `fast`   | `-> [5]` | `NULL` |

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                         ^
                        slow            fast = NULL (loop ends)
```

`fast` is `NULL`, so the while loop exits. `slow` points to `[3]`, which
is the node **before** the target `[4]`.

### Phase 4: Remove the target

```c
target = slow->next;        /* target = [4]              */
slow->next = target->next;  /* [3]->next = [5]           */
free(target);               /* free node [4]             */
```

```
dummy -> [1] -> [2] -> [3] ---------> [5] -> NULL
```

### Result

Return `dummy.next`, which points to `[1]`. The final list is
`[1, 2, 3, 5]`.

---

## ASCII Flowchart

```
                +-------+
                | START |
                +---+---+
                    |
                    v
        +------------------------+
        | dummy = {0, head}      |
        | fast = head            |
        +------------------------+
                    |
                    v
            +--------------+
            |  i < n ?     |<----+
            +------+-------+     |
                   |             |
              yes  |  no         |
                   v  |          |
        +------------------+     |
        | fast = fast->next|     |
        | i++              +-----+
        +------------------+
                   |
              no   | (i == n)
                   v
        +------------------------+
        | slow = &dummy          |
        +------------------------+
                   |
                   v
         +----------------+
         | fast != NULL ? |<---------+
         +-------+--------+         |
                 |                   |
            yes  |  no               |
                 v  |                |
     +-------------------+           |
     | slow = slow->next |           |
     | fast = fast->next +-----------+
     +-------------------+
                 |
            no   | (fast == NULL)
                 v
     +---------------------------+
     | target = slow->next       |
     +---------------------------+
                 |
                 v
     +---------------------------+
     | slow->next = target->next |
     +---------------------------+
                 |
                 v
     +---------------------------+
     | free(target)              |
     +---------------------------+
                 |
                 v
     +---------------------------+
     | return dummy.next         |
     +-------------+-------------+
                   |
                   v
               +-------+
               |  END  |
               +-------+
```

---

## Where It Gets Tricky

### (a) Why starting `slow` from `&dummy` eliminates `prev`

In the original version, `slow` and `fast` both start at `head`, and a
separate `prev` pointer trails one step behind `slow`:

```c
/* original */
prev = &dummy;
slow = head;
fast = head;
while (fast) {
    prev = slow;       /* extra assignment every iteration */
    slow = slow->next;
    fast = fast->next;
}
prev->next = slow->next;   /* prev is one behind slow */
```

Three pointers doing two pointers' worth of work. The insight is: if you
shift `slow` back by one position at the start (to `&dummy`), then `slow`
is always one behind where it would have been. When the loop ends,
`slow` is already the node before the target -- it *is* the `prev`. No
extra variable, no extra assignment per iteration.

```c
/* optimized */
slow = &dummy;          /* one step behind head */
while (fast) {
    slow = slow->next;
    fast = fast->next;
}
/* slow is now the node BEFORE the target */
```

### (b) Why `free` must come after unlinking

The removal sequence is:

```c
target = slow->next;        /* 1. save pointer to doomed node */
slow->next = target->next;  /* 2. unlink it from the chain    */
free(target);               /* 3. release its memory          */
```

You cannot call `free(target)` before step 2. After `free()`, reading
`target->next` is undefined behavior -- the memory might already be
reused. You must read `target->next` (via `slow->next = target->next`)
while the node is still alive.

You also cannot skip the `target` temporary and write
`slow->next = slow->next->next; free(???)` -- you would lose the pointer
to the node you need to free.

### (c) Edge case: removing the head node

When `n` equals the length of the list, `fast` becomes `NULL` after the
for loop. The while loop body never executes, so `slow` stays at `&dummy`.
Then `target = slow->next` is the head node itself. The dummy node
handles this edge case transparently -- no special-case `if` needed.

---

## Comparison with Original

| Aspect | Original | Optimized |
|--------|----------|-----------|
| Pointer variables | `slow`, `fast`, `prev` (3) | `slow`, `fast`, `target` (3 declared, but `target` is only used after the loop) |
| Assignments per while iteration | 3 (`prev = slow`, `slow = slow->next`, `fast = fast->next`) | 2 (`slow = slow->next`, `fast = fast->next`) |
| `slow` starts at | `head` | `&dummy` (one position behind) |
| Separate `prev` tracking | Yes, updated every iteration | No, `slow` naturally ends up before the target |
| Frees removed node | No (memory leak) | Yes (`free(target)`) |
| Time complexity | O(n) | O(n) |
| Space complexity | O(1) | O(1) |
| Number of passes | 1 | 1 |

The runtime behavior is identical. The optimized version does less work
per iteration (one fewer assignment) and avoids leaking memory.

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time** | O(n) | The for loop runs n iterations. The while loop runs (length - n) iterations. Together, every node is visited at most once. Total: O(n). |
| **Space** | O(1) | Only a fixed number of pointers (`slow`, `fast`, `target`) and one stack-allocated dummy node, regardless of input size. |

---

## Function Reference

All functions are defined in `optimized/main.c`.

| Function | Description |
|----------|-------------|
| `struct ListNode *removeNthFromEnd(struct ListNode *head, int n)` | **Core algorithm.** Uses the two-pointer technique with `slow` starting from a dummy node. Removes the nth node from the end, frees it, and returns the updated head. |
| `struct ListNode *insert_list(int *nums, int size)` | Build a linked list from an array of integers. Allocates nodes from back to front so the list order matches the array order. |
| `void show_list(char *type, struct ListNode *head)` | Print every element in the list, prefixed with a label string `type`. |
| `static void free_list(struct ListNode *head)` | Walk the list and `free()` every node. |
| `static int check_list(struct ListNode *head, int *expected, int len)` | Verify that the list matches an expected integer array. Returns 1 on match, 0 on mismatch. |
| `static void run_test(const char *name, int *nums, int len, int n, int *expected, int expected_len)` | Run a single test case: build list, call `removeNthFromEnd`, print results, assert correctness, then free remaining nodes. |

---

## Summary

The optimized version of Remove Nth Node From End of List keeps the same
O(n) single-pass, O(1) space two-pointer approach but cleans up two
problems in the original. First, by starting `slow` at `&dummy` instead of
`head`, the pointer naturally lands one node before the target when the loop
ends, eliminating the separate `prev` pointer and saving one assignment per
iteration. Second, the removed node is properly freed instead of being
silently leaked. The dummy-node technique also handles the edge case of
removing the head node without any special-case branching. The result is
fewer moving parts, less work per iteration, and correct memory management
-- all without changing the algorithmic complexity.
