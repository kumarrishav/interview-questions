/* You are given an array with the result of the in-order traversal of a tree.
 * Write a method that calculates the number of trees that yield the same array
 * for an in-order traversal.
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

static unsigned possible_trees_aux(size_t begin, size_t end) {
	if (begin >= end) {
		return 1;
	}

	unsigned res = 0;
	size_t i;
	for (i = begin; i < end; i++) {
		res += possible_trees_aux(begin, i)*possible_trees_aux(i+1, end);
	}

	return res;
}

unsigned possible_trees(size_t tree_size) {
	return possible_trees_aux(0, tree_size);
}

int main(void) {
	printf("Enter a tree size N to see how many trees exist such that\n"
	       "the inorder traversal yields [0, 1, ... N-1]\n");
	printf("> ");

	size_t tree_sz;
	while (scanf("%zu", &tree_sz) == 1) {
		printf("%u\n", possible_trees(tree_sz));
		printf("> ");
	}

	return 0;
}
