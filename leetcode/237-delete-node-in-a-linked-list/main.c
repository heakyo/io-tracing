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
void deleteNode(struct ListNode* node)
{
	node->val = node->next->val;
	node->next = node->next->next;
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

static struct ListNode *find_node(struct ListNode *head, int val)
{
	while (head) {
		if (head->val == val)
			return head;
		head = head->next;
	}
	return NULL;
}

static void run_test(const char *name, int *nums, int len,
		     int del_val, int *expected, int expected_len)
{
	struct ListNode *head, *target;

	printf("=== %s ===\n", name);
	head = insert_list(nums, len);
	show_list("Input ", head);
	printf("delete node with val=%d\n", del_val);

	target = find_node(head, del_val);
	assert(target != NULL);
	deleteNode(target);
	show_list("Output", head);

	assert(check_list(head, expected, expected_len));
	printf("PASS\n\n");

	free_list(head);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 — delete 5 */
	{
		int in[] = {4, 5, 1, 9};
		int ex[] = {4, 1, 9};
		run_test("LeetCode example 1", in, ARRAYSIZE(in),
			 5, ex, ARRAYSIZE(ex));
	}

	/* Test 2: LeetCode example 2 — delete 1 */
	{
		int in[] = {4, 5, 1, 9};
		int ex[] = {4, 5, 9};
		run_test("LeetCode example 2", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 3: Delete head node */
	{
		int in[] = {4, 5, 1, 9};
		int ex[] = {5, 1, 9};
		run_test("Delete head node", in, ARRAYSIZE(in),
			 4, ex, ARRAYSIZE(ex));
	}

	/* Test 4: Two nodes, delete first */
	{
		int in[] = {1, 2};
		int ex[] = {2};
		run_test("Two nodes, delete first", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 5: Three nodes, delete middle */
	{
		int in[] = {1, 2, 3};
		int ex[] = {1, 3};
		run_test("Three nodes, delete middle", in, ARRAYSIZE(in),
			 2, ex, ARRAYSIZE(ex));
	}

	/* Test 6: Three nodes, delete head */
	{
		int in[] = {1, 2, 3};
		int ex[] = {2, 3};
		run_test("Three nodes, delete head", in, ARRAYSIZE(in),
			 1, ex, ARRAYSIZE(ex));
	}

	/* Test 7: Five nodes, delete head */
	{
		int in[] = {10, 20, 30, 40, 50};
		int ex[] = {20, 30, 40, 50};
		run_test("Five nodes, delete head", in, ARRAYSIZE(in),
			 10, ex, ARRAYSIZE(ex));
	}

	/* Test 8: Five nodes, delete middle */
	{
		int in[] = {10, 20, 30, 40, 50};
		int ex[] = {10, 20, 40, 50};
		run_test("Five nodes, delete middle", in, ARRAYSIZE(in),
			 30, ex, ARRAYSIZE(ex));
	}

	/* Test 9: Five nodes, delete second-to-last */
	{
		int in[] = {10, 20, 30, 40, 50};
		int ex[] = {10, 20, 30, 50};
		run_test("Five nodes, delete second-to-last", in, ARRAYSIZE(in),
			 40, ex, ARRAYSIZE(ex));
	}

	/* Test 10: Long list, delete middle */
	{
		int in[] = {1, 2, 3, 4, 5, 6, 7, 8};
		int ex[] = {1, 2, 3, 5, 6, 7, 8};
		run_test("Long list, delete middle", in, ARRAYSIZE(in),
			 4, ex, ARRAYSIZE(ex));
	}

	printf("All tests passed!\n");
	return 0;
}
