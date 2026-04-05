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
	int i;

	if (!array || low >= high)
		return 0;

	pivot = high;

	if (low < high) {
		for (i = low; i < pivot; i++) {
			if (array[i] > array[pivot]) {
				swap(&array[i], &array[pivot]);
				pivot = low + i;
				break;
			}
		}

		for (i = high; i > pivot; i--) {
			if (array[pivot] > array[i]) {
				swap(&array[pivot], &array[i]);
				pivot = high - i;
				break;
			}
		}
	}

	quick_sort(array, 0, pivot - 1);
	quick_sort(array, pivot + 1, high);

	return 0;
}

int main(int argc, char *argv[])
{
	int array[5] = {1, 7, 3, 8, 4};
	int array_len = ARRAYSIZE(array);
	int i;

	/* Input */
	printf("Input: [");
	for (i = 0; i < array_len - 1; i++)
		printf("%d ", array[i]);
	printf("%d]\n", array[i]);

	/* Algorithm */
	quick_sort(array, 0, array_len - 1);

	/* Output */
	printf("Output: [");
	for (i = 0; i < array_len - 1; i++)
		printf("%d ", array[i]);
	printf("%d]\n", array[i]);

	return 0;
}
