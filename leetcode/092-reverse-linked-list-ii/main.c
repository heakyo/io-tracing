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
 *     struct Lissssss *next;
 * };
 */
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
		     int left, int right,
		     int *expected, int expected_len)
{
	struct ListNode *head, *result;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);
	printf("left=%d, right=%d\n", left, right);

	result = reverseBetween(head, left, right);
	show_list("Output", result);

	assert(check_list(result, expected, expected_len));
	printf("PASS\n\n");

	free_list(result);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 — reverse middle */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 4, 3, 2, 5};
		run_test("LeetCode example 1", in, ARRAYSIZE(in),
			 2, 4, ex, ARRAYSIZE(ex));
	}

	/* Test 2: LeetCode example 2 — single element */
	{
		int in[] = {5};
		int ex[] = {5};
		run_test("Single element", in, ARRAYSIZE(in),
			 1, 1, ex, ARRAYSIZE(ex));
	}

	/* Test 3: Reverse entire list */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {5, 4, 3, 2, 1};
		run_test("Reverse entire list", in, ARRAYSIZE(in),
			 1, 5, ex, ARRAYSIZE(ex));
	}

	/* Test 4: left == right at head */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 3, 4, 5};
		run_test("left==right at head", in, ARRAYSIZE(in),
			 1, 1, ex, ARRAYSIZE(ex));
	}

	/* Test 5: left == right at tail */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 3, 4, 5};
		run_test("left==right at tail", in, ARRAYSIZE(in),
			 5, 5, ex, ARRAYSIZE(ex));
	}

	/* Test 6: Two elements, reverse all */
	{
		int in[] = {1, 2};
		int ex[] = {2, 1};
		run_test("Two elements, reverse all", in, ARRAYSIZE(in),
			 1, 2, ex, ARRAYSIZE(ex));
	}

	/* Test 7: Reverse prefix */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {3, 2, 1, 4, 5};
		run_test("Reverse prefix (1..3)", in, ARRAYSIZE(in),
			 1, 3, ex, ARRAYSIZE(ex));
	}

	/* Test 8: Reverse suffix */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 5, 4, 3};
		run_test("Reverse suffix (3..5)", in, ARRAYSIZE(in),
			 3, 5, ex, ARRAYSIZE(ex));
	}

	/* Test 9: Reverse last two */
	{
		int in[] = {1, 2, 3};
		int ex[] = {1, 3, 2};
		run_test("Reverse last two (2..3)", in, ARRAYSIZE(in),
			 2, 3, ex, ARRAYSIZE(ex));
	}

	/* Test 10: Long list, reverse middle section */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int ex[] = {1, 2, 8, 7, 6, 5, 4, 3, 9, 10};
		run_test("Long list, reverse (3..8)", in, ARRAYSIZE(in),
			 3, 8, ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}

