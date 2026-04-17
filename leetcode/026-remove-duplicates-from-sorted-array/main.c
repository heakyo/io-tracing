#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int removeDuplicates(int* nums, int numsSize)
{
	int i, k = 1;

	for (i = 1; i < numsSize; i++) {
		if (nums[i] != nums[k-1]) {
			swap(&nums[k], &nums[i]);
			k++;
		}
	}

	return k;
}

static void print_array(const char *label, int *array, int len)
{
	int i;

	printf("%s[", label);
	for (i = 0; i < len - 1; i++)
		printf("%d ", array[i]);
	if (len > 0)
		printf("%d", array[i]);
	printf("]\n");
}

static void run_test(const char *name, int *src, int len, int expected_k,
		     int *expected_nums)
{
	int *nums = malloc(len * sizeof(int));
	int k, i;

	assert(nums != NULL);
	memcpy(nums, src, len * sizeof(int));

	printf("=== %s ===\n", name);
	print_array("Input:    ", nums, len);

	k = removeDuplicates(nums, len);

	printf("k = %d (expected %d)\n", k, expected_k);
	print_array("Output:   ", nums, k);

	assert(k == expected_k);
	for (i = 0; i < k; i++)
		assert(nums[i] == expected_nums[i]);

	printf("PASS\n\n");
	free(nums);
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 */
	int t1[] = {1, 1, 2};
	int e1[] = {1, 2};
	run_test("LeetCode example 1", t1, ARRAYSIZE(t1), 2, e1);

	/* Test 2: LeetCode example 2 */
	int t2[] = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
	int e2[] = {0, 1, 2, 3, 4};
	run_test("LeetCode example 2", t2, ARRAYSIZE(t2), 5, e2);

	/* Test 3: Single element */
	int t3[] = {1};
	int e3[] = {1};
	run_test("Single element", t3, ARRAYSIZE(t3), 1, e3);

	/* Test 4: All same */
	int t4[] = {5, 5, 5, 5};
	int e4[] = {5};
	run_test("All same", t4, ARRAYSIZE(t4), 1, e4);

	/* Test 5: Already unique */
	int t5[] = {1, 2, 3, 4, 5};
	int e5[] = {1, 2, 3, 4, 5};
	run_test("Already unique", t5, ARRAYSIZE(t5), 5, e5);

	/* Test 6: Two elements same */
	int t6[] = {3, 3};
	int e6[] = {3};
	run_test("Two elements same", t6, ARRAYSIZE(t6), 1, e6);

	/* Test 7: Two elements different */
	int t7[] = {1, 2};
	int e7[] = {1, 2};
	run_test("Two elements different", t7, ARRAYSIZE(t7), 2, e7);

	/* Test 8: Negative numbers */
	int t8[] = {-3, -1, 0, 0, 2};
	int e8[] = {-3, -1, 0, 2};
	run_test("Negative numbers", t8, ARRAYSIZE(t8), 4, e8);

	/* Test 9: Large range */
	int t9[] = {-100, 0, 0, 0, 100};
	int e9[] = {-100, 0, 100};
	run_test("Large range", t9, ARRAYSIZE(t9), 3, e9);

	/* Test 10: Many duplicates */
	int t10[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	int e10[] = {1};
	run_test("Many duplicates", t10, ARRAYSIZE(t10), 1, e10);

	printf("All tests passed!\n");
	return 0;
}
