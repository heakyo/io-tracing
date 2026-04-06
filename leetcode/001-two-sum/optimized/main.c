#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "hashtable.h"

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

/*
 * Hash table approach - O(n) time, O(n) space.
 *
 * Uses libhashtable (mylib/hashtable) for the hash table.
 * Since the library uses string keys, we convert integer keys
 * to strings via snprintf. Values are stored by casting the
 * integer index to (void *)(intptr_t) to avoid heap allocation.
 */

static void int_to_key(int num, char *buf, size_t bufsize)
{
	snprintf(buf, bufsize, "%d", num);
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int *twoSum(int *nums, int numsSize, int target, int *returnSize)
{
	int *returned;
	hashtable_t *ht;
	int i, complement;
	char key_buf[32];
	void *val;

	*returnSize = 2;
	returned = (int *)malloc((*returnSize) * sizeof(*returned));
	assert(returned);
	memset(returned, 0x0, (*returnSize) * sizeof(*returned));

	ht = ht_create(0);
	assert(ht);

	for (i = 0; i < numsSize; i++) {
		complement = target - nums[i];
		int_to_key(complement, key_buf, sizeof(key_buf));

		val = ht_get(ht, key_buf);
		if (val) {
			returned[0] = (int)(intptr_t)val - 1;
			returned[1] = i;
			ht_destroy(ht);
			return returned;
		}

		int_to_key(nums[i], key_buf, sizeof(key_buf));
		/* Store index + 1 so that index 0 is distinguishable from NULL */
		ht_set(ht, key_buf, (void *)(intptr_t)(i + 1));
	}

	ht_destroy(ht);
	return returned;
}

static void print_array(const char *label, int *array, int len)
{
	int i;

	printf("%s[", label);
	for (i = 0; i < len - 1; i++)
		printf("%d ", array[i]);
	if (len > 0)
		printf("%d", array[i]);
	printf("]", label);
}

static void run_test(const char *name, int *nums, int numsSize,
		     int target, int exp0, int exp1)
{
	int returnSize;
	int *result;

	printf("=== %s ===\n", name);
	print_array("Input:  ", nums, numsSize);
	printf(" target=%d\n", target);

	result = twoSum(nums, numsSize, target, &returnSize);

	printf("Output: [%d %d]\n", result[0], result[1]);
	printf("Expect: [%d %d]\n", exp0, exp1);

	assert(returnSize == 2);
	assert(nums[result[0]] + nums[result[1]] == target);
	assert(result[0] == exp0 && result[1] == exp1);
	printf("PASS\n\n");

	free(result);
}

int main(int argc, char *argv[])
{
	/* Test 1: Original test case */
	int t1[] = {3, 2, 3};

	run_test("Original case", t1, ARRAYSIZE(t1), 6, 0, 2);

	/* Test 2: LeetCode example 1 */
	int t2[] = {2, 7, 11, 15};

	run_test("LeetCode example 1", t2, ARRAYSIZE(t2), 9, 0, 1);

	/* Test 3: Two elements */
	int t3[] = {1, 2};

	run_test("Two elements", t3, ARRAYSIZE(t3), 3, 0, 1);

	/* Test 4: Duplicate values */
	int t4[] = {3, 3};

	run_test("Duplicate values", t4, ARRAYSIZE(t4), 6, 0, 1);

	/* Test 5: Negative numbers */
	int t5[] = {-1, -2, -3, -4, -5};

	run_test("Negative numbers", t5, ARRAYSIZE(t5), -8, 2, 4);

	/* Test 6: Mixed positive and negative */
	int t6[] = {-3, 4, 3, 90};

	run_test("Mixed pos/neg", t6, ARRAYSIZE(t6), 0, 0, 2);

	/* Test 7: Zero target with zeros */
	int t7[] = {0, 4, 3, 0};

	run_test("Zero target", t7, ARRAYSIZE(t7), 0, 0, 3);

	/* Test 8: Answer at the end */
	int t8[] = {1, 2, 3, 4, 5};

	run_test("Answer at end", t8, ARRAYSIZE(t8), 9, 3, 4);

	/* Test 9: Large numbers */
	int t9[] = {1000000, 500000, -1000000, 500000};

	run_test("Large numbers", t9, ARRAYSIZE(t9), 0, 0, 2);

	/* Test 10: LeetCode example 2 */
	int t10[] = {3, 2, 4};

	run_test("LeetCode example 2", t10, ARRAYSIZE(t10), 6, 1, 2);

	printf("All tests passed!\n");
	return 0;
}
