/* You have an integer and you can flip exactly one bit from a 0 to a 1. Write code to
 * find the length of the longest sequence of 1s you could create
 *
 * EXAMPLE
 *
 * Input: 1775 (11011101111)
 * Output: 8
 *
 * Source: Cracking the Coding Interview, 6th Edition, Chapter 5, Exercise 5.3
 */
#include <stdio.h>
#include <limits.h>

/*
 * This works by trying to flip each 0 in turn and counting the size of the resulting group
 * If our input is N, then ~N results in an integer that contains 1s only in the positions
 * that we can flip. Call that integer M. At each step, we get the next bit to flip with
 * M & -M (this gives us a number with only 1 bit set that corresponds to the LSB in M),
 * and then we count the number of adjacent 1s in N to the left and right, and update the
 * result if necessary. We always store the group count to the right, and use the left
 * group count as the right group count of the next iteration. Also, we update M by doing
 * M &= M-1 to pass to the next bit that we can flip.
 *
 * This runs in O(b) time and O(1) memory, where b is the number of bits in an integer, and,
 * quite frankly, this is considerably more compact and elegant than the optimal solution that
 * the book suggests.
 *
 */

unsigned flip_to_win(unsigned n) {
	unsigned m = ~n;

	if (m == 0) {
		return sizeof(n)*CHAR_BIT;
	}

	unsigned right, left, mask;
	right = left = 0;
	for (mask = (m&-m) >> 1U; mask != 0; right++, mask >>= 1U)
		; /* Intentionally left blank */

	unsigned res = 0;
	while (m != 0) {
		left = 0;
		for (mask = (m&-m) << 1U; mask != 0 && (n & mask); left++, mask <<= 1U)
			; /* Intentionally left blank */
		if (left+right+1 > res) {
			res = left+right+1;
		}

		right = left;
		m &= m-1;
	}

	return res;
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
		print_binary(n);
		printf("%u\n> ", flip_to_win(n));
	}

	return 0;
}
