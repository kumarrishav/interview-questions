
#include <stdio.h>
#include <stdlib.h>

struct tree_node {
	int value;
	unsigned occurrences;
	unsigned left_children;
	struct tree_node *left;
	struct tree_node *right;
};

static struct tree_node *tree_find(struct tree_node *root, unsigned *lower_nodes, int value) {
	*lower_nodes = 0;

	while (root != NULL && root->value != value) {
		if (value < root->value) {
			root = root->left;
		} else {
			*lower_nodes += root->left_children+root->occurrences;
			root = root->right;
		}
	}

	return root;
}

static struct tree_node *tree_insert(struct tree_node *root, int value) {
	struct tree_node *prev = NULL;
	struct tree_node *curr = root;

	while (curr != NULL) {
		prev = curr;
		if (value == curr->value) {
			curr->occurrences++;
			return root;
		}
		if (value < curr->value) {
			curr->left_children++;
			curr = curr->left;
		} else {
			curr = curr->right;
		}
	}

	struct tree_node *new_node = malloc(sizeof(*new_node));
	new_node->value = value;
	new_node->occurrences = 1;
	new_node->left_children = 0;
	new_node->left = new_node->right = NULL;

	if (prev == NULL) {
		return new_node;
	}

	if (value < prev->value) {
		prev->left = new_node;
	} else {
		prev->right = new_node;
	}

	return root;
}

static void print_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}
	print_tree(root->left);
	print_tree(root->right);
}

static void destroy_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	destroy_tree(root->left);
	destroy_tree(root->right);
	free(root);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static struct tree_node *root = NULL;

void track(int x) {
	root = tree_insert(root, x);
}

int rank(int x) {
	unsigned less_than;
	struct tree_node *n = tree_find(root, &less_than, x);

	if (n == NULL) {
		return -1;
	}

	return less_than+n->left_children+n->occurrences-1;
}

int main(void) {
	printf("Available commands:\n"
	       "I n - Insert n into the stream.\n"
	       "R n - Get the rank of n.\n");

	while (1) {
		int val;
		if (scanf(" I %d", &val) == 1) {
			track(val);
		} else if (scanf(" R %d", &val) == 1) {
			print_tree(root);
			int idx = rank(val);
			if (idx == -1) {
				printf("%d never appeared in the stream.\n", val);
			} else {
				printf("rank(%d) = %d\n", val, idx);
			}
		} else {
			break;
		}
	}

	destroy_tree(root);
	return 0;
}
