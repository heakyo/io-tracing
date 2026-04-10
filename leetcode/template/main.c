#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

void merge(int* nums1, int nums1Size, int m, int* nums2, int nums2Size, int n)
{

}

void show_array(char *type, int *a, int len)
{
	printf("%s: [", type);
	for (int i = 0; i < len - 1; i++)
		printf("%d ", a[i]);
	printf("%d]\n", a[len - 1]);
}

int main(int argc, char *argv[])
{
	int nums1[6] = {1, 2, 3, 0, 0, 0};
	int nums2[3] = {2, 5, 6};
	int m = 3, n = 3;
	int i;

	/* Input */
	show_array("Input", nums1, ARRAYSIZE(nums1));
	show_array("Input", nums2, ARRAYSIZE(nums2));

	/* Algorithm */

	/* Output */
	show_array("Output", nums1, ARRAYSIZE(nums1));

	return 0;
}
