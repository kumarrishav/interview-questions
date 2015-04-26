/* You are given two 32-bit numbers, N and M, and two bit positions, i and j. Write
 * a method to insert M into N such that M starts at bit j and ends at bit i. You can
 * assume that the bits j through i have enough space to fit all of M. That is, if
 * M = 10011, you can assume that there are at least 5 bits between j and i. You
 * would not, for example, have j = 3 and i = 2, because M could not fully fit
 * between bit 3 and bit 2.
 * EXAMPLE
 * Input: N = 10000000000, M = 10011, i = 2, j = 6
 * Output: N = 10001001100
 *
 * Source: Cracking the Coding Interview, page 91, question 5.1
 */

#include <stdio.h>
#include <assert.h>
#include <limits.h>

unsigned insert_into(unsigned N, unsigned M, unsigned j, unsigned i) {
	assert(j >= i);
	unsigned mask = ~(~(~0 << (j-i+1)) << i);
	unsigned N_cleared = N & mask;
	return N_cleared | (M << i);
}

void print_binary(unsigned n) {
	size_t unsigned_bits = sizeof(unsigned)*CHAR_BIT;
	unsigned i;
	for (i = 0; i < unsigned_bits; i++) {
		printf("%c", (n & (1 << (unsigned_bits-i-1))) ? '1' : '0');
	}
}

int main(void) {
	printf("Enter N, M, j, and i\n");

	unsigned N, M, j, i;

	while (scanf("%u%u%u%u", &N, &M, &j, &i) == 4) {
		printf("N =\t\t");
		print_binary(N);
		printf("\nM =\t\t");
		print_binary(M);
		printf("\nResult =\t");
		print_binary(insert_into(N, M, j, i));
		printf("\n");
	}

	return 0;
}
