

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
