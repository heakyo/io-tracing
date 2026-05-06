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
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
	struct ListNode dummy = {0, head};
	struct ListNode *slow, *fast, *prev;
	int i;

	slow = head;
	fast = head;
	prev = &dummy;
	for (i = 0; i < n; i++)
		fast = fast->next;

	while (fast) {
		prev = slow;
		slow = slow->next;

		fast = fast->next;
	}

	prev->next = slow->next;

	return dummy.next;
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
		     int n, int *expected, int expected_len)
{
	struct ListNode *head, *result;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);
	printf("n=%d\n", n);

	result = removeNthFromEnd(head, n);
	show_list("Output", result);

	assert(check_list(result, expected, expected_len));
	printf("PASS\n\n");

	free_list(result);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 3, 5};
		run_test("LeetCode example 1", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 2: Single element, remove it */
	{
		int in[] = {1};
		run_test("Single element", in, ARRAYSIZE(in),
			 1, NULL, 0);
	}

	/* Test 3: Two elements, remove tail */
	{
		int in[] = {1, 2};
		int ex[] = {1};
		run_test("Two elements, remove tail", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 4: Two elements, remove head */
	{
		int in[] = {1, 2};
		int ex[] = {2};
		run_test("Two elements, remove head", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 5: Three elements, remove middle */
	{
		int in[] = {1, 2, 3};
		int ex[] = {1, 3};
		run_test("Three elements, remove middle", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 6: Four elements, remove head (n=len) */
	{
		int in[] = {1, 2, 3, 4};
		int ex[] = {2, 3, 4};
		run_test("Four elements, remove head", in, ARRAYSIZE(in),
			 4, ex, ARRAYSIZE(ex));
	}

	/* Test 7: Four elements, remove tail (n=1) */
	{
		int in[] = {1, 2, 3, 4};
		int ex[] = {1, 2, 3};
		run_test("Four elements, remove tail", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 8: Non-sequential values, remove 3rd from end */
	{
		int in[] = {10, 20, 30, 40, 50};
		int ex[] = {10, 20, 40, 50};
		run_test("Non-sequential values", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	/* Test 9: Six elements, remove head (n=len) */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {2, 3, 4, 5, 6};
		run_test("Six elements, remove head", in, ARRAYSIZE(in),
			 6, ex, ARRAYSIZE(ex));
	}

	/* Test 10: Six elements, remove 3rd from end */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {1, 2, 3, 5, 6};
		run_test("Six elements, remove 3rd from end", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}

