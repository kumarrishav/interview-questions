/* You have two very large binary trees: T1, with millions of nodes, and T2, with
 * hundreds of nodes. Create an algorithm to decide if T2 is a subtree of T1.
 *
 * A tree T2 is a subtree of T1 if there exists a node n in T1 such that the subtree of
 * n is identical to T2. That is, if you cut off the tree at node n, the two trees would
 * be identical.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.8
 */
#include <stdio.h>
#include <stdlib.h>

struct tree_node {
	int key;
	struct tree_node *left;
	struct tree_node *right;
};

int subtree_equal(struct tree_node *t1, struct tree_node *t2) {

	if ((t1 == NULL) != (t2 == NULL)) {
		return 0;
	}

	if (t1 == NULL && t2 == NULL) {
		return 1;
	}

	return t1->key == t2->key &&
		subtree_equal(t1->left, t2->left) &&
		subtree_equal(t1->right, t2->right);
}

int is_subtree(struct tree_node *big_tree, struct tree_node *small) {

	if (big_tree == NULL) {
		return small == NULL;
	}

	return subtree_equal(big_tree, small) ||
		is_subtree(big_tree->left, small) ||
		is_subtree(big_tree->right, small);
}

/* A driver program to perform basic testing */
#define MAX_NODES 1024
static struct tree_node tree1_nodes[MAX_NODES];
static size_t tree1_nodes_sz;

static struct tree_node tree2_nodes[MAX_NODES];
static size_t tree2_nodes_sz;

struct tree_node *read_tree(struct tree_node *buff, size_t *cursor) {
	size_t nodes, i;
	size_t root = *cursor;

	scanf("%zu", &nodes);
	if (nodes == 0) {
		return NULL;
	}

	for (i = 0; i < nodes; i++) {
		int val;
		size_t left, right;
		scanf("%d %zu %zu", &val, &left, &right);
		buff[*cursor].key = val;
		if (left == 0) {
			buff[*cursor].left = NULL;
		} else {
			buff[*cursor].left = &buff[left];
		}
		if (right == 0) {
			buff[*cursor].right = NULL;
		} else {
			buff[*cursor].right = &buff[right];
		}
		(*cursor)++;
	}

	return &buff[root];
}

void print_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	printf("Entered: %d\n", root->key);
	printf("L:\n");
	print_tree(root->left);
	printf("R:\n");
	print_tree(root->right);
	printf("Leaving: %d\n", root->key);
}

int main(void) {
	printf("Tree input format:\n<Number of nodes>\n<node_i value> <left> <right>\n"
	       "0 means no child\n");
	printf("\nEnter tree 1:\n");

	struct tree_node *tree1 = read_tree(tree1_nodes, &tree1_nodes_sz);
	printf("\nEnter tree 2:\n");
	struct tree_node *tree2 = read_tree(tree2_nodes, &tree2_nodes_sz);

	//print_tree(tree1);
	//printf("\n\n\n");
	//print_tree(tree2);

	printf("Tree 2 is a subtree of tree 1: %s\n", is_subtree(tree1, tree2) ? "yes" : "no");

	return 0;
}
