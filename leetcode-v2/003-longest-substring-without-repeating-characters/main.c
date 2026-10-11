#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* brute force */
int lengthOfLongestSubstring_brute_force(char* s)
{
	int max;
	char *anchor, *ap;
	bool record[256];
	unsigned char idx;

	anchor = s;
	max =  0;

	while (*anchor) {

		memset(record, false, sizeof(record));

		for (ap = anchor; *ap; ap++) {
			idx = *ap;
			if (record[idx])
				break;

			record[idx] = true;
		}

		if (((ap - anchor) > max))
			max = ap - anchor;

		anchor++;
	}

	return max;
}

#define INVAL -1

/* sliding window V1 */
int lengthOfLongestSubstring_V1(char* s)
{
	int max;
	char *left, *right;
	int record[256];
	unsigned char idx;

	left = s;
	right = left;
	max =  0;

	memset(record, INVAL, sizeof(record));

	while (*right && *left) {

		for (; *right; right++) {
			idx = *right;
			if (record[idx] != INVAL && (record[idx] >= (left - s)))
				break;

			record[idx] = right - s;
		}

		if ((right - left) > max)
			max = right - left;

		left += (record[idx] - (left - s)) + 1;
	}

	return max;
}

/* sliding window V2*/
int lengthOfLongestSubstringV2(char* s)
{
	char *left, *right;
	int max, record[256];
	unsigned char idx;

	left = right = s;
	max =  0;

	memset(record, INVAL, sizeof(record));

	while (*right) {

		idx = *right;
		if (record[idx] != INVAL && record[idx] >= (left - s)) {

			if (right - left > max)
				max = right - left;

			left = s + record[idx] + 1;
		}

		record[idx] = right - s;

		right++;
	}

	if (right - left > max)
		max = right - left;

	return max;
}

/* sliding window V3*/
int lengthOfLongestSubstring(char* s)
{
	char *left, *right;
	int max, record[256];
	unsigned char idx;

	left = right = s;
	max =  0;

	memset(record, INVAL, sizeof(record));

	while (*right) {

		idx = *right;
		if (record[idx] != INVAL && record[idx] >= (left - s))
			left = s + record[idx] + 1;

		record[idx] = right - s;

		/*
		 * Loop Invariant:
		 * 	A condition that must hold whenever we reach this point in the loop.
		 * 	Whenever execution reaches the point to compute `max`, the range [left, right] contains no duplicate characters.
		 */
		if (right - left + 1 > max)
			max = right - left + 1;

		right++;
	}


	return max;
}

static void run_test(const char *name, char *s, int expected)
{
	int ret;

	printf("=== %s ===\n", name);
	printf("Input:    \"%s\"\n", s);

	ret = lengthOfLongestSubstring(s);

	printf("Output:   %d\n", ret);
	printf("Expected: %d\n", expected);

	assert(ret == expected);
	printf("PASS\n\n");
}

int main(int argc, char *argv[])
{
#if 1
	/* Test 1: Original case */
	run_test("Original case", "abcb", 3);

	/* Test 2: LeetCode example 1 */
	run_test("LeetCode example 1", "abcabcbb", 3);

	/* Test 3: All same characters */
	run_test("All same characters", "bbbbb", 1);

	/* Test 4: LeetCode example 3 */
	run_test("LeetCode example 3", "pwwkew", 3);

	/* Test 5: Empty string */
	run_test("Empty string", "", 0);

	/* Test 6: Single character */
	run_test("Single character", "a", 1);

	/* Test 7: Single space */
	run_test("Single space", " ", 1);

	/* Test 8: Non-adjacent duplicate */
	run_test("Non-adjacent duplicate", "dvdf", 3);

	/* Test 9: All unique */
	run_test("All unique", "abcdefg", 7);

	/* Test 10: Duplicate at start */
	run_test("Duplicate at start", "aab", 2);

	/* Test 11: LeetCode example */
	run_test("Duplicate at start", "cdcda", 3);

	/* Test 12: LeetCode example */
	run_test("Duplicate at start", "abcdc", 4);

	printf("All tests passed!\n");
#endif
	return 0;
}
