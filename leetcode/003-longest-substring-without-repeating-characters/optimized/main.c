#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 * Optimized sliding window with last-seen position array.
 *
 * Single pass: for each character, if it was seen at or after 'left',
 * move 'left' past its last occurrence. Update the character's last
 * position and track the maximum window size.
 *
 * Time:  O(n) — one pass, no memset per window position.
 * Space: O(1) — fixed 256-entry array.
 */
int lengthOfLongestSubstring(char *s)
{
	int last[256];
	int left, right, max_len, len;

	memset(last, 0xff, sizeof(last)); /* -1 for all entries */

	left = 0;
	max_len = 0;
	len = strlen(s);

	for (right = 0; right < len; right++) {
		unsigned char ch = s[right];

		if (last[ch] >= left)
			left = last[ch] + 1;

		last[ch] = right;

		if (right - left + 1 > max_len)
			max_len = right - left + 1;
	}

	return max_len;
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

	printf("All tests passed!\n");
	return 0;
}
