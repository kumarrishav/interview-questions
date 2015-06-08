/* Given a binary tree, change the value in each node to the sum of that node's value with all the
 * values on that node's left subtree.
 *
 * Source: Careercup (Yahoo interview)
 */
#include <stdio.h>

struct tree_node {
	int data;
	struct tree_node *left;
	struct tree_node *right;
};

int tree_sum(struct tree_node *root) {
	if (root == NULL) {
		return 0;
	}
	root->data += tree_sum(root->left);
	return root->data+tree_sum(root->right);
}

static void print_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}
	printf("%d -> ", root->data);
	if (root->left) {
		printf("%d ", root->left->data);
	} else {
		printf("NULL ");
	}
	if (root->right) {
		printf("%d\n", root->right->data);
	} else {
		printf("NULL\n");
	}
	print_tree(root->left);
	print_tree(root->right);
}

static struct tree_node nodes[1024];

int main(void) {
	printf("Enter number of nodes, and for each node, enter the node's value, left and right children (-1 means no child)\n");
	printf("Root is node 0\n");
	printf("> ");

	int nodes_count;
	while (scanf("%d", &nodes_count) == 1) {
		int i;
		for (i = 0; i < nodes_count; i++) {
			int val, left, right;
			scanf("%d%d%d", &val, &left, &right);
			nodes[i].data = val;
			if (left == -1) {
				nodes[i].left = NULL;
			} else {
				nodes[i].left = &nodes[left];
			}
			if (right == -1) {
				nodes[i].right = NULL;
			} else {
				nodes[i].right = &nodes[right];
			}
		}

		printf("*** Before:\n");
		print_tree(&nodes[0]);

		printf("*** After:\n");
		printf("Total sum: %d\n", tree_sum(&nodes[0]));
		print_tree(&nodes[0]);

		printf("> ");
	}

	return 0;
}
