/* Given a positive integer, print the next smallest and the next largest number
 * that have the same number of 1 bits in their binary representation.
 *
 * Source: Cracking the Coding Interview, page 92, question 5.3
 */
#include <stdio.h>
#include <limits.h>
#include <assert.h>

unsigned next_largest(unsigned n) {
	assert(n != 0);
	assert(n != ~0);

	unsigned ls_bit = n & ~(n-1);
	unsigned first_half = n+ls_bit;

	// n can't be 11111111...00000000
	assert(first_half != 0);

	unsigned ms_bit = (first_half) & ~(first_half-1);
	unsigned rest = ((ms_bit/ls_bit)-1) >> 1;

	return first_half | rest;
}

unsigned next_smallest(unsigned n) {
	assert(n != 0);

	unsigned no_trailing_grp = n & (n+1);

	// n can't be 00000000...111111111
	assert(no_trailing_grp != 0);

	unsigned has_trailing_grp = (no_trailing_grp != n);
	unsigned ls_bit = no_trailing_grp & ~(no_trailing_grp-1);
	ls_bit >>= 1;
	no_trailing_grp &= no_trailing_grp-1;
	no_trailing_grp |= ls_bit;

	if (!has_trailing_grp) {
		return no_trailing_grp;
	}

	unsigned ms_bit_trailing_grp = (n+1) & ~n;

/* Doesn't work on a boundary condition - TODO Write about this
	unsigned r_lim = (ls_bit/ms_bit_trailing_grp) >> 1;
	unsigned limits = ls_bit | r_lim;
	unsigned shifted_grp = (~limits) & ~(r_lim-1);
	shifted_grp &= ls_bit-1;

*/

	unsigned r_lim = ls_bit/ms_bit_trailing_grp;
	unsigned limits = ls_bit | (r_lim >> 1);
	unsigned shifted_grp = (~limits) | r_lim;
	shifted_grp &= ~(r_lim-1);
	shifted_grp &= ls_bit-1;

	return no_trailing_grp | shifted_grp;
}

void print_binary(unsigned n) {
	size_t unsigned_bits = sizeof(unsigned)*CHAR_BIT;
	unsigned i;
	for (i = 0; i < unsigned_bits; i++) {
		printf("%c", (n & (1 << (unsigned_bits-i-1))) ? '1' : '0');
	}
}

int main(void) {
	printf("Enter a number\n");

	unsigned n;

	while (scanf("%u", &n) == 1) {
		printf("Received:\t");
		print_binary(n);
		printf("\nNext smallest:\t");
		print_binary(next_smallest(n));
		printf("\nNext largest:\t");
		print_binary(next_largest(n));
		printf("\n");
	}

	return 0;
}
