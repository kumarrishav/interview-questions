/* Write a function that determines the number of 1 bits in the binary
 * representation of a given integer.
 *
 * Source: Programming Interviews Exposed, page 203
 */
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
