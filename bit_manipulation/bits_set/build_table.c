#include <stdio.h>

int bits_count(unsigned x) {
	int res = 0;
	for (; x != 0; res++, x &= x-1);
	return res;
}

int main(void) {
	printf("static int bits_table[] = {\n");

	unsigned i;
	for (i = 0; i < 256; i++) {
		printf("\t%d, // %d\n", bits_count(i), i);
	}

	printf("};\n");

	return 0;
}
