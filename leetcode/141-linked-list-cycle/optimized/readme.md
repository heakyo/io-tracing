# 141 - Linked List Cycle (Optimized)

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

Given the `head` of a linked list, determine if the linked list has a cycle
in it.

A cycle exists when some node in the list can be reached again by
continuously following the `next` pointer. Return `true` if there is a
cycle, `false` otherwise.

---

## Core Idea

Imagine two runners on a circular track. One runs twice as fast as the
other. If the track loops back on itself, the fast runner will eventually
lap the slow runner and they will be at the same spot. If the track has an
end, the fast runner simply reaches it first.

That is Floyd's tortoise-and-hare algorithm in one sentence.

The implementation is three lines of logic:

```c
bool hasCycle(struct ListNode *head)
{
	struct ListNode *slow = head, *fast = head;

	while (fast && fast->next) {
		slow = slow->next;
		fast = fast->next->next;

		if (slow == fast)
			return true;
	}

	return false;
}
```

Both pointers start at `head`. Each iteration, slow advances by one node
and fast advances by two. After advancing, we check: are they pointing at
the same node? If yes, we found a cycle. If fast runs off the end of the
list (hits `NULL`), there is no cycle.

This optimized version is cleaner than the original because:

1. **Conventional naming** — `slow` and `fast` instead of `sp` and `qp`.
   Anyone who has seen Floyd's algorithm recognizes these names instantly.
2. **Both start at head** — The original started them one step apart
   (`sp = head`, `qp = sp->next`). Starting both at `head` is the textbook
   pattern.
3. **Advance-then-check** — The standard order. The original checked before
   advancing, which required starting the pointers apart to avoid a false
   positive on the first comparison.
4. **No early-return guard** — The original had
   `if (head == NULL || head->next == NULL) return false;` up front. The
   `while` condition `fast && fast->next` already handles both cases.
5. **Fewer lines** — No separate initialization block, no redundant guard.

---

## Step-by-Step Walkthrough

Input: `[3, 2, 0, -4]` with `pos = 1` (tail connects back to index 1).

The list looks like this:

```
Node0(3) -> Node1(2) -> Node2(0) -> Node3(-4)
               ^                        |
               +------------------------+
```

Initial state: `slow = Node0(3)`, `fast = Node0(3)`.

| Iteration | Action                                         | slow   | fast   | Equal? |
|-----------|-------------------------------------------------|--------|--------|--------|
| —         | Initialize both at head                        | Node0(3) | Node0(3) | (skip) |
| 1         | slow = Node0→next = **Node1**; fast = Node0→next→next = **Node2** | Node1(2) | Node2(0) | No  |
| 2         | slow = Node1→next = **Node2**; fast = Node2→next→next = Node3→next = **Node1** | Node2(0) | Node1(2) | No  |
| 3         | slow = Node2→next = **Node3**; fast = Node1→next→next = Node2→next = **Node3** | Node3(-4) | Node3(-4) | **Yes!** |

`slow == fast` at Node3. Return `true`. Cycle detected.

---

## ASCII Flowchart

```
               +---------------------+
               |        START        |
               +---------------------+
                         |
                         v
              +------------------------+
              | slow = head            |
              | fast = head            |
              +------------------------+
                         |
                         v
              +------------------------+
          +-->| fast && fast->next ?   |
          |   +------------------------+
          |        |             |
          |       YES            NO
          |        |             |
          |        v             v
          |   +-----------+   +---------------+
          |   | slow =    |   | return false  |
          |   | slow->next|   +---------------+
          |   | fast =    |
          |   | fast->next|
          |   |   ->next  |
          |   +-----------+
          |        |
          |        v
          |   +-----------+
          |   | slow ==   |
          |   | fast ?    |
          |   +-----------+
          |     |       |
          |    YES      NO
          |     |       |
          |     v       +--+
          |  +------+      |
          |  |return|      |
          |  | true |      |
          |  +------+      |
          |                |
          +----------------+
```

---

## Function Reference

All functions are defined in `optimized/main.c`.

### `struct ListNode *insert_list(int *nums, int size)`

Build a linked list from an array. Iterates from the last element to the
first, inserting each at the head. The resulting list preserves the original
array order.

### `void show_list(char *type, struct ListNode *head)`

Print every node's value in the list, prefixed with a label. Useful for
debugging. Output format: `type: [ v1 v2 v3 ]`.

### `bool hasCycle(struct ListNode *head)`

Floyd's cycle detection. Two pointers start at `head`; slow moves one step,
fast moves two steps each iteration. If they meet, a cycle exists.
**O(n) time, O(1) space.**

### `static struct ListNode *make_cycle_list(int *nums, int size, int pos)`

Build a list from `nums[0..size-1]`. If `pos >= 0`, connect the tail node's
`next` pointer to the node at index `pos`, creating a cycle. If `pos < 0`,
the list remains acyclic.

### `static void free_cycle_list(struct ListNode *head, int size)`

Free exactly `size` nodes starting from `head`. This is safe for cyclic
lists because it counts nodes freed rather than walking until `NULL` (which
would loop forever in a cycle).

### `static void run_test(const char *name, int *nums, int size, int pos, bool expected)`

Run a single test case: build a (possibly cyclic) list, call `hasCycle`,
assert the result matches `expected`, then free the list.

---

## Where It Gets Tricky

### Why advance before comparing?

Both pointers start at `head`. If we checked `slow == fast` before moving
them, the very first check would be `head == head` — always `true`. We
would incorrectly report a cycle on every non-empty list.

The original code worked around this by starting the pointers one step
apart (`sp = head`, `qp = head->next`), which meant it could
check-then-advance. This version avoids that asymmetry by using the
textbook advance-then-check order.

### Why does the while condition check both `fast` AND `fast->next`?

Fast moves two steps per iteration: `fast = fast->next->next`. If `fast` is
`NULL`, dereferencing `fast->next` is undefined behavior. If `fast->next` is
`NULL`, dereferencing `fast->next->next` is undefined behavior. We need both
guards because fast skips a node each time.

Slow only moves one step, so it never outruns fast — we do not need a
separate `NULL` check for slow.

### Why does the fast pointer always catch the slow one?

Once slow enters the cycle, think about the gap between them. Each
iteration, slow moves 1 step and fast moves 2 steps, so fast closes the gap
by exactly 1 node per iteration. If the gap is `k` nodes, they meet after
exactly `k` more iterations. The maximum gap is the cycle length, so they
are guaranteed to meet within one full traversal of the cycle.

### What about edge cases the original guarded against?

- **Empty list** (`head == NULL`): `fast` starts as `NULL`. The while
  condition `fast && fast->next` is immediately `false`. Return `false`.
  Correct — no special case needed.
- **Single node, no cycle** (`head->next == NULL`): `fast` is `head`,
  `fast->next` is `NULL`. The while condition is `false`. Return `false`.
  Correct — no special case needed.

The original's `if (head == NULL || head->next == NULL) return false;` guard
was redundant. Removing it costs nothing and reduces noise.

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time**  | O(n) | If no cycle, fast reaches the end in n/2 iterations. If a cycle exists, slow enters the cycle within n steps, and fast catches it within cycle_length more steps. Total ≤ 2n. |
| **Space** | O(1) | Two pointers. No extra data structures. |

The complexity is identical to the original. The improvement is purely in
code clarity and adherence to the conventional Floyd's algorithm pattern.

---

## Summary

The optimized version is the textbook Floyd's tortoise-and-hare algorithm,
no more, no less. Both pointers start at `head`. Advance first, then check
for a meeting. Use `slow` and `fast` as names. Let the while condition
handle `NULL` — no redundant early-return guards.

It produces identical results to the original with fewer lines, no
asymmetric initialization, and a structure that anyone familiar with the
algorithm will recognize on sight. The time and space complexity remain
O(n) and O(1) respectively.
