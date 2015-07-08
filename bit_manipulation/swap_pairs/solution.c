
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

	return (0U | ((n & odd_mask) >> 1U)) | ((n & (odd_mask >> 1U)) << 1U);
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
