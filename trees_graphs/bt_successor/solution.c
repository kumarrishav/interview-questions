/* Write an algorithm to find the 'next' node (i.e. in-order successor) of a given node
 * in a binary search tree. You may assume that each node has a link to its parent.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.6
 */
#include <stdio.h>
#include <stdlib.h>

struct tree_node {
	int key;
	struct tree_node *parent;
	struct tree_node *left;
	struct tree_node *right;
};

struct tree_node *tree_minimum(struct tree_node *node) {
	if (node == NULL) {
		return NULL;
	}

	while (node->left != NULL) {
		node = node->left;
	}

	return node;
}

struct tree_node *tree_successor(struct tree_node *node) {
	if (node == NULL) {
		return NULL;
	}

	if (node->right != NULL) {
		return tree_minimum(node->right);
	}

	struct tree_node *p = node->parent;
	while (p != NULL && node != p->left) {
		node = p;
		p = p->parent;
	}

	return p;
}

/* A driver program to perform basic testing */
#define MAX_TREE_SZ 1024

static struct tree_node *new_tree_node(int k, struct tree_node *left, struct tree_node *right,
				struct tree_node *parent) {

	struct tree_node *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;

	ret->key = k;
	ret->left = left;
	ret->right = right;
	ret->parent = parent;

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

	if (prev == NULL)
		return new_tree_node(k, NULL, NULL, NULL);

	struct tree_node *to_insert = new_tree_node(k, NULL, NULL, prev);
	if (to_insert == NULL) {
		return NULL;
	}

	if (k < prev->key) {
		prev->left = to_insert;
	} else {
		prev->right = to_insert;
	}

	return root;
}

struct tree_node *tree_find(struct tree_node *root, int k) {

	while (root != NULL && root->key != k) {
		if (k < root->key) {
			root = root->left;
		} else {
			root = root->right;
		}
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

void pre_order_traversal(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	printf("*** Entered %d\n", root->key);
	printf("    L:\n");
	pre_order_traversal(root->left);
	printf("    R:\n");
	pre_order_traversal(root->right);
	printf("*** Leaving %d\n", root->key);
}

// Main driver
static char input_buff[1024];
int main(void) {
	printf("The available commands are:\n"
	       "I K - Inserts a key with the value K\n"
	       "F K - Finds the entry with value K\n"
	       "S K - Searches for the successor of K\n"
	       "D - Dumps the tree (pre-order traversal)\n"
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
		} else if (sscanf(input_buff, " F %d", &key) == 1) {
			struct tree_node *n = tree_find(root, key);
			if (n == NULL) {
				printf("No such key: %d\n", key);
			} else {
				printf("Found key %d. Left = ", key);

				if (n->left != NULL) {
					printf("%d; ", n->left->key);
				} else {
					printf("N/A; ");
				}

				printf("Right = ");

				if (n->right != NULL) {
					printf("%d; ", n->right->key);
				} else {
					printf("N/A; ");
				}

				printf("Parent = ");

				if (n->parent != NULL) {
					printf("%d; ", n->parent->key);
				} else {
					printf("N/A; ");
				}

				printf("\n");
			}
		} else if (sscanf(input_buff, " S %d", &key) == 1) {
			struct tree_node *n1 = tree_find(root, key);
			if (n1 == NULL) {
				fprintf(stderr, "No such key: %d\n", key);
				continue;
			}
			struct tree_node *n2 = tree_successor(n1);
			if (n2 == NULL) {
				printf("Node %d has no successor.\n", n1->key);
			} else {
				printf("Successor of %d is %d\n", n1->key, n2->key);
			}
		} else {
			char op;
			if (sscanf(input_buff, " %c", &op) == 1) {
				switch (op) {
				case 'D':
					pre_order_traversal(root);
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
