# LeetCode 237 -- Delete Node in a Linked List

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Core Idea](#core-idea)
3. [Step-by-Step Walkthrough](#step-by-step-walkthrough)
4. [ASCII Flowchart](#ascii-flowchart)
5. [Where It Gets Tricky](#where-it-gets-tricky)
6. [Complexity Analysis](#complexity-analysis)
7. [Function Reference](#function-reference)
8. [Summary](#summary)

---

## Problem Statement

You are given a pointer to a single node inside a singly-linked list. Your job
is to delete that node from the list. There are two constraints that make this
unusual:

- **You do NOT have access to the head of the list.** The only thing you receive
  is the node itself. You cannot traverse backward, and you cannot start from
  the beginning.
- **The given node is guaranteed not to be the tail.** There is always at least
  one node after it.

You must modify the list in-place so that, when someone later traverses from the
head, the node you were given appears to be gone.

---

## Core Idea

In a normal linked-list deletion you would find the *previous* node and make it
skip over the target. But here you have no way to reach the previous node.

The trick is: **instead of removing the node, make the node become its
successor, then remove the successor.**

Think of it like a row of people holding name tags. You cannot pull someone out
of the line, but you *can* tell them to grab the next person's name tag and then
have the next person step out. To every outside observer the effect is the same
-- the name you wanted gone is gone.

In code this is two assignments:

```c
node->val  = node->next->val;   /* copy the next node's value into this node */
node->next = node->next->next;  /* skip over the next node                   */
```

The node that was originally `node->next` is now orphaned from the list. Its
data has been copied forward and no pointer references it any longer.

---

## Step-by-Step Walkthrough

Starting list: `[4, 5, 1, 9]`. We want to delete the node whose value is **5**.

### Initial state

```
 head
  |
  v
+---+    +---+    +---+    +---+
| 4 |--->| 5 |--->| 1 |--->| 9 |--->NULL
+---+    +---+    +---+    +---+
           ^
           |
         node  (the pointer we receive)
```

We have a pointer to the node containing 5. We have no pointer to the node
containing 4 (the previous node) and no pointer to the head.

### Step 1 -- Copy the next node's value

```c
node->val = node->next->val;   /* node->next is the "1" node */
```

The value 1 overwrites the value 5 in the current node:

```
 head
  |
  v
+---+    +---+    +---+    +---+
| 4 |--->| 1 |--->| 1 |--->| 9 |--->NULL
+---+    +---+    +---+    +---+
           ^        ^
           |        |
         node    node->next
```

Now there are two nodes with value 1. The list is temporarily inconsistent.

### Step 2 -- Skip the next node

```c
node->next = node->next->next;   /* bypass the old "1" node */
```

The current node's `next` pointer now points to the node containing 9, skipping
over the duplicate:

```
 head
  |
  v
+---+    +---+         +---+
| 4 |--->| 1 |-------->| 9 |--->NULL
+---+    +---+         +---+
           ^
           |
         node

         (the old "1" node is now orphaned -- nothing points to it)
```

### Final result

Traversing from the head yields `[4, 1, 9]`. The value 5 is gone. From the
perspective of any external observer, node 5 has been deleted.

---

## ASCII Flowchart

```
            +-------------------------------+
            |  Receive pointer to 'node'    |
            +-------------------------------+
                          |
                          v
            +-------------------------------+
            |  Copy value from next node:   |
            |  node->val = node->next->val  |
            +-------------------------------+
                          |
                          v
            +-------------------------------+
            |  Bypass next node:            |
            |  node->next = node->next->next|
            +-------------------------------+
                          |
                          v
            +-------------------------------+
            |  Done -- node now holds the   |
            |  next node's data and the old |
            |  next node is orphaned        |
            +-------------------------------+
```

There are no branches or loops. The algorithm is a straight-line sequence of
two pointer operations, which is what makes it O(1).

---

## Where It Gets Tricky

### Memory leak

After `node->next = node->next->next`, the old next node is unreachable but has
not been freed. In the implementation in `main.c` the line

```c
node->next = node->next->next;
```

overwrites the only pointer to that node. A correct version would save the
pointer first:

```c
struct ListNode *tmp = node->next;
node->val  = tmp->val;
node->next = tmp->next;
free(tmp);
```

The current code does not do this, so the orphaned node leaks.

### You cannot delete the tail

The algorithm copies from `node->next`. If `node` is the tail, `node->next` is
NULL and the dereference `node->next->val` is undefined behavior. The problem
statement guarantees the node is never the tail, so this case does not arise in
practice, but the function has no guard against it.

### This does not truly "delete" the node

The physical memory block that was originally `node` still exists in the list.
What gets removed is the *next* node's memory block. Any external pointer that
someone held to the next node now dangles. In most interview contexts this
subtlety is accepted, but in production code it would be a concern.

---

## Complexity Analysis

| Metric | Value | Justification |
|--------|-------|---------------|
| Time   | O(1)  | Exactly two assignments, no traversal. |
| Space  | O(1)  | No extra memory allocated; only pointer manipulation. |

---

## Function Reference

All functions are defined in `main.c`.

### `deleteNode`

```c
void deleteNode(struct ListNode *node);
```

- **Purpose:** Deletes the given node from its linked list without access to the
  head pointer.
- **Parameters:**
  - `node` -- pointer to the node to delete. Must not be the tail.
- **Return value:** None.

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

### `find_node`

```c
static struct ListNode *find_node(struct ListNode *head, int val);
```

- **Purpose:** Searches the list for the first node whose value equals `val`.
- **Parameters:**
  - `head` -- pointer to the head of the list.
  - `val` -- the value to search for.
- **Return value:** Pointer to the matching node, or `NULL` if not found.

### `run_test`

```c
static void run_test(const char *name, int *nums, int len,
                     int del_val, int *expected, int expected_len);
```

- **Purpose:** Runs a single end-to-end test case: builds a list, finds and
  deletes a node, prints the result, and asserts correctness.
- **Parameters:**
  - `name` -- descriptive name for the test (printed to stdout).
  - `nums` -- array of values for the input list.
  - `len` -- length of `nums`.
  - `del_val` -- value of the node to delete.
  - `expected` -- array of values the list should contain after deletion.
  - `expected_len` -- length of `expected`.
- **Return value:** None. Aborts via `assert` on failure.

---

## Summary

LeetCode 237 asks you to delete a linked-list node when all you have is a
pointer to that node -- no head, no previous node. The solution is a two-line
trick: copy the next node's value into the current node, then redirect the
current node's `next` pointer to skip over the (now redundant) next node. This
runs in O(1) time and O(1) space. The main subtlety is that the code does not
free the orphaned node, creating a minor memory leak, and the technique cannot
work on the tail node because there is no successor to copy from.
