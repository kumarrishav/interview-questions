/* You are given two 32-bit numbers, N and M, and two bit positions, i and j. Write
 * a method to insert M into N such that M starts at bit j and ends at bit i. You can
 * assume that the bits j through i have enough space to fit all of M. That is, if
 * M = 10011, you can assume that there are at least 5 bits between j and i. You
 * would not, for example, have j = 3 and i = 2, because M could not fully fit
 * between bit 3 and bit 2.
 * EXAMPLE
 * Input: N = 10000000000, M = 10011, i = 2, j = 6
 * Output: N = 10001001100
 *
 * Source: Cracking the Coding Interview, page 91, question 5.1
 */

#include <stdio.h>

void print_binary_rep(double n) {
	static size_t max_bits = 32;
	// \0 Terminator + "0."
	char buff[max_bits+3];

	size_t cursor = 0;
	cursor += sprintf(buff, "0.");

	while (n != 0 && cursor < 34) {
		double new_n = n*2;
		int new_n_as_int = new_n;
		buff[cursor++] = (new_n_as_int == 1 ? '1' : '0');
		n = new_n-new_n_as_int;
	}

	if (n != 0) {
		sprintf(buff, "ERROR");
	} else {
		buff[cursor] = '\0';
	}

	printf("%s\n", buff);
}

int main(void) {
	printf("Enter a real number in the range ]0, 1[\n");

	double n;
	while (scanf("%lf", &n) == 1) {
		print_binary_rep(n);
	}

	return 0;
}
