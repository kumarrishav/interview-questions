/* Given two integers A and B, a start index i and an end index j, write a function
 * that copies the bits in A[i..j] to B[i..j]
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <assert.h>
#include <limits.h>

/* Copy bits in `from[begin..end]` to `to[begin..end]`.
 * Bits are 0-indexed from LSB to MSB.
 *
 * 0 <= begin <= end <= sizeof(unsigned)*CHAR_BIT-1
 *
 */
unsigned copy_bits(unsigned from, unsigned to, unsigned begin, unsigned end) {
	assert(begin <= end);
	unsigned mask = ~(~0 << (end-begin+1)) << begin;
	return (to & ~mask) | (from & mask);
}

static void print_binary(unsigned n) {
	unsigned i = 1 << (sizeof(i)*CHAR_BIT-1);
	while (i != 0) {
		putchar('0'+!!(n&i));
		i >>= 1;
	}
	putchar('\n');
}

int main(void) {
	printf("Enter source number, target number, and indexes i, j\n");
	printf("This will copy the bits in `source[i..j]` to `target[i..j]`\n");
	printf("> ");

	unsigned src, dest, i, j;
	while (scanf("%u%u%u%u", &src, &dest, &i, &j) == 4) {
		printf("Input:\n");
		printf("src =\t");
		print_binary(src);
		printf("dest =\t");
		print_binary(dest);
		printf("Result:\n");
		unsigned copied = copy_bits(src, dest, i, j);
		printf("dest =\t");
		print_binary(copied);
		printf("> ");
	}

	return 0;
}
