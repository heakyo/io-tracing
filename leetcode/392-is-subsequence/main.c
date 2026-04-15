#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

bool isSubsequence(char* s, char* t)
{
	char *p1 = s, *p2 = t;
	bool found;

	while (*p1) {
		found = false;
		while (*p2) {
			if (*p2++ == *p1) {
				found = true;
				break;
			}
		}

		if (!found)
			return false;

		p1++;
	}

	return true;
}

int main(int argc, char *argv[])
{
	char *s = "aaaaaa";
	char *t = "bbaaaa";
	bool ret;

	/* Input */
	printf("Input: s = %s, t = %s\n", s, t);

	/* Algorithm */
	ret = isSubsequence(s, t);

	/* Output */
	printf("Output: %s\n", ret ? "true" : "false");

	return 0;
}
