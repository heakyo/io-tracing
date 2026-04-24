# 206 - Reverse Linked List

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

Given the `head` of a singly linked list, reverse the list, and return the
reversed list.

**Constraints:**

- The number of nodes in the list is in the range `[0, 5000]`.
- `-5000 <= Node.val <= 5000`

---

## Core Idea

Imagine you have a line of people standing in a queue, each person holding the
shoulder of the person in front of them. You want to reverse the line so the
last person is now first and everyone faces the opposite direction.

Here is the trick: you don't pick everyone up and rearrange them. Instead, you
walk along the line from front to back. At each person, you tap them on the
shoulder, say "let go of the person in front of you, and grab onto this other
line instead," and hand them the end of a *second*, growing line that you're
building in reverse.

That second line is `rh` — the **reverse head**. It starts empty (`NULL`). One
by one, each node is detached from the forward chain and prepended to `rh`.
The `head` parameter is reused as a temporary variable to remember who was next
in the original line before you severed the link.

After you've tapped every person, `rh` points to the new head of the fully
reversed list. One pass, no extra memory, done.

```c
struct ListNode* reverseList(struct ListNode* head)
{
	struct ListNode *rh = NULL, *p = head;

	while (p) {
		head = head->next;   /* save next node in head */
		p->next = rh;        /* reverse the link       */
		rh = p;              /* advance reverse-head    */
		p = head;            /* advance cursor          */
	}

	return rh;
}
```

Three pointers do all the work:

| Pointer | Role |
|---------|------|
| `rh`    | Head of the reversed chain being built (starts `NULL`) |
| `p`     | Cursor — the node we're currently detaching |
| `head`  | Temp — saves the next node before we overwrite `p->next` |

---

## Step-by-Step Walkthrough

Walk through the list `[1, 2, 3, 4, 5]`:

| Step    | `head` (next saved) | `p->next = rh` (reverse link) | `rh` (reversed chain)       | `p` (cursor moves to)  |
|---------|---------------------|-------------------------------|-----------------------------|------------------------|
| Initial | —                   | —                             | `NULL`                      | `1->2->3->4->5`        |
| Iter 1  | `2->3->4->5`        | `1->NULL`                     | `1->NULL`                   | `2->3->4->5`           |
| Iter 2  | `3->4->5`           | `2->1->NULL`                  | `2->1->NULL`                | `3->4->5`              |
| Iter 3  | `4->5`              | `3->2->1->NULL`               | `3->2->1->NULL`             | `4->5`                 |
| Iter 4  | `5`                 | `4->3->2->1->NULL`            | `4->3->2->1->NULL`          | `5`                    |
| Iter 5  | `NULL`              | `5->4->3->2->1->NULL`         | `5->4->3->2->1->NULL`       | `NULL`                 |

`p` is now `NULL`, so the loop exits. Return `rh`: **5 → 4 → 3 → 2 → 1 → NULL**.

Think of it like moving cards one at a time from the top of a face-down deck
onto a new pile. The first card you move ends up on the bottom, and the last
card you move ends up on top — the order is reversed naturally.

---

## ASCII Flowchart

```
              +---------------------+
              |        START        |
              +---------------------+
                        |
                        v
              +---------------------+
              |  rh = NULL, p = head|
              +---------------------+
                        |
                        v
                 +--------------+
            +--->| p != NULL ?  |
            |    +--------------+
            |      |         |
            |     yes        no
            |      |         |
            |      v         v
            |  +-----------------+  +-------------+
            |  | head=head->next |  |  return rh  |
            |  | p->next = rh   |  +-------------+
            |  | rh = p         |        |
            |  | p = head       |        v
            |  +-----------------+  +----------+
            |      |                |   END    |
            +------+                +----------+
```

---

## Where It Gets Tricky

### (a) Reusing `head` as a temp variable

Most textbook implementations declare an explicit `next` pointer:

```c
struct ListNode *next = p->next;   /* obvious intent */
```

This code instead repurposes the function parameter `head` to hold that value.
Functionally it is identical — `head` is a local copy of the pointer, so
mutating it doesn't affect the caller. But when you read the code, seeing
`head = head->next` in the middle of a reversal loop is surprising. Your brain
expects `head` to mean "the beginning of the list," not "a scratch register."

It works. It just makes you do a double-take.

### (b) Empty list (head is NULL)

When the input list is empty, `head` is `NULL`, so `p` starts as `NULL`. The
`while (p)` condition is immediately false, the loop body never runs, and the
function returns `rh`, which is still `NULL`. This is the correct answer — the
reverse of an empty list is an empty list. No special-case code needed.

### (c) Order of operations matters

Inside the loop, the four statements must happen in exactly this order:

1. **Save next:** `head = head->next` — stash the pointer to the next node.
2. **Reverse link:** `p->next = rh` — point current node backward.
3. **Advance rh:** `rh = p` — the current node is now the new head of the
   reversed chain.
4. **Advance cursor:** `p = head` — move on to the saved next node.

If you swapped steps 1 and 2 — reversing the link *before* saving `next` —
you'd overwrite `p->next` (which equals `head->next` in the first iteration
when `p == head`) and lose the rest of the list forever. The rest of the nodes
would be leaked, unreachable. Save first, then cut.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time**  | O(n) | Single pass through all `n` nodes; each node is visited exactly once. |
| **Space** | O(1) | Only three pointer variables (`rh`, `p`, and reused `head`), regardless of list size. |

---

## Summary

The algorithm performs a **single-pass, in-place reversal** by maintaining a
reverse chain (`rh`) and prepending each node to it. On every iteration it:
saves the next pointer, redirects the current node's link to point backward,
advances the reverse head, and moves the cursor forward.

It reuses the `head` parameter as a temporary variable to save the next
pointer, which is functionally correct but less readable than using an explicit
`next` variable. If you were explaining this to a friend, you'd probably
rename `head` to `next` inside the loop — the algorithm is the same, but the
intent becomes obvious at a glance.

The beauty of this approach is its simplicity: three pointers, four lines in a
loop, O(n) time, O(1) space, and it handles every edge case — empty lists,
single nodes, long lists — without a single `if` statement.
