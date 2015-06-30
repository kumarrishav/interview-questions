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

/* A naive approach to this problem would be to have an array of stacks and update the necessary
 * stacks on each folding operation. The problem is that each fold requires us to reverse half of
 * the stacks at each step, or, to put it another way, pop everything from half of the stacks
 * and push it again on the other half.
 *
 * When processing the i-th fold, each stack has 2^i elements, and we would have to pop
 * 2^i elements out of 2^N/2^(i+1) stacks and push them back into the other 2^N/2^(i+1) stacks.
 * So for each step we pop half of the total elements and push them again.
 *
 * For an array of size M, this amounts to O(M log(M)), since there are log(M) steps, and each step
 * does O(M/2 + M/2) work (pop + push).
 *
 * This code goes a little further and optimizes the stack merging step by doing it in O(1). To do
 * so, it has each stack element point to two elements - the node below and the node underneath.
 * These pointers are not always accurante: because we don't actually reverse the stacks when
 * merging, traversing a stack from top to bottom is not as easy as following pointers to the
 * element that comes next, since a merge operation effectively turns a stack upside down
 * (so the element below is now the element above, and vice versa). We just have to be careful
 * with this when traversing the stack by following the link that does NOT takes us to the previous
 * node we just came from.
 *
 * Also, each stack base (the bottommost node, that never changes) in the array keeps a pointer to
 * the current top of the stack whose bottom is that node. This ensures that we can find the top
 * of a stack in O(1) given any node in the array, and merge them in O(1).
 *
 * This optimization reduces the runtime down to O(M) at the cost of a little memory overhead to
 * store the pointers.
 *
 * Note that a naive complexity analysis would argue that the runtime is still O(M log(M)), because
 * at each step we do at most M/2 merges and there are log(M) steps. This is wrong though: O(1)
 * merges amortize the total runtime to O(M/2) because the stack sizes double on each step.
 *
 * So, step 1 does M/2 merges, step 2 does M/4, step 3 does M/8:
 *
 * M/2 + M/4 + M/8 + M/16 + ...
 *
 * Which is bounded by 2M and thus the algorithm is O(M).
 *
 * Note, however, that M = 2^N, but we need to at least represent the paper elements to move them
 * around, so this is probably the best asymptotic complexity we can get.
 */

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
