#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

int quick_sort(int *array, int low, int high)
{
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
