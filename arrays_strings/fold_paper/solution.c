/* A piece of translucid paper contains a series of consecutive numbers from 1 up to
 * (and including) 2^N.
 *
 * For example, for N = 1, the paper contains the values:
 * 1 2
 *
 * For N = 2, the paper contains the values:
 * 1 2 3 4
 *
 * And so forth.
 *
 * We are going to fold the paper N times. At each step, we can either fold left or right:
 *
 * Fold left: this folds the paper by putting the left half above the right half
 * Fold right: this folds the paper by putting the right half above the left half
 *
 * For example, if the paper currently has 1 2 3 4, then after folding left we will have
 * 2 1. Below 2 we can find 3, and below 1 we can find 4.
 *
 * If the paper currently has 1 2 3 4, then after folding right we will have 4 3. Below 4
 * we will find 1, and below 3 we will find 2.
 *
 * Since we always fold the paper N times and the paper is 2^N long, we always end up with a paper
 * of length 1 (and height 2^N), where all numbers but the bottommost sit above another number.
 *
 * Design an algorithm that prints the sequence of numbers that we will find from top to bottom
 * after folding the paper N times.
 *
 * EXAMPLE
 *
 * For N = 3, we have this paper:
 *
 * 1 2 3 4 5 6 7 8
 *
 * Input: L R L (Fold left, then right, then left)
 * Output: 5 4 1 8 7 2 3 6
 *
 * Explanation:
 * After folding left, the paper has length 4 and 2 layers. Here's a view from topmost to bottommost
 * layer:
 *
 * 4 3 2 1
 * 5 6 7 8
 *
 * After folding right, we get:
 *
 * 8 7
 * 1 2
 * 4 3
 * 5 6
 *
 * Finally, we fold left again, getting:
 *
 * 5
 * 4
 * 1
 * 8
 * 7
 * 2
 * 3
 * 6
 *
 * So, going from top to bottom, we will see the numbers 5 4 1 8 7 2 3 6
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <assert.h>

struct array_node {
	int value;
	struct array_node *up;
	struct array_node *down;
	struct array_node *top;
};

static void merge_stacks(struct array_node *below, struct array_node *above) {
	if (below->up == NULL) {
		below->up = above;
	} else {
		assert(below->down == NULL);
		below->down = above;
	}

	if (above->up == NULL) {
		above->up = below;
	} else {
		assert(above->down == NULL);
		above->down = below;
	}
}

void fold(struct array_node arr[], const char cmds[], size_t cmds_sz) {

	size_t beg = 0;
	size_t arr_sz = 1 << cmds_sz;
	size_t len = arr_sz;

	size_t i;
	for (i = 0; i < cmds_sz; i++) {
		size_t j, k;
		for (j = beg, k = beg+arr_sz-1; j < beg+arr_sz/2; j++, k--) {
			if (cmds[i] == 'L') {
				merge_stacks(arr[k].top, arr[j].top);
				arr[k].top = &arr[j];
			} else if (cmds[i] == 'R') {
				merge_stacks(arr[j].top, arr[k].top);
				arr[j].top = &arr[k];
			} else {
				assert(0);
			}
		}

		arr_sz /= 2;

		if (cmds[i] == 'L') {
			beg += arr_sz;
		}
	}

	assert(arr_sz == 1);

	struct array_node *curr = arr[beg].top;
	struct array_node *prev = NULL;
	struct array_node *next;

	for (i = 0; i < len; i++) {
		printf("%d ", curr->value);
		if (curr->up == NULL || curr->up == prev) {
			next = curr->down;
		} else {
			next = curr->up;
		}
		prev = curr;
		curr = next;
	}

	printf("\n");
}

int main(void) {
	printf("Enter N followed by the sequence of commands.\n");
	printf("> ");

	size_t n;
	while (scanf("%zu", &n) == 1) {
		struct array_node values[1 << n];

		size_t i;
		for (i = 0; i < (1 << n); i++) {
			values[i].value = i+1;
			values[i].top = &values[i];
			values[i].up = values[i].down = NULL;
		}

		char cmds[n];
		for (i = 0; i < n; i++) {
			scanf(" %c", &cmds[i]);
		}

		fold(values, cmds, n);
		printf("> ");
	}

	return 0;
}
