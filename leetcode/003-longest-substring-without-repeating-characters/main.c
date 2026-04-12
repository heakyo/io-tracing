#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct mark {
	int cnt;
	int offset;
};

int lengthOfLongestSubstring(char* s)
{
	struct mark m[256];
	int max;
	char *p1, *p2;	

	max = 0;
	p1 = s;
	p2 = p1;

	while(*p1) {
		memset(m, 0x0, sizeof(struct mark)*256);
		p2 = p1;
		while (*p2) {

			if (m[*p2].cnt > 0) {
				m[*p2].cnt = 0;
				if (p2 - p1 > max)
					max = p2 - p1;
				break;
			}

			m[*p2].cnt++;
			m[*p2].offset = p2 - p1;

			p2++;
		}

		if (*p2 == '\0' && p2 - p1 > max) {
			max = p2 - p1;
			break;
		}

		p1 += m[*p2].offset + 1;
		m[*p2].offset = 0;
	}

	return max;
}

int main(int argc, char *argv[])
{
	//char *s = "abcabcbb";
	char *s = "abcb";
	int ret;

	/* Input */
	printf("Input: [%s]\n", s);

	/* Algorithm */
	ret = lengthOfLongestSubstring(s);

	/* Output */
	printf("Output:%d\n", ret);

	return 0;
}
