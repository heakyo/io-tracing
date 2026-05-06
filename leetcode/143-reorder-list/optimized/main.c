#include <stdio.h>
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
void reorderList(struct ListNode* head)
{
	struct ListNode *slow, *fast, *prev, *curr, *next;
	struct ListNode *first, *second, *tmp;

	if (head == NULL || head->next == NULL)
		return;

	/* Step 1: find the middle using slow/fast pointers */
	slow = head;
	fast = head;
	while (fast->next && fast->next->next) {
		slow = slow->next;
		fast = fast->next->next;
	}

	/* Step 2: reverse the second half */
	prev = NULL;
	curr = slow->next;
	slow->next = NULL;

	while (curr) {
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}

	/* Step 3: merge the two halves */
	first = head;
	second = prev;
	while (second) {
		tmp = first->next;
		first->next = second;
		second = second->next;
		first->next->next = tmp;
		first = tmp;
	}
}

static void free_list(struct ListNode *head)
{
	struct ListNode *tmp;

	while (head) {
		tmp = head;
		head = head->next;
		free(tmp);
	}
}

static int check_list(struct ListNode *head, int *expected, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if (head == NULL || head->val != expected[i])
			return 0;
		head = head->next;
	}
	return head == NULL;
}

static void run_test(const char *name, int *nums, int len,
		     int *expected, int expected_len)
{
	struct ListNode *head;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);

	reorderList(head);
	show_list("Output", head);

	assert(check_list(head, expected, expected_len));
	printf("PASS\n\n");

	free_list(head);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 — even length */
	{
		int in[] = {1, 2, 3, 4};
		int ex[] = {1, 4, 2, 3};
		run_test("LeetCode example 1", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 2: LeetCode example 2 — odd length */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 5, 2, 4, 3};
		run_test("LeetCode example 2", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 3: Single element */
	{
		int in[] = {1};
		int ex[] = {1};
		run_test("Single element", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 4: Two elements */
	{
		int in[] = {1, 2};
		int ex[] = {1, 2};
		run_test("Two elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 5: Three elements */
	{
		int in[] = {1, 2, 3};
		int ex[] = {1, 3, 2};
		run_test("Three elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 6: Six elements (even) */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {1, 6, 2, 5, 3, 4};
		run_test("Six elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 7: Seven elements (odd) */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7};
		int ex[] = {1, 7, 2, 6, 3, 5, 4};
		run_test("Seven elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 8: Eight elements (even) */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8};
		int ex[] = {1, 8, 2, 7, 3, 6, 4, 5};
		run_test("Eight elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 9: Non-sequential values */
	{
		int in[] = {10, 20, 30, 40, 50};
		int ex[] = {10, 50, 20, 40, 30};
		run_test("Non-sequential values", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 10: Ten elements */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int ex[] = {1, 10, 2, 9, 3, 8, 4, 7, 5, 6};
		run_test("Ten elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}
