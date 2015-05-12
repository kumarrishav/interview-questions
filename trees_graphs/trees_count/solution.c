
#include <stdio.h>

unsigned long long trees(unsigned n) {
	if (n <= 1) {
		return 1;
	}

	unsigned long long res = 0;
	unsigned i;
	for (i = 0; i < n; i++) {
		res = res + 2*trees(i)*trees(n-i-1);
	}

	return res;
}

int main(void) {
	printf("Enter a positive integer to calculate the number of binary trees of size N\n");
	printf("> ");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		printf("%llu\n", trees(n));
		printf("> ");
	}

	return 0;
}
