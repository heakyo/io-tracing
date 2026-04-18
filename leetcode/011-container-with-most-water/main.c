#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int maxArea(int* height, int heightSize)
{
	int max_area, area;
	int left, right;

	max_area = 0;

	left = 0;
	right = heightSize - 1;

	while (left < right) {
		area = MIN(height[left], height[right]) * (right - left);
		if ( area > max_area)
			max_area = area;

		(height[left] < height[right]) ? left++ : right--;
	}

	return max_area;
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

static void run_test(const char *name, int *height, int len, int expected)
{
	int ret;

	printf("=== %s ===\n", name);
	print_array("Input:    ", height, len);

	ret = maxArea(height, len);

	printf("Output:   %d\n", ret);
	printf("Expected: %d\n", expected);

	assert(ret == expected);
	printf("PASS\n\n");
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example */
	int t1[] = {1, 8, 6, 2, 5, 4, 8, 3, 7};
	run_test("LeetCode example", t1, ARRAYSIZE(t1), 49);

	/* Test 2: Two elements */
	int t2[] = {1, 1};
	run_test("Two elements", t2, ARRAYSIZE(t2), 1);

	/* Test 3: Single element */
	int t3[] = {5};
	run_test("Single element", t3, ARRAYSIZE(t3), 0);

	/* Test 4: Decreasing */
	int t4[] = {5, 4, 3, 2, 1};
	run_test("Decreasing", t4, ARRAYSIZE(t4), 6);

	/* Test 5: Increasing */
	int t5[] = {1, 2, 3, 4, 5};
	run_test("Increasing", t5, ARRAYSIZE(t5), 6);

	/* Test 6: All same */
	int t6[] = {3, 3, 3, 3};
	run_test("All same", t6, ARRAYSIZE(t6), 9);

	/* Test 7: Tall at edges */
	int t7[] = {10, 1, 1, 1, 10};
	run_test("Tall at edges", t7, ARRAYSIZE(t7), 40);

	/* Test 8: Peak in middle */
	int t8[] = {1, 2, 4, 3};
	run_test("Peak in middle", t8, ARRAYSIZE(t8), 4);

	/* Test 9: Symmetric ends */
	int t9[] = {4, 3, 2, 1, 4};
	run_test("Symmetric ends", t9, ARRAYSIZE(t9), 16);

	/* Test 10: Mixed heights */
	int t10[] = {2, 3, 10, 5, 7, 8, 9};
	run_test("Mixed heights", t10, ARRAYSIZE(t10), 36);

	printf("All tests passed!\n");
	return 0;
}
