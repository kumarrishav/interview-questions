
#include <stdio.h>

unsigned count_ones(unsigned int n) {
	unsigned res = 0;
	for (; n != 0; res++, n &= n-1)
		; /* Intentionally left blank */
	return res;
}

int main(void) {
	unsigned int val;
	printf("Enter a positive value\n");
	printf("> ");

	while (scanf("%u", &val) == 1) {
		printf("Number of 1's: %u\n", count_ones(val));
		printf("> ");
	}

	return 0;
}
