#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct mark {
	int seen;
	int len;
};

/* brute force */
int lengthOfLongestSubstring(char* s)
{
	int max;
	bool found;
	size_t slen;
	char *anchor, *ap, *q, *qp;

	anchor = s;

	slen = strlen(s);
	q = calloc(slen + 1, sizeof(*q));

	max =  0;

	while (*anchor) {

		found = false;
		memset(q, 0x0, slen);
		*q = *anchor;

		for (ap = anchor + 1; *ap; ap++) {
			qp = q;
			for (; *qp; qp++) {
				if (*ap == *qp) {
					found = true;
					break;
				}
			}

			if (found)
				break;

			*qp = *ap;
		}

		if ((*ap == 0 || found) && strlen(q) > max)
			max = strlen(q);

		anchor++;
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

	printf("All tests passed!\n");
#endif
	return 0;
}
