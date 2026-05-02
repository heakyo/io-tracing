# 092 - Reverse Linked List II (Optimized)

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

Given the head of a singly linked list and two integers `left` and `right`
(where `left <= right`), reverse the nodes from position `left` to position
`right` and return the modified list.

Positions are 1-indexed: position 1 is the head.

Example: `[1, 2, 3, 4, 5]`, `left = 2`, `right = 4` produces `[1, 4, 3, 2, 5]`.

---

## Core Idea

Imagine you have a row of people standing in line. You need to reverse the
order of people from position 2 to position 4. Instead of picking all of them
up and flipping them around at once, you do something simpler: you repeatedly
take the *next* person after the sub-group and move them to the *front* of the
sub-group. After enough moves, the sub-group is reversed.

This is the **head-insertion technique**. Both the original and optimized
solutions use it. The optimized version improves the *style*, not the
algorithm:

1. **Stack-based dummy node** — `struct ListNode dummy = { .val = 0, .next = head };`
   lives on the stack. No `malloc`, no memory leak, no `free` needed.
2. **Descriptive names** — `prev` (the node right before the reversed segment),
   `tail` (the node that will sink to the tail of the reversed segment),
   `move` (the node being plucked and moved to the front).
3. **Simplified navigation** — A single pointer `prev` walks forward in one
   loop. No need to track two pointers during navigation.
4. **Cleaner reversal loop** — Four pointer updates in a natural order:
   extract, detach, link, anchor.

---

## Step-by-Step Walkthrough

Trace with `[1, 2, 3, 4, 5]`, `left = 2`, `right = 4`.

### Phase 1: Setup

```
dummy = { val: 0, next: -> 1 }
prev  = &dummy
```

```
prev
 |
[0] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
dummy
```

### Phase 2: Navigate to position `left`

Loop runs for `i = 1` (since `left = 2`, one iteration):

```
prev = prev->next;   // prev now points to node [1]
```

```
        prev
         |
[0] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
```

Then set `tail`:

```
tail = prev->next;   // tail points to node [2]
```

```
        prev  tail
         |     |
[0] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
```

Node `[2]` is at position `left`. It will stay in place throughout the
reversal loop and gradually sink to position `right` as other nodes leapfrog
over it. That is why it is called `tail`.

### Phase 3: Reversal loop

The loop runs for `i = 2, 3` (two iterations: `right - left = 4 - 2 = 2`).

**Iteration 1** (`i = 2`): move node `[3]` to the front.

```
move = tail->next;         // move = [3]
tail->next = move->next;   // [2]->next = [4]   (skip over [3])
move->next = prev->next;   // [3]->next = [2]   (link [3] before [2])
prev->next = move;         // [1]->next = [3]   (anchor [3] after prev)
```

```
        prev        tail
         |           |
[0] -> [1] -> [3] -> [2] -> [4] -> [5] -> NULL
```

Node `[3]` jumped in front of `[2]`. `tail` still points to `[2]`, which has
sunk one position deeper.

**Iteration 2** (`i = 3`): move node `[4]` to the front.

```
move = tail->next;         // move = [4]
tail->next = move->next;   // [2]->next = [5]   (skip over [4])
move->next = prev->next;   // [4]->next = [3]   (link [4] before [3])
prev->next = move;         // [1]->next = [4]   (anchor [4] after prev)
```

```
        prev              tail
         |                 |
[0] -> [1] -> [4] -> [3] -> [2] -> [5] -> NULL
```

Node `[4]` jumped to the front. The segment `[2, 3, 4]` is now `[4, 3, 2]`.
Done.

### Phase 4: Return

```
return dummy.next;   // returns pointer to [1]
```

Final list: `[1, 4, 3, 2, 5]`.

---

## ASCII Flowchart

```
                    START
                      |
                      v
        +----------------------------+
        | Create stack-based dummy   |
        | dummy = { 0, head }        |
        | prev = &dummy              |
        +----------------------------+
                      |
                      v
              +---------------+
              | i = 1         |
              +---------------+
                      |
                      v
              +--------------+     yes    +-------------------+
              | i < left ?   |----------->| prev = prev->next |
              +--------------+            | i++               |
                  |                       +-------------------+
                  | no                            |
                  v                               +-----> (loop back)
        +-------------------+
        | tail = prev->next |
        +-------------------+
                  |
                  v
          +---------------+
          | i = left       |
          +---------------+
                  |
                  v
          +--------------+     yes    +-------------------------+
          | i < right ?  |----------->| move = tail->next       |
          +--------------+            | tail->next = move->next |
              |                       | move->next = prev->next |
              | no                    | prev->next = move       |
              v                       | i++                     |
    +------------------+              +-------------------------+
    | return dummy.next|                      |
    +------------------+                      +-----> (loop back)
```

---

## Function Reference

All functions in `optimized/main.c`:

### `struct ListNode *insert_list(int *nums, int size)`

Build a singly linked list from an integer array. Iterates from the last
element to the first, prepending each node so the final list preserves the
array order. Returns the head of the new list.

### `void show_list(char *type, struct ListNode *head)`

Print a list to stdout in the format `type: [ v1 v2 v3 ]`. Used for
debugging and test output.

### `struct ListNode *reverseBetween(struct ListNode *head, int left, int right)`

The core algorithm. Reverse the sub-list from position `left` to position
`right` (1-indexed) using the head-insertion technique. Uses a stack-based
dummy node. Returns the (possibly new) head of the modified list.

### `static void free_list(struct ListNode *head)`

Free all nodes in a linked list. Walks the list, freeing each node. Used
in test cleanup to avoid memory leaks from `insert_list` allocations.

### `static int check_list(struct ListNode *head, int *expected, int len)`

Verify that a linked list matches an expected integer array. Returns 1 if
every element matches and the list has exactly `len` nodes; returns 0
otherwise.

### `static void run_test(const char *name, int *nums, int len, int left, int right, int *expected, int expected_len)`

Run a single test case. Builds a list from `nums`, calls `reverseBetween`,
prints input/output, asserts the result matches `expected`, then frees the
list. Prints `PASS` on success; aborts on failure via `assert`.

---

## Where It Gets Tricky

### (a) Stack dummy vs malloc dummy

The original code does this:

```c
dummy = malloc(sizeof(*dummy));
dummy->next = head;
```

...and never calls `free(dummy)`. That is a memory leak every time the
function is called. The optimized version declares the dummy on the stack:

```c
struct ListNode dummy = { .val = 0, .next = head };
```

This is safe because we never return a pointer *to* the dummy itself. We
return `dummy.next`, which points into the heap-allocated list. The dummy
vanishes when the function returns, and that is fine — nobody holds a
reference to it.

If you ever needed to return the dummy node itself (you would not, but
hypothetically), stack allocation would be a use-after-free bug. The key
insight: the dummy is a *temporary scaffold*, not part of the result.

### (b) `tail` stays put

This is the most confusing part if you are reading the code for the first
time. `tail` is assigned once:

```c
tail = prev->next;
```

It always points to the same node — the node originally at position `left`.
It never moves to a different node. But the *position* of that node changes:
with each iteration, another node leapfrogs over it, pushing it one position
to the right. After `right - left` iterations, the node originally at
position `left` now sits at position `right`.

The name `tail` reflects its *final role*: it ends up as the tail of the
reversed segment.

In the original code this pointer is called `cur`, which is misleading
because `cur` usually implies "the pointer that advances." Here it does not
advance — everything else moves around it.

### (c) The reversal loop body: order matters

The four lines in the loop must execute in exactly this order:

```c
move = tail->next;         // 1. Extract: grab the node to move
tail->next = move->next;   // 2. Detach:  skip move in the chain
move->next = prev->next;   // 3. Link:    move points to current front
prev->next = move;         // 4. Anchor:  prev now points to move
```

If you swap lines 3 and 4, you lose the reference to the old front of the
reversed segment. If you swap lines 1 and 2, you lose the reference to
`move` itself. Each line depends on state that the *next* line is about to
overwrite.

Think of it like a card trick: you must pick the card up before you close
the gap, and you must remember where the top of the deck is before you
place the card there.

---

## Complexity Analysis

**Time: O(n)**

- Navigation loop: at most `left - 1` steps.
- Reversal loop: exactly `right - left` steps.
- Total: at most `right - 1` steps, which is at most `n - 1`. Linear.

**Space: O(1)**

Truly O(1). The dummy node lives on the stack — no heap allocation at all.
The original version allocates a dummy on the heap (`malloc`) and never
frees it, so while it is also "O(1) extra space" in Big-O terms, it leaks
that allocation. The optimized version has zero leaks and zero allocations.

Only a fixed number of pointer variables (`prev`, `tail`, `move`, `i`) are
used regardless of input size.

---

## Summary

The optimized solution uses the exact same O(n) head-insertion algorithm as
the original. Every improvement is purely stylistic:

- **No memory leak** — stack-based dummy instead of `malloc` without `free`.
- **Descriptive variable names** — `prev`, `tail`, `move` tell you what each
  pointer *means*, not just what letter of the alphabet was next.
- **Simpler navigation** — one pointer, one loop, one assignment afterward.
- **Cleaner reversal body** — extract, detach, link, anchor. Four lines,
  four verbs, easy to trace.

Same algorithm. Same complexity. Easier to read, harder to mess up.
