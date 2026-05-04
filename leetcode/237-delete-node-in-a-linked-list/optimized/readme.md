# LeetCode 237 -- Delete Node in a Linked List (Optimized)

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

You are given a node in a singly-linked list. You do **not** have access to
the head of the list. Your job is to delete that node from the list.

Constraints:

- The node to delete is **never** the tail node.
- All node values are unique.
- The list has at least two nodes.

Because you only receive a pointer to the node itself (not to its
predecessor), you cannot unlink it in the traditional way. Instead you must
use the copy-and-skip trick.

---

## Core Idea

The optimized algorithm works in three steps:

1. **Save the victim.** Store a pointer to `node->next` in a local variable
   called `victim`. This is the node whose memory will be freed.
2. **Copy and skip.** Copy `victim->val` into `node->val`, then point
   `node->next` past the victim to `victim->next`. From the outside the
   list looks as if the original node was removed.
3. **Free the victim.** Call `free(victim)` to release the heap memory that
   the skipped node occupied.

```c
void deleteNode(struct ListNode* node)
{
	struct ListNode *victim = node->next;

	node->val = victim->val;
	node->next = victim->next;
	free(victim);
}
```

The key improvement over the original two-line version is the explicit
`free(victim)`. Without it, the node that gets bypassed is still allocated
on the heap but no pointer references it any longer -- a textbook memory
leak.

---

## Step-by-Step Walkthrough

Input list: `[4, 5, 1, 9]`. Delete the node whose value is `5`.

We receive a pointer `node` that points to the node containing `5`.

### State 0 -- Initial list

```
  node
   |
   v
 [4] -> [5] -> [1] -> [9] -> NULL
```

### Step 1 -- Save the victim

```c
struct ListNode *victim = node->next;
```

```
  node   victim
   |       |
   v       v
 [4] -> [5] -> [1] -> [9] -> NULL
```

`victim` now holds the address of the `[1]` node (the node physically after
`node`).

### Step 2 -- Copy value

```c
node->val = victim->val;
```

```
  node   victim
   |       |
   v       v
 [4] -> [1] -> [1] -> [9] -> NULL
```

The value `1` from `victim` overwrites the `5` in `node`.

### Step 3 -- Skip over victim

```c
node->next = victim->next;
```

```
  node   victim
   |       |
   v       v
 [4] -> [1] -+  [1] -> [9] -> NULL
              |          ^
              +----------+
```

`node->next` now points to `[9]`, bypassing the victim. The victim is still
allocated but is no longer reachable from the list.

### Step 4 -- Free the victim

```c
free(victim);
```

```
  node
   |
   v
 [4] -> [1] -> [9] -> NULL
```

The victim's memory is returned to the heap. The list is now
`[4, 1, 9]` with no leaked memory.

---

## ASCII Flowchart

```
+------------------------------+
|  Entry: deleteNode(node)     |
+------------------------------+
              |
              v
+------------------------------+
|  victim = node->next         |
|  (save pointer before we     |
|   lose track of it)          |
+------------------------------+
              |
              v
+------------------------------+
|  node->val = victim->val     |
|  (copy the next node's value |
|   into the current node)     |
+------------------------------+
              |
              v
+------------------------------+
|  node->next = victim->next   |
|  (skip over the victim in    |
|   the linked list)           |
+------------------------------+
              |
              v
+------------------------------+
|  free(victim)                |
|  (release the bypassed       |
|   node's heap memory)        |
+------------------------------+
              |
              v
+------------------------------+
|  Return                      |
+------------------------------+
```

---

## Where It Gets Tricky

### Why saving `victim` first matters

Consider what happens if you do **not** save `node->next` before
overwriting `node->next`:

```c
/* WRONG -- loses the pointer before freeing */
node->val  = node->next->val;
node->next = node->next->next;   /* old node->next is now unreachable */
free(???);                        /* we no longer have a pointer to free */
```

After the second line executes, the original `node->next` has been
overwritten. There is no way to reach the orphaned node, and calling
`free` on it is impossible. The memory is leaked.

By storing `victim = node->next` on the very first line, the pointer is
preserved in a local variable regardless of what happens to `node->next`
afterward.

### Dangling pointer risks

After `free(victim)`, the local variable `victim` holds a dangling pointer.
Dereferencing it would be undefined behavior. In this implementation it does
not matter because `victim` goes out of scope immediately after `free`, but
in more complex code you would set `victim = NULL` as a safety measure.

### Not applicable to the tail node

The algorithm copies from `node->next`. If `node` is the tail,
`node->next` is `NULL` and `victim` would be `NULL`. Dereferencing it
would crash. The problem guarantees this case never occurs.

---

## Comparison with Original

| Aspect               | Original (2 lines)            | Optimized (with `free`)          |
|----------------------|-------------------------------|----------------------------------|
| Lines of code        | 2                             | 4                                |
| Saves `node->next`   | No                            | Yes, as `victim`                 |
| Copies value         | `node->val = node->next->val` | `node->val = victim->val`        |
| Skips node           | `node->next = node->next->next` | `node->next = victim->next`    |
| Frees skipped node   | No                            | Yes, `free(victim)`              |
| Memory leak          | Yes -- bypassed node is never freed | No -- victim is explicitly freed |
| Time complexity      | O(1)                          | O(1)                             |
| Space complexity     | O(1)                          | O(1)                             |
| Valgrind clean       | No (definitely lost bytes)    | Yes                              |

The original version works correctly from a logical standpoint: the list
appears to have the right values in the right order. But every call to
`deleteNode` leaks one `struct ListNode` worth of heap memory. In a
long-running program or a loop that deletes many nodes, this adds up. The
optimized version prevents this by freeing the victim at the cost of two
extra lines of code.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| Time   | O(1)  | The function performs a fixed number of pointer operations and one `free` call, none of which depend on the size of the list. |
| Space  | O(1)  | Only one local pointer variable (`victim`) is allocated on the stack. No auxiliary data structures are used. |

---

## Function Reference

All functions are defined in `optimized/main.c`.

| Function | Signature | Description |
|----------|-----------|-------------|
| `deleteNode` | `void deleteNode(struct ListNode *node)` | The optimized delete algorithm. Saves `node->next` as `victim`, copies the victim's value into `node`, relinks `node->next` to skip the victim, then frees the victim. |
| `insert_list` | `struct ListNode *insert_list(int *nums, int size)` | Builds a singly-linked list from an integer array. Iterates from the last element to the first, prepending each new node to the head. Returns the head pointer. |
| `show_list` | `void show_list(char *type, struct ListNode *head)` | Prints the linked list to stdout. `type` is a label printed before the list (e.g., `"Input "` or `"Output"`). |
| `free_list` | `static void free_list(struct ListNode *head)` | Walks the list from head to tail, freeing every node. Used in the test harness to clean up after each test case. |
| `check_list` | `static int check_list(struct ListNode *head, int *expected, int len)` | Validates that the linked list matches an expected integer array. Returns `1` if every value matches and the list has exactly `len` nodes, `0` otherwise. |
| `find_node` | `static struct ListNode *find_node(struct ListNode *head, int val)` | Searches the list for a node whose `val` field equals `val`. Returns a pointer to the first match, or `NULL` if not found. |
| `run_test` | `static void run_test(const char *name, int *nums, int len, int del_val, int *expected, int expected_len)` | Runs a single test case. Builds the list from `nums`, finds the node with value `del_val`, calls `deleteNode`, verifies the result against `expected` using `check_list`, then frees the list. Prints `PASS` on success; aborts via `assert` on failure. |

---

## Summary

LeetCode 237 asks you to delete a node from a singly-linked list when you
only have a pointer to that node, not to its predecessor. The standard trick
is to copy the next node's value into the current node and then skip over
the next node in the chain. The original two-line solution does exactly this
but never frees the skipped node, causing a memory leak. The optimized
version fixes this by saving a `victim` pointer to `node->next` before
modifying anything, performing the same copy-and-skip operation, and then
calling `free(victim)` to release the orphaned memory. The result is
identical list behavior with clean memory management -- O(1) time, O(1)
space, and zero leaked bytes.
