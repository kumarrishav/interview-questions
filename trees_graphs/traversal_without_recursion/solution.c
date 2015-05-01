/* Design an algorithm to perform in-order and pre-order traversal of a binary search tree
 * without recursion and O(1) memory usage.
 *
 * Source: Somewhere on the internet
 */
#include <stdio.h>
#include <stdlib.h>

struct tree_node {
	int key;
	struct tree_node *left;
	struct tree_node *right;
};

void morris_in_order(struct tree_node *root) {
	struct tree_node *curr = root;

	while (curr != NULL) {
		if (curr->left == NULL) {
			printf("%d\n", curr->key);
			curr = curr->right;
		} else {
			struct tree_node *predecessor = curr->left;

			while (predecessor->right != NULL && predecessor->right != curr) {
				predecessor = predecessor->right;
			}

			if (predecessor->right != NULL) {
				printf("%d\n", curr->key);
				predecessor->right = NULL;
				curr = curr->right;
			} else {
				predecessor->right = curr;
				curr = curr->left;
			}
		}
	}
}

void morris_pre_order(struct tree_node *root) {
	struct tree_node *curr = root;

	while (curr != NULL) {
		if (curr->left == NULL) {
			printf("%d\n", curr->key);
			curr = curr->right;
		} else {
			struct tree_node *predecessor = curr->left;

			while (predecessor->right != NULL && predecessor->right != curr) {
				predecessor = predecessor->right;
			}

			if (predecessor->right != NULL) {
				predecessor->right = NULL;
				curr = curr->right;
			} else {
				printf("%d\n", curr->key);
				predecessor->right = curr;
				curr = curr->left;
			}
		}
	}
}

/****************************************************************************************/

static struct tree_node *new_tree_node(int k, struct tree_node *left, struct tree_node *right) {

	struct tree_node *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;

	ret->key = k;
	ret->left = left;
	ret->right = right;

	return ret;
}

static void destroy_tree_node(struct tree_node *n) {
	free(n);
}

/* If it returns NULL, there was a memory error and the node couldn't be inserted
 * Otherwise, the tree's root is returned
 */
struct tree_node *tree_insert(struct tree_node *root, int k) {
	struct tree_node *prev = NULL;
	struct tree_node *curr = root;

	while (curr != NULL) {
		prev = curr;
		if (k < curr->key) {
			curr = curr->left;
		} else {
			curr = curr->right;
		}
	}

	struct tree_node *to_insert = new_tree_node(k, NULL, NULL);

	if (prev == NULL || to_insert == NULL)
		return to_insert;

	if (k < prev->key) {
		prev->left = to_insert;
	} else {
		prev->right = to_insert;
	}

	return root;
}

void destroy_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	destroy_tree(root->left);
	destroy_tree(root->right);
	destroy_tree_node(root);
}

// Main driver
static char input_buff[1024];
int main(void) {
	printf("The available commands are:\n"
	       "I K - Inserts a key with the value K\n"
	       "D - Dumps the tree (pre-order and then in-order)\n"
	       "Q - Quit\n");

	int key;
	struct tree_node *root = NULL;
	struct tree_node *new_root = NULL;

	int finish = 0;

	while (!finish) {
		printf("> ");
		fflush(stdout);

		if (fgets(input_buff, sizeof input_buff, stdin) == NULL) {
			break;
		}

		if (sscanf(input_buff, " I %d", &key) == 1) {
			new_root = tree_insert(root, key);
			if (new_root == NULL) {
				fprintf(stderr, "Out of memory; couldn't insert node.\n");
			} else {
				root = new_root;
			}
		} else {
			char op;
			if (sscanf(input_buff, " %c", &op) == 1) {
				switch (op) {
				case 'D':
					printf("Pre-order traversal:\n");
					morris_pre_order(root);
					printf("In-order traversal:\n");
					morris_in_order(root);
					break;
				case 'Q':
					finish = 1;
					break;
				default:
					fprintf(stderr, "Invalid input\n");
					break;
				}
			} else {
				fprintf(stderr, "Invalid input\n");
				break;
			}
		}
	}

	destroy_tree(root);

	return 0;
}

