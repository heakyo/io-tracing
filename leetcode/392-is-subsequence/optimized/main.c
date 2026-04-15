#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

bool isSubsequence(char *s, char *t)
{
	for (; *t; t++)
		if (*t == *s)
			s++;
	return !*s;
}

static void run_test(const char *name, char *s, char *t, bool expected)
{
	bool ret;

	printf("=== %s ===\n", name);
	printf("Input:    s = \"%s\", t = \"%s\"\n", s, t);

	ret = isSubsequence(s, t);

	printf("Output:   %s\n", ret ? "true" : "false");
	printf("Expected: %s\n", expected ? "true" : "false");

	assert(ret == expected);
	printf("PASS\n\n");
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example 1 */
	run_test("LeetCode example 1", "abc", "ahbgdc", true);

	/* Test 2: LeetCode example 2 */
	run_test("LeetCode example 2", "axc", "ahbgdc", false);

	/* Test 3: Original case - not enough repeats */
	run_test("Not enough repeats", "aaaaaa", "bbaaaa", false);

	/* Test 4: Empty s (empty is subsequence of anything) */
	run_test("Empty s", "", "ahbgdc", true);

	/* Test 5: Empty t (non-empty can't be subseq of empty) */
	run_test("Empty t", "abc", "", false);

	/* Test 6: Both empty */
	run_test("Both empty", "", "", true);

	/* Test 7: Single char match */
	run_test("Single char match", "a", "a", true);

	/* Test 8: Single char no match */
	run_test("Single char no match", "a", "b", false);

	/* Test 9: s equals t */
	run_test("s equals t", "abc", "abc", true);

	/* Test 10: s longer than t */
	run_test("s longer than t", "abcdef", "abc", false);

	/* Test 11: Subsequence at end */
	run_test("Subsequence at end", "dc", "ahbgdc", true);

	/* Test 12: Repeated chars - enough repeats */
	run_test("Enough repeats", "aaa", "aaaa", true);

	printf("All tests passed!\n");
	return 0;
}
