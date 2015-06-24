/* Given an infinite number of quarters (25 cents), dimes (10 cents), nickels (5 cents),
 * and pennies (1 cent), write code to calculate the number of ways of representing
 * n cents.
 *
 * Source: Cracking the Coding Interview, page 110, question 9.8
 */
#include <stdio.h>

static unsigned quantities[] = { 25, 10, 5, 1 };
static size_t quantities_sz = sizeof(quantities)/sizeof(quantities[0]);

unsigned ways_aux(size_t next_qty, int n) {
	if (n == 0) {
		return 1;
	}
	if (next_qty == quantities_sz) {
		return 0;
	}

	unsigned res = 0;
	unsigned i;
	for (i = 0; i*quantities[next_qty] <= n; i++) {
		res += ways_aux(next_qty+1, n-i*quantities[next_qty]);
	}

	return res;
}

unsigned ways(int n) {
	size_t first_qty;
	if (n >= 25) {
		first_qty = 0;
	} else if (n >= 10) {
		first_qty = 1;
	} else if (n >= 5) {
		first_qty = 2;
	} else {
		first_qty = 3;
	}
	return ways_aux(first_qty, n);
}

int main(void) {
	int n;
	printf("Enter the cents quantity\n");
	printf("> ");
	fflush(stdout);

	while (scanf("%d", &n) == 1) {
		unsigned res = ways(n);
		if (res == 1) {
			printf("There's 1 way to represent %d.\n", n);
		} else if (res == 0) {
			printf("Can't represent %d\n", res);
		} else {
			printf("There are %u ways to represent %d.\n", res, n);
		}

		printf("> ");
		fflush(stdout);
	}

	return 0;
}
