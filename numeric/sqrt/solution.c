/* Given a number less than 10000, determine whether that number is a perfect square.
 * If it is, find the square root in O(1) time.
 *
 * Source: Careercup (Microsoft interview)
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>

int perfect_square(unsigned n, unsigned *sqrt) {
	assert(0 < n && n < 10000);

	static int initialized = 0;
	static unsigned sq_root[100000];

	if (!initialized) {
		memset(sq_root, 0, sizeof(sq_root));
		size_t i;
		for (i = 1; i < 100; i++) {
			sq_root[i*i] = i;
		}
		initialized = 1;
	}

	*sqrt = sq_root[n];
	return *sqrt != 0;
}

int main(void) {
	unsigned res;
	unsigned tests[] = { 1, 2, 3, 4, 10, 15, 20, 25, 90, 99, 100, 2000, 1000, 3000, 5000, 9801 };

	size_t i;
	for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
		int is_perfect = perfect_square(tests[i], &res);
		printf("perfect_square(%u) = ", tests[i]);
		if (!is_perfect) {
			printf("false\n");
		} else {
			printf("true (sqrt = %u)\n", res);
		}
	}

	return 0;
}
