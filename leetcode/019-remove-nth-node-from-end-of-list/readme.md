# 19 - Remove Nth Node From End of List

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Function Reference](#function-reference)
- [Summary](#summary)

---

## Problem Statement

Given the `head` of a singly linked list, remove the nth node from the **end**
of the list and return its head.

For example, given `[1, 2, 3, 4, 5]` and `n = 2`, the 2nd node from the end
is `4`. Remove it and the list becomes `[1, 2, 3, 5]`.

**Follow up:** Could you do this in one pass?

**Constraints:**

- The number of nodes in the list is `sz`.
- `1 <= sz <= 30`
- `0 <= Node.val <= 100`
- `1 <= n <= sz`

---

## Core Idea

The problem is: remove the nth node from the end, but you don't know the length
of the list. You could count the nodes first, then walk again to the right
position -- but that's two passes. Can you do it in one?

Yes. The trick is the **two-pointer gap technique**:

> Imagine two people walking along a narrow bridge, one behind the other,
> connected by a rope exactly `n` steps long. When the person in front steps
> off the end of the bridge, the person in back is standing exactly `n` steps
> from the end. That's your target.

In code:

1. Start both `fast` and `slow` at the head.
2. Advance `fast` by `n` steps. Now there's an `n`-node gap between them.
3. Move both pointers forward together, one step at a time, until `fast` falls
   off the end (`NULL`).
4. `slow` is now sitting on the node that needs to be removed.

The gap is maintained throughout the walk. When `fast` hits `NULL`, it has
traveled exactly `n` nodes past where `slow` is -- so `slow` is the nth node
from the end.

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
	struct ListNode dummy = {0, head};
	struct ListNode *slow, *fast, *prev;
	int i;

	slow = head;
	fast = head;
	prev = &dummy;
	for (i = 0; i < n; i++)
		fast = fast->next;

	while (fast) {
		prev = slow;
		slow = slow->next;
		fast = fast->next;
	}

	prev->next = slow->next;

	return dummy.next;
}
```

Four pointers do the work:

| Pointer | Role |
|---------|------|
| `dummy` | Stack-allocated sentinel node whose `next` points to `head`. Handles the edge case of removing the head node. |
| `fast`  | Lead pointer -- runs `n` steps ahead of `slow`. |
| `slow`  | Trails `fast` by exactly `n` nodes. Lands on the target node. |
| `prev`  | Tracks the node immediately before `slow`, so we can unlink the target. |

---

## Step-by-Step Walkthrough

Walk through the list `[1, 2, 3, 4, 5]` with `n = 2`:

```
Initial list:
  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
```

**Phase 1: Advance `fast` by n = 2 steps.**

| Step | `fast` moves to |
|------|-----------------|
| i=0  | Node 2          |
| i=1  | Node 3          |

After phase 1:

```
  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
           ^         ^
          slow      fast
  prev = &dummy
```

The gap between `slow` and `fast` is exactly 2 nodes.

**Phase 2: Move all three pointers together until `fast` is NULL.**

| Iter | `prev` (before) | `slow` (before) | `fast` (before) | Action |
|------|------------------|-----------------|-----------------|--------|
| 1    | dummy            | Node 1          | Node 3          | prev=Node1, slow=Node2, fast=Node4 |
| 2    | Node 1           | Node 2          | Node 4          | prev=Node2, slow=Node3, fast=Node5 |
| 3    | Node 2           | Node 3          | Node 5          | prev=Node3, slow=Node4, fast=NULL   |

Now `fast` is `NULL`, so the loop exits.

```
  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
                      ^    ^
                    prev  slow
```

`slow` is Node 4 -- the 2nd node from the end. That's our target.

**Phase 3: Remove the target.**

```c
prev->next = slow->next;   // Node3->next = Node5
```

```
  dummy -> 1 -> 2 -> 3 --------> 5 -> NULL
                           4 (orphaned)
```

Return `dummy.next`, which is Node 1 -- the head of the modified list
`[1, 2, 3, 5]`.

---

## ASCII Flowchart

```
                          START
                            |
                            v
                +-------------------------+
                | dummy = {0, head}       |
                | slow = head             |
                | fast = head             |
                | prev = &dummy           |
                +-------------------------+
                            |
                            v
                   +----------------+
               +-->| i < n ?        |
               |   +----------------+
               |     |          |
               |    yes         no
               |     |          |
               |     v          |
               |  fast = fast   |
               |    ->next      |
               |  i++           |
               |     |          |
               +-----+         |
                                |
                                v
                       +---------------+
                   +-->| fast != NULL? |
                   |   +---------------+
                   |     |          |
                   |    yes         no
                   |     |          |
                   |     v          v
                   | prev = slow    prev->next =
                   | slow = slow      slow->next
                   |   ->next           |
                   | fast = fast        v
                   |   ->next     return dummy.next
                   |     |              |
                   +-----+             END
```

---

## Where It Gets Tricky

### (a) The dummy node is essential for removing the head

Consider the list `[1]` with `n = 1`. The only node is the target. After
advancing `fast` by 1 step, `fast` is already `NULL`. The `while` loop never
executes. Now `slow` is the head and `prev` is `&dummy`.

Without the dummy node, `prev` would have nothing to point to -- there's no
node before the head. The dummy gives us a stable anchor: `prev->next =
slow->next` becomes `dummy.next = NULL`, and we return `dummy.next` which is
`NULL`. Correct.

This is the classic reason for sentinel/dummy nodes in linked list problems:
they eliminate the special case of removing the first element.

### (b) The gap invariant

The correctness of this algorithm rests on one invariant: after phase 1, there
are exactly `n` nodes between `slow` and `fast` (inclusive of the node `fast`
points to, exclusive of `slow`). In phase 2, both pointers move at the same
speed, so the gap is preserved. When `fast` reaches `NULL`, `slow` is exactly
`n` positions from the end.

If `n` equals the list length, `fast` becomes `NULL` after phase 1 and the
loop never runs. `slow` still points to the head -- the nth node from the end
when n equals the length. The dummy node catches this case cleanly.

### (c) The `prev` pointer is redundant

The implementation maintains a separate `prev` pointer that tracks the node
before `slow`. This works, but it's unnecessary. If you started `slow` at
`&dummy` instead of `head`, then after the loop `slow` would point to the
node *before* the target:

```c
// Simplified alternative
slow = &dummy;
fast = head;
for (i = 0; i < n; i++)
    fast = fast->next;
while (fast) {
    slow = slow->next;
    fast = fast->next;
}
slow->next = slow->next->next;
```

This eliminates one pointer variable and simplifies the loop body. The current
code is correct, just slightly more verbose than it needs to be.

### (d) The removed node is not freed

After `prev->next = slow->next`, the node that `slow` points to is orphaned --
it's still allocated on the heap but nothing references it. This is a minor
memory leak. In a LeetCode submission this doesn't matter (the judge doesn't
check for leaks), but in production code you'd want:

```c
struct ListNode *target = slow;
prev->next = slow->next;
free(target);
```

### (e) The dummy node lives on the stack

The line `struct ListNode dummy = {0, head}` creates a `ListNode` on the stack,
not the heap. This is good -- it means there's no `malloc` to clean up and no
risk of forgetting to `free` it. The dummy goes out of scope when the function
returns, which is exactly what we want.

---

## Complexity Analysis

| Metric | Value | Justification |
|--------|-------|---------------|
| Time   | O(n)  | Phase 1 advances `fast` by `n` steps. Phase 2 advances all pointers until `fast` reaches `NULL`, which takes at most `sz - n` steps. Total: `n + (sz - n) = sz` steps, where `sz` is the list length. One pass through the list. |
| Space  | O(1)  | Only a fixed number of pointer variables (`dummy`, `slow`, `fast`, `prev`) regardless of input size. The dummy node is stack-allocated, not heap-allocated. |

---

## Function Reference

All functions are defined in `main.c`.

### `removeNthFromEnd`

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n);
```

- **Purpose:** Removes the nth node from the end of the linked list using the
  two-pointer gap technique.
- **Parameters:**
  - `head` -- pointer to the head of the singly-linked list.
  - `n` -- 1-based position from the end of the list (guaranteed valid).
- **Return value:** Pointer to the head of the modified list. May differ from
  the original `head` if the first node was removed.

### `insert_list`

```c
struct ListNode *insert_list(int *nums, int size);
```

- **Purpose:** Builds a singly-linked list from an integer array, preserving the
  array order.
- **Parameters:**
  - `nums` -- array of integer values.
  - `size` -- number of elements in `nums`.
- **Return value:** Pointer to the head of the newly created list.

### `show_list`

```c
void show_list(char *type, struct ListNode *head);
```

- **Purpose:** Prints every value in the list to stdout, prefixed by a label.
- **Parameters:**
  - `type` -- label string printed before the list (e.g., `"Input "`).
  - `head` -- pointer to the head of the list.
- **Return value:** None.

### `free_list`

```c
static void free_list(struct ListNode *head);
```

- **Purpose:** Frees every node in the list, walking from head to tail.
- **Parameters:**
  - `head` -- pointer to the head of the list.
- **Return value:** None.

### `check_list`

```c
static int check_list(struct ListNode *head, int *expected, int len);
```

- **Purpose:** Validates that the list matches an expected sequence of values.
- **Parameters:**
  - `head` -- pointer to the head of the list.
  - `expected` -- array of expected integer values.
  - `len` -- length of the expected array.
- **Return value:** `1` if the list matches exactly, `0` otherwise.

### `run_test`

```c
static void run_test(const char *name, int *nums, int len,
                     int n, int *expected, int expected_len);
```

- **Purpose:** Runs a single end-to-end test case: builds a list, removes the
  nth node from the end, prints the result, and asserts correctness.
- **Parameters:**
  - `name` -- descriptive name for the test (printed to stdout).
  - `nums` -- array of values for the input list.
  - `len` -- length of `nums`.
  - `n` -- the position from the end to remove.
  - `expected` -- array of values the list should contain after removal.
  - `expected_len` -- length of `expected`.
- **Return value:** None. Aborts via `assert` on failure.

---

## Summary

LeetCode 19 asks you to remove the nth node from the end of a linked list,
ideally in a single pass. The solution uses the two-pointer gap technique:
advance a `fast` pointer `n` steps ahead of a `slow` pointer, then move both
together until `fast` hits `NULL`. At that point `slow` sits on the target
node, and a `prev` pointer (tracking one step behind `slow`) lets us unlink
it with `prev->next = slow->next`. A stack-allocated dummy node acts as a
sentinel before the head, cleanly handling the edge case where the head itself
is the node to remove. The algorithm runs in O(n) time and O(1) space. Two
minor issues worth noting: the removed node is not freed (a small memory leak),
and the `prev` pointer is redundant -- starting `slow` from the dummy node
instead of `head` would eliminate it entirely.
