/* Write a method that calculates the number of binary trees of size N.
 *
 * FOLLOW UP
 * What if we want the number of binary search trees of size N?
 *
 * Source: past interview experience
 */
#include <stdio.h>

unsigned long long trees(unsigned n) {
	if (n <= 1) {
		return 1;
	}

	unsigned long long res = 0;
	unsigned i;
	for (i = 0; i < n; i++) {
		res += 2*trees(i)*trees(n-i-1);
	}

	return res;
}

unsigned long long bsts(unsigned n) {
	if (n <= 1) {
		return 1;
	}

	unsigned long long res = 0;
	unsigned i;
	for (i = 0; i < n; i++) {
		res += bsts(i)*bsts(n-i-1);
	}

	return res;
}

int main(void) {
	printf("Enter a positive integer to calculate the number of binary trees of size N\n"
	       "and the number of binary search trees of size N\n");
	printf("> ");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		printf("Binary trees: %llu\nBinary search trees: %llu\n", trees(n), bsts(n));
		printf("> ");
	}

	return 0;
}
