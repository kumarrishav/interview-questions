/* Write an algorithm to find the 'next' node (i.e. in-order successor) of a given node
 * in a binary search tree. You may assume that each node has a link to its parent.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.6
 */
#include <stdio.h>
#include <stdlib.h>

struct tree_node {
	int key;
	struct tree_node *left;
	struct tree_node *right;
};

struct lca_result {
	unsigned matched;
	struct tree_node *node;
};

struct lca_result lca_aux(struct tree_node *root, struct tree_node *n1, struct tree_node *n2) {
	struct lca_result ret = { 0, NULL };

	if (root == NULL) {
		return ret;
	}

	if (root == n1 && root == n2) {
		ret.matched = 2;
		ret.node = root;
		return ret;
	}

	struct lca_result lca_res_l = lca_aux(root->left, n1, n2);
	if (lca_res_l.matched == 2) {
		return lca_res_l;
	} else if (lca_res_l.matched == 1) {
		ret.matched++;
		ret.node = lca_res_l.node;
	}

	struct lca_result lca_res_r = lca_aux(root->right, n1, n2);
	if (lca_res_r.matched == 2) {
		return lca_res_r;
	} else if (lca_res_r.matched == 1) {
		ret.matched++;
		ret.node = lca_res_r.node;
	}

	if (ret.matched == 2) {
		ret.node = root;
	} else if (root == n1 || root == n2) {
		ret.matched++;
		ret.node = root;
	}

	return ret;
}

struct tree_node *lca(struct tree_node *root, struct tree_node *n1, struct tree_node *n2) {
	struct lca_result res = lca_aux(root, n1, n2);
	return res.node;
}

/* A driver program to perform basic testing */
#define MAX_NODES 1024
static struct tree_node tree_nodes[MAX_NODES];
static size_t nodes_in_use;

int main(void) {

	printf("Number of nodes?\n");

	scanf("%zu", &nodes_in_use);

	size_t i;
	for (i = 0; i < nodes_in_use; i++) {
		tree_nodes[i].key = i;
		printf("Node %zu:\n", i);

		printf("Left child (0 if no child): \n");
		size_t child;
		scanf("%zu", &child);
		if (child == 0) {
			tree_nodes[i].left = NULL;
		} else {
			tree_nodes[i].left = &tree_nodes[child];
		}

		printf("Right child (0 if no child): \n");
		scanf("%zu", &child);
		if (child == 0) {
			tree_nodes[i].right = NULL;
		} else {
			tree_nodes[i].right = &tree_nodes[child];
		}
	}

	printf("Enter 2 node IDs to find LCA\n");
	size_t n1, n2;
	while (scanf("%zu %zu", &n1, &n2) == 2) {
		printf("LCA = %d\n", lca(&tree_nodes[0], &tree_nodes[n1], &tree_nodes[n2])->key);
	}

	return 0;
}
