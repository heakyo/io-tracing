# 025 - Reverse Nodes in k-Group

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

Given the `head` of a linked list, reverse the nodes of the list `k` at a
time, and return the modified list.

`k` is a positive integer and is less than or equal to the length of the
linked list. If the number of nodes is not a multiple of `k`, the remaining
nodes at the end stay in their original order.

You may not alter the values in the nodes -- only the nodes themselves may be
changed.

**Constraints:**

- `1 <= k <= n <= 5000` (where *n* is the number of nodes)
- `0 <= Node.val <= 1000`

**Examples:**

```
Input:  1 -> 2 -> 3 -> 4 -> 5,  k = 2
Output: 2 -> 1 -> 4 -> 3 -> 5

Input:  1 -> 2 -> 3 -> 4 -> 5,  k = 3
Output: 3 -> 2 -> 1 -> 4 -> 5
```

---

## Core Idea

Imagine a row of people standing in line. A drill sergeant shouts: "Every
group of *k* people -- reverse your order!" Each group of *k* shuffles in
place, but the leftover people at the end (fewer than *k*) stay put.

The algorithm does this in two phases:

1. **Count first.** Walk the entire list once to find out how many nodes there
   are. This tells you how many complete groups of *k* exist and avoids the
   need to look ahead during reversal.

2. **Reverse group by group using head-insertion.** For each group, use the
   same four-line pointer surgery from problem 092 (Reverse Linked List II):
   repeatedly pull the node after `cur` and insert it right after `pre`. After
   `k - 1` such extractions, the group is fully reversed. Then slide `pre` and
   `cur` forward to the next group.

The trick is that `cur` never moves within a group -- it always points to the
node that was originally first in the group. As nodes are inserted ahead of
it, `cur` sinks to the back. Once the group is done, `cur` is sitting at the
last position of the reversed group, exactly where you need `pre` to be for
the next group.

```c
struct ListNode* reverseKGroup(struct ListNode* head, int k)
{
	struct ListNode *cur, *pre, *tmp, *dummy;
	int size, i;

	dummy = (struct ListNode *)malloc(sizeof(*dummy));
	dummy->next = head;
	pre = dummy;

	size = 0;
	cur = head;
	while (cur) {
		size++;
		cur = cur->next;
	}

	cur = head;
	while (size >= k) {
		for (i = 0; i < k - 1; i++) {
			tmp = pre->next;
			pre->next = cur->next;
			cur->next = cur->next->next;
			pre->next->next = tmp;
		}

		size -= k;
		pre = cur;
		cur = cur->next;
	}

	return dummy->next;
}
```

---

## Step-by-Step Walkthrough

Let's trace through: `[1, 2, 3, 4, 5]`, `k = 2`.

### Initialization

```
dummy = malloc(...)
dummy->next = head
pre = dummy

dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
```

### Counting Phase

Walk the list to count nodes:

```
cur = head (Node 1), size = 0
  size = 1, cur = Node 2
  size = 2, cur = Node 3
  size = 3, cur = Node 4
  size = 4, cur = Node 5
  size = 5, cur = NULL

Result: size = 5
```

### Reset `cur`

```
cur = head

dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
  ^      ^
 pre    cur
```

### Group 1 (size = 5 >= k = 2)

The inner loop runs `k - 1 = 1` time.

#### Iteration i = 0

```
Before:
  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
    ^      ^
   pre    cur

Step 1:  tmp = pre->next              =>  tmp = Node(1)
Step 2:  pre->next = cur->next        =>  dummy->next = Node(2)
Step 3:  cur->next = cur->next->next  =>  Node(1)->next = Node(3)
         (Node(1) skips over Node(2), now points to Node(3))
Step 4:  pre->next->next = tmp        =>  Node(2)->next = Node(1)

After:
  dummy -> 2 -> 1 -> 3 -> 4 -> 5 -> NULL
    ^           ^
   pre         cur
```

#### Advance to next group

```
size -= 2  =>  size = 3
pre = cur  =>  pre = Node(1)
cur = cur->next  =>  cur = Node(3)

  dummy -> 2 -> 1 -> 3 -> 4 -> 5 -> NULL
                ^    ^
               pre  cur
```

### Group 2 (size = 3 >= k = 2)

#### Iteration i = 0

```
Before:
  ... 1 -> 3 -> 4 -> 5 -> NULL
      ^    ^
     pre  cur

Step 1:  tmp = pre->next              =>  tmp = Node(3)
Step 2:  pre->next = cur->next        =>  Node(1)->next = Node(4)
Step 3:  cur->next = cur->next->next  =>  Node(3)->next = Node(5)
         (Node(3) skips over Node(4), now points to Node(5))
Step 4:  pre->next->next = tmp        =>  Node(4)->next = Node(3)

After:
  dummy -> 2 -> 1 -> 4 -> 3 -> 5 -> NULL
                ^         ^
               pre       cur
```

#### Advance to next group

```
size -= 2  =>  size = 1
pre = cur  =>  pre = Node(3)
cur = cur->next  =>  cur = Node(5)

  dummy -> 2 -> 1 -> 4 -> 3 -> 5 -> NULL
                          ^    ^
                         pre  cur
```

### Exit (size = 1 < k = 2)

The while loop condition fails. Node(5) stays in place.

### Return

```
return dummy->next  =>  2 -> 1 -> 4 -> 3 -> 5
```

Two complete groups of 2 have been reversed. The leftover node (5) remains
at the end.

---

## ASCII Flowchart

```
                +-------------------------------+
                |             START             |
                +---------------+---------------+
                                |
                                v
                +-------------------------------+
                |  dummy = malloc(sizeof(*d))   |
                |  dummy->next = head           |
                |  pre = dummy                  |
                +---------------+---------------+
                                |
                                v
                +-------------------------------+
                |  size = 0                     |
                |  cur = head                   |
                +---------------+---------------+
                                |
                                v
                        +-------+-------+
                        |  cur != NULL? |------- No ------+
                        +-------+-------+                 |
                                | Yes                     |
                                v                         |
                +-------------------------------+         |
                |  size++                       |         |
                |  cur = cur->next              |         |
                +---------------+---------------+         |
                                |                         |
                                +--- (loop back) ---+     |
                                                          |
                                +-------------------------+
                                |
                                v
                +-------------------------------+
                |  cur = head                   |
                +---------------+---------------+
                                |
                                v
                        +-------+-------+
                        |  size >= k ?  |------- No ------+
                        +-------+-------+                 |
                                | Yes                     |
                                v                         |
                        +-------+-------+                 |
                        |   i = 0       |                 |
                        +-------+-------+                 |
                                |                         |
                                v                         |
                        +-------+--------+                |
                        |  i < k - 1 ?   |--- No ---+     |
                        +-------+--------+          |     |
                                | Yes               |     |
                                v                   |     |
                +-------------------------------+   |     |
                |  tmp = pre->next              |   |     |
                |  pre->next = cur->next        |   |     |
                |  cur->next = cur->next->next  |   |     |
                |  pre->next->next = tmp        |   |     |
                |  i++                          |   |     |
                +---------------+---------------+   |     |
                                |                   |     |
                                +-- (loop back) -+  |     |
                                                    |     |
                                +-------------------+     |
                                |                         |
                                v                         |
                +-------------------------------+         |
                |  size -= k                    |         |
                |  pre = cur                    |         |
                |  cur = cur->next              |         |
                +---------------+---------------+         |
                                |                         |
                                +--- (loop back) ---+     |
                                                          |
                                +-------------------------+
                                |
                                v
                +-------------------------------+
                |       return dummy->next      |
                +-------------------------------+
```

---

## Where It Gets Tricky

### (a) Counting first vs. looking ahead

An alternative design would skip the counting phase and instead peek ahead *k*
nodes at the start of each group to check whether a full group exists. This
implementation counts the total length up front, then simply decrements `size`
by *k* after each group. The trade-off is straightforward: one extra pass over
the list (still O(n) total) in exchange for a simpler loop condition
(`size >= k`) that avoids the fiddly look-ahead logic inside the reversal loop.

### (b) `cur` stays put within a group

Just like in problem 092, `cur` never advances during the inner loop. It
always points to the same node -- the one that was first in the group before
reversal began. As other nodes are inserted ahead of it, `cur` drifts to the
back of the group. When the inner loop finishes, `cur` sits at the last
position of the reversed group. That is why `pre = cur` is the correct setup
for the next group: the last node of one group is the anchor point for the
next.

### (c) The `pre = cur` hand-off

After reversing a group, the code sets `pre = cur` and `cur = cur->next`.
This works because `cur` is now the tail of the just-reversed group, which
is exactly the node right before the next group. If you mistakenly moved `cur`
during the inner loop, this hand-off would break and subsequent groups would
be wired incorrectly.

### (d) The 4-line pointer swap: order matters

```c
tmp = pre->next;             /* 1. save current front of reversed portion    */
pre->next = cur->next;       /* 2. link pre to the node being extracted      */
cur->next = cur->next->next; /* 3. cur skips over the extracted node         */
pre->next->next = tmp;       /* 4. extracted node points to old front        */
```

Line 2 must come before line 3 because line 3 overwrites `cur->next`, which
line 2 needs. Line 1 must come before line 4 because line 4 writes through
`pre->next->next`, and without `tmp` you would lose the old front. Rearranging
any of these lines corrupts the list.

### (e) Memory leak

The `dummy` node is allocated with `malloc` but never freed:

```c
dummy = malloc(sizeof(*dummy));
/* ... */
return dummy->next;   /* dummy itself is leaked */
```

On LeetCode this is harmless. In production code, save `dummy->next` to a
local, call `free(dummy)`, then return the saved pointer -- or just use a stack
variable instead of `malloc`.

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time** | O(n) | The counting phase visits every node once. The reversal phase also visits every node at most once (each node is extracted and re-inserted exactly once across all groups). Total work is 2n, which is O(n). |
| **Space** | O(1) | Only a fixed number of pointers (`dummy`, `pre`, `cur`, `tmp`) and two integers (`size`, `i`) are used, regardless of input size. The `malloc`'d dummy node is constant-size overhead. |

---

## Function Reference

### `reverseKGroup`

```c
struct ListNode* reverseKGroup(struct ListNode* head, int k);
```

**Purpose:** Reverses nodes of the linked list in groups of `k`. Nodes in an
incomplete trailing group (fewer than `k` remaining) are left in their original
order.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `head` | `struct ListNode*` | Pointer to the first node of the input list, or `NULL` for an empty list. |
| `k` | `int` | Group size. Must satisfy `1 <= k <= n`. |

**Return value:** Pointer to the head of the reordered list.

---

### `insert_list`

```c
struct ListNode *insert_list(int *nums, int size);
```

**Purpose:** Builds a singly linked list from an integer array, preserving the
array's order.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `nums` | `int*` | Array of integer values for the nodes. |
| `size` | `int` | Number of elements in `nums`. |

**Return value:** Pointer to the head of the newly created list.

---

### `show_list`

```c
void show_list(char *type, struct ListNode* head);
```

**Purpose:** Prints the linked list to stdout in the format `type: [ v1 v2 ... ]`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `type` | `char*` | Label string printed before the list values (e.g., `"Input "` or `"Output"`). |
| `head` | `struct ListNode*` | Pointer to the first node, or `NULL` for an empty list. |

**Return value:** None (`void`).

---

### `free_list`

```c
static void free_list(struct ListNode *head);
```

**Purpose:** Frees every node in the linked list, walking from head to tail.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `head` | `struct ListNode*` | Pointer to the first node, or `NULL` (no-op). |

**Return value:** None (`void`).

---

### `check_list`

```c
static int check_list(struct ListNode *head, int *expected, int len);
```

**Purpose:** Validates that the linked list matches an expected array of values,
element by element, and that the list has exactly `len` nodes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `head` | `struct ListNode*` | Pointer to the first node of the list to check. |
| `expected` | `int*` | Array of expected values in order. |
| `len` | `int` | Number of elements in `expected`. |

**Return value:** `1` if the list matches the expected array exactly, `0`
otherwise.

---

### `run_test`

```c
static void run_test(const char *name, int *nums, int len,
                     int k, int *expected, int expected_len);
```

**Purpose:** Runs a single test case: builds a list from `nums`, calls
`reverseKGroup`, prints the input and output, asserts the result matches
`expected`, and frees the result list.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `name` | `const char*` | Descriptive name for the test case, printed in the header. |
| `nums` | `int*` | Input array of node values. |
| `len` | `int` | Number of elements in `nums`. |
| `k` | `int` | Group size passed to `reverseKGroup`. |
| `expected` | `int*` | Expected output array after reversal. |
| `expected_len` | `int` | Number of elements in `expected`. |

**Return value:** None (`void`). Aborts via `assert` on mismatch.

---

## Summary

The algorithm reverses a linked list in groups of *k* using two passes: a
counting pass to determine the total number of nodes, followed by a reversal
pass that processes one group at a time. Each group is reversed in place with
the head-insertion technique -- repeatedly extracting the node after `cur` and
inserting it at the front of the group (right after `pre`). The key insight is
that `cur` stays fixed on the original first node of each group, which
naturally sinks to the tail of the reversed group, setting up `pre` for the
next iteration. Leftover nodes (fewer than *k*) are left untouched because the
outer loop condition `size >= k` simply fails. The algorithm runs in **O(n)
time** and **O(1) space**, making it an efficient single-allocation, in-place
solution.
