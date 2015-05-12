/* You are given printouts from an algorithm which ran over an
 * unsorted binary tree. One printout is from an in-order run and
 * another from a pre-order run. Can you reconstruct the tree?
 * If so, then write and implement an algorithm.
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static size_t array_search(size_t n, int arr[n], size_t b, size_t e, int val) {
	size_t i;
	for (i = b; i < e; i++) {
		if (arr[i] == val) {
			return i;
		}
	}
	return e;
}

static struct tree_node *rebuild_tree_aux(size_t io_sz, int in_order[io_sz],
					  size_t po_sz, int pre_order[po_sz],
					  size_t io_begin, size_t io_end,
					  size_t po_begin, size_t po_end) {
	if (io_begin == io_end) {
		assert(po_begin == po_end);
		return NULL;
	}

	struct tree_node *root = malloc(sizeof(*root));
	assert(root != NULL);
	root->value = pre_order[po_begin];

	size_t i = array_search(io_sz, in_order, io_begin, io_end, pre_order[po_begin]);
	assert(i != io_end);

	size_t left_subtree_sz = i - io_begin;

	root->left = rebuild_tree_aux(io_sz, in_order, po_sz, pre_order,
				      io_begin, i, po_begin+1, po_begin+1+left_subtree_sz);
	root->right = rebuild_tree_aux(io_sz, in_order, po_sz, pre_order,
				       i+1, io_end, po_begin+1+left_subtree_sz, po_end);

	return root;
}

struct tree_node *rebuild_tree(size_t io_sz, int in_order[io_sz],
			       size_t po_sz, int pre_order[po_sz]) {
	return rebuild_tree_aux(io_sz, in_order, po_sz, pre_order, 0, io_sz, 0, po_sz);
}

static void destroy_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	destroy_tree(root->left);
	destroy_tree(root->right);
	free(root);

}

static void read_array(size_t n, int arr[n]) {
	size_t i;
	for (i = 0; i < n; i++) {
		scanf("%d", &arr[i]);
	}
}

static void print_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	printf("%d -> ", root->value);
	if (root->left != NULL) {
		printf("%d ", root->left->value);
	} else {
		printf("N/A ");
	}
	if (root->right != NULL) {
		printf("%d\n", root->right->value);
	} else {
		printf("N/A\n");
	}

	print_tree(root->left);
	print_tree(root->right);
}

int main(void) {
	printf("Enter the size of the tree, followed by the in-order traversal values,\n"
	       "followed by the pre-oder traversal values (space separated)\n");
	printf("> ");

	size_t tree_sz;
	while (scanf("%zu", &tree_sz) == 1) {
		int in_order[tree_sz];
		int pre_order[tree_sz];

		read_array(tree_sz, in_order);
		read_array(tree_sz, pre_order);

		struct tree_node *root = rebuild_tree(tree_sz, in_order, tree_sz, pre_order);
		print_tree(root);
		destroy_tree(root);

		printf("> ");
	}

	return 0;
}
