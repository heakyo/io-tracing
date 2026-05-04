#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

struct ListNode {
	int val;
	struct ListNode *next;
};

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* reverseKGroup(struct ListNode* head, int k)
{
	struct ListNode *cur, *pre, *tmp, *dummy;
	int size, i;

	dummy = (struct ListNode *)malloc(sizeof(*dummy));
	dummy->next = head;
	pre = dummy;

	size = 0;
	cur = head;
	while (cur) {
		size++;
		cur = cur->next;
	}

	cur = head;
	while (size >= k) {
		for (i = 0; i < k - 1; i++) {
			tmp = pre->next;
			pre->next = cur->next;
			cur->next = cur->next->next;
			pre->next->next = tmp;
		}

		size -= k;
		pre = cur;
		cur = cur->next;
	}

	return dummy->next;
}

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

	result = reverseKGroup(head, k);
	show_list("Output", result);

	assert(check_list(result, expected, expected_len));
	printf("PASS\n\n");

	free_list(result);
}

int main(int argc, char *argv[])
{
	/* MyTest 1: LeetCode my example 1 — k=3 */
	{
		int in[] = {1, 2, 3};
		int ex[] = {3, 2, 1};
		run_test("LeetCode example 1 (k=3)", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	/* MyTest 2: LeetCode my example 2 — k=3 */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {3, 2, 1, 6, 5, 4};
		run_test("LeetCode example 1 (k=3)", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	/* Test 1: LeetCode example 1 — k=2 */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {2, 1, 4, 3, 5};
		run_test("LeetCode example 1 (k=2)", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 2: LeetCode example 2 — k=3 */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {3, 2, 1, 4, 5};
		run_test("LeetCode example 2 (k=3)", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	/* Test 3: k=1, no change */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {1, 2, 3, 4, 5};
		run_test("k=1, no change", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 4: k=n, reverse entire list */
	{
		int in[] = {1, 2, 3, 4, 5};
		int ex[] = {5, 4, 3, 2, 1};
		run_test("k=n, reverse entire list", in, ARRAYSIZE(in),
			 5, ex, ARRAYSIZE(ex));
	}

	/* Test 5: Single element */
	{
		int in[] = {1};
		int ex[] = {1};
		run_test("Single element", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 6: Two elements, k=2 */
	{
		int in[] = {1, 2};
		int ex[] = {2, 1};
		run_test("Two elements, k=2", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 7: Three elements, k=2 — last node left over */
	{
		int in[] = {1, 2, 3};
		int ex[] = {2, 1, 3};
		run_test("k=2, remainder 1", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 8: Four elements, k=2 — exact multiple */
	{
		int in[] = {1, 2, 3, 4};
		int ex[] = {2, 1, 4, 3};
		run_test("k=2, exact multiple", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 9: Six elements, k=3 — two full groups */
	{
		int in[] = {1, 2, 3, 4, 5, 6};
		int ex[] = {3, 2, 1, 6, 5, 4};
		run_test("k=3, two full groups", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	/* Test 10: Eight elements, k=3 — two groups + remainder */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8};
		int ex[] = {3, 2, 1, 6, 5, 4, 7, 8};
		run_test("k=3, two groups + remainder", in, ARRAYSIZE(in),
			 3, ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}
