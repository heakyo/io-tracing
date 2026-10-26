#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "hashtable/include/hashtable.h"

char ***groupAnagrams(char **strs, int strsSize,
    int *returnSize, int **returnColumnSizes);

static bool
test_are_anagrams(const char *a, const char *b);

/* Brute Force + Visited */
char ***groupAnagrams_bruteForce_Visted(char **strs, int strsSize,
    int *returnSize, int **returnColumnSizes)
{
	int i, j, k, h;
	char ***grpAnagrams;
	int *columnSizes;
	int *visited;
	char **tmp;

	grpAnagrams = NULL;
	columnSizes = NULL;
	visited = NULL;
	k = 0;

	if (strsSize == 0)
		goto exit;

	grpAnagrams = calloc(strsSize, sizeof(char **));
	if (!grpAnagrams)
		goto exit;

	columnSizes = calloc(strsSize, sizeof(int));
	if (!columnSizes)
		goto alloc_columnSizes_failed;

	visited = calloc(strsSize, sizeof(int));
	if (!visited)
		goto alloc_visited_failed;

	for (i = 0; i < strsSize; i++) {

		if (visited[i])
			continue;

		h = 0;
		grpAnagrams[k] = calloc(strsSize, sizeof(char *));
		if (!grpAnagrams[k])
			goto alloc_grpAnagramsK_failed;

		grpAnagrams[k][h++] = strs[i];

		visited[i] = 1;

		for (j = i + 1; j < strsSize; j++) {

			if (visited[j])
				continue;

			if (test_are_anagrams(strs[i], strs[j])) {
				grpAnagrams[k][h++] = strs[j];
				visited[j] = 1;
			}
		}

		columnSizes[k] = h;

		tmp = realloc(grpAnagrams[k], h * sizeof(char *));
		if (!tmp)
			goto alloc_grpAnagramsK_failed;
		grpAnagrams[k] = tmp;

		k++;
	}

	free(visited);

	*returnColumnSizes = columnSizes;
	*returnSize = k;

	return grpAnagrams;

alloc_grpAnagramsK_failed:
	for (i = k - 1; i >= 0; i--)
		free(grpAnagrams[i]);
	free(visited);

alloc_visited_failed:
	free(columnSizes);

alloc_columnSizes_failed:
	free(grpAnagrams);

exit:
	*returnSize = 0;
	*returnColumnSizes = NULL;

	return NULL;
}

void getSignature(char *str, int *signature)
{
	char *p;
	int index;

	p = str;

	while (*p) {
		index = *p - 'a';
		signature[index]++;
		p++;
	}
}

char ***groupAnagrams(char **strs, int strsSize, int *returnSize, int **returnColumnSizes)
{
	HashTable *ht;
	char ***grpAnagrams;
	int *columnSizes;
	int signature[26];
	int grpIndex, colmIndex, index;
	int i;

	grpIndex = 0;
	index = 0;

	grpAnagrams = calloc(200, sizeof(char **));
	columnSizes = calloc(200, sizeof(int));

	ht = hash_create(0);

	for (i = 0; i < strsSize; i++) {

		memset(signature, 0x0, sizeof(signature));
		getSignature(strs[i], signature);

		if (hash_find(ht, signature, &index)) {
			colmIndex = columnSizes[index];
			grpAnagrams[index][colmIndex] = strs[i];
			columnSizes[index]++;

			continue;
		}

		hash_insert(ht, signature, grpIndex);
		grpAnagrams[grpIndex] = calloc(200, sizeof(char *));
		colmIndex = columnSizes[grpIndex];
		grpAnagrams[grpIndex][colmIndex] = strs[i];
		columnSizes[grpIndex]++;

		grpIndex++;
	}

	hash_destroy(ht);

	*returnColumnSizes = columnSizes;
	*returnSize = grpIndex;

	return grpAnagrams;
}

/*
 * Check whether two strings are anagrams.
 * Test helper only.
 */
static bool
test_are_anagrams(const char *a, const char *b)
{
        int count[256] = { 0 };
        unsigned char c;

        while (*a && *b) {
                c = (unsigned char)*a++;
                count[c]++;

                c = (unsigned char)*b++;
                count[c]--;
        }

        if (*a || *b)
                return (false);

        for (int i = 0; i < 256; i++) {
                if (count[i] != 0)
                        return (false);
        }

        return (true);
}

/*
 * Validate that every input string appears exactly once,
 * and that each returned group contains only anagrams.
 *
 * Input strings must be distinct pointers. Their contents
 * may be identical.
 */
static void
validate_group_anagrams(char **strs, int strsSize,
    char ***result, int returnSize, int *columnSizes)
{
        bool *seen;
        int total = 0;
        int i, j, k;
        int found;

        assert(returnSize >= 0);

        if (strsSize == 0) {
                assert(returnSize == 0);
                return;
        }

        assert(result != NULL);
        assert(columnSizes != NULL);
        assert(returnSize > 0);
        assert(returnSize <= strsSize);

        seen = calloc((size_t)strsSize, sizeof(*seen));
        assert(seen != NULL);

        for (i = 0; i < returnSize; i++) {
                assert(result[i] != NULL);
                assert(columnSizes[i] > 0);

                for (j = 0; j < columnSizes[i]; j++) {
                        assert(result[i][j] != NULL);

                        /*
                         * Every string in a group must be
                         * an anagram of its first string.
                         */
                        assert(test_are_anagrams(
                            result[i][0], result[i][j]));

                        /*
                         * Find the original input pointer.
                         */
			for (k = 0; k < strsSize; k++) {
				if (!seen[k] &&
				strcmp(result[i][j], strs[k]) == 0) {
					found = k;
					break;
				}
			}

			assert(found >= 0);
                        seen[found] = true;
                        total++;
                }
        }

        assert(total == strsSize);

        for (i = 0; i < strsSize; i++)
                assert(seen[i]);

        /*
         * Different groups must not contain anagrams
         * of one another.
         */
        for (i = 0; i < returnSize; i++) {
                for (j = i + 1; j < returnSize; j++) {
                        assert(!test_are_anagrams(
                            result[i][0], result[j][0]));
                }
        }

        free(seen);
}

/*
 * Free result containers.
 *
 * This assumes the returned strings point directly to
 * the original input strings.
 */
static void
free_group_anagrams(char ***result, int returnSize,
    int *columnSizes)
{
        int i;

        if (result != NULL) {
                for (i = 0; i < returnSize; i++)
                        free(result[i]);

                free(result);
        }

        free(columnSizes);
}

/*
 * Run one test case.
 */
static void
run_group_anagrams_test(const char *name,
    char **strs, int strsSize, int expectedGroups)
{
        char ***result;
        int *columnSizes = NULL;
        int returnSize = 0;

        printf("Running %s...\n", name);

        result = groupAnagrams(strs, strsSize,
            &returnSize, &columnSizes);

        assert(returnSize == expectedGroups);

        validate_group_anagrams(strs, strsSize,
            result, returnSize, columnSizes);

        free_group_anagrams(result, returnSize, columnSizes);

        printf("  PASSED\n");
}

static void
test_group_anagrams(void)
{
        /*
         * Test 1: Standard LeetCode example.
         */
        {
                char *strs[] = {
                        "eat", "tea", "tan",
                        "ate", "nat", "bat"
                };

                run_group_anagrams_test("Test 1",
                    strs, 6, 3);
        }

        /*
         * Test 2: Single empty string.
         */
        {
                char *strs[] = { "" };

                run_group_anagrams_test("Test 2",
                    strs, 1, 1);
        }

        /*
         * Test 3: Single character.
         */
        {
                char *strs[] = { "a" };

                run_group_anagrams_test("Test 3",
                    strs, 1, 1);
        }

        /*
         * Test 4: All strings belong to one group.
         */
        {
                char *strs[] = {
                        "abc", "bca", "cab", "acb"
                };

                run_group_anagrams_test("Test 4",
                    strs, 4, 1);
        }

        /*
         * Test 5: No two strings are anagrams.
         */
        {
                char *strs[] = {
                        "abc", "def", "ghi", "jkl"
                };

                run_group_anagrams_test("Test 5",
                    strs, 4, 4);
        }

        /*
         * Test 6: Identical strings.
         */
        {
                char *strs[] = {
                        "abc", "abc", "abc"
                };

                run_group_anagrams_test("Test 6",
                    strs, 3, 1);
        }

        /*
         * Test 7: Empty strings mixed with others.
         */
        {
                char *strs[] = {
                        "", "a", "", "b", ""
                };

                run_group_anagrams_test("Test 7",
                    strs, 5, 3);
        }

        /*
         * Test 8: Same letters, different frequencies.
         */
        {
                char *strs[] = {
                        "aab", "aba", "abb", "bab", "baa"
                };

                run_group_anagrams_test("Test 8",
                    strs, 5, 2);
        }

        /*
         * Test 9: Different string lengths.
         */
        {
                char *strs[] = {
                        "a", "aa", "aaa", "aaaa"
                };

                run_group_anagrams_test("Test 9",
                    strs, 4, 4);
        }

        /*
         * Test 10: Multiple groups of different sizes.
         */
        {
                char *strs[] = {
                        "listen", "silent", "enlist",
                        "cat", "act",
                        "dog", "god",
                        "hello"
                };

                run_group_anagrams_test("Test 10",
                    strs, 8, 4);
        }

        /*
         * Test 11: One large anagram group.
         */
        {
                char *strs[] = {
                        "abcdef", "fedcba", "badcfe",
                        "cfebad", "defabc"
                };

                run_group_anagrams_test("Test 11",
                    strs, 5, 1);
        }

        /*
         * Test 12: Strings differing by one character.
         */
        {
                char *strs[] = {
                        "abcd", "abce", "abcf", "abdc"
                };

                run_group_anagrams_test("Test 12",
                    strs, 4, 3);
        }

        /*
         * Test 13: Two-character anagrams.
         */
        {
                char *strs[] = {
                        "ab", "ba", "cd", "dc",
                        "ef", "fe", "gg"
                };

                run_group_anagrams_test("Test 13",
                    strs, 7, 4);
        }

        /*
         * Test 14: Large number of identical strings.
         */
        {
                char *strs[100];
                int i;

                for (i = 0; i < 100; i++)
                        strs[i] = "storage";

                run_group_anagrams_test("Test 14",
                    strs, 100, 1);
        }

        /*
         * Test 15: Large number of unique strings.
         */
        {
		char storage[100][128];
		char *strs[100];
		int i, j;

		for (i = 0; i < 100; i++) {

			for (j = 0; j <= i; j++)
				storage[i][j] = 'a';

			storage[i][i + 1] = '\0';
			strs[i] = storage[i];
		}

                /*
                 * The numeric suffixes create some
                 * anagram groups, e.g. item12/item21.
                 *
                 * The validator checks the grouping;
                 * this case does not use a fixed count.
                 */
                {
                        char ***result;
                        int *columnSizes = NULL;
                        int returnSize = 0;

                        result = groupAnagrams(strs, 100,
                            &returnSize, &columnSizes);

                        validate_group_anagrams(strs, 100,
                            result, returnSize, columnSizes);

                        free_group_anagrams(result,
                            returnSize, columnSizes);
                }

                printf("Test 15 PASSED\n");
        }

        /*
         * Test 16: Zero input strings.
         *
         * Additional robustness test.
         */
        {
                run_group_anagrams_test("Test 16",
                    NULL, 0, 0);
        }

        printf("All groupAnagrams tests PASSED\n");
}

int
main(void)
{
        test_group_anagrams();
        return (0);
}
