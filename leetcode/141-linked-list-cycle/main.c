#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

struct ListNode {
	int val;
	struct ListNode *next;
};

struct ListNode *insert_list(int *nums, int size)
{
	struct ListNode *new, *head;
	int i;

	head = NULL;

	for (i = size - 1; i >= 0; i--) {
		new = (struct ListNode *)malloc(sizeof(*new));
		new->val = nums[i];
		new->next = head;

		head = new;
	}

	return head;
}

void show_list(char *type, struct ListNode* head)
{
	struct ListNode *p = head;

	printf("%s: [ ", type);
	while (p) {
		printf("%d ", p->val);
		p = p->next;
	}
	printf("]\n");
}

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
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

/*
 * Build a list from nums[0..size-1].
 * If pos >= 0, connect the tail to the node at index pos (creating a cycle).
 * If pos < 0, no cycle.
 */
static struct ListNode *make_cycle_list(int *nums, int size, int pos)
{
	struct ListNode *head, *tail, *target;
	int i;

	head = insert_list(nums, size);
	if (pos < 0 || head == NULL)
		return head;

	target = head;
	for (i = 0; i < pos; i++)
		target = target->next;

	tail = head;
	while (tail->next)
		tail = tail->next;
	tail->next = target;

	return head;
}

static void free_cycle_list(struct ListNode *head, int size)
{
	struct ListNode *tmp;
	int i;

	for (i = 0; i < size; i++) {
		tmp = head;
		head = head->next;
		free(tmp);
	}
}

static void run_test(const char *name, int *nums, int size,
		     int pos, bool expected)
{
	struct ListNode *head;
	bool result;

	printf("=== %s ===\n", name);
	head = make_cycle_list(nums, size, pos);

	printf("Input:    size=%d, pos=%d\n", size, pos);

	result = hasCycle(head);

	printf("Output:   %s\n", result ? "true" : "false");
	printf("Expected: %s\n", expected ? "true" : "false");

	assert(result == expected);
	printf("PASS\n\n");

	free_cycle_list(head, size);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 — cycle at pos 1 */
	{
		int nums[] = {3, 2, 0, -4};
		run_test("LeetCode example 1", nums, ARRAYSIZE(nums),
			 1, true);
	}

	/* Test 2: LeetCode example 2 — cycle at pos 0 */
	{
		int nums[] = {1, 2};
		run_test("LeetCode example 2", nums, ARRAYSIZE(nums),
			 0, true);
	}

	/* Test 3: LeetCode example 3 — single node, no cycle */
	{
		int nums[] = {1};
		run_test("Single node, no cycle", nums, ARRAYSIZE(nums),
			 -1, false);
	}

	/* Test 4: Empty list */
	run_test("Empty list", NULL, 0, -1, false);

	/* Test 5: Five nodes, no cycle */
	{
		int nums[] = {1, 2, 3, 4, 5};
		run_test("Five nodes, no cycle", nums, ARRAYSIZE(nums),
			 -1, false);
	}

	/* Test 6: Five nodes, tail connects to head */
	{
		int nums[] = {1, 2, 3, 4, 5};
		run_test("Tail connects to head", nums, ARRAYSIZE(nums),
			 0, true);
	}

	/* Test 7: Self-loop on last node */
	{
		int nums[] = {1, 2, 3, 4, 5};
		run_test("Self-loop on last node", nums, ARRAYSIZE(nums),
			 4, true);
	}

	/* Test 8: Single node with self-loop */
	{
		int nums[] = {1};
		run_test("Single node, self-loop", nums, ARRAYSIZE(nums),
			 0, true);
	}

	/* Test 9: Long list, cycle in middle */
	{
		int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		run_test("Long list, cycle at pos 4", nums, ARRAYSIZE(nums),
			 4, true);
	}

	/* Test 10: Two nodes, no cycle */
	{
		int nums[] = {1, 2};
		run_test("Two nodes, no cycle", nums, ARRAYSIZE(nums),
			 -1, false);
	}

	printf("All tests passed!\n");
	return 0;
}

