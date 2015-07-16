/* Write a function that takes the root of a binary search tree and flattens it into a new binary
 * search tree such that each node has at most one right child and no left child.
 *
 * EXAMPLE
 *
 * This tree:
 *
 *     10
 *    /  \
 *   5    15
 *  /
 * 1
 *
 * Gets converted into:
 *
 * 1
 *  \
 *   5
 *    \
 *    10
 *      \
 *      15
 *
 * Source: Careercup (Microsoft interview)
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

void flatten_bst_aux(struct tree_node *curr_node, struct tree_node **last_visited) {
	if (curr_node == NULL) {
		return;
	}

	flatten_bst_aux(curr_node->left, last_visited);
	(*last_visited)->right = curr_node;
	*last_visited = curr_node;
	curr_node->left = NULL;
	flatten_bst_aux(curr_node->right, last_visited);
}

struct tree_node *flatten_bst(struct tree_node *root) {
	struct tree_node sentinel;
	struct tree_node *last = &sentinel;
	flatten_bst_aux(root, &last);
	last->left = last->right = NULL;
	return sentinel.right;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Tree deserialization library
 * See trees_graphs/serialize_tree to learn it
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

static size_t find_inorder(int inorder[], size_t len, int value) {
	size_t l = 0;
	size_t r = len-1;
	size_t mid;
	while (l <= r) {
		mid = l+(r-l)/2;
		if (inorder[mid] == value) {
			return mid;
		} else if (inorder[mid] < value) {
			l = mid+1;
		} else {
			r = mid-1;
		}
	}

	assert(0);
	return -1;
}

static struct tree_node *deserialize_internal(int preorder[], int inorder[], size_t len) {

	if (len == 0) {
		return NULL;
	}

	struct tree_node *ret = malloc(sizeof(*ret));
	assert(ret != NULL);

	ret->value = preorder[0];
	size_t root_io = find_inorder(inorder, len, preorder[0]);

	ret->left = deserialize_internal(&preorder[1], &inorder[0], root_io);
	ret->right = deserialize_internal(&preorder[root_io+1], &inorder[root_io+1], len-root_io-1);

	return ret;
}

struct tree_node *deserialize(size_t nodes) {
	static int buff[4096];
	int val;
	size_t i;
	for (i = 0; i < nodes*2; i++) {
		scanf("%d", &val);
		assert(i < sizeof(buff)/sizeof(buff[0]));
		buff[i] = val;
	}
	return deserialize_internal(&buff[0], &buff[i/2], i/2);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void destroy_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}
	destroy_tree(root->left);
	destroy_tree(root->right);
	free(root);
}

static void right_traverse(struct tree_node *root) {
	while (root != NULL) {
		printf("%d ", root->value);
		root = root->right;
	}
}

int main(void) {
	printf("Enter the number of nodes in the BST, followed by the preorder traversal, followed by the inorder traversal\n");
	printf("> ");

	size_t nodes;
	while (scanf("%zu", &nodes) == 1) {
		struct tree_node *root = deserialize(nodes);
		struct tree_node *flattenned = flatten_bst(root);

		printf("BST is now flattenned, traversing through right links only:\n");
		right_traverse(flattenned);
		printf("\n> ");
		destroy_tree(root);
	}

	return 0;
}
