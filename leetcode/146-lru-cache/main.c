#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof (a) / sizeof *(a))

typedef struct {
    
} LRUCache;


LRUCache* lRUCacheCreate(int capacity) {
    
}

int lRUCacheGet(LRUCache* obj, int key) {
    
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    
}

void lRUCacheFree(LRUCache* obj) {
    
}

/**
 * Your LRUCache struct will be instantiated and called as such:
 * LRUCache* obj = lRUCacheCreate(capacity);
 * int param_1 = lRUCacheGet(obj, key);
 
 * lRUCachePut(obj, key, value);
 
 * lRUCacheFree(obj);
*/

enum { OP_PUT, OP_GET };

struct op {
	int type;
	int key;
	int value;
	int expected;
};

#define PUT(k, v)    {OP_PUT, k, v, 0}
#define GET(k, exp)  {OP_GET, k, 0, exp}

static void run_test(const char *name, int capacity,
		     struct op *ops, int n)
{
	LRUCache *cache;
	int i, result;

	printf("=== %s ===\n", name);
	printf("capacity=%d\n", capacity);
	cache = lRUCacheCreate(capacity);

	for (i = 0; i < n; i++) {
		if (ops[i].type == OP_PUT) {
			printf("  put(%d, %d)\n", ops[i].key, ops[i].value);
			lRUCachePut(cache, ops[i].key, ops[i].value);
		} else {
			result = lRUCacheGet(cache, ops[i].key);
			printf("  get(%d) = %d, expected %d\n",
			       ops[i].key, result, ops[i].expected);
			assert(result == ops[i].expected);
		}
	}

	lRUCacheFree(cache);
	printf("PASS\n\n");
}

int main(int argc, char *argv[])
{
	/* Test 1: LeetCode example */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2), GET(1, 1),
			PUT(3, 3), GET(2, -1), PUT(4, 4),
			GET(1, -1), GET(3, 3), GET(4, 4),
		};
		run_test("LeetCode example", 2, ops, ARRAYSIZE(ops));
	}

	/* Test 2: Capacity 1 — every put evicts */
	{
		struct op ops[] = {
			PUT(1, 1), GET(1, 1),
			PUT(2, 2), GET(1, -1), GET(2, 2),
		};
		run_test("Capacity 1", 1, ops, ARRAYSIZE(ops));
	}

	/* Test 3: Get miss on empty cache */
	{
		struct op ops[] = {
			GET(1, -1), GET(99, -1),
		};
		run_test("Get miss on empty cache", 2, ops, ARRAYSIZE(ops));
	}

	/* Test 4: Update existing key (no eviction) */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(1, 2), GET(1, 2),
		};
		run_test("Update existing key", 2, ops, ARRAYSIZE(ops));
	}

	/* Test 5: Get refreshes LRU order */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2),
			GET(1, 1),		/* refreshes 1, LRU is now 2 */
			PUT(3, 3),		/* evicts 2 */
			GET(1, 1), GET(2, -1), GET(3, 3),
		};
		run_test("Get refreshes LRU order", 2, ops, ARRAYSIZE(ops));
	}

	/* Test 6: Put existing key refreshes LRU order */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2),
			PUT(1, 10),		/* update 1, refreshes it, LRU is 2 */
			PUT(3, 3),		/* evicts 2 */
			GET(1, 10), GET(2, -1), GET(3, 3),
		};
		run_test("Put existing refreshes LRU", 2, ops, ARRAYSIZE(ops));
	}

	/* Test 7: Capacity 3, evict oldest */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2), PUT(3, 3),
			PUT(4, 4),		/* evicts 1 */
			GET(1, -1), GET(2, 2), GET(3, 3), GET(4, 4),
		};
		run_test("Capacity 3 evict oldest", 3, ops, ARRAYSIZE(ops));
	}

	/* Test 8: Multiple consecutive evictions */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2),
			PUT(3, 3),		/* evicts 1 */
			PUT(4, 4),		/* evicts 2 */
			GET(1, -1), GET(2, -1), GET(3, 3), GET(4, 4),
		};
		run_test("Multiple consecutive evictions", 2,
			 ops, ARRAYSIZE(ops));
	}

	/* Test 9: Get miss doesn't affect LRU order */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2),
			GET(3, -1),		/* miss, order unchanged */
			PUT(3, 3),		/* evicts 1 (still LRU) */
			GET(1, -1), GET(2, 2),
		};
		run_test("Get miss no effect on order", 2,
			 ops, ARRAYSIZE(ops));
	}

	/* Test 10: Long mixed sequence with get/put refreshes */
	{
		struct op ops[] = {
			PUT(1, 1), PUT(2, 2), PUT(3, 3),
			GET(1, 1),		/* refresh 1, LRU order: 2,3,1 */
			PUT(4, 4),		/* evicts 2 */
			GET(2, -1),
			PUT(5, 5),		/* evicts 3 */
			GET(3, -1),
			GET(1, 1), GET(4, 4), GET(5, 5),
		};
		run_test("Long mixed sequence", 3, ops, ARRAYSIZE(ops));
	}

	printf("All tests passed!\n");
	return 0;
}
