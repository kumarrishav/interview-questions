#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_TREE_SZ 1024

struct tree_node {
	int key;
	struct tree_node *parent; // Needed to support successor() and predecessor()
	struct tree_node *left;
	struct tree_node *right;
};

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

struct tree_node *tree_minimum(struct tree_node *root) {
	if (root == NULL) {
		return NULL;
	}

	for (; root->left != NULL; root = root->left); // Empty loop body

	return root;
}

struct tree_node *tree_maximum(struct tree_node *root) {
	if (root == NULL) {
		return NULL;
	}

	for (; root->right != NULL; root = root->right); // Empty loop body

	return root;
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

/* Replaces subtree rooted at `u` with subtree rooted at `v`, updating parent pointers
 * `v` can be NULL, but `u` can't
 */
static void transplant(struct tree_node *u, struct tree_node *v) {
	if (u->parent != NULL) {
		if (u == u->parent->left) {
			u->parent->left = v;
		} else {
			u->parent->right = v;
		}
	}

	if (v != NULL) {
		v->parent = u->parent;
	}
}

static struct tree_node *tree_delete_internal(struct tree_node *root, struct tree_node *to_del) {
	struct tree_node *newroot = root;

	if (to_del->left == NULL) {
		transplant(to_del, to_del->right);
		if (to_del->parent == NULL) {
			newroot = to_del->right;
		}
	} else if (to_del->right == NULL) {
		transplant(to_del, to_del->left);
		if (to_del->parent == NULL) {
			newroot = to_del->left;
		}
	} else {
		struct tree_node *successor = tree_minimum(to_del->right);
		if (successor == to_del->right) {
			successor->left = to_del->left;
			transplant(to_del, successor);
		} else {
			transplant(successor, successor->right);
			successor->right = to_del->right;
			successor->left = to_del->left;
			successor->left->parent = successor->right->parent = successor;
			transplant(to_del, successor);
		}
		if (successor->parent == NULL) {
			newroot = successor;
		}
	}

	destroy_tree_node(to_del);

	return newroot;
}

struct tree_node *tree_delete(struct tree_node *root, int k) {
	struct tree_node *to_delete = tree_find(root, k);

	if (to_delete == NULL) {
		return root;
	}

	return tree_delete_internal(root, to_delete);
}

struct tree_node *tree_successor(struct tree_node *root) {
	if (root == NULL) {
		return NULL;
	}

	if (root->right != NULL) {
		return tree_minimum(root->right);
	}

	struct tree_node *p = root->parent;
	while (p != NULL && root != p->left) {
		root = p;
		p = root->parent;
	}

	return p;
}

struct tree_node *tree_predecessor(struct tree_node *root) {
	if (root == NULL) {
		return NULL;
	}

	if (root->left != NULL) {
		return tree_maximum(root->left);
	}

	struct tree_node *p = root->parent;
	while (p != NULL && root != p->right) {
		root = p;
		p = root->parent;
	}

	return p;
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
	       "R K - Removes the key with value K\n"
	       "F K - Finds the entry with value K\n"
	       "P K - Searches for the predecessor of K\n"
	       "S K - Searches for the successor of K\n"
	       "H K - Finds the maximum key of the subtree rooted at K\n"
	       "L K - Finds the minimum key of the subtree rooted at K\n"
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
		} else if (sscanf(input_buff, " R %d", &key) == 1) {
			root = tree_delete(root, key);
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
		} else if (sscanf(input_buff, " P %d", &key) == 1) {
			struct tree_node *n1 = tree_find(root, key);
			if (n1 == NULL) {
				fprintf(stderr, "No such key: %d\n", key);
				continue;
			}
			struct tree_node *n2 = tree_predecessor(n1);
			if (n2 == NULL) {
				printf("Node %d has no predecessor.\n", n1->key);
			} else {
				printf("Predecessor of %d is %d\n", n1->key, n2->key);
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
		} else if (sscanf(input_buff, " H %d", &key) == 1) {
			struct tree_node *n1 = tree_find(root, key);
			if (n1 == NULL) {
				fprintf(stderr, "No such key: %d\n", key);
				continue;
			}
			struct tree_node *n2 = tree_maximum(n1);
			printf("Subtree rooted at %d has maximum = %d\n", n1->key, n2->key);
		} else if (sscanf(input_buff, " L %d", &key) == 1) {
			struct tree_node *n1 = tree_find(root, key);
			if (n1 == NULL) {
				fprintf(stderr, "No such key: %d\n", key);
				continue;
			}
			struct tree_node *n2 = tree_minimum(n1);
			printf("Subtree rooted at %d has minimum = %d\n", n1->key, n2->key);
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
