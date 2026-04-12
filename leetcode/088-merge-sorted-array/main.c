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

void merge(int* nums1, int nums1Size, int m, int* nums2, int nums2Size, int n)
{
	int *p1, *p2, *p;

	p1 = nums1 + m - 1;
	p2 = nums2 + n - 1;
	p = nums1 + m + n - 1;

	while (nums1 <= p1 && nums2 <= p2) {
		if (*p2 > *p1) {
			*p-- = *p2--;
		} else {
			*p-- = *p1--;
		}
	}

	while (nums2 <= p2)
		*p-- = *p2--;
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
	int nums1[6] = {4,0,0,0,0,0};
	int nums2[5] = {1,2,3,5,6};
	int m = 1, n = 5;

	/* Input */
	show_array("Input", nums1, ARRAYSIZE(nums1));
	show_array("Input", nums2, ARRAYSIZE(nums2));

	/* Algorithm */
	merge(nums1, ARRAYSIZE(nums1), m, nums2, ARRAYSIZE(nums2), n);

	/* Output */
	show_array("Output", nums1, ARRAYSIZE(nums1));

	return 0;
}
