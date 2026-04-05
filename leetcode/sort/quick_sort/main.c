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

int quick_sort(int *array, int low, int high)
{
	int pivot;
	int i, j;

	if (!array || low >= high)
		return 0;

	/* Use last element as pivot, two-pointer partition */
	pivot = array[high];
	i = low;
	j = high - 1;

	while (i <= j) {
		while (i <= j && array[i] <= pivot)
			i++;
		while (i <= j && array[j] > pivot)
			j--;
		if (i < j)
			swap(&array[i], &array[j]);
	}
	/* Place pivot in its correct position */
	swap(&array[i], &array[high]);

	quick_sort(array, low, i - 1);
	quick_sort(array, i + 1, high);

	return 0;
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

static int check_sorted(int *array, int len)
{
	int i;

	for (i = 0; i < len - 1; i++) {
		if (array[i] > array[i + 1])
			return 0;
	}
	return 1;
}

static void run_test(const char *name, int *src, int len)
{
	int *array = malloc(len * sizeof(int));

	assert(array != NULL);
	memcpy(array, src, len * sizeof(int));

	printf("=== %s ===\n", name);
	print_array("Input:  ", array, len);
	quick_sort(array, 0, len - 1);
	print_array("Output: ", array, len);

	assert(check_sorted(array, len));
	printf("PASS\n\n");

	free(array);
}

int main(int argc, char *argv[])
{
	/* Test 1: Original test case */
	int t1[] = {1, 7, 3, 8, 4};

	run_test("Normal case", t1, ARRAYSIZE(t1));

	/* Test 2: Already sorted */
	int t2[] = {1, 2, 3, 4, 5};

	run_test("Already sorted", t2, ARRAYSIZE(t2));

	/* Test 3: Reverse sorted */
	int t3[] = {5, 4, 3, 2, 1};

	run_test("Reverse sorted", t3, ARRAYSIZE(t3));

	/* Test 4: All same elements */
	int t4[] = {3, 3, 3, 3, 3};

	run_test("All same", t4, ARRAYSIZE(t4));

	/* Test 5: Two elements */
	int t5[] = {2, 1};

	run_test("Two elements", t5, ARRAYSIZE(t5));

	/* Test 6: Single element */
	int t6[] = {42};

	run_test("Single element", t6, ARRAYSIZE(t6));

	/* Test 7: With duplicates */
	int t7[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

	run_test("With duplicates", t7, ARRAYSIZE(t7));

	/* Test 8: Negative numbers */
	int t8[] = {-3, 5, -1, 0, 4, -2};

	run_test("Negative numbers", t8, ARRAYSIZE(t8));

	/* Test 9: Large range */
	int t9[] = {100, -100, 0, 50, -50, 99, -99};

	run_test("Large range", t9, ARRAYSIZE(t9));

	/* Test 10: Already sorted two elements */
	int t10[] = {1, 2};

	run_test("Sorted two elements", t10, ARRAYSIZE(t10));

	printf("All tests passed!\n");
	return 0;
}
