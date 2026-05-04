# LeetCode 25 -- Reverse Nodes in k-Group (Optimized)

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Core Idea](#core-idea)
3. [Step-by-Step Walkthrough](#step-by-step-walkthrough)
4. [ASCII Flowchart](#ascii-flowchart)
5. [Where It Gets Tricky](#where-it-gets-tricky)
6. [Comparison with Original](#comparison-with-original)
7. [Complexity Analysis](#complexity-analysis)
8. [Function Reference](#function-reference)
9. [Summary](#summary)

---

## Problem Statement

Given a linked list, reverse the nodes in groups of `k`. If the number of
remaining nodes is less than `k`, leave them in their original order. You may
not alter the values in the nodes -- only the nodes themselves may be changed.

Example: `[1,2,3,4,5]`, `k=2` produces `[2,1,4,3,5]`.

---

## Core Idea

The algorithm uses **head-insertion reversal**: for each k-node group, the first
node of the group stays in place (it will become the tail after reversal), and
the remaining `k-1` nodes are plucked one by one and inserted at the front of
the group.

The optimized version improves on the original in two ways:

1. **Stack-allocated dummy node.** Instead of `malloc`-ing a dummy node (which
   the original never frees, causing a memory leak), this version declares
   `struct ListNode dummy = { .val = 0, .next = head }` on the stack. It
   disappears automatically when the function returns.

2. **Self-documenting variable names.** The original uses `pre`, `cur`, and
   `tmp`, where `cur` is misleading -- it does not advance within a group. The
   optimized version uses `prev` (the node before the current group), `tail`
   (the first node of the group, which becomes its tail after reversal), and
   `move` (the node being plucked and re-inserted at the front).

A single counting pass determines the total number of nodes up front, so the
main loop knows exactly when fewer than `k` nodes remain and can stop.

---

## Step-by-Step Walkthrough

Input: `[1,2,3,4,5]`, `k=2`.

### Initialization

Count nodes: `size = 5`. Set `prev = &dummy`, `tail = head` (node 1).

```
  prev   tail
   |      |
   v      v
 dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
```

### Group 1 (size=5 >= k=2)

The inner loop runs `k-1 = 1` iteration.

**i=0:**

```
move = tail->next          -->  move = node 2
tail->next = move->next    -->  node 1 -> node 3   (detach node 2)
move->next = prev->next    -->  node 2 -> node 1   (point move at old front)
prev->next = move          -->  dummy  -> node 2   (insert move at front)
```

After group 1:

```
                    prev   tail
                     |      |
                     v      v
 dummy -> 2 -> 1 -> 3 -> 4 -> 5 -> NULL
```

Wait -- `prev` and `tail` have not advanced yet. The post-group update is:

```
size = 5 - 2 = 3
prev = tail            -->  prev = node 1
tail = tail->next      -->  tail = node 3
```

```
              prev   tail
               |      |
               v      v
 dummy -> 2 -> 1 -> 3 -> 4 -> 5 -> NULL
```

### Group 2 (size=3 >= k=2)

**i=0:**

```
move = tail->next          -->  move = node 4
tail->next = move->next    -->  node 3 -> node 5   (detach node 4)
move->next = prev->next    -->  node 4 -> node 3   (point move at old front)
prev->next = move          -->  node 1 -> node 4   (insert move at front)
```

After group 2:

```
size = 3 - 2 = 1
prev = tail            -->  prev = node 3
tail = tail->next      -->  tail = node 5
```

```
                            prev   tail
                             |      |
                             v      v
 dummy -> 2 -> 1 -> 4 -> 3 -> 5 -> NULL
```

### Termination (size=1 < k=2)

The while loop exits. Return `dummy.next`:

```
 2 -> 1 -> 4 -> 3 -> 5 -> NULL
```

Output: `[2,1,4,3,5]`.

---

## ASCII Flowchart

```
                   +---------------------------+
                   |  Allocate dummy on stack   |
                   |  dummy.next = head         |
                   +---------------------------+
                                |
                                v
                   +---------------------------+
                   |  Count nodes: size = n     |
                   |  for (cur=head; cur;       |
                   |       cur=cur->next)       |
                   |      size++                |
                   +---------------------------+
                                |
                                v
                   +---------------------------+
                   |  prev = &dummy             |
                   |  tail = head               |
                   +---------------------------+
                                |
                                v
                   +---------------------------+
               +-->|  size >= k ?               |---No---> return dummy.next
               |   +---------------------------+
               |                |
               |               Yes
               |                |
               |                v
               |   +---------------------------+
               |   |  for i = 0 .. k-2:        |
               |   |    move = tail->next       |
               |   |    tail->next = move->next |
               |   |    move->next = prev->next |
               |   |    prev->next = move       |
               |   +---------------------------+
               |                |
               |                v
               |   +---------------------------+
               |   |  size -= k                 |
               |   |  prev = tail               |
               |   |  tail = tail->next         |
               +---+---------------------------+
```

---

## Where It Gets Tricky

1. **tail does not move inside the inner loop.** The node pointed to by `tail`
   was the first node of the group before reversal. After the `k-1` pluck-and-
   insert operations, that same node is now the last node of the reversed group.
   Understanding this is the single most important insight.

2. **The inner loop runs `k-1` times, not `k` times.** Reversing a group of `k`
   nodes requires only `k-1` insertions because the first node (tail) stays
   put -- the other `k-1` nodes move in front of it.

3. **prev->next vs. tail->next.** `prev->next` always points to the current
   front of the group (which changes on every iteration of the inner loop).
   `tail->next` always points to the next node to be plucked. Confusing these
   two is the most common source of bugs.

4. **Forgetting to advance prev and tail after a group.** After reversing a
   group, `prev` must move to `tail` (the new last node of the group), and
   `tail` must move to `tail->next` (the first node of the next group). If
   either update is missing, the next group will be spliced into the wrong
   position.

5. **Off-by-one in size counting.** If you count nodes incorrectly, you might
   attempt to reverse a partial group, causing NULL pointer dereferences inside
   the inner loop.

---

## Comparison with Original

| Aspect | Original | Optimized |
|---|---|---|
| Dummy node | `malloc(sizeof(*dummy))` -- heap-allocated, never freed (memory leak) | `struct ListNode dummy = { .val = 0, .next = head }` -- stack-allocated, automatically cleaned up |
| Variable names | `pre`, `cur`, `tmp` -- `cur` is misleading because it does not advance within a group | `prev`, `tail`, `move` -- each name describes the role precisely |
| Counting loop | `cur = head; while (cur) { size++; cur = cur->next; }` -- uses a separate assignment and while loop | `for (cur = head; cur; cur = cur->next) size++` -- compact for-loop, reuses `cur` locally |
| Inner-loop clarity | `tmp = pre->next; pre->next = cur->next; cur->next = cur->next->next; pre->next->next = tmp;` -- reads the same node through two different paths (`cur->next` and `pre->next`) | `move = tail->next; tail->next = move->next; move->next = prev->next; prev->next = move;` -- every operation goes through a named pointer, no aliased access |
| Asymptotic complexity | O(n) time, O(1) space (ignoring the leaked malloc) | O(n) time, O(1) space (truly) |
| Correctness | Functionally correct, but leaks the dummy node | Functionally correct, no leaks |

---

## Complexity Analysis

**Time: O(n)**

The counting pass visits every node once: O(n). The main loop visits every node
in a full group exactly once during the inner loop (each of the `k-1` iterations
does O(1) work). Across all groups the total inner-loop iterations sum to at
most `n - (n mod k)`, which is O(n). The two passes together give O(n) + O(n) =
O(n).

**Space: O(1)**

The dummy node lives on the stack. The only other variables are a fixed number
of pointers (`prev`, `tail`, `move`, `cur`) and two integers (`size`, `i`). No
heap allocation, no recursion.

---

## Function Reference

All functions are defined in `optimized/main.c`.

### reverseKGroup

```c
struct ListNode* reverseKGroup(struct ListNode* head, int k);
```

- **Purpose:** Reverse every group of `k` nodes in the linked list. Nodes in a
  final group smaller than `k` are left in their original order.
- **Parameters:**
  - `head` -- pointer to the first node of the linked list (may be NULL).
  - `k` -- group size (positive integer).
- **Return value:** Pointer to the head of the modified list.

### insert_list

```c
struct ListNode *insert_list(int *nums, int size);
```

- **Purpose:** Build a singly-linked list from an integer array. Nodes are
  allocated with `malloc` and linked in the same order as the array.
- **Parameters:**
  - `nums` -- pointer to an array of integers.
  - `size` -- number of elements in `nums`.
- **Return value:** Pointer to the head of the newly created list, or NULL if
  `size` is 0.

### show_list

```c
void show_list(char *type, struct ListNode* head);
```

- **Purpose:** Print a linked list to stdout in the format
  `type: [ v1 v2 ... ]`.
- **Parameters:**
  - `type` -- label string printed before the list (e.g. `"Input "`,
    `"Output"`).
  - `head` -- pointer to the first node of the list (may be NULL).
- **Return value:** None.

### free_list

```c
static void free_list(struct ListNode *head);
```

- **Purpose:** Free all nodes in a linked list.
- **Parameters:**
  - `head` -- pointer to the first node of the list (may be NULL).
- **Return value:** None.

### check_list

```c
static int check_list(struct ListNode *head, int *expected, int len);
```

- **Purpose:** Verify that a linked list matches an expected array of values,
  element by element, and that the list has exactly `len` nodes.
- **Parameters:**
  - `head` -- pointer to the first node of the list.
  - `expected` -- pointer to an array of expected integer values.
  - `len` -- number of elements in `expected`.
- **Return value:** 1 if the list matches, 0 otherwise.

### run_test

```c
static void run_test(const char *name, int *nums, int len,
                     int k, int *expected, int expected_len);
```

- **Purpose:** Execute a single test case: build a list from `nums`, call
  `reverseKGroup`, print input and output, assert the result matches
  `expected`, and free the result list.
- **Parameters:**
  - `name` -- descriptive name for the test case (printed to stdout).
  - `nums` -- pointer to the input array of integers.
  - `len` -- number of elements in `nums`.
  - `k` -- group size passed to `reverseKGroup`.
  - `expected` -- pointer to the expected output array.
  - `expected_len` -- number of elements in `expected`.
- **Return value:** None. Aborts via `assert` on failure.

---

## Summary

The optimized version of Reverse Nodes in k-Group eliminates the memory leak in
the original by placing the dummy node on the stack with a designated
initializer, renames the pointer variables to reflect their actual roles (`prev`
for the node before the group, `tail` for the node that starts as the group head
and ends as the group tail, `move` for the node being plucked), and condenses
the counting pass into a single for-loop. The underlying algorithm is unchanged
-- head-insertion reversal applied group by group -- and the complexity remains
O(n) time and O(1) space, but the code is now leak-free and substantially easier
to read.
