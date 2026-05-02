# 092 - Reverse Linked List II

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

Given the `head` of a singly linked list and two integers `left` and `right`
(1-indexed, `left <= right`), reverse the nodes of the list from position
`left` to position `right`, and return the modified list.

**Constraints:**

- `1 <= n <= 500` (where *n* is the number of nodes)
- `-500 <= Node.val <= 500`
- `1 <= left <= right <= n`

**Example:**

```
Input:  1 -> 2 -> 3 -> 4 -> 5,  left = 2, right = 4
Output: 1 -> 4 -> 3 -> 2 -> 5
```

---

## Core Idea

Imagine you have a hand of cards fanned out on a table, left to right. Someone
tells you: "Reverse the cards from position 2 to position 4." You wouldn't pick
up all three cards, flip them, and put them back. Instead, you'd do something
simpler — you'd repeatedly pull the *next* card from the right side of the
group and tuck it in at the left edge of the group, one at a time.

That is exactly what the **head-insertion technique** does:

1. **Plant a dummy node** in front of the list. Think of it as an anchor post
   at the very beginning of the card row — it never moves and gives you
   something to attach cards to, even when the first card itself needs to be
   rearranged.

2. **Navigate** to the action zone. Walk two pointers forward: `pre` stops
   just *before* the segment to reverse, and `cur` stops at the *first* node
   of the segment (position `left`).

3. **Reverse by head-insertion.** In each iteration, grab the node sitting
   immediately after `cur`, detach it, and re-insert it right after `pre`.
   It's like pulling a card out of the middle of the fan and tucking it at
   the front of the group. `cur` never moves — it just sinks one position
   deeper each time a new node is inserted ahead of it. After `right - left`
   iterations the segment is fully reversed.

```c
struct ListNode* reverseBetween(struct ListNode* head, int left, int right)
{
	struct ListNode *dummy, *pre, *cur, *tmp;
	int i;

	dummy = malloc(sizeof(*dummy));
	dummy->next = head;
	pre = dummy;
	cur = pre->next;

	for (i = 1; i < left; i++) {
		pre = cur;
		cur = cur->next;
	}

	for (i = left; i < right; i++) {
		tmp = pre->next;
		pre->next = cur->next;
		cur->next = cur->next->next;
		pre->next->next = tmp;
	}

	return dummy->next;
}
```

---

## Step-by-Step Walkthrough

Let's trace through the example: `[1, 2, 3, 4, 5]`, `left = 2`, `right = 4`.

### Initialization

```
dummy = malloc(...)
dummy->next = head

  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
  pre = dummy
  cur = 1  (pre->next)
```

### Navigation Loop (`for i = 1; i < left`)

Only one iteration (`i = 1`, since `left = 2`):

```
pre = cur        =>  pre = Node(1)
cur = cur->next  =>  cur = Node(2)
```

After navigation:

```
  dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
           ^    ^
          pre  cur
```

### Reversal Loop (`for i = left; i < right`)

| Iter | i | Operation | List after iteration | `cur` |
|------|---|-----------|---------------------|-------|
| — | — | *(start)* | `dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL` | Node(2) |
| 1 | 2 | Extract Node(3), insert after `pre` | `dummy -> 1 -> 3 -> 2 -> 4 -> 5 -> NULL` | Node(2) |
| 2 | 3 | Extract Node(4), insert after `pre` | `dummy -> 1 -> 4 -> 3 -> 2 -> 5 -> NULL` | Node(2) |

Let's zoom into each iteration to see the pointer surgery:

#### Iteration 1 (`i = 2`)

```
Before:
  pre -> 1    cur -> 2 -> 3 -> 4 -> 5 -> NULL
         |           ^
         +-----------+
         (pre->next = cur is Node(2))

Step 1:  tmp = pre->next             =>  tmp = Node(2)
Step 2:  pre->next = cur->next       =>  pre->next = Node(3)
Step 3:  cur->next = cur->next->next =>  cur->next = Node(4)
         (Node(2) now skips Node(3), points to Node(4))
Step 4:  pre->next->next = tmp       =>  Node(3)->next = Node(2)

After:
  dummy -> 1 -> 3 -> 2 -> 4 -> 5 -> NULL
           ^         ^
          pre       cur
```

#### Iteration 2 (`i = 3`)

```
Before:
  dummy -> 1 -> 3 -> 2 -> 4 -> 5 -> NULL
           ^         ^
          pre       cur

Step 1:  tmp = pre->next             =>  tmp = Node(3)
Step 2:  pre->next = cur->next       =>  pre->next = Node(4)
Step 3:  cur->next = cur->next->next =>  cur->next = Node(5)
         (Node(2) now skips Node(4), points to Node(5))
Step 4:  pre->next->next = tmp       =>  Node(4)->next = Node(3)

After:
  dummy -> 1 -> 4 -> 3 -> 2 -> 5 -> NULL
           ^              ^
          pre            cur
```

### Return

```
return dummy->next  =>  1 -> 4 -> 3 -> 2 -> 5
```

The segment `[2, 3, 4]` has been reversed to `[4, 3, 2]`. Node(2) — the
original `cur` — now sits at the *end* of the reversed segment (position 4),
exactly where you'd expect.

---

## ASCII Flowchart

```
                ┌──────────────────────────────┐
                │            START              │
                └──────────────┬───────────────┘
                               │
                               ▼
                ┌──────────────────────────────┐
                │  dummy = malloc(sizeof(*d))  │
                │  dummy->next = head          │
                │  pre = dummy                 │
                │  cur = pre->next             │
                │  i = 1                       │
                └──────────────┬───────────────┘
                               │
                               ▼
                       ┌───────────────┐
                       │   i < left ?  │──── No ───┐
                       └───────┬───────┘           │
                               │ Yes               │
                               ▼                   │
                ┌──────────────────────────────┐   │
                │  pre = cur                   │   │
                │  cur = cur->next             │   │
                │  i++                         │   │
                └──────────────┬───────────────┘   │
                               │                   │
                               └── (loop back) ──┘ │
                                                   │
                               ┌───────────────────┘
                               │
                               ▼
                       ┌────────────────┐
                       │ i = left       │
                       └───────┬────────┘
                               │
                               ▼
                       ┌───────────────┐
                       │  i < right ?  │──── No ───┐
                       └───────┬───────┘           │
                               │ Yes               │
                               ▼                   │
                ┌──────────────────────────────┐   │
                │  tmp = pre->next             │   │
                │  pre->next = cur->next       │   │
                │  cur->next = cur->next->next │   │
                │  pre->next->next = tmp       │   │
                │  i++                         │   │
                └──────────────┬───────────────┘   │
                               │                   │
                               └── (loop back) ──┘ │
                                                   │
                               ┌───────────────────┘
                               │
                               ▼
                ┌──────────────────────────────┐
                │     return dummy->next       │
                └──────────────────────────────┘
```

---

## Where It Gets Tricky

### (a) Why a dummy node?

When `left == 1`, the very first node of the list gets moved — the `head`
pointer itself changes. Without a dummy, you'd need a special case to update
`head`. The dummy acts like a signpost cemented into the ground before the first
house on the street: no matter how you rearrange the houses, the signpost stays
put and always points to whatever house ends up first.

With the dummy, `pre` starts at `dummy` and `cur` starts at `head`. The
reversal loop inserts nodes after `pre` (which is `dummy`), so `dummy->next`
always reflects the true new head. Returning `dummy->next` handles every case
uniformly — whether `left` is 1 or 100.

### (b) `cur` never moves

This is the most counter-intuitive part. You might expect `cur` to walk forward
through the list during reversal. It doesn't. `cur` always points to the same
node — the one that was originally at position `left`.

Think of it this way: `cur` is like a person standing in a queue. Other people
keep cutting in front of them. The person hasn't moved, but their position in
the queue keeps shifting backwards. After all the cutting is done, the person
who was 2nd in line ends up 4th — exactly at position `right`.

### (c) Memory leak

The `dummy` node is allocated with `malloc` but never freed:

```c
dummy = malloc(sizeof(*dummy));
/* ... */
return dummy->next;   /* dummy itself is leaked! */
```

On LeetCode this is harmless — the online judge doesn't check for leaks.
But in production code this is a bug. The fix is either:

- **Free before returning:** save `dummy->next` in a local variable, call
  `free(dummy)`, then return the saved pointer.
- **Use a stack variable:** declare `struct ListNode dummy_node;` on the stack
  instead of calling `malloc`. No allocation, no leak.

### (d) The 4-line swap: order matters

The reversal loop performs four pointer updates in a very specific order:

```c
tmp = pre->next;             /* 1. save the current front of reversed segment */
pre->next = cur->next;       /* 2. link pre to the node we're extracting      */
cur->next = cur->next->next; /* 3. skip over the extracted node               */
pre->next->next = tmp;       /* 4. extracted node points to old front         */
```

If you swap lines 2 and 3, `cur->next` would already be overwritten before you
read it. If you skip line 1, you lose the pointer to the old front of the
reversed segment. The order is a careful chain: each line depends on values that
the *next* line is about to overwrite. The `tmp` variable breaks the dependency
cycle, just like a temporary variable in a classic swap.

---

## Complexity Analysis

| Metric | Value | Explanation |
|--------|-------|-------------|
| **Time** | O(n) | Navigate to position `left` takes at most *n* steps. The reversal loop runs `right - left` times. Total work ≤ *n*. Single pass, no nested loops. |
| **Space** | O(1) | Only a fixed number of pointers (`dummy`, `pre`, `cur`, `tmp`) regardless of input size. The `malloc`'d dummy node is constant-size overhead (and should ideally be a stack variable). |

---

## Summary

The head-insertion technique elegantly reverses a sub-list in one pass by
repeatedly extracting the next node after `cur` and inserting it at the front
of the reversed segment (right after `pre`). Picture a card dealer pulling
cards from one spot in the fan and tucking them into another — one at a time,
always at the same insertion point.

The **dummy node** simplifies edge cases when `left == 1` by providing a
stable anchor that never moves. The key insight is that **`cur` never moves**
— it always points to the original node at position `left`, which gradually
sinks to position `right` as new nodes are inserted ahead of it.

The algorithm is **O(n) time** and **O(1) space**, making it optimal for
in-place linked list reversal of a sub-segment.
