/* Write a function to determine the number of bits required to convert integer A
 * to integer B.
 * EXAMPLE
 * Input: 31, 14
 * Output: 2
 *
 * Source: Cracking the Coding Interview, page 92, question 5.5
 */
#include <stdio.h>
#include <limits.h>

unsigned convert_bits(unsigned a, unsigned b) {
	unsigned diff = a^b;

	unsigned bits;
	for (bits = 0; diff != 0; bits++, diff &= (diff-1))
		; /* Intentionally left blank */

	return bits;
}

void print_binary(unsigned n) {
	size_t unsigned_bits = sizeof(unsigned)*CHAR_BIT;
	unsigned i;
	for (i = 0; i < unsigned_bits; i++) {
		printf("%c", (n & (1 << (unsigned_bits-i-1))) ? '1' : '0');
	}
}

int main(void) {
	printf("Enter A and B\n");
	unsigned a, b;

	while (scanf("%u%u", &a, &b) == 2) {
		printf("A = ");
		print_binary(a);
		printf("\nB = ");
		print_binary(b);
		printf("\nBits to convert A to B: %u\n", convert_bits(a, b));
	}

	return 0;
}
