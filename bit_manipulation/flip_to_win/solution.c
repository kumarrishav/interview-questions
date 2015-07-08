
#include <stdio.h>
#include <limits.h>

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
