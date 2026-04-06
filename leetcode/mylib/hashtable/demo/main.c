#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hashtable.h"

int main(void)
{
	hashtable_t *ht;
	ht_iter_t iter;
	int v1 = 100, v2 = 200, v3 = 300, v4 = 400;
	int *val;

	/* ========== ht_create ========== */
	printf("=== ht_create ===\n");
	ht = ht_create(0);
	assert(ht != NULL);
	printf("Hash table created (default capacity)\n");
	printf("Initial size: %zu\n\n", ht_size(ht));

	/* ========== ht_set / ht_get ========== */
	printf("=== ht_set / ht_get ===\n");
	assert(ht_set(ht, "apple", &v1) == 0);
	assert(ht_set(ht, "banana", &v2) == 0);
	assert(ht_set(ht, "cherry", &v3) == 0);
	printf("Inserted: apple=%d, banana=%d, cherry=%d\n", v1, v2, v3);
	printf("Size after inserts: %zu\n", ht_size(ht));

	val = (int *)ht_get(ht, "apple");
	assert(val && *val == 100);
	printf("ht_get(\"apple\")  = %d\n", *val);

	val = (int *)ht_get(ht, "banana");
	assert(val && *val == 200);
	printf("ht_get(\"banana\") = %d\n", *val);

	val = (int *)ht_get(ht, "cherry");
	assert(val && *val == 300);
	printf("ht_get(\"cherry\") = %d\n\n", *val);

	/* ========== ht_set (update existing key) ========== */
	printf("=== ht_set (update) ===\n");
	assert(ht_set(ht, "apple", &v4) == 0);
	val = (int *)ht_get(ht, "apple");
	assert(val && *val == 400);
	printf("Updated apple: ht_get(\"apple\") = %d\n", *val);
	printf("Size after update: %zu (unchanged)\n\n", ht_size(ht));

	/* ========== ht_contains ========== */
	printf("=== ht_contains ===\n");
	printf("ht_contains(\"banana\") = %d\n", ht_contains(ht, "banana"));
	printf("ht_contains(\"grape\")  = %d\n\n", ht_contains(ht, "grape"));
	assert(ht_contains(ht, "banana") == 1);
	assert(ht_contains(ht, "grape") == 0);

	/* ========== ht_remove ========== */
	printf("=== ht_remove ===\n");
	val = (int *)ht_remove(ht, "banana");
	assert(val && *val == 200);
	printf("Removed banana: value was %d\n", *val);
	printf("ht_contains(\"banana\") after remove = %d\n", ht_contains(ht, "banana"));
	printf("Size after remove: %zu\n\n", ht_size(ht));
	assert(ht_contains(ht, "banana") == 0);
	assert(ht_size(ht) == 2);

	/* ========== ht_get (missing key) ========== */
	printf("=== ht_get (missing key) ===\n");
	val = (int *)ht_get(ht, "notexist");
	assert(val == NULL);
	printf("ht_get(\"notexist\") = NULL\n\n");

	/* ========== ht_iter ========== */
	printf("=== ht_iter ===\n");
	ht_iter_init(ht, &iter);
	printf("Iterating all entries:\n");
	while (ht_iter_next(&iter))
		printf("  key=\"%s\", value=%d\n", iter.key, *(int *)iter.value);
	printf("\n");

	/* ========== ht_destroy ========== */
	printf("=== ht_destroy ===\n");
	ht_destroy(ht);
	printf("Hash table destroyed\n\n");

	printf("All demos passed!\n");
	return 0;
}
