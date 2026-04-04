#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {

	int *returned;
	int k;

	*returnSize = 2;
	returned = (int *)malloc((*returnSize) * sizeof(*returned));
	assert(returned);
	memset(returned, 0x0, (*returnSize) * sizeof(*returned));

	for (int i = 0; i < numsSize - 1; i++) {
		k = i;

		for (int j = k + 1; j < numsSize; j++)

			if (nums[k] + nums[j] == target) {
				returned[0] = k;
				returned[1] = j;

				return returned;
			}
	}

	return returned;
}

int main(int argc, char *argv[])
{
	int nums[3] = {3, 2, 3};
	int numsSize = ARRAYSIZE(nums);
	int target = 6, returnSize;
	int *returned, i;

	printf("input: [");
	for (i = 0; i < numsSize - 1; i++)
		printf("%d ", nums[i]);
	printf("%d] ", nums[i]);
	printf("numsSize: %d\n", numsSize);

	returned = twoSum(nums, numsSize, target, &returnSize);

	printf("output: [");
	for (i = 0; i < returnSize - 1; i++)
		printf("%d ", returned[i]);
	printf("%d]\n", returned[i]);

	return 0;
}

