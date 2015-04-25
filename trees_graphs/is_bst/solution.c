/* Implement a function to check if a binary tree is a binary search tree
 *
 * Source: Cracking the Coding Interview, page 86, question 4.5
 */
#include <stdio.h>
#include <limits.h>

struct tree_node {
	int key;
	struct tree_node *left;
	struct tree_node *right;
};

static int is_bst_aux(struct tree_node *root, int min, int max) {
	if (root == NULL) {
		return 1;
	}

	if (!(min < root->key && root->key < max)) {
		return 0;
	}

	if (!is_bst_aux(root->left, min, root->key)) {
		return 0;
	}

	return is_bst_aux(root->right, root->key, max);
}

int is_bst(struct tree_node *root) {
	return is_bst_aux(root, INT_MIN, INT_MAX);
}

#define MAX_NODES 1024
static struct tree_node tree_nodes[MAX_NODES];
static size_t nodes_in_use;

int main(void) {

	printf("Input format:\n");
	printf("<number of nodes>\n");
	printf("<node0 value> <number of children of node0> <child1> <child2>\n");
	printf("<node1 value> <number of children of node1> <child1> <child2>\n");
	printf("node IDs go from 0 to <number of nodes>-1\n");
	printf("The root is node 0\n");

	scanf("%zu", &nodes_in_use);

	size_t i;
	for (i = 0; i < nodes_in_use; i++) {

		scanf("%d", &tree_nodes[i].key);

		size_t children;
		scanf("%zu", &children);

		size_t child;
		if (children > 2)
			children = 2;

		if (children > 0) {
			scanf("%zu", &child);
			tree_nodes[i].left = &tree_nodes[child];
		}

		if (children > 1) {
			scanf("%zu", &child);
			tree_nodes[i].right = &tree_nodes[child];
		}

	}

        printf("Is BST: %s\n", is_bst(&tree_nodes[0]) ? "Yes" : "No");

	return 0;
}

