/* Write a function that receives the root of a binary tree and traverses
 * the tree doing zig zags, printing each node along the way.
 * EXAMPLE
 * For this tree:
 *
 *           1
 *         /   \
 *        /     \
 *       /       \
 *      2         3 
 *    /   \     /   \
 *   4     5   6     7
 *  / \
 * 8   9
 *
 * The code would print: 1, 2, 3, 7, 6, 5, 4, 8, 9
 *
 * Source: Careercup (Amazon SDE interviews)
 */
#include <stdio.h>
#include <assert.h>
#define NODE_BUFF_SZ 512

struct tree_node {
	int val;
	struct tree_node *left;
	struct tree_node *right;
};

void zig_zag_traversal(struct tree_node *root) {

	static struct tree_node *node_buff[NODE_BUFF_SZ];
	size_t buff_top = 0;
	size_t buff_offset = 0;

	int level = 1;
	unsigned nodes_in_level = 1;
	unsigned nodes_in_next_level;
	node_buff[buff_top++] = root;

	while (nodes_in_level > 0) {
		nodes_in_next_level = 0;
		unsigned i;
		for (i = 0; i < nodes_in_level; i++) {
			struct tree_node *popped = node_buff[buff_offset+nodes_in_level-i-1];
			printf("%d ", popped->val);

			if ((level & 1) && popped->right != NULL) {
				node_buff[buff_top++] = popped->right;
				nodes_in_next_level++;
			}
			if ((level & 1) && popped->left != NULL) {
				node_buff[buff_top++] = popped->left;
				nodes_in_next_level++;
			}
			if (!(level & 1) && popped->left != NULL) {
				node_buff[buff_top++] = popped->left;
				nodes_in_next_level++;
			}
			if (!(level & 1) && popped->right != NULL) {
				node_buff[buff_top++] = popped->right;
				nodes_in_next_level++;
			}
		}
		buff_offset += nodes_in_level;
		nodes_in_level = nodes_in_next_level;
		level = (level+1)%2;
	}
	putchar('\n');
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static struct tree_node nodes[32];

int main(void) {
	printf("Enter number of nodes, and for each node, enter: nodeID leftChild rightChild (-1 means no child)\n");
	printf("> ");

	unsigned nodes_count;
	while (scanf("%u", &nodes_count) == 1) {
		unsigned i;
		unsigned root;
		for (i = 0; i < nodes_count; i++) {
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
		zig_zag_traversal(&nodes[root]);
		printf("> ");
	}

	return 0;
}
