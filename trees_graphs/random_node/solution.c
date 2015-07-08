/* You are implementing a binary tree class from scratch which, in addition to insert, find and
 * delete, has a method getRandomNode() which returns a random node from the tree. All nodes
 * should be equally likely to be chosen. Design and implement an algorithm for getRandomNode(),
 * and explain how you would implement the rest of the methods.
 *
 * Source: Cracking the Coding Interview, 6th Edition, Exercise 4.11 (page 111)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static struct tree_node *random_node_aux(struct tree_node *root, struct tree_node *curr_choice,
					 int *seen) {
	if (root == NULL) {
		return curr_choice;
	}

	(*seen)++;

	int n = rand();
	int block_sz = RAND_MAX / *seen;

	if (n >= (*seen-1)*block_sz) {
		curr_choice = root;
	}

	curr_choice = random_node_aux(root->left, curr_choice, seen);
	curr_choice = random_node_aux(root->right, curr_choice, seen);

	return curr_choice;
}

struct tree_node *random_node(struct tree_node *root) {
	int seen = 0;
	return random_node_aux(root, NULL, &seen);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Tree deserialization library
 * See trees_graphs/serialize_tree to learn it
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

static size_t find_inorder(int inorder[], size_t len, int value) {
	size_t l = 0;
	size_t r = len-1;
	size_t mid;
	while (l <= r) {
		mid = l+(r-l)/2;
		if (inorder[mid] == value) {
			return mid;
		} else if (inorder[mid] < value) {
			l = mid+1;
		} else {
			r = mid-1;
		}
	}

	assert(0);
	return -1;
}

static struct tree_node *deserialize_internal(int preorder[], int inorder[], size_t len) {

	if (len == 0) {
		return NULL;
	}

	struct tree_node *ret = malloc(sizeof(*ret));
	assert(ret != NULL);

	ret->value = preorder[0];
	size_t root_io = find_inorder(inorder, len, preorder[0]);

	ret->left = deserialize_internal(&preorder[1], &inorder[0], root_io);
	ret->right = deserialize_internal(&preorder[root_io+1], &inorder[root_io+1], len-root_io-1);

	return ret;
}

static struct tree_node *deserialize(size_t nodes) {
	static int buff[4096];
	int val;
	size_t i;
	for (i = 0; i < nodes*2; i++) {
		scanf("%d", &val);
		assert(i < sizeof(buff)/sizeof(buff[0]));
		buff[i] = val;
	}
	return deserialize_internal(&buff[0], &buff[i/2], i/2);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void destroy_tree(struct tree_node *root) {
	if (root == NULL) {
		return;
	}
	destroy_tree(root->left);
	destroy_tree(root->right);
	free(root);
}

int main(void) {
	printf("Enter the number of nodes in the tree, followed by the preorder traversal, followed by the inorder traversal\n");
	printf("Then, enter the number of times to select a random node\n");
	printf("> ");

	srand(time(NULL));

	size_t nodes;
	while (scanf("%zu", &nodes) == 1) {
		struct tree_node *root = deserialize(nodes);

		size_t i, j;
		scanf("%zu", &i);
		for (j = 0; j < i; j++) {
			struct tree_node *chosen = random_node(root);
			assert(chosen != NULL);
			printf("Picked %d\n", chosen->value);
		}
		

		destroy_tree(root);
		printf("> ");
	}

	return 0;
}
