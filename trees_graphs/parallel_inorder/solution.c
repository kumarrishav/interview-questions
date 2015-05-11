
#include <stdio.h>

struct tree_node {
	int val;
	struct tree_node *left;
	struct tree_node *right;
};

static struct tree_node *next_node(struct tree_node *curr_node, struct tree_node **iterator,
				   int in_progress) {

	struct tree_node *curr;

	if (!in_progress && *iterator == NULL) {
		curr = curr_node;
	} else {
		curr = *iterator;
	}

	while (curr != NULL) {
		if (curr->left == NULL) {
			*iterator = curr->right;
			return curr;
		} else {
			struct tree_node *predecessor = curr->left;
			while (predecessor->right != NULL && predecessor->right != curr) {
				predecessor = predecessor->right;
			}

			if (predecessor->right == NULL) {
				predecessor->right = curr;
				curr = curr->left;
			} else {
				// Restore
				predecessor->right = NULL;
				*iterator = curr->right;
				return curr;
			}
		}
	}

	return NULL;
}

int same_inorder(struct tree_node *root1, struct tree_node *root2) {
	struct tree_node *iter_1 = NULL;
	struct tree_node *iter_2 = NULL;

	root1 = next_node(root1, &iter_1, 0);
	root2 = next_node(root2, &iter_2, 0);

	while (root1 != NULL && root2 != NULL) {
		if (root1->val != root2->val) {
			return 0;
		}
		root1 = next_node(root1, &iter_1, 1);
		root2 = next_node(root2, &iter_2, 1);
	}

	return root1 == NULL && root2 == NULL;
}

static struct tree_node tree_1[32];
static struct tree_node tree_2[32];

static struct tree_node *read_tree(struct tree_node nodes[], unsigned count) {
	unsigned i;
	unsigned root;
	for (i = 0; i < count; i++) {
		unsigned n;
		int l, r;
		scanf("%u%d%d", &n, &l, &r);
		nodes[n].val = n;
		nodes[n].left = NULL;
		nodes[n].right = NULL;
		
		if (i == 0) {
			root = n;
		}
		
		if (l != -1) {
			nodes[n].left = &nodes[l];
		}
		if (r != -1) {
			nodes[n].right = &nodes[r];
		}
	}

	return &nodes[root];
}

int main(void) {
	printf("Enter number of nodes in tree 1 followed by number of nodes in tree 2,\n"
	       "followed by tree 1 followed by tree 2.\n"
	       "For each node, enter: nodeID leftChild rightChild (-1 means no child)\n");
	printf("> ");

	unsigned nodes_a, nodes_b;
	while (scanf("%u%u", &nodes_a, &nodes_b) == 2) {
		struct tree_node *root1 = read_tree(tree_1, nodes_a);
		struct tree_node *root2 = read_tree(tree_2, nodes_b);

		if (same_inorder(root1, root2)) {
			printf("Inorder traversal is equal\n");
		} else {
			printf("Inorder traversal is not equal\n");
		}
		printf("> ");
	}

	return 0;
}
