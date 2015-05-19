
#include <stdio.h>
#include <limits.h>
#define MAX_CANOE_WEIGHT 150
#define min(a, b) ((a) < (b) ? (a) : (b))

static void swap(unsigned arr[], size_t i, size_t j) {
	unsigned tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static unsigned canoes_aux(size_t N, unsigned kids[N], size_t kid_i) {
	if (kid_i >= N || kids[kid_i] > MAX_CANOE_WEIGHT) {
		return 0;
	}

	unsigned res = UINT_MAX;

	size_t i;
	for (i = kid_i+1; i < N; i++) {
		if (kids[kid_i]+kids[i] > MAX_CANOE_WEIGHT) {
			continue;
		}
		swap(kids, kid_i+1, i);
		unsigned canoes_i = canoes_aux(N, kids, kid_i+2);
		res = min(res, canoes_i+1);
		swap(kids, kid_i+1, i);
	}

	unsigned canoes_alone = canoes_aux(N, kids, kid_i+1);
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
