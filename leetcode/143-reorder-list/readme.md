# LeetCode 143 -- Reorder List

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

You are given the head of a singly linked list whose nodes are numbered in
order:

```
L0 -> L1 -> L2 -> ... -> Ln-1 -> Ln
```

Reorder the list so that the nodes alternate between the front and the back of
the original sequence:

```
L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
```

You must not copy node values into an array and rearrange them; the
transformation must happen **in-place** by rewiring the `next` pointers of the
existing nodes.

### Examples

| Input             | Output            |
|-------------------|-------------------|
| `[1, 2, 3, 4]`   | `[1, 4, 2, 3]`   |
| `[1, 2, 3, 4, 5]`| `[1, 5, 2, 4, 3]` |
| `[1]`             | `[1]`             |
| `[1, 2]`          | `[1, 2]`          |

---

## Core Idea

Think of a deck of cards laid out in a row. You want to take one from the left,
then one from the right, then one from the left, and so on, until you run out.
You cannot look at the right end of a singly linked list directly, so the
algorithm uses three phases to work around that limitation:

1. **Split** -- Walk the list once to count nodes. Then walk again to the
   midpoint and cut the list into two halves. The front half keeps the extra
   node when the total count is odd.

2. **Reverse** -- Reverse the second half in place. Now its first node is what
   used to be the last node of the original list. You can finally traverse the
   tail end from right to left.

3. **Merge** -- Interleave the two halves by alternating pointers: take one
   node from the front half, then one node from the (now reversed) back half,
   and repeat until both halves are exhausted.

Because every phase visits each node at most once and uses only a handful of
pointer variables, the algorithm runs in O(n) time with O(1) extra space.

---

## Step-by-Step Walkthrough

Input list: `[1, 2, 3, 4, 5]` (size = 5).

### Phase 1 -- Count and Split

```
revert_size = 5 >> 1 = 2
front keeps  size - revert_size = 3 nodes
```

Walk 3 steps from `head`, tracking `pre` (the node before `cur`). After the
loop, `pre` points to node 3 and `cur` points to node 4. Cut by setting
`pre->next = NULL`.

```
Front half:   1 -> 2 -> 3 -> NULL
                              ^pre

Back half:    4 -> 5 -> NULL
              ^cur
```

### Phase 2 -- Reverse the Back Half

Iteratively reverse `4 -> 5` into `5 -> 4`:

```
Step 0:  cur=4, revert_head=NULL
         Save tmp = cur->next (5)
         4->next = NULL  (revert_head)
         revert_head = 4
         cur = 5

         revert_head:  4 -> NULL
         cur:          5 -> NULL

Step 1:  cur=5, revert_head=4
         Save tmp = cur->next (NULL)
         5->next = 4
         revert_head = 5
         cur = NULL

         revert_head:  5 -> 4 -> NULL
```

Result after reversal:

```
Front half:      1 -> 2 -> 3 -> NULL
Reversed half:   5 -> 4 -> NULL
```

### Phase 3 -- Merge / Interleave

Walk both halves simultaneously, stitching them together one pair at a time.

```
Iteration 1:
  tmp  = 1  (from front)
  tmp2 = 5  (from reversed)
  Advance: cur = 2, revert_head = 4
  Wire:    1 -> 5 -> 2

  List so far: 1 -> 5 -> 2 -> 3 -> NULL
                              (3 still linked from original front)

Iteration 2:
  tmp  = 2  (from front)
  tmp2 = 4  (from reversed)
  Advance: cur = 3, revert_head = NULL
  Wire:    2 -> 4 -> 3

  List so far: 1 -> 5 -> 2 -> 4 -> 3 -> NULL

Iteration 3:
  revert_head == NULL  -->  loop exits
```

Final list:

```
1 -> 5 -> 2 -> 4 -> 3 -> NULL
```

This matches the expected output `[1, 5, 2, 4, 3]`.

---

## ASCII Flowchart

```
                      +---------------------+
                      |  head == NULL?       |
                      +---------------------+
                         |  yes       | no
                         v            v
                      [return]   +-----------------+
                                 | Count nodes     |
                                 | size = len(list)|
                                 +-----------------+
                                         |
                                         v
                                 +-----------------------+
                                 | revert_size = size>>1 |
                                 +-----------------------+
                                         |
                                         v
                                 +---------------------------+
                                 | Walk (size - revert_size) |
                                 | nodes from head           |
                                 | pre = last node of front  |
                                 | cur = first node of back  |
                                 +---------------------------+
                                         |
                                         v
                                 +------------------+
                                 | Cut: pre->next   |
                                 |      = NULL      |
                                 +------------------+
                                         |
                                         v
                                 +---------------------+
                                 | Reverse back half   |
                                 | (iterative)         |
                                 |                     |
                                 | revert_head = NULL  |
                                 | while (cur):        |
                                 |   tmp = cur->next   |
                                 |   cur->next =       |
                                 |       revert_head   |
                                 |   revert_head = cur |
                                 |   cur = tmp         |
                                 +---------------------+
                                         |
                                         v
                                 +-------------------------+
                                 | Merge / interleave      |
                                 |                         |
                                 | cur = head              |
                                 | while cur && revert_head|
                                 |   tmp  = cur            |
                                 |   tmp2 = revert_head    |
                                 |   advance both cursors  |
                                 |   tmp->next  = tmp2     |
                                 |   tmp2->next = cur      |
                                 +-------------------------+
                                         |
                                         v
                                      [done]
```

---

## Where It Gets Tricky

### Counting-based split vs. slow/fast pointer

A common alternative finds the midpoint using a slow pointer (advances 1 step)
and a fast pointer (advances 2 steps). The implementation here counts nodes
explicitly instead. The trade-off:

- **Counting approach (this code):** two separate passes -- one to count, one to
  reach the split point. The logic is straightforward and easy to verify.
- **Slow/fast approach:** a single pass finds the midpoint, but you have to be
  careful about whether `slow` ends up at the last node of the front half or
  the first node of the back half, especially with odd-length lists.

Both are O(n) in time and O(1) in space. The counting approach is arguably
easier to reason about because `size - revert_size` gives an unambiguous split
index.

### Cut point for odd vs. even length

When the list length is odd, the middle node must stay in the front half (it
will end up as the final node of the reordered list). The formula used here
makes that happen naturally:

| size | revert_size (size >> 1) | front keeps | back gets |
|------|-------------------------|-------------|-----------|
| 4    | 2                       | 2 nodes     | 2 nodes   |
| 5    | 2                       | 3 nodes     | 2 nodes   |
| 6    | 3                       | 3 nodes     | 3 nodes   |
| 7    | 3                       | 4 nodes     | 3 nodes   |

The front half always has `ceil(size / 2)` nodes and the back half has
`floor(size / 2)` nodes. When the halves are equal in length, every front node
pairs with a back node. When the front is one longer, its last node has no
partner and simply remains at the tail after the merge loop ends.

### Merge termination

The merge loop runs while **both** `cur` (front) and `revert_head` (reversed
back) are non-NULL. Because the front half is at least as long as the back
half, the back half is always exhausted first (or at the same time). The
remaining tail node of the front half (if any) is already correctly linked by
the last `tmp2->next = cur` assignment, so no special post-loop fixup is
needed.

---

## Complexity Analysis

| Metric | Value | Justification |
|--------|-------|---------------|
| **Time**  | O(n) | Counting traverses all n nodes once. Splitting walks at most n nodes. Reversal visits each back-half node once. Merging visits each node once. Total: roughly 3n pointer operations. |
| **Space** | O(1) | Only a fixed number of pointer variables (`cur`, `pre`, `tmp`, `tmp2`, `revert_head`) and integer counters (`size`, `revert_size`, `i`) are used regardless of input length. |

---

## Function Reference

All functions are defined in `main.c`.

### `reorderList`

```c
void reorderList(struct ListNode *head);
```

- **Purpose:** Reorders a singly linked list in-place so that the nodes
  alternate between the first and last positions of the original order.
- **Parameters:**
  - `head` -- pointer to the first node of the list (may be `NULL`).
- **Return value:** None. The list is modified in place through pointer
  manipulation.

### `insert_list`

```c
struct ListNode *insert_list(int *nums, int size);
```

- **Purpose:** Builds a singly linked list from a C array by allocating one
  node per element, preserving the array order.
- **Parameters:**
  - `nums` -- pointer to an integer array containing the node values.
  - `size` -- number of elements in `nums`.
- **Return value:** Pointer to the head of the newly created list.

### `show_list`

```c
void show_list(char *type, struct ListNode *head);
```

- **Purpose:** Prints the contents of a linked list to stdout in the format
  `type: [ v1 v2 v3 ]`.
- **Parameters:**
  - `type` -- a label string printed before the list values (e.g., `"Input "`
    or `"Output"`).
  - `head` -- pointer to the first node of the list.
- **Return value:** None.

### `free_list`

```c
static void free_list(struct ListNode *head);
```

- **Purpose:** Frees every node in the linked list to avoid memory leaks.
- **Parameters:**
  - `head` -- pointer to the first node of the list.
- **Return value:** None.

### `check_list`

```c
static int check_list(struct ListNode *head, int *expected, int len);
```

- **Purpose:** Compares the linked list node-by-node against an expected
  integer array. Returns true only if the list has exactly `len` nodes whose
  values match the array in order.
- **Parameters:**
  - `head` -- pointer to the first node of the list.
  - `expected` -- pointer to an integer array of expected values.
  - `len` -- number of elements in `expected`.
- **Return value:** `1` if the list matches; `0` otherwise.

### `run_test`

```c
static void run_test(const char *name, int *nums, int len,
                     int *expected, int expected_len);
```

- **Purpose:** Runs a single test case: builds a list from `nums`, calls
  `reorderList`, prints the before/after state, asserts correctness against
  `expected`, and frees the list.
- **Parameters:**
  - `name` -- descriptive name for the test case (printed to stdout).
  - `nums` -- input array of node values.
  - `len` -- length of `nums`.
  - `expected` -- array of expected node values after reordering.
  - `expected_len` -- length of `expected`.
- **Return value:** None. Aborts via `assert` on failure.

---

## Summary

The reorder-list algorithm works by splitting the linked list at its midpoint,
reversing the second half, and then interleaving the two halves back together.
Splitting is done by counting nodes and walking to the cut point, which
naturally handles both odd and even lengths by giving the front half the extra
node when the count is odd. Reversing the back half converts the problem of
reaching the last node (impossible in O(1) for a singly linked list) into a
simple forward traversal. The final merge loop alternates pointers between the
two halves until the shorter (reversed) half is exhausted, leaving the
reordered list intact without any extra memory allocation. The entire process
runs in O(n) time and O(1) space.
