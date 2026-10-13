#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "../mylib/hashtable/include/hashtable.h"

/* brute force */
bool containsDuplicate_bruteForce(int* nums, int numsSize)
{
	int i, j;

	for (i = 0; i < numsSize; i++) {
		for (j = i + 1; j < numsSize; j++) {
			if (nums[j] == nums[i])
				return true;
		}
	}

	return false;
}

/* Hash Table */
bool containsDuplicate(int* nums, int numsSize)
{
	HashTable *ht;
	int i;

	ht = hash_create(numsSize);
	if (!ht)
		return false;


	for (i = 0; i < numsSize; i++) {
		if (hash_find(ht, nums[i], NULL)) {
			hash_destroy(ht);
			return true;
		}

		if (hash_insert(ht, nums[i], 1)) {
			hash_destroy(ht);
			return false;
		}
	}

	hash_destroy(ht);
	return false;
}

static void
test_contains_duplicate(void)
{
        bool ret;

        printf("Running test_contains_duplicate...\n");

        /*
         * Test 1: Basic case with a duplicate.
         */
        {
                int nums[] = { 1, 2, 3, 1 };

                ret = containsDuplicate(nums, 4);

                assert(ret == true);
        }

        /*
         * Test 2: No duplicate elements.
         */
        {
                int nums[] = { 1, 2, 3, 4 };

                ret = containsDuplicate(nums, 4);

                assert(ret == false);
        }

        /*
         * Test 3: Multiple duplicate elements.
         */
        {
                int nums[] = { 1, 1, 1, 3, 3, 4, 3, 2, 4, 2 };

                ret = containsDuplicate(nums, 10);

                assert(ret == true);
        }

        /*
         * Test 4: A single element cannot be a duplicate.
         */
        {
                int nums[] = { 5 };

                ret = containsDuplicate(nums, 1);

                assert(ret == false);
        }

        /*
         * Test 5: Two identical elements.
         */
        {
                int nums[] = { 7, 7 };

                ret = containsDuplicate(nums, 2);

                assert(ret == true);
        }

        /*
         * Test 6: Negative numbers with a duplicate.
         */
        {
                int nums[] = { -1, -2, -3, -1 };

                ret = containsDuplicate(nums, 4);

                assert(ret == true);
        }

        /*
         * Test 7: Negative and positive numbers without
         * duplicates.
         */
        {
                int nums[] = { -3, -2, -1, 0, 1, 2, 3 };

                ret = containsDuplicate(nums, 7);

                assert(ret == false);
        }

        /*
         * Test 8: Zero appears more than once.
         */
        {
                int nums[] = { 0, 1, 2, 0 };

                ret = containsDuplicate(nums, 4);

                assert(ret == true);
        }

        /*
         * Test 9: INT_MIN appears twice.
         *
         * This tests whether the implementation handles
         * the smallest signed integer correctly.
         */
        {
                int nums[] = { INT_MIN, 1, 2, INT_MIN };

                ret = containsDuplicate(nums, 4);

                assert(ret == true);
        }

        /*
         * Test 10: INT_MAX appears twice.
         */
        {
                int nums[] = { INT_MAX, 0, -1, INT_MAX };

                ret = containsDuplicate(nums, 4);

                assert(ret == true);
        }

        /*
         * Test 11: INT_MIN and INT_MAX are distinct values.
         */
        {
                int nums[] = { INT_MIN, INT_MAX };

                ret = containsDuplicate(nums, 2);

                assert(ret == false);
        }

        /*
         * Test 12: Duplicate elements at the end.
         */
        {
                int nums[] = { 10, 20, 30, 40, 50, 10 };

                ret = containsDuplicate(nums, 6);

                assert(ret == true);
        }

        /*
         * Test 13: Duplicate elements next to each other.
         */
        {
                int nums[] = { 1, 2, 2, 3, 4 };

                ret = containsDuplicate(nums, 5);

                assert(ret == true);
        }

        /*
         * Test 14: An empty array has no duplicates.
         *
         * This is an additional robustness test outside
         * the original LeetCode input constraints.
         */
        {
                ret = containsDuplicate(NULL, 0);

                assert(ret == false);
        }

        /*
         * Test 15: Many distinct values.
         */
        {
                int nums[1000];
                int i;

                for (i = 0; i < 1000; i++)
                        nums[i] = i;

                ret = containsDuplicate(nums, 1000);

                assert(ret == false);
        }

        /*
         * Test 16: A duplicate near the end of a large array.
         */
        {
                int nums[1000];
                int i;

                for (i = 0; i < 1000; i++)
                        nums[i] = i;

                nums[999] = 500;

                ret = containsDuplicate(nums, 1000);

                assert(ret == true);
        }

        printf("test_contains_duplicate PASSED\n");
}

int
main(void)
{
        test_contains_duplicate();

        return (0);
}
