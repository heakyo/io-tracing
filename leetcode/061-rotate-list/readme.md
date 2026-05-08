# LeetCode 61 -- Rotate List

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

You are given the head of a singly linked list and an integer `k`. Rotate the
list to the right by `k` places.

Rotating to the right by one place means taking the last node off the tail and
placing it at the head. Doing that `k` times gives the final result.

**Example 1:**

```
Input:  1 -> 2 -> 3 -> 4 -> 5,  k = 2
Output: 4 -> 5 -> 1 -> 2 -> 3
```

The last two nodes (4 and 5) move to the front.

**Example 2:**

```
Input:  0 -> 1 -> 2,  k = 4
Output: 2 -> 0 -> 1
```

Since `k = 4` is larger than the list length (3), the effective rotation is
`4 % 3 = 1`. Only the last node moves to the front.

**Constraints:**

- The number of nodes is in the range `[0, 500]`.
- `k >= 0` and can be much larger than the list length.

---

## Core Idea

Moving nodes one at a time would be slow. Instead, notice that a right rotation
by `k` places is the same as chopping the list at the right spot and
re-connecting the two halves in reverse order.

The algorithm in three sentences:

1. **Count** how many nodes the list has (`size`), and while you are walking to
   the end, remember the tail node.
2. **Reduce** `k` with the modulo operation (`n = k % size`) so that any
   value of `k` larger than the list length is brought into range.
3. **Split** the list after position `size - n - 1` (zero-indexed). The second
   half becomes the new head, and the old tail connects to the old head.

That is the entire algorithm. One pass to count, one partial pass to find the
split point, and a constant number of pointer rewires.

---

## Step-by-Step Walkthrough

List: `1 -> 2 -> 3 -> 4 -> 5`, `k = 2`.

### Pass 1 -- Count nodes and find the tail

Walk the entire list. Increment `size` for every node. When the loop ends,
`prev` points to the last node visited (the tail).

```
cur: 1  2  3  4  5  NULL
         size increments each step

Result: size = 5, prev (tail) = node 5
```

### Compute effective rotation

```
n = k % size = 2 % 5 = 2
```

`n` is not zero, so a rotation is needed.

### Pass 2 -- Find the split point

We need the node at index `size - n - 1 = 5 - 2 - 1 = 2` (zero-indexed).
Start `cur` at head and advance 2 times.

```
Start:  cur = node 1  (index 0)
Step 1: cur = node 2  (index 1)
Step 2: cur = node 3  (index 2)   <-- this is the new tail
```

### Rewire pointers

```
next = cur->next          next = node 4  (new head)
cur->next = NULL          3 -> NULL      (cut the list here)
prev->next = head         5 -> 1         (old tail links to old head)
return next               return node 4
```

### Result

```
Before: 1 -> 2 -> 3 -> 4 -> 5

After:  4 -> 5 -> 1 -> 2 -> 3
        ^              |
        new head       new tail (3 -> NULL)
```

---

## ASCII Flowchart

```
+------------------------------+
|   k == 0  OR  head == NULL?  |
+------------------------------+
        |  yes       |  no
        v            v
   return head   +-----------------------+
                 | Walk entire list:     |
                 |   count size          |
                 |   prev = tail node    |
                 +-----------------------+
                          |
                          v
                 +-----------------------+
                 | n = k % size          |
                 | n == 0?               |
                 +-----------------------+
                   |  yes       |  no
                   v            v
              return head   +--------------------------+
                            | Walk (size - n - 1) from |
                            | head to find new tail    |
                            +--------------------------+
                                       |
                                       v
                            +--------------------------+
                            | next = cur->next         |
                            | cur->next = NULL         |
                            | prev->next = head        |
                            | return next              |
                            +--------------------------+
```

---

## Where It Gets Tricky

### k % size -- handling huge k values

`k` can be much larger than the list length. Without the modulo reduction,
you would walk past the end of the list. The key insight is that rotating a
list of length `size` by exactly `size` places returns it to the original
order, so only the remainder matters. The code also returns early when
`n == 0` (meaning the rotation is a no-op), which covers the case `k == size`,
`k == 2*size`, and so on.

### The unnecessary dummy node

The code declares a `dummy` node on the stack:

```c
struct ListNode dummy = {0, head};
```

It is later updated with `dummy.next = next` and the function returns
`dummy.next`. But at that point `next` already holds the new head, so the
function could simply `return next`. The dummy node adds nothing here -- it
is leftover scaffolding that could be removed without changing behavior.

### The confusing `prev` variable

The variable is called `prev`, but after the counting loop it points to the
**last** node in the list (the tail). A name like `tail` would communicate the
intent far more clearly. When reading the rewire section (`prev->next = head`),
the name `prev` makes it sound like we are linking some predecessor, when we
are actually linking the tail to the old head to form the circular connection.

### `next` declared early, used late

`next` is declared alongside `cur` and `prev` at the top of the function, yet
it is not assigned until the very end (line 78 in main.c). In a longer
function this kind of distant declaration-use gap can make the code harder to
follow. Declaring `next` closer to its first use would improve readability.

---

## Complexity Analysis

| Metric | Value |
|--------|-------|
| **Time**  | O(n) -- one full traversal to count, one partial traversal to find the split point. At most 2n node visits. |
| **Space** | O(1) -- only a fixed number of pointer variables and integers, regardless of list size. |

---

## Function Reference

All functions are defined in `main.c`.

| Function | Signature | Description |
|----------|-----------|-------------|
| `rotateRight` | `struct ListNode* rotateRight(struct ListNode* head, int k)` | Core algorithm. Rotates the linked list to the right by `k` places using the count-modulo-split approach. Returns the new head. |
| `insert_list` | `struct ListNode *insert_list(int *nums, int size)` | Builds a singly linked list from an integer array. Allocates nodes with `malloc` and returns the head. |
| `show_list` | `void show_list(char *type, struct ListNode* head)` | Prints every node value in the list, prefixed by a label string (`type`). Used for debugging output. |
| `free_list` | `void free_list(struct ListNode *head)` | Frees all nodes in the list by walking from head to tail. |
| `check_list` | `int check_list(struct ListNode *head, int *expected, int len)` | Compares the list node-by-node against an expected integer array. Returns 1 if they match, 0 otherwise. |
| `run_test` | `void run_test(const char *name, int *nums, int len, int k, int *expected, int expected_len)` | Builds a list, runs `rotateRight`, prints the result, and asserts correctness via `check_list`. |

---

## Summary

Rotating a linked list to the right by `k` places boils down to finding where
to cut. First walk the entire list to learn its length and grab the tail
pointer. Then reduce `k` with modulo to handle values larger than the list
length. Walk `size - n - 1` steps from the head to reach the new tail, cut the
list there, and stitch the old tail to the old head. The result is two pointer
passes and a constant number of rewires -- O(n) time and O(1) space. The
implementation works correctly, though the dummy node is unnecessary, the
`prev` variable would be clearer if named `tail`, and the `next` variable
could be declared closer to its point of use.
