# leetcode-workflow

## Description

Workflow rules for all code changes under the `leetcode/` directory. Covers git commit conventions, test case style, bug-fix file organization, and documentation requirements.

## Rules

### 1. Git Commit

Every code change inside `leetcode/` **must** end with:

```bash
git add <changed files> && git commit -s -m "<message>"
```

- Always use `-s` (Signed-off-by).
- Commit message should follow the existing style: `<topic>: <short description>`.

### 2. Test Case Code Style

When asked to write test cases, follow the style in `sort/quick_sort/main.c`:

- **Do NOT modify the user's algorithm**. Only add test infrastructure and test data.
- Use these helper patterns:

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

static void print_array(const char *label, int *array, int len)
{
	int i;

	printf("%s[", label);
	for (i = 0; i < len - 1; i++)
		printf("%d ", array[i]);
	if (len > 0)
		printf("%d", array[i]);
	printf("]\n");
}

static int check_sorted(int *array, int len)
{
	int i;

	for (i = 0; i < len - 1; i++) {
		if (array[i] > array[i + 1])
			return 0;
	}
	return 1;
}

static void run_test(const char *name, int *src, int len)
{
	int *array = malloc(len * sizeof(int));

	assert(array != NULL);
	memcpy(array, src, len * sizeof(int));

	printf("=== %s ===\n", name);
	print_array("Input:  ", array, len);
	/* call the algorithm under test here */
	print_array("Output: ", array, len);

	assert(check_sorted(array, len));
	printf("PASS\n\n");

	free(array);
}
```

- Adapt `run_test` / `check_sorted` to match the specific problem (e.g. for non-sorting problems, replace `check_sorted` with the appropriate validation logic).
- Use `assert()` for pass/fail.
- Print `PASS` per case, `All tests passed!` at the end.
- Cover edge cases: empty/single element, already-solved, reverse, duplicates, negative numbers, large range, etc.
- Use tab indentation, K&R brace style, matching the user's existing code.

### 3. Bug Fix Directory

When test cases **fail** (the user's algorithm has bugs):

1. Create a `fixed/` subdirectory in the **same directory** as the original `main.c`.
2. Copy `main.c` into `fixed/main.c`.
3. Fix the bugs **only in `fixed/main.c`**. Never modify the original `main.c` algorithm.
4. Include the same test cases in `fixed/main.c`.
5. Copy or create a `Makefile` in `fixed/` so it can be built independently.

Directory structure example:

```
leetcode/sort/quick_sort/
├── Makefile
├── main.c          # original (untouched algorithm)
├── fixed/
│   ├── Makefile
│   └── main.c      # fixed version with same test cases
└── readme.md
```

### 4. Documentation (readme.md)

For every problem directory, create a `readme.md` in the **same directory** as `main.c`. The document must:

- Be written in **Markdown**.
- Start with a **Table of Contents** (linked headings).
- Use the **Feynman Learning Technique** to explain the solution — write as if teaching someone who has never seen the problem before:
  1. **Problem Statement** — Describe the problem in plain language.
  2. **Core Idea** — Explain the key insight / algorithm in the simplest terms possible, using analogies if helpful.
  3. **Step-by-Step Walkthrough** — Walk through a concrete example by hand, showing every step.
  4. **Where It Gets Tricky** — Identify the parts that are easy to get wrong and explain why.
  5. **Complexity Analysis** — Time and space complexity with brief justification.
  6. **Summary** — One-paragraph recap as if explaining to a friend.

### 5. Checklist

Before finishing any leetcode task, verify:

- [ ] Algorithm in `main.c` is untouched (only test code added).
- [ ] If tests fail, `fixed/` directory exists with corrected code.
- [ ] `readme.md` exists with TOC and Feynman-style explanation.
- [ ] `git commit -s` has been executed for all changes.
