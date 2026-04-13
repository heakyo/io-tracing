#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct mark {
	int seen;
	int len;
};

int lengthOfLongestSubstring(char* s)
{
	struct mark m[256];
	char *p1, *p2;	
	int max;

	max = 0;
	p1 = s;

	while(*p1) {
		memset(m, 0x0, sizeof(m));
		p2 = p1;
		while (*p2) {

			if (m[*p2].seen)
				break;

			m[*p2].seen = 1;
			m[*p2].len = p2 - p1;

			p2++;
		}

		if (p2 - p1 > max)
			max = p2 - p1;

		p1 += m[*p2].len + 1;
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
