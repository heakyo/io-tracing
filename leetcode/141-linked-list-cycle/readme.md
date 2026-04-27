# 141 - Linked List Cycle

## Table of Contents

- [Problem Statement](#problem-statement)
- [Core Idea](#core-idea)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [ASCII Flowchart](#ascii-flowchart)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Summary](#summary)

---

## Problem Statement

Given the head of a linked list, determine if the linked list has a cycle in
it. A cycle exists if some node's `next` pointer points back to a previously
visited node. The `pos` parameter denotes the index of the node that the tail
connects to (0-indexed). If `pos` is -1, there is no cycle. Return `true` if
there is a cycle, `false` otherwise.

**Constraints:**

- The number of nodes is in the range [0, 10⁴].
- -10⁵ ≤ `Node.val` ≤ 10⁵.
- `pos` is -1 or a valid index in the linked list.

---

## Core Idea

This is **Floyd's cycle detection algorithm**, also known as the
tortoise-and-hare algorithm. The idea is dead simple once you see it:

> Imagine two runners on a track. One runs twice as fast as the other. If the
> track is a straight road, the fast runner reaches the end and stops — no
> meeting. But if the track loops back on itself, the fast runner will
> eventually lap the slow runner and they'll be at the same spot at the same
> time.

That's the whole algorithm. Use two pointers moving at different speeds through
the list:

- **Slow pointer (`sp`)** — moves one node at a time.
- **Quick pointer (`qp`)** — moves two nodes at a time.

If there's a cycle, the fast pointer enters it first, keeps looping, and
eventually lands on the same node as the slow pointer. If there's no cycle, the
fast pointer hits `NULL` and we're done.

```c
bool hasCycle(struct ListNode *head)
{
	struct ListNode *qp, *sp;

	if (head == NULL || head->next == NULL)
		return false;

	sp = head;
	qp = sp->next;

	while (qp && qp->next) {
		if (qp == sp)
			return true;

		sp = sp->next;
		qp = qp->next->next;
	}

	return false;
}
```

---

## Step-by-Step Walkthrough

Let's trace through the list `[3, 2, 0, -4]` with `pos = 1`, meaning the tail
node (-4) connects back to node index 1 (value 2). The list looks like this:

```
Index:  0     1     2     3
       [3] -> [2] -> [0] -> [-4]
               ^               |
               |               |
               +---------------+
```

**Initial setup:** `sp = Node0(3)`, `qp = Node1(2)` — they start one step
apart, because `qp = sp->next`.

Now let's trace each iteration. Remember: the equality check happens *before*
advancing the pointers.

| Iter | sp (before) | qp (before) | qp == sp? | Action                                          |
|------|-------------|-------------|-----------|--------------------------------------------------|
| 1    | Node0 (3)   | Node1 (2)   | No        | sp = Node1 (2), qp = Node3 (-4)                 |
| 2    | Node1 (2)   | Node3 (-4)  | No        | sp = Node2 (0), qp = Node3→next→next = Node2 (0)|
| 3    | Node2 (0)   | Node2 (0)   | **Yes!**  | Return `true`                                    |

Let's unpack iteration 2 carefully, because the wrap-around is where the magic
happens:

- `qp` is at Node3 (-4). Its `next` is Node1 (2) — that's the cycle link.
- `qp->next->next` is Node1's next, which is Node2 (0).
- So `qp` jumps from Node3 → (Node1) → Node2.
- Meanwhile `sp` just moves from Node1 → Node2.
- Now they're on the same node. The fast pointer lapped the slow pointer inside
  the cycle. Done.

---

## ASCII Flowchart

```
                        START
                          |
                          v
            +-----------------------------+
            | head == NULL                |
            |       or                    |
            | head->next == NULL?         |
            +-----------------------------+
                 |               |
                yes              no
                 |               |
                 v               v
           +-----------+   sp = head
           |return false|   qp = head->next
           +-----------+        |
                                v
                   +------------------------+
               +-->| qp && qp->next?        |
               |   +------------------------+
               |        |             |
               |       yes            no
               |        |             |
               |        v             v
               |   +-----------+ +-----------+
               |   | qp == sp? | |return false|
               |   +-----------+ +-----------+
               |     |       |
               |    yes      no
               |     |       |
               |     v       v
               | +----------+ sp = sp->next
               | |return true| qp = qp->next->next
               | +----------+    |
               |                 |
               +-----------------+
```

---

## Where It Gets Tricky

### (a) The starting positions are unusual

In this implementation, `sp` starts at `head` and `qp` starts at
`head->next` — they begin **one step apart**. The equality check
`if (qp == sp)` happens *before* advancing the pointers.

The more standard version of Floyd's algorithm starts both pointers at `head`
and checks for equality *after* advancing:

```c
// Standard version (for comparison)
slow = head;
fast = head;
while (fast && fast->next) {
	slow = slow->next;
	fast = fast->next->next;
	if (slow == fast)
		return true;
}
```

Both are correct. They're equivalent — just shifted by one step. But if you're
used to seeing the standard form, this version can look subtly wrong at first
glance. It isn't.

### (b) The early return is technically redundant

The check `head->next == NULL` is not strictly necessary. If `head->next` is
`NULL`, then `qp` would be `NULL`, and the `while (qp && qp->next)` loop
simply wouldn't execute — we'd fall through to `return false` anyway.

So why is it there? It's a minor optimization: it avoids the pointer assignment
`qp = sp->next` and the loop condition evaluation. In practice, the difference
is negligible, but it makes the intent explicit — a single-node list can't have
a cycle.

### (c) Variable naming is cryptic

`sp` and `qp` stand for "slow pointer" and "quick pointer," but that's not
obvious at a glance. The conventional names are `slow`/`fast` or
`tortoise`/`hare`. If you're reading this code for the first time, the names
don't help you. Once you know the convention, it's fine — but it's a speed bump
for new readers.

### (d) Why does the fast pointer always catch the slow one?

This is the part that feels like it shouldn't work. Here's why it does:

Once both pointers are inside the cycle, think about the *gap* between them.
Each iteration, the slow pointer moves 1 step forward and the fast pointer
moves 2 steps forward. That means the gap between them changes by exactly
1 each iteration. Since the gap starts at some value ≤ cycle_length and
decreases by 1 every step, it will reach 0 within at most cycle_length
iterations. When the gap is 0, they're on the same node.

It's like a clock: the minute hand will always eventually line up with the
hour hand, because one moves faster than the other.

---

## Complexity Analysis

**Time: O(n)**

- If there is **no cycle**, the fast pointer reaches `NULL` in at most n/2
  steps, where n is the number of nodes. That's O(n).
- If there **is** a cycle, the slow pointer enters the cycle after at most n
  steps. Once both pointers are in the cycle, the fast pointer catches the slow
  pointer within at most cycle_length steps. Total ≤ n + cycle_length = O(n).

**Space: O(1)**

Only two pointer variables (`sp` and `qp`) are used, regardless of input size.
No hash sets, no marking nodes, no auxiliary data structures.

---

## Summary

Floyd's cycle detection algorithm works by running two pointers through the
list at different speeds. If the list has a cycle, the fast pointer eventually
catches the slow one — guaranteed by the fact that the gap closes by exactly 1
on every iteration. If there's no cycle, the fast pointer hits `NULL`.

This implementation is **correct** and **efficient** — O(n) time, O(1) space.
It has two minor style quirks worth noting:

1. The naming (`sp`/`qp`) is less readable than the conventional `slow`/`fast`
   or `tortoise`/`hare`.
2. The starting positions (one step apart, check before advance) differ from
   the more common both-at-head, advance-then-check pattern.

Neither of these affects correctness. The algorithm is sound, the edge cases
are handled, and the complexity is optimal. You can't do better than O(n) time
for this problem, and you can't do better than O(1) space without changing
the problem.
