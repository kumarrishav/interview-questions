/* You have a knapsack that can take at most a total weight of W.
 * There is a set of N items, and each item i weighs w[i] and is worth v[i].
 *
 * Write a function that computes the largest possible value that can be carried
 * in the knapsack without exceeding its capacity.
 *
 */

#include <stdio.h>

unsigned max(unsigned x, unsigned y) {
	return x > y ? x : y;
}

unsigned knapsack(unsigned weights[], unsigned values[], unsigned capacity, unsigned items) {
	if (items == 0) {
		return 0;
	}

	unsigned res = knapsack(weights, values, capacity, items-1);
	if (weights[items-1] <= capacity) {
		res = max(res, values[items-1]+knapsack(weights, values, capacity-weights[items-1], items-1));
	}

	return res;
}

static unsigned weights_buf[1024];
static unsigned values_buf[1024];

int main(void) {

	printf("Enter number of items, followed by the weights, followed by the values, followed by capacity\n");
	printf("> ");

	size_t items;
	while (scanf("%zu", &items) == 1) {
		size_t i;
		for (i = 0; i < items; i++) {
			scanf("%u", &weights_buf[i]);
		}
		for (i = 0; i < items; i++) {
			scanf("%u", &values_buf[i]);
		}
		unsigned capacity;
		scanf("%u", &capacity);
		printf("%u\n", knapsack(weights_buf, values_buf, capacity, items));
		printf("> ");
	}

	return 0;
}
