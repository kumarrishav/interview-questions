/* Given a sorted (increasing order) array with unique integer elements, write an
 * algorithm to create a binary search tree with minimal height.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.3
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#define MAX_ARRAY_SZ 512

static int sorted_array[MAX_ARRAY_SZ];
static size_t sorted_array_sz;

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static struct tree_node nodes_store[MAX_ARRAY_SZ];
static size_t next_node = 0;

struct tree_node *allocate_node(int value) {
	assert(next_node < sizeof(nodes_store)/sizeof(nodes_store[0]));

	nodes_store[next_node].value = value;
	nodes_store[next_node].left = nodes_store[next_node].right = NULL;

	return &nodes_store[next_node++];
}

struct tree_node *build_optimal_tree(int array[], size_t from, size_t to) {
	if (from >= to)
		return NULL;

	size_t value_i = (from+to)/2;
	struct tree_node *res = allocate_node(array[value_i]);
	res->left = build_optimal_tree(array, from, value_i);
	res->right = build_optimal_tree(array, value_i+1, to);

	return res;
}

void preorder_traversal(struct tree_node *root, unsigned depth) {
	if (root == NULL)
		return;

	char spaces[depth*4+1];
	memset(spaces, ' ', depth*4);
	spaces[depth*4] = '\0';

       	printf("%s%d\n", spaces, root->value);
	printf("%sL:\n", spaces);
	preorder_traversal(root->left, depth+1);
	printf("%sR:\n", spaces);
	preorder_traversal(root->right, depth+1);
}

int main(void) {

	printf("Enter array size, followed by array elements.\n");

	while (scanf("%zu", &sorted_array_sz) == 1) {
		size_t i;
		for (i = 0; i < sorted_array_sz; i++)
			scanf("%d", &sorted_array[i]);

		struct tree_node *root = build_optimal_tree(sorted_array, 0, sorted_array_sz);
		printf("Optimal binary search tree is:\n");
		preorder_traversal(root, 0);

		next_node = 0;
	}

	return 0;
}
