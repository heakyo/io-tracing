#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

/*
 * Optimized Merge Sorted Array — O(m+n) time, O(1) space.
 *
 * Same backward-merge algorithm as the original, but uses index-based
 * access instead of pointer arithmetic for clarity.  The unused swap()
 * function is removed.
 */
void merge(int *nums1, int nums1Size, int m, int *nums2, int nums2Size, int n)
{
	int i = m - 1;
	int j = n - 1;
	int k = m + n - 1;

	while (i >= 0 && j >= 0) {
		if (nums2[j] > nums1[i])
			nums1[k--] = nums2[j--];
		else
			nums1[k--] = nums1[i--];
	}

	while (j >= 0)
		nums1[k--] = nums2[j--];
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

static void run_test(const char *name, int *nums1_src, int m,
		     int *nums2_src, int n, int *expected)
{
	int total = m + n;
	int *nums1 = malloc(total * sizeof(int));
	int *nums2 = malloc((n > 0 ? n : 1) * sizeof(int));
	int i;

	assert(nums1 != NULL && nums2 != NULL);

	memcpy(nums1, nums1_src, m * sizeof(int));
	memset(nums1 + m, 0, n * sizeof(int));
	if (n > 0)
		memcpy(nums2, nums2_src, n * sizeof(int));

	printf("=== %s ===\n", name);
	print_array("nums1:    ", nums1, total);
	if (n > 0)
		print_array("nums2:    ", nums2, n);
	else
		printf("nums2:    []\n");

	merge(nums1, total, m, nums2, n, n);

	print_array("Output:   ", nums1, total);
	print_array("Expected: ", expected, total);

	for (i = 0; i < total; i++)
		assert(nums1[i] == expected[i]);
	printf("PASS\n\n");

	free(nums1);
	free(nums2);
}

int main(int argc, char *argv[])
{
	/* Test 1: Original test case */
	int n1_1[] = {4};
	int n2_1[] = {1, 2, 3, 5, 6};
	int ex_1[] = {1, 2, 3, 4, 5, 6};

	run_test("Original case", n1_1, 1, n2_1, 5, ex_1);

	/* Test 2: LeetCode example 1 */
	int n1_2[] = {1, 2, 3};
	int n2_2[] = {2, 5, 6};
	int ex_2[] = {1, 2, 2, 3, 5, 6};

	run_test("LeetCode example 1", n1_2, 3, n2_2, 3, ex_2);

	/* Test 3: nums2 empty (n=0) */
	int n1_3[] = {1};
	int ex_3[] = {1};

	run_test("nums2 empty", n1_3, 1, NULL, 0, ex_3);

	/* Test 4: nums1 empty (m=0) */
	int n2_4[] = {1};
	int ex_4[] = {1};

	run_test("nums1 empty", ex_4, 0, n2_4, 1, ex_4);

	/* Test 5: nums1 all smaller */
	int n1_5[] = {1, 2};
	int n2_5[] = {3, 4};
	int ex_5[] = {1, 2, 3, 4};

	run_test("nums1 all smaller", n1_5, 2, n2_5, 2, ex_5);

	/* Test 6: nums2 all smaller */
	int n1_6[] = {3, 4};
	int n2_6[] = {1, 2};
	int ex_6[] = {1, 2, 3, 4};

	run_test("nums2 all smaller", n1_6, 2, n2_6, 2, ex_6);

	/* Test 7: Single element each */
	int n1_7[] = {2};
	int n2_7[] = {1};
	int ex_7[] = {1, 2};

	run_test("Single element each", n1_7, 1, n2_7, 1, ex_7);

	/* Test 8: Duplicates */
	int n1_8[] = {1, 3, 3};
	int n2_8[] = {1, 2, 3};
	int ex_8[] = {1, 1, 2, 3, 3, 3};

	run_test("Duplicates", n1_8, 3, n2_8, 3, ex_8);

	/* Test 9: Negative numbers */
	int n1_9[] = {-3, -1, 0};
	int n2_9[] = {-2, 1, 2};
	int ex_9[] = {-3, -2, -1, 0, 1, 2};

	run_test("Negative numbers", n1_9, 3, n2_9, 3, ex_9);

	/* Test 10: Large range */
	int n1_10[] = {-100, 100};
	int n2_10[] = {-50, 0, 50};
	int ex_10[] = {-100, -50, 0, 50, 100};

	run_test("Large range", n1_10, 2, n2_10, 3, ex_10);

	printf("All tests passed!\n");
	return 0;
}
