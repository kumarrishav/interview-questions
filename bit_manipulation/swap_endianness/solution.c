/* Write a function that takes an integer and returns another integer with the opposite endianness
 *
 * Source: past interview experience
 */
#include <stdio.h>
#include <limits.h>

// Assumes a char is 8 bits
unsigned swap_endianness(unsigned n) {
	unsigned res = 0;
	unsigned mask = 0xFF;

	size_t i;
	for (i = 0; i < sizeof(unsigned); i++) {
		res = (res << 8) | (n & mask);
		n >>= 8;
	}

	return res;
}

void print_binary(unsigned n) {
	unsigned  i = 1 << (sizeof(unsigned)*CHAR_BIT-1);
	unsigned j = 0;
	for (; i != 0; j++, i >>= 1) {
		// Easier to distinguish bytes in the output
		if (j > 0 && j%CHAR_BIT == 0) {
			putchar(' ');
		}
		putchar(n & i ? '1' : '0');
	}
}

int main(void) {
	printf("Enter an integer to see how it looks when endianness changes\n");
	printf("> ");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		printf("Received:\n");
		printf("\tBase 10: %u\n\tBinary: ", n);
		print_binary(n);

		unsigned swapped = swap_endianness(n);
		printf("\nAfter swapping endianness:\n");
		printf("\tBase 10: %u\n\tBinary: ", swapped);
		print_binary(swapped);
		printf("\n> ");
	}

	return 0;
}
