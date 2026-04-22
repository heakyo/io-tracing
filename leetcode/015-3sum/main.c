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

void show_array(char *type, int *a, int len)
{
	int i;

	printf("%s: [", type);
	for (i = 0; i < len - 1; i++)
		printf("%d ", a[i]);
	printf("%d]\n", a[i]);
}

void show_array_dint(char *type, int **a, int size, int *a_column_size)
{
	int i, j;

	if (size == 0) {
		printf("%s: []\n", type);
		return;
	}

	printf("%s: [", type);
	for (i = 0; i < size - 1; i++) {
		printf("[");
		for (j = 0; j < a_column_size[i] - 1; j++)
			printf("%d ", a[i][j]);
		printf("%d],", a[i][j]);
	}

	printf("[");
	for (j = 0; j < a_column_size[i] - 1; j++)
		printf("%d ", a[i][j]);
	printf("%d]]\n", a[i][j]);
}

int compare(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** threeSum(int* nums, int numsSize, int* returnSize, int** returnColumnSizes)
{
	int **output;
	int capacity;
	int left,right,k, val, found;

	qsort(nums, numsSize, sizeof(int), compare);
	//show_array("Test", nums, numsSize);

	capacity = 1000;
	output = (int **)malloc(sizeof(int *)*capacity);
	output[0] = NULL;
	*returnColumnSizes = (int *)malloc(sizeof(int)*capacity);

	if (nums[0] > 0 || nums[numsSize - 1] < 0) {

		*returnSize = 0;
		*returnColumnSizes = (int *)malloc(sizeof(int)*capacity);

		return NULL;
	}

	left = 0;
	right = numsSize - 1;
	*returnSize = 0;

	while (left < right) {
		found = 0;
		val = nums[left] + nums[right];
		if (val < 0) {
			k = right - 1;
			while (nums[k] > 0) {
				if (val+nums[k] == 0) {
					found = 1;
					break;
				}
				k--;
			}
		} else if (val > 0) {
			k = left + 1;
			while (nums[k] < 0) {
				if (val+nums[k] == 0) {
					found = 1;
					break;
				}
				k++;
			}
		} else { /* val == 0 */
			for (k = left + 1; k < right; k++) {
				if (nums[k] == 0) {
					found = 1;
					break;
				}
			}
		}

		if (found) {
			output[(*returnSize)] = (int *)malloc(sizeof(int)*3);
			output[(*returnSize)][0] = nums[left];
			output[(*returnSize)][1] = nums[k];
			output[(*returnSize)][2] = nums[right];
			(*returnColumnSizes)[(*returnSize)] = 3;
			(*returnSize) += 1;
		}

		if (val < 0) {
			if (val+nums[++k]>0) {
				k = right;
				while (left < right && nums[right] == nums[k]) right--;
			} else {
				k = left;
				while (left < right && nums[left] == nums[k]) left++;
			}
		} else if (val > 0) {
			if (val+nums[--k]<0) {
				k = left;
				while (left < right && nums[left] == nums[k]) left++;
			} else {
				k = right;
				while (left < right && nums[right] == nums[k]) right--;
			}
		} else { /* val == 0 */
			if (k < (right - left)/2) {
				k = right;
				while (left < right && nums[right] == nums[k]) right--;
			} else {
				k = left;
				while (left < right && nums[left] == nums[k]) left++;
			}
		}
	}

	return output;
}


static int compare_triplet(const void *a, const void *b)
{
	const int *ta = *(const int **)a;
	const int *tb = *(const int **)b;

	if (ta[0] != tb[0]) return ta[0] - tb[0];
	if (ta[1] != tb[1]) return ta[1] - tb[1];
	return ta[2] - tb[2];
}

static int compare_flat(const void *a, const void *b)
{
	const int *ta = (const int *)a;
	const int *tb = (const int *)b;

	if (ta[0] != tb[0]) return ta[0] - tb[0];
	if (ta[1] != tb[1]) return ta[1] - tb[1];
	return ta[2] - tb[2];
}

static void run_test(const char *name, int *nums, int numsSize,
		     int expected[][3], int expected_size)
{
	int **output, *returnColumnSizes, returnSize;
	int *copy;
	int i, pass;

	copy = (int *)malloc(numsSize * sizeof(int));
	assert(copy != NULL);
	memcpy(copy, nums, numsSize * sizeof(int));

	printf("=== %s ===\n", name);

	output = threeSum(copy, numsSize, &returnSize, &returnColumnSizes);

	printf("returnSize = %d (expected %d)\n", returnSize, expected_size);

	if (output != NULL && returnSize > 1)
		qsort(output, returnSize, sizeof(int *), compare_triplet);
	if (expected_size > 1)
		qsort(expected, expected_size, 3 * sizeof(int), compare_flat);

	pass = (returnSize == expected_size);
	for (i = 0; i < returnSize && pass; i++) {
		if (output[i][0] != expected[i][0] ||
		    output[i][1] != expected[i][1] ||
		    output[i][2] != expected[i][2])
			pass = 0;
	}

	if (!pass) {
		printf("FAIL\n");
		if (output != NULL && returnSize > 0)
			show_array_dint("  Got", output, returnSize,
					returnColumnSizes);
		printf("  Expected %d triplets:", expected_size);
		for (i = 0; i < expected_size; i++)
			printf(" [%d,%d,%d]", expected[i][0],
			       expected[i][1], expected[i][2]);
		printf("\n\n");
	} else {
		printf("PASS\n\n");
	}

	for (i = 0; i < returnSize; i++)
		free(output[i]);
	if (output)
		free(output);
	if (returnColumnSizes)
		free(returnColumnSizes);
	free(copy);
}

int main(int argc, char *argv[])
{
	int pass = 1;

	/* Test 1: LeetCode example */
	{
		int nums[] = {-1, 0, 1, 2, -1, -4};
		int exp[][3] = {{-1, -1, 2}, {-1, 0, 1}};
		run_test("LeetCode example", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	/* Test 2: No triplet possible */
	{
		int nums[] = {0, 1, 1};
		run_test("No triplet [0,1,1]", nums, ARRAYSIZE(nums),
			 NULL, 0);
	}

	/* Test 3: All zeros */
	{
		int nums[] = {0, 0, 0};
		int exp[][3] = {{0, 0, 0}};
		run_test("All zeros [0,0,0]", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	/* Test 4: Four zeros */
	{
		int nums[] = {0, 0, 0, 0};
		int exp[][3] = {{0, 0, 0}};
		run_test("Four zeros", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	/* Test 5: Two pairs */
	{
		int nums[] = {-2, 0, 1, 1, 2};
		int exp[][3] = {{-2, 0, 2}, {-2, 1, 1}};
		run_test("Two pairs [-2,0,1,1,2]", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	/* Test 6: Three triplets — algorithm tends to miss [-3,1,2] */
	{
		int nums[] = {-3, -1, 0, 1, 2, 4};
		int exp[][3] = {{-3, -1, 4}, {-3, 1, 2}, {-1, 0, 1}};
		run_test("Three triplets [-3,-1,0,1,2,4]", nums,
			 ARRAYSIZE(nums), exp, ARRAYSIZE(exp));
	}

	/* Test 7: Minimal triplet */
	{
		int nums[] = {-1, 0, 1};
		int exp[][3] = {{-1, 0, 1}};
		run_test("Minimal triplet [-1,0,1]", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	/* Test 8: All positive */
	{
		int nums[] = {1, 2, 3};
		run_test("All positive", nums, ARRAYSIZE(nums), NULL, 0);
	}

	/* Test 9: All negative */
	{
		int nums[] = {-1, -1, -1};
		run_test("All negative", nums, ARRAYSIZE(nums), NULL, 0);
	}

	/* Test 10: Large mixed — user's original test */
	{
		int nums[] = {2, -3, 0, -2, -5, -5, -4, 1, 2, -2,
			      2, 0, 2, -4, 5, 5, -10};
		int exp[][3] = {{-10, 5, 5}, {-5, 0, 5}, {-4, 2, 2},
				{-3, -2, 5}, {-3, 1, 2}, {-2, 0, 2}};
		run_test("Large mixed (user's test)", nums, ARRAYSIZE(nums),
			 exp, ARRAYSIZE(exp));
	}

	printf("Done.\n");
	return 0;
}

