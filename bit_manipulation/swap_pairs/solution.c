/* Write a program to swap odd and even bits in an integer with as few instructions as
 * possible (e.g. bit 0 and bit 1 are swapped, bit 2 and bit 3 are swapped, and so on).
 *
 * Source: Cracking the Coding Interview, 6th Edition, Chapter 5, Exercise 5.7 (page 116)
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

unsigned swap_pairs(unsigned n) {

#if UINT_MAX == UINT32_MAX
	unsigned odd_mask = 0xAAAAAAAA;
#elif UINT_MAX == UINT64_MAX
	unsigned odd_mask = 0xAAAAAAAAAAAAAAAA;
#else
#error "Unsupported size for unsigned integers"
#endif

	return ((n & odd_mask) >> 1U) | ((n & (odd_mask >> 1U)) << 1U);
}

static void print_binary(unsigned n) {
	unsigned i = 1U << (sizeof(n)*CHAR_BIT-1);
	while (i != 0) {
		putchar('0'+!!(n&i));
		i >>= 1U;
	}
	putchar('\n');
}

int main(void) {
	printf("Enter a number for the Flip Bit to Win game\n");
	printf("> ");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		printf("Input:\t");
		print_binary(n);
		printf("Output:\t");
		print_binary(swap_pairs(n));
		printf("> ");
	}

	return 0;
}
