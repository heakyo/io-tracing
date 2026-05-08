# 61 - Rotate List (Optimized)

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

Given the `head` of a linked list, rotate the list to the right by `k`
places.

For example, given `[1, 2, 3, 4, 5]` and `k = 2`, the last 2 nodes
(`4` and `5`) move to the front. The result is `[4, 5, 1, 2, 3]`.

---

## Core Idea

Form a cycle, then break it at the right spot.

The key observation is that rotating a linked list right by `k` positions
is the same as:

1. Making the list circular by connecting the tail to the head.
2. Walking to the node that should become the new tail.
3. Cutting the circle open at that point.

If the list has `size` nodes and the effective rotation is `n = k % size`,
the new tail is the node at position `size - n - 1` from the head (0-indexed).
Everything after the new tail becomes the front of the rotated list.

The original version does the same logical work but expresses it as
"find the split point, detach the second half, attach the second half
to the front, attach the first half to its end" -- four separate pointer
operations using a dummy node and confusingly named variables. The
cycle-based approach collapses all of that into one connect and one cut,
with no dummy node needed.

---

## Step-by-Step Walkthrough

Let's trace through `[1, 2, 3, 4, 5]` with `k = 2`.

### Phase 1: Early exit check

```
head != NULL, head->next != NULL, k != 0  -->  no early exit
```

### Phase 2: Count nodes and find the tail

Start with `size = 1` and `tail = head` (node `[1]`). Walk `tail` forward
until `tail->next` is `NULL`:

| Iteration | `tail` before | `tail->next` | `size` after | `tail` after |
|-----------|---------------|--------------|--------------|--------------|
| 1         | `[1]`         | `[2]`        | 2            | `[2]`        |
| 2         | `[2]`         | `[3]`        | 3            | `[3]`        |
| 3         | `[3]`         | `[4]`        | 4            | `[4]`        |
| 4         | `[4]`         | `[5]`        | 5            | `[5]`        |

`tail->next` is `NULL`, so the while loop ends. Now `size = 5` and
`tail` points to `[5]`.

```
[1] -> [2] -> [3] -> [4] -> [5] -> NULL
 ^                           ^
head                        tail         size = 5
```

### Phase 3: Compute effective rotation

```
n = k % size = 2 % 5 = 2
n != 0  -->  continue (no full-rotation shortcut)
```

### Phase 4: Form the cycle

Connect the tail back to the head:

```c
tail->next = head;
```

```
+---> [1] -> [2] -> [3] -> [4] -> [5] ---+
|                                         |
+-----------------------------------------+
```

The list is now circular. Every node is reachable from every other node.

### Phase 5: Walk to the new tail

We need to walk `size - n - 1 = 5 - 2 - 1 = 2` steps from `head` to
find `new_tail`:

```c
new_tail = head;    /* new_tail = [1] */
```

| Step (`i`) | `new_tail` before | `new_tail` after |
|------------|-------------------|------------------|
| 0          | `[1]`             | `[2]`            |
| 1          | `[2]`             | `[3]`            |

After the loop, `new_tail` points to `[3]`.

```
+---> [1] -> [2] -> [3] -> [4] -> [5] ---+
|                    ^                    |
+-----------------new_tail----------------+
```

### Phase 6: Break the cycle

```c
head = new_tail->next;    /* head = [4] */
new_tail->next = NULL;    /* [3]->next = NULL */
```

```
[4] -> [5] -> [1] -> [2] -> [3] -> NULL
 ^
head
```

### Result

Return `head`, which points to `[4]`. The final list is `[4, 5, 1, 2, 3]`.

---

## ASCII Flowchart

```
                    +-------+
                    | START |
                    +---+---+
                        |
                        v
          +----------------------------+
          | head == NULL ||            |    yes
          | head->next == NULL ||  +---------> return head
          | k == 0 ?               |
          +----------------------------+
                        | no
                        v
          +----------------------------+
          | size = 1, tail = head      |
          +----------------------------+
                        |
                        v
               +----------------+
               | tail->next ?   |<--------+
               +-------+--------+         |
                       |                  |
                  yes  |  no              |
                       v  |               |
           +-------------------+          |
           | size++            |          |
           | tail = tail->next +----------+
           +-------------------+
                       |
                  no   | (tail->next == NULL)
                       v
          +----------------------------+
          | n = k % size               |
          +----------------------------+
                       |
                       v
              +----------------+
              | n == 0 ?       |    yes
              +-------+--------+---------> return head
                      |
                 no   |
                      v
          +----------------------------+
          | tail->next = head          |
          | (form cycle)               |
          +----------------------------+
                      |
                      v
          +----------------------------+
          | new_tail = head            |
          +----------------------------+
                      |
                      v
            +-------------------+
            | i < size - n - 1? |<--------+
            +-------+-----------+         |
                    |                     |
               yes  |  no                 |
                    v  |                  |
        +-----------------------+         |
        | new_tail = new_tail   |         |
        |            ->next     |         |
        | i++                   +---------+
        +-----------------------+
                    |
               no   | (i == size - n - 1)
                    v
          +----------------------------+
          | head = new_tail->next      |
          | new_tail->next = NULL      |
          | (break cycle)              |
          +----------------------------+
                    |
                    v
          +----------------------------+
          | return head                |
          +-------------+--------------+
                        |
                        v
                    +-------+
                    |  END  |
                    +-------+
```

---

## Where It Gets Tricky

### (a) Cycle formation is safe because we never dereference into the void

After `tail->next = head`, the list is circular. Every `->next` pointer
in the ring points to a valid node. The for loop that follows walks
`new_tail` forward exactly `size - n - 1` steps -- always within the
ring, never past it. After the loop, `new_tail->next` is the future head
and `new_tail` itself is the future tail. We read `new_tail->next` once,
then set it to `NULL` to break the cycle. At no point do we follow a
`NULL` pointer or dereference freed memory.

If you were to get the step count wrong (for example, walking `size - n`
steps instead of `size - n - 1`), `new_tail` would overshoot by one
position and the cut would land in the wrong place. The cycle means you
would not segfault -- you would just get an incorrect rotation. This
makes the bug harder to catch by crashing, so getting the arithmetic
right matters.

### (b) Off-by-one in the walk: why `size - n - 1` and not `size - n`

We want `new_tail` to be the node **before** the new head. The new head
is at position `size - n` from the original head (0-indexed). So we need
`new_tail` at position `size - n - 1`. Starting from `head` (position 0),
we take `size - n - 1` steps.

Concrete check with our example (`size = 5`, `n = 2`):

- `size - n = 3` -- position of the new head (`[4]` is at index 3).
- `size - n - 1 = 2` -- position of the new tail (`[3]` is at index 2).
- Starting at `head` (index 0), we take 2 steps: `[1] -> [2] -> [3]`.

`new_tail` lands on `[3]`, and `new_tail->next` is `[4]` -- correct.

### (c) Counting starts at 1 with `tail = head`

The original version starts `size = 0` and counts inside the while loop
body, requiring a separate `prev` pointer to remember the node before
`cur` when `cur` reaches `NULL`. The optimized version starts `size = 1`
and `tail = head`, then only enters the while loop if `tail->next` is
non-NULL. This means `tail` always points to an actual node (the last
one), and the count is always correct. No need to track two pointers
during counting.

### (d) The `n == 0` early return is required

If `k` is a multiple of `size`, `n = k % size = 0`, meaning the list
does not change. Without this check, we would form a cycle
(`tail->next = head`) and then walk `size - 0 - 1 = size - 1` steps,
landing back at the original tail, producing the same list. But returning
early avoids the unnecessary cycle formation and is clearer in intent.

---

## Comparison with Original

| Aspect | Original | Optimized |
|--------|----------|-----------|
| Dummy node | Yes (`struct ListNode dummy = {0, head}`) | No, not needed |
| Pointer variables | `cur`, `next`, `prev` (3) | `tail`, `new_tail` (2) |
| Counting approach | `size = 0`, increments inside loop; `prev` trails `cur` | `size = 1`, `tail = head`; `tail` naturally ends at last node |
| Reconnect strategy | Four-step: detach second half, null-terminate first half, link old tail to head, update dummy | Two-step: form cycle (`tail->next = head`), break cycle (`new_tail->next = NULL`) |
| Variable naming | `prev` for the tail, `cur` for the split point, `next` declared early but used only at the end | `tail` for the tail, `new_tail` for the split point; names match their roles |
| Return value | `dummy.next` (indirection through dummy) | `new_tail->next` directly (then stored in `head`) |
| Time complexity | O(n) | O(n) |
| Space complexity | O(1) | O(1) |
| Conceptual model | Split and reconnect | Form cycle, then break |

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time** | O(n) | The first while loop visits every node once to count and find the tail (n iterations). The for loop walks at most n - 1 steps to find the new tail. Together: at most 2n - 1 node accesses, which is O(n). |
| **Space** | O(1) | Only a fixed number of pointer variables (`tail`, `new_tail`) and integer variables (`size`, `n`, `i`), regardless of input size. No heap allocation, no dummy node on the stack. |

---

## Function Reference

All functions are defined in `optimized/main.c`.

| Function | Description |
|----------|-------------|
| `struct ListNode *rotateRight(struct ListNode *head, int k)` | **Core algorithm.** Forms a cycle by connecting the tail to the head, walks to the node that should become the new tail, then breaks the cycle. Returns the new head. |
| `struct ListNode *insert_list(int *nums, int size)` | Build a linked list from an array of integers. Allocates nodes from back to front so the list order matches the array order. |
| `void show_list(char *type, struct ListNode *head)` | Print every element in the list, prefixed with a label string `type`. |
| `static void free_list(struct ListNode *head)` | Walk the list and `free()` every node. |
| `static int check_list(struct ListNode *head, int *expected, int len)` | Verify that the list matches an expected integer array. Returns 1 on match, 0 on mismatch. |
| `static void run_test(const char *name, int *nums, int len, int k, int *expected, int expected_len)` | Run a single test case: build list, call `rotateRight`, print results, assert correctness, then free remaining nodes. |

---

## Summary

The optimized version of Rotate List replaces the original's split-and-reconnect
approach with a simpler mental model: form a cycle, then break it. Instead of
juggling a dummy node, three confusingly named pointers (`prev`, `cur`, `next`),
and four separate pointer rewirings, the optimized code uses two pointers with
self-documenting names (`tail` and `new_tail`) and exactly two pointer operations
-- one to close the ring, one to open it. Counting starts at 1 with `tail = head`,
so `tail` naturally ends up pointing to the last node without any extra tracking.
The algorithmic complexity stays at O(n) time and O(1) space, but the code is
shorter, the variable names say what they mean, and the cycle-based pattern makes
it immediately clear why the rotation is correct.
