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
struct ListNode* reverseList(struct ListNode* head)
{
	struct ListNode *rh = NULL, *p = head;

	while (p) {

		head = head->next;

		p->next = rh;
		rh = p;

		p = head;
	}

	return rh;
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
	struct ListNode *head, *result;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);

	result = reverseList(head);
	show_list("Output", result);

	assert(check_list(result, expected, expected_len));
	printf("PASS\n\n");

	free_list(result);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {5, 4, 3, 2, 1};
		run_test("LeetCode example", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 2: Two elements */
	{
		int in[] = {1, 2};
		int ex[] = {2, 1};
		run_test("Two elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 3: Single element */
	{
		int in[] = {1};
		int ex[] = {1};
		run_test("Single element", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 4: Empty list */
	run_test("Empty list", NULL, 0, NULL, 0);

	/* Test 5: Three elements */
	{
		int in[] = {1, 2, 3};
		int ex[] = {3, 2, 1};
		run_test("Three elements", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 6: Negative values */
	{
		int in[] = {-1, -2, -3};
		int ex[] = {-3, -2, -1};
		run_test("Negative values", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 7: All same values */
	{
		int in[] = {1, 1, 1, 1};
		int ex[] = {1, 1, 1, 1};
		run_test("All same values", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 8: Mixed positive/negative */
	{
		int in[] = {-5, 0, 5};
		int ex[] = {5, 0, -5};
		run_test("Mixed positive/negative", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 9: Large range */
	{
		int in[] = {-100, 0, 100};
		int ex[] = {100, 0, -100};
		run_test("Large range", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	/* Test 10: Longer list */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int ex[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
		run_test("Longer list (10 elements)", in, ARRAYSIZE(in),
			 ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}

