/* A canoe can hold 2 kids and a maximum of 150 lbs.
 * Given a list of each kid's weight, write a function that computes the minimum number of canoes
 * needed to take all the kids on a trip.
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>
#include <limits.h>
#define MAX_CANOE_WEIGHT 150
#define min(a, b) ((a) < (b) ? (a) : (b))

/* Note: this approach is very inefficient. It is O(n!), but it is a good illustration of how to
 * use recursion, and it is surely a great way to practice recursion.
 * The optimal approach is an iterative solution that runs in a much more acceptable O(n log(n))
 * time. See solution_iterative.cpp
 */

static void swap(unsigned arr[], size_t i, size_t j) {
	unsigned tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static unsigned canoes_aux(size_t N, unsigned kids[N], size_t kid_i) {
	if (kid_i >= N) {
		return 0;
	}
	if (kids[kid_i] > MAX_CANOE_WEIGHT) {
		return UINT_MAX;
	}

	unsigned res = UINT_MAX;

	size_t i;
	for (i = kid_i+1; i < N; i++) {
		if (kids[kid_i]+kids[i] > MAX_CANOE_WEIGHT) {
			continue;
		}
		swap(kids, kid_i+1, i);
		unsigned canoes_i = canoes_aux(N, kids, kid_i+2);
		if (canoes_i != UINT_MAX) {
			res = min(res, canoes_i+1);
		}
		swap(kids, kid_i+1, i);
	}

	unsigned canoes_alone = canoes_aux(N, kids, kid_i+1);
	if (canoes_alone == UINT_MAX) {
		return UINT_MAX;
	}

	return min(res, canoes_alone+1);
}

unsigned canoes(size_t N, unsigned kids[N]) {
	return canoes_aux(N, kids, 0);
}

int main(void) {
	printf("Enter the number of kids, followed by each kid's weight\n");
	printf("> ");

	size_t kids_sz;
	while (scanf("%zu", &kids_sz) == 1) {
		unsigned kids[kids_sz];

		size_t i;
		for (i = 0; i < kids_sz; i++) {
			scanf("%u", &kids[i]);
		}

		unsigned min_canoes = canoes(kids_sz, kids);
		if (min_canoes == UINT_MAX) {
			printf("Impossible\n");
		} else {
			printf("Min canoes: %u\n", min_canoes);
		}

		printf("> ");
	}

	return 0;
}
