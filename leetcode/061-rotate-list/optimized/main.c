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
struct ListNode* rotateRight(struct ListNode* head, int k)
{
	struct ListNode *tail, *new_tail;
	int size, n, i;

	if (head == NULL || head->next == NULL || k == 0)
		return head;

	/* count nodes and find the tail */
	size = 1;
	tail = head;
	while (tail->next) {
		size++;
		tail = tail->next;
	}

	n = k % size;
	if (n == 0)
		return head;

	/* form a cycle: tail connects back to head */
	tail->next = head;

	/* walk (size - n - 1) steps from head to find new tail */
	new_tail = head;
	for (i = 0; i < size - n - 1; i++)
		new_tail = new_tail->next;

	/* break the cycle */
	head = new_tail->next;
	new_tail->next = NULL;

	return head;
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
		     int k, int *expected, int expected_len)
{
	struct ListNode *head, *result;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);
	printf("k=%d\n", k);

	result = rotateRight(head, k);
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
		int ex[] = {4, 5, 1, 2, 3};
		run_test("LeetCode example 1", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 2: LeetCode example 2 — k > len */
	{
		int in[] = {0, 1, 2};
		int ex[] = {2, 0, 1};
		run_test("LeetCode example 2 (k>len)", in, ARRAYSIZE(in),
			 4, ex, ARRAYSIZE(ex));
	}

	/* Test 3: Single element, k=0 */
	{
		int in[] = {1};
		int ex[] = {1};
		run_test("Single element, k=0", in, ARRAYSIZE(in),
			 0, ex, ARRAYSIZE(ex));
	}

	/* Test 4: Single element, k=1 */
	{
		int in[] = {1};
		int ex[] = {1};
		run_test("Single element, k=1", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 5: Two elements, rotate by 1 */
	{
		int in[] = {1, 2};
		int ex[] = {2, 1};
		run_test("Two elements, k=1", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 6: Two elements, k=len (no change) */
	{
		int in[] = {1, 2};
		int ex[] = {1, 2};
		run_test("Two elements, k=len", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 7: k=len, full rotation (no change) */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 3, 4, 5};
		run_test("k=len, no change", in, ARRAYSIZE(in),
			 5, ex, ARRAYSIZE(ex));
	}

	/* Test 8: k > len, wraps around */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {4, 5, 1, 2, 3};
		run_test("k>len, wraps around", in, ARRAYSIZE(in),
			 7, ex, ARRAYSIZE(ex));
	}

	/* Test 9: Three elements, rotate by 1 */
	{
		int in[] = {1, 2, 3};
		int ex[] = {3, 1, 2};
		run_test("Three elements, k=1", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 10: Six elements, rotate by 4 */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {3, 4, 5, 6, 1, 2};
		run_test("Six elements, k=4", in, ARRAYSIZE(in),
			 4, ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}
