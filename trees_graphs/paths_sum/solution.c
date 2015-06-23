/* You are given a binary tree in which each node contains a value. Design an
 * algorithm to print all paths which sum to a given value. The path does not need to
 * start or end at the root or a leaf.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.9
 */

#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

struct tree_node {
	int key;
	struct tree_node *left;
	struct tree_node *right;
};

static void print_path(struct tree_node *path[], size_t path_sz) {
	size_t i;

	if (path_sz > 0) {
		printf("%d", path[0]->key);
	}

	for (i = 1; i < path_sz; i++) {
		printf(" -> %d", path[i]->key);
	}

	printf("\n");
}

static void paths_sum_aux(struct tree_node *node, struct tree_node *path[], size_t depth,
			  int curr_sum, int sum) {

	if (node == NULL) {
		return;
	}

	path[depth] = node;
	curr_sum += node->key;

	if (curr_sum == sum) {
		print_path(path, depth+1);
	}

	size_t i;
	int subpath_sum = curr_sum;
	for (i = 0; i < depth; i++) {
		subpath_sum -= path[i]->key;
		if (subpath_sum == sum) {
			print_path(path+i+1, depth-i);
		}
	}

	paths_sum_aux(node->left, path, depth+1, curr_sum, sum);
	paths_sum_aux(node->right, path, depth+1, curr_sum, sum);
}

static size_t tree_depth(struct tree_node *root) {
	if (root == NULL) {
		return 0;
	}

	size_t depth_l = tree_depth(root->left);
	size_t depth_r = tree_depth(root->right);

	return max(depth_l+1, depth_r+1);
}

void paths_sum(struct tree_node *root, int sum) {
	size_t depth = tree_depth(root);
	if (depth == 0) {
		return;
	}

	struct tree_node **paths_buff = malloc(sizeof(*paths_buff)*depth*depth);
	if (paths_buff == NULL) {
		fprintf(stderr, "Out of memory.\n");
		return;
	}

	paths_sum_aux(root, paths_buff, 0, 0, sum);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define MAX_NODES 1024
static struct tree_node tree_nodes[MAX_NODES];
static size_t tree_nodes_sz;

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
	printf("\nEnter tree:\n");

	struct tree_node *root = read_tree(tree_nodes, &tree_nodes_sz);

	printf("Sum?\n");
	int s;
	scanf("%d", &s);
	paths_sum(root, s);

	return 0;
}
