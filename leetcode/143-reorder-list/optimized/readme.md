# LeetCode 143 - Reorder List (Optimized)

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

Given the head of a singly linked list:

```
L0 -> L1 -> L2 -> ... -> Ln-1 -> Ln
```

Reorder it into:

```
L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
```

You must modify the list in-place. You may not change node values -- only
rearrange the node pointers themselves.

---

## Core Idea

The optimized algorithm breaks the problem into three clean phases:

1. **Find the middle** -- use slow/fast pointers. `slow` advances one node at
   a time; `fast` advances two. When `fast` reaches the end, `slow` sits at
   the midpoint. This takes a single pass with no counting.

2. **Reverse the second half** -- starting from `slow->next`, reverse the
   remainder of the list in-place using `prev`/`curr`/`next`. After this,
   `prev` points to the new head of the reversed second half.

3. **Merge the two halves** -- interleave nodes from the first half and the
   reversed second half. Walk `first` through the left half and `second`
   through the right half, stitching them together one pair at a time.

Because each phase is a single linear scan, the overall algorithm runs in
O(n) time and O(1) space.

---

## Step-by-Step Walkthrough

Input list: `[1, 2, 3, 4, 5]`

### Phase 1: Find the middle with slow/fast pointers

Both `slow` and `fast` start at `head` (node 1).

```
Step 0:  1 -> 2 -> 3 -> 4 -> 5 -> NULL
         ^
       slow
       fast

Step 1:  slow moves to 2, fast moves to 3
         1 -> 2 -> 3 -> 4 -> 5 -> NULL
              ^         ^
            slow       fast

Step 2:  slow moves to 3, fast moves to 5
         1 -> 2 -> 3 -> 4 -> 5 -> NULL
                   ^              ^
                 slow            fast
```

The loop condition is `fast->next && fast->next->next`. At step 2,
`fast->next` is NULL, so the loop stops. `slow` is at node 3 -- the middle.

We cut the list: `slow->next = NULL`.

```
First half:   1 -> 2 -> 3 -> NULL
Second half:  4 -> 5 -> NULL     (curr starts here)
```

### Phase 2: Reverse the second half

We reverse `4 -> 5 -> NULL` using `prev`/`curr`/`next`:

```
Initial:    prev = NULL, curr = 4

Iteration 1:
  next = 5           (save curr->next)
  curr->next = NULL  (4 points to prev, which is NULL)
  prev = 4           (advance prev)
  curr = 5           (advance curr)

  NULL <- 4    5 -> NULL
          ^    ^
        prev  curr

Iteration 2:
  next = NULL        (save curr->next)
  curr->next = 4     (5 points to prev, which is 4)
  prev = 5           (advance prev)
  curr = NULL        (advance curr)

  NULL <- 4 <- 5
               ^
             prev
```

Reversed second half: `5 -> 4 -> NULL` (head is `prev`, which is node 5).

### Phase 3: Merge the two halves

```
first  = head of first half:      1 -> 2 -> 3 -> NULL
second = head of reversed half:   5 -> 4 -> NULL
```

**Iteration 1:**

```
tmp = first->next          --> tmp = node 2
first->next = second       --> 1 -> 5
second = second->next      --> second = node 4
first->next->next = tmp    --> 1 -> 5 -> 2
first = tmp                --> first = node 2

State: 1 -> 5 -> 2 -> 3 -> NULL
                  ^         second = 4 -> NULL
                first
```

**Iteration 2:**

```
tmp = first->next          --> tmp = node 3
first->next = second       --> 2 -> 4
second = second->next      --> second = NULL
first->next->next = tmp    --> 2 -> 4 -> 3
first = tmp                --> first = node 3

State: 1 -> 5 -> 2 -> 4 -> 3 -> NULL
                             ^    second = NULL
                           first
```

`second` is NULL, so the loop ends.

**Final result:** `1 -> 5 -> 2 -> 4 -> 3 -> NULL`

---

## ASCII Flowchart

```
                +---------------------+
                |  head == NULL or    |
                |  head->next == NULL |
                +---------+-----------+
                          |
                    +-----+-----+
                    | Yes       | No
                    v           v
                 return    +---------+
                           | Phase 1 |
                           | slow/   |
                           | fast    |
                           +----+----+
                                |
                   slow = head, fast = head
                                |
                     +----------+-----------+
                     | while fast->next &&  |
                     | fast->next->next     |
                     +----------+-----------+
                                |
                       slow = slow->next
                       fast = fast->next->next
                                |
                     +----------+-----------+
                     | Loop done: slow is   |
                     | at the middle node   |
                     +----------+-----------+
                                |
                     curr = slow->next
                     slow->next = NULL
                                |
                           +----+----+
                           | Phase 2 |
                           | reverse |
                           +----+----+
                                |
                       prev = NULL
                                |
                     +----------+-----------+
                     | while curr != NULL   |
                     |   next = curr->next  |
                     |   curr->next = prev  |
                     |   prev = curr        |
                     |   curr = next        |
                     +----------+-----------+
                                |
                     prev = head of reversed
                     second half
                                |
                           +----+----+
                           | Phase 3 |
                           |  merge  |
                           +----+----+
                                |
                     first = head
                     second = prev
                                |
                     +----------+-----------+
                     | while second != NULL |
                     |   tmp = first->next  |
                     |   first->next =      |
                     |       second         |
                     |   second =           |
                     |       second->next   |
                     |   first->next->next  |
                     |       = tmp          |
                     |   first = tmp        |
                     +----------+-----------+
                                |
                              done
```

---

## Where It Gets Tricky

### 1. The slow/fast stopping condition

The loop condition is `fast->next && fast->next->next`, not
`fast && fast->next`. This distinction matters because we want `slow` to
land on the **last node of the first half**, not the first node of the second
half. We need `slow` to be the node just before the split point so we can
sever the list with `slow->next = NULL`.

If we checked `fast && fast->next` instead, `slow` would overshoot by one
position, and the split would be wrong.

### 2. Odd-length vs even-length lists

For an **odd** list like `[1, 2, 3, 4, 5]`, `slow` lands on node 3. The
first half is `[1, 2, 3]` (3 nodes) and the second half is `[4, 5]` (2
nodes). The middle element stays in the first half and ends up as the tail
of the final list.

For an **even** list like `[1, 2, 3, 4]`, `slow` lands on node 2. The first
half is `[1, 2]` and the second half is `[3, 4]`. Both halves have equal
length.

The merge loop `while (second)` handles both cases correctly. When the first
half is longer by one (odd case), the extra node is already attached as the
tail after the last merge iteration completes.

### 3. The merge loop pointer juggling

The merge body does four things in order:

```c
tmp = first->next;           // save the next node in the first half
first->next = second;        // splice the second-half node after first
second = second->next;       // advance second BEFORE we overwrite its next
first->next->next = tmp;     // link the spliced node back to the first half
first = tmp;                 // advance first to the saved node
```

The order is critical. We must advance `second` before the assignment
`first->next->next = tmp` because `first->next` is now the old `second`
node -- that assignment overwrites its `next` pointer. If we advanced
`second` after this line, we would lose the rest of the second half.

---

## Comparison with Original

| Aspect | Original | Optimized |
|---|---|---|
| Finding the middle | Count all nodes (`size`), then walk `size - revert_size` steps. Two passes over the first half. | Slow/fast pointers. One pass. |
| Extra integer variables | `size`, `revert_size`, `i` | None |
| Reversal variable names | `revert_head`, `pre`, `cur`, `tmp` | `prev`, `curr`, `next` |
| Merge variable names | `cur`, `revert_head`, `tmp`, `tmp2` | `first`, `second`, `tmp` |
| Early return | Checks `head == NULL` only | Checks `head == NULL` or `head->next == NULL` |
| Merge loop body | 6 statements with `tmp` and `tmp2` | 4 statements with `tmp` |
| Time complexity | O(n) | O(n) |
| Space complexity | O(1) | O(1) |

The original algorithm requires computing the list length first, then walking
forward again to reach the split point. This means nodes in the first half are
visited twice before any real work begins. The optimized version finds the
middle and reaches the split point in a single traversal using the slow/fast
pointer technique.

The naming improvements make each phase self-documenting: `slow`/`fast` are
the standard names for the tortoise-and-hare pattern, `prev`/`curr`/`next`
are the standard names for in-place reversal, and `first`/`second` make the
two-list merge obvious.

---

## Complexity Analysis

**Time: O(n)**

- Phase 1 (find middle): slow visits n/2 nodes, fast visits n nodes. Total: O(n).
- Phase 2 (reverse): visits each node in the second half once. Total: O(n/2).
- Phase 3 (merge): visits each node once. Total: O(n).
- Overall: O(n) + O(n/2) + O(n) = O(n).

**Space: O(1)**

All operations are performed by rearranging existing node pointers. The only
extra storage is a fixed number of pointer variables (`slow`, `fast`, `prev`,
`curr`, `next`, `first`, `second`, `tmp`) regardless of input size.

---

## Function Reference

All functions are defined in `optimized/main.c`.

| Function | Signature | Description |
|---|---|---|
| `reorderList` | `void reorderList(struct ListNode *head)` | The optimized reorder algorithm. Finds the middle with slow/fast pointers, reverses the second half, and merges the two halves in-place. |
| `insert_list` | `struct ListNode *insert_list(int *nums, int size)` | Builds a singly linked list from an integer array. Allocates nodes from back to front so the list order matches the array order. Returns the head. |
| `show_list` | `void show_list(char *type, struct ListNode *head)` | Prints the linked list to stdout with a label. Format: `type: [ v1 v2 v3 ]`. |
| `free_list` | `void free_list(struct ListNode *head)` | Frees every node in the list by walking from head to tail. |
| `check_list` | `int check_list(struct ListNode *head, int *expected, int len)` | Validates the list against an expected integer array. Returns 1 if every node value matches and the list has exactly `len` nodes, 0 otherwise. |
| `run_test` | `void run_test(const char *name, int *nums, int len, int *expected, int expected_len)` | Runs a single test case: builds the list from `nums`, calls `reorderList`, prints input and output, asserts correctness via `check_list`, then frees the list. |

---

## Summary

The optimized Reorder List algorithm splits the work into three linear-time
phases: find the middle with slow/fast pointers, reverse the second half
in-place, and merge the two halves by interleaving nodes. Compared to the
original counting-based approach, it eliminates the separate length-counting
pass, removes all integer bookkeeping variables, and uses self-descriptive
pointer names (`slow`/`fast`, `prev`/`curr`/`next`, `first`/`second`) that
map directly to each phase. The result is the same O(n) time and O(1) space
but with fewer passes over the data and clearer code structure.
