#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "../mylib/hashtable/include/hashtable.h"

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))


static void validate_two_sum_result(const int *nums, int nums_size, int target, const int *ret, int return_size);
static void test_two_sum(void);

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum_brute_force(int* nums, int numsSize, int target, int* returnSize) {

	int *returned;
	int i, j;

	for (i = 0; i < numsSize; i++) {
		for (j = i + 1; j < numsSize; j++) {
			if (nums[i] + nums[j] == target) {

				returned = malloc(sizeof(*returned) * 2);
				*returnSize = 2;

				returned[0] = i;
				returned[1] = j;

				return returned;
			}
		}
	}

	*returnSize = 0;
	return NULL;
}

/* Hash Table */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {

	HashTable *ht;
	long long complement;
	int i, value, *returned;

	ht = hash_create(numsSize);
	if (!ht)
		goto hash_create_failed;

	for (i = 0; i < numsSize; i++) {
		complement = (long long)target - nums[i];

		if (complement >= INT_MIN && complement <= INT_MAX) {

			if (hash_find(ht, (int)complement, &value)) {
				returned = malloc(sizeof(*returned) * 2);
				if (!returned)
					goto failed;

				*returnSize = 2;

				returned[0] = i;
				returned[1] = value;

				hash_destroy(ht);
				return returned;
			}
		}

		if (hash_insert(ht, nums[i], i))
			goto failed;
	}

failed:
	hash_destroy(ht);

hash_create_failed:
	*returnSize = 0;
	return NULL;
}

int main(int argc, char *argv[])
{
	test_two_sum();

	return 0;
}

static void
validate_two_sum_result(const int *nums, int nums_size, int target,
    const int *ret, int return_size)
{
        int index1, index2;

        /*
         * A valid result must contain exactly two indexes.
         */
        assert(ret != NULL);
        assert(return_size == 2);

        index1 = ret[0];
        index2 = ret[1];

        /*
         * Both indexes must be within the array bounds.
         */
        assert(index1 >= 0);
        assert(index1 < nums_size);
        assert(index2 >= 0);
        assert(index2 < nums_size);

        /*
         * The same array element cannot be used twice.
         */
        assert(index1 != index2);

        /*
         * The values at the returned indexes must add up
         * to the requested target.
         */
        assert(nums[index1] + nums[index2] == target);
}

static void
test_two_sum(void)
{
        int *ret;
        int return_size;

        printf("Running test_two_sum...\n");

        /*
         * Test 1: Basic case.
         */
        {
                int nums[] = { 2, 7, 11, 15 };

                return_size = 0;
                ret = twoSum(nums, 4, 9, &return_size);

                validate_two_sum_result(nums, 4, 9, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 2: The matching elements are not at the
         * beginning of the array.
         */
        {
                int nums[] = { 3, 2, 4 };

                return_size = 0;
                ret = twoSum(nums, 3, 6, &return_size);

                validate_two_sum_result(nums, 3, 6, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 3: Two equal values at different indexes.
         *
         * The implementation must not use the same array
         * element twice.
         */
        {
                int nums[] = { 3, 3 };

                return_size = 0;
                ret = twoSum(nums, 2, 6, &return_size);

                validate_two_sum_result(nums, 2, 6, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 4: Negative numbers.
         */
        {
                int nums[] = { -3, 4, 3, 90 };

                return_size = 0;
                ret = twoSum(nums, 4, 0, &return_size);

                validate_two_sum_result(nums, 4, 0, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 5: The matching elements are near the end
         * of the array.
         */
        {
                int nums[] = { 1, 2, 3, 8, 9 };

                return_size = 0;
                ret = twoSum(nums, 5, 17, &return_size);

                validate_two_sum_result(nums, 5, 17, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 6: Multiple valid answers exist.
         *
         * Valid answers include:
         *
         *     nums[0] + nums[3] = 1 + 4 = 5
         *     nums[1] + nums[2] = 2 + 3 = 5
         *
         * The implementation may return either pair in
         * either order.
         */
        {
                int nums[] = { 1, 2, 3, 4 };

                return_size = 0;
                ret = twoSum(nums, 4, 5, &return_size);

                validate_two_sum_result(nums, 4, 5, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 7: Negative target.
         */
        {
                int nums[] = { -1, -2, -3, -4, -5 };

                return_size = 0;
                ret = twoSum(nums, 5, -8, &return_size);

                validate_two_sum_result(nums, 5, -8, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 8: Zero values.
         */
        {
                int nums[] = { 0, 4, 3, 0 };

                return_size = 0;
                ret = twoSum(nums, 4, 0, &return_size);

                validate_two_sum_result(nums, 4, 0, ret,
                    return_size);

                free(ret);
        }

        /*
         * Test 9: No solution.
         */
        {
                int nums[] = { 1, 2, 3, 4 };

                return_size = -1;
                ret = twoSum(nums, 4, 100, &return_size);

                assert(ret == NULL);
                assert(return_size == 0);
        }

        /*
         * Test 10: A single element cannot be used twice.
         */
        {
                int nums[] = { 5 };

                return_size = -1;
                ret = twoSum(nums, 1, 10, &return_size);

                assert(ret == NULL);
                assert(return_size == 0);
        }

        printf("test_two_sum PASSED\n");
}
