/* Write code to determine if an integer is a power of 2
 *
 * Source: Adapted from Hacker's Delight chapter 1
 */

#include <stdio.h>

int is_power_two(unsigned n) {
	return (n & (n-1)) == 0;
}

int main(void) {
	printf("Enter an unsigned number to check if it's a power of 2.\n");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		if (is_power_two(n)) {
			printf("%u is a power of 2.\n", n);
		} else {
			printf("%u is not a power of 2.\n", n);
		}
	}

	return 0;
}
