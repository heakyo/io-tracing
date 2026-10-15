#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

bool isAnagram(char *s, char *t);

bool isAnagramV1(char *s, char *t)
{
	int records[256];
	int recordt[256];
	unsigned char idx;
	int i;
	char *sp, *tp;

	memset(records, 0x0, sizeof(records));
	memset(recordt, 0x0, sizeof(recordt));

	sp = s;
	tp = t;

	while (*sp) {
		idx = (unsigned char)*sp;
		records[idx]++;

		sp++;
	}

	while (*tp) {
		idx = (unsigned char)*tp;
		recordt[idx]++;

		tp++;
	}

	for (i = 0; i < 256; i++) {
		if (records[i] != recordt[i])
			return false;
	}


	return true;
}

bool isAnagram(char *s, char *t)
{
	int record[256];
	unsigned char idx;
	int i;
	char *sp, *tp;

	memset(record, 0x0, sizeof(record));

	sp = s;
	tp = t;

	while (*sp && *tp) {
		idx = (unsigned char)*sp;
		record[idx]++;

		idx = (unsigned char)*tp;
		record[idx]--;

		tp++;
		sp++;
	}

	if (*sp || *tp)
		return false;

	for (i = 0; i < 256; i++) {
		if (record[i])
			return false;
	}

	return true;
}

static void
test_is_anagram(void)
{
        bool ret;

        printf("Running test_is_anagram...\n");

        /*
         * Test 1: Basic valid anagram.
         */
        {
                char s[] = "anagram";
                char t[] = "nagaram";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 2: Basic invalid anagram.
         */
        {
                char s[] = "rat";
                char t[] = "car";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 3: Identical strings.
         */
        {
                char s[] = "abcdef";
                char t[] = "abcdef";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 4: Same characters in reverse order.
         */
        {
                char s[] = "abcdef";
                char t[] = "fedcba";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 5: Different string lengths.
         */
        {
                char s[] = "abc";
                char t[] = "abcd";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 6: Same length, different character counts.
         */
        {
                char s[] = "aabb";
                char t[] = "abbb";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 7: Repeated characters with equal counts.
         */
        {
                char s[] = "aaabbbccc";
                char t[] = "cbacbacba";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 8: Single identical character.
         */
        {
                char s[] = "a";
                char t[] = "a";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 9: Single different character.
         */
        {
                char s[] = "a";
                char t[] = "b";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 10: Empty strings.
         *
         * Additional robustness test outside LeetCode constraints.
         */
        {
                char s[] = "";
                char t[] = "";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 11: One empty string.
         *
         * Additional robustness test outside LeetCode constraints.
         */
        {
                char s[] = "";
                char t[] = "a";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 12: Case-sensitive comparison.
         *
         * Additional robustness test outside LeetCode constraints.
         */
        {
                char s[] = "aA";
                char t[] = "Aa";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 13: Uppercase and lowercase are different.
         *
         * Additional robustness test outside LeetCode constraints.
         */
        {
                char s[] = "abc";
                char t[] = "Abc";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 14: Numeric ASCII characters.
         *
         * Additional robustness test outside LeetCode constraints.
         */
        {
                char s[] = "123321";
                char t[] = "112233";

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 15: Same characters with different frequencies.
         */
        {
                char s[] = "aabbcc";
                char t[] = "aabccc";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 16: Long strings with identical frequencies.
         */
        {
                char s[1001];
                char t[1001];
                int i;

                for (i = 0; i < 1000; i++) {
                        s[i] = 'a' + (i % 26);
                        t[999 - i] = s[i];
                }

                s[1000] = '\0';
                t[1000] = '\0';

                ret = isAnagram(s, t);
                assert(ret == true);
        }

        /*
         * Test 17: Long strings with one modified character.
         */
        {
                char s[1001];
                char t[1001];
                int i;

                for (i = 0; i < 1000; i++) {
                        s[i] = 'a';
                        t[i] = 'a';
                }

                s[1000] = '\0';
                t[1000] = '\0';

                t[999] = 'b';

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        /*
         * Test 18: Same total length and same set of characters,
         * but different character frequencies.
         */
        {
                char s[] = "aaaabbbb";
                char t[] = "aaabbbbb";

                ret = isAnagram(s, t);
                assert(ret == false);
        }

        printf("test_is_anagram PASSED\n");
}

int
main(void)
{
        test_is_anagram();

        return (0);
}
