#include <stdio.h>

int divide(int dividend, int divisor) {
	int res = 0;
	unsigned mask = ~(~0U >> 1U);
	int divd = 0;

	int sign = ((dividend < 0) != (divisor < 0)) ? -1 : 1;
	if (dividend < 0) {
		dividend = -dividend;
	}
	if (divisor < 0) {
		divisor = -divisor;
	}

	while (mask != 0) {
		divd = (divd << 1) | !!(dividend & mask);
		if (divd < divisor) {
			res <<= 1;
		} else {
			res = (res << 1) | 1;
			divd -= divisor;
		}
		mask >>= 1U;
	}

	return res*sign;
}

int main(void) {
	printf("Enter dividend and divisor\n");
	printf("> ");

	int dividend, divisor;
	while (scanf("%d%d", &dividend, &divisor) == 2) {
		printf("%d\n", divide(dividend, divisor));
		printf("> ");
	}

	return 0;
}
