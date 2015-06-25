/* You are given a piece of wood of length N, and an array of N elements, where A[i] stores the
 * price of a piece of wood of length i+1 (0 <= i < N).
 *
 * Write a function that finds the optimal cutting sequence in order to maximize profit.
 *
 */

#include <stdio.h>
#include <assert.h>

/* Naive approach */

int max(int a, int b) {
	return a > b ? a : b;
}

int cut_rod(int prices[], ssize_t rod_len) {
	if (rod_len <= 0) {
		return 0;
	}

	int res = prices[0]+cut_rod(prices, rod_len-1);
	ssize_t i;
	for (i = 1; i < rod_len; i++) {
		res = max(res, prices[i]+cut_rod(prices, rod_len-i-1));
	}

	return res;
}

static int prices_buff[1024];

int main(void) {
	printf("Enter size of rod followed by the prices array.\n");
	printf("> ");

	size_t rod_len;

	while (scanf("%zu", &rod_len) == 1) {
		size_t i;
		for (i = 0; i < rod_len; i++) {
			scanf("%d", &prices_buff[i]);
		}

		printf("Best value = %d\n", cut_rod(prices_buff, rod_len));
		printf("> ");
	}

	return 0;
}
