#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))
#define INSERTION_CUTOFF 10

/*
 * Optimized Quick Sort
 *
 * Improvements over the original:
 * 1. Fix: left recursion uses "low" instead of "0"
 * 2. Median-of-three pivot selection to avoid worst-case on sorted input
 * 3. Insertion sort for small subarrays (fewer than INSERTION_CUTOFF)
 * 4. Tail call elimination via while loop — guarantees O(log n) stack depth
 */

static void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

static void insertion_sort(int *array, int low, int high)
{
	int i, j, key;

	for (i = low + 1; i <= high; i++) {
		key = array[i];
		j = i - 1;
		while (j >= low && array[j] > key) {
			array[j + 1] = array[j];
			j--;
		}
		array[j + 1] = key;
	}
}

static int median_of_three(int *array, int low, int high)
{
	int mid = low + (high - low) / 2;

	if (array[low] > array[mid])
		swap(&array[low], &array[mid]);
	if (array[low] > array[high])
		swap(&array[low], &array[high]);
	if (array[mid] > array[high])
		swap(&array[mid], &array[high]);

	/* Move median to high-1 as sentinel; low and high are already placed */
	swap(&array[mid], &array[high - 1]);
	return array[high - 1];
}

static int partition(int *array, int low, int high)
{
	int pivot_val = median_of_three(array, low, high);
	int i = low;
	int j = high - 1;

	for (;;) {
		while (array[++i] < pivot_val)
			;
		while (array[--j] > pivot_val)
			;
		if (i >= j)
			break;
		swap(&array[i], &array[j]);
	}

	/* Restore pivot to its final position */
	swap(&array[i], &array[high - 1]);
	return i;
}

void quick_sort(int *array, int low, int high)
{
	int p;

	if (!array)
		return;

	while (low < high) {
		/* Use insertion sort for small subarrays */
		if (high - low + 1 <= INSERTION_CUTOFF) {
			insertion_sort(array, low, high);
			return;
		}

		p = partition(array, low, high);

		/* Recurse on smaller half, iterate on larger (tail call elimination) */
		if (p - low < high - p) {
			quick_sort(array, low, p - 1);
			low = p + 1;
		} else {
			quick_sort(array, p + 1, high);
			high = p - 1;
		}
	}
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
