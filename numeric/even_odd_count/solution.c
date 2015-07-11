/* Write code to determine how many numbers between 1 and 100000 contain an even number of digits
 *
 * Source: Careercup (Microsoft interview)
 */

#include <stdio.h>
#define N 100000

unsigned even_odd_digits(void) {
	static unsigned odd_digits[N+1];

	unsigned res = 0;
	size_t i;
	for (i = 0; i < N+1 && i < 10; i++) {
		odd_digits[i] = i & 1;
		res += !odd_digits[i];
	}

	size_t mult;
	for (mult = 10; mult < N+1; mult *= 10) {
		for (i = mult; i < N+1 && i < mult*10; i++) {
			odd_digits[i] = odd_digits[i%mult] + odd_digits[i/mult];
			res += !(odd_digits[i] & 1U);
		}
	}

	return res-1;
}

unsigned even_odd_digits_naive(void) {
	unsigned res = 0;
	size_t i;
	for (i = 1; i < N+1; i++) {
		size_t j = i;
		unsigned odd_digits = 0;
		while (j != 0) {
			if ((j%10) & 1) {
				odd_digits++;
			}
			j /= 10;
		}
		if (!(odd_digits & 1)) {
			res++;
		}
	}

	return res;
}

int main(void) {
	printf("Naive: %u\n", even_odd_digits_naive());
	printf("Lookup table: %u\n", even_odd_digits());
	return 0;
}
