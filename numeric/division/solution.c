/* Write a function which, given two integers (a numerator and a denominator), prints the first
 * N digits of a rational number. For example, for 5 / 3 with N=5, the result should be "1.66666".
 * With N = 2, the result is 1.66
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

void divide(int numerator, int denominator, int n) {
	printf("%d/%d = %d.", numerator, denominator, numerator/denominator);

	int left = (numerator%denominator)*10;
	while (n--) {
		printf("%d", left/denominator);
		left = (left%denominator)*10;
	}

	printf("\n");
}

int main(void) {
	divide(5, 3, 5);
	divide(5, 3, 2);
	divide(5, 3, 1);
	divide(5, 3, 0);

	divide(60, 7, 3);
	divide(60, 7, 5);
	divide(613, 9, 8);

	return 0;
}
