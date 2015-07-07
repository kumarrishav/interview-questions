/* A binary search tree was created by traversing through an array from left to right and inserting
 * each element. Given a binary search tree with distinct elements, print all possible arrays that
 * could have led to this tree.
 *
 * EXAMPLE
 *
 * Input:
 *
 *        2
 *      /   \
 *     1     3
 *
 * Output:
 * 
 * [ 2, 1, 3 ]
 * [ 2, 3, 1 ]
 *
 * Source: Cracking the Coding Interview, 6th Edition, Exercise 4.9 (page 110)
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* This is an interesting problem
 *
 * We solve this by keeping a set of nodes on each level. Think of it as a BFS on which each
 * level of the BFS is handled by a single recursive call.
 *
 * It's not a BFS by levels though, because it can skew itself into one of the sides of the tree.
 *
 * We do as follows:
 *
 * On each call, we can choose to place any node in the current BFS queue into the current array
 * position. If we choose a node, then we take that node out of the possible nodes queue and insert
 * its children, because they are now possible candidates for the set of the next call.
 *
 * Note, however, that each call only handles the nodes that were originally in the set.
 *
 * This is better explained in a whiteboard though.
 *
 * Note: I do believe that this solution is much more elegant and easier to implement than the
 * solution presented in the book.
 *
 * An example goes a long way towards understanding this approach
 *
 * Consider the tree
 *
 *              8
 *           /     \
 *          4      12
 *        /   \   /  \
 *       2     5 10   15
 *      / \
 *     1   3
 *
 * We know that the root must be the first element in the array. That's easy to see.
 *
 * Once we have 8 in the array, then we have two choices: 4 and 12. If we choose 4,
 * the next position can be any of 2, 5, or 12. If we choose 12, then the next
 * position can be any of 4, 10 or 15.
 *
 * If we choose 4 and 2, then the next position can be any of 1, 3, 5, 10, or 15.
 *
 * So, on each level, we have a set of possible choices. We just loop through every candidate,
 * choosing one at a time for the current position, insert its children in the candidates list,
 * and recurse.
 *     
 */

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static void swap(struct tree_node *array[], size_t i, size_t j) {
	struct tree_node *tmp = array[i];
	array[i] = array[j];
	array[j] = tmp;
}

static void print_array(struct tree_node *array[], size_t array_sz) {
	printf("[ ");

	if (array_sz > 0) {
		printf("%d", array[0]->value);
	}

	size_t i;
	for (i = 1; i < array_sz; i++) {
		printf(", %d", array[i]->value);
	}

	printf(" ]\n");
}

static void bst_sequences_aux(struct tree_node *array[], size_t array_sz, size_t nodes) {
	if (array_sz == 0) {
		print_array(array-nodes, nodes);
		return;
	}

	size_t i;
	for (i = 0; i < array_sz; i++) {
		swap(array, i, 0);

		size_t inserted = 0;
		if (array[0]->left != NULL) {
			array[array_sz+inserted] = array[0]->left;
			inserted++;
		}
		if (array[0]->right != NULL) {
			array[array_sz+inserted] = array[0]->right;
			inserted++;
		}

		bst_sequences_aux(array+1, array_sz-1+inserted, nodes+inserted);

		swap(array, i, 0);
	}
}

void bst_sequences(struct tree_node *root) {
	/* In the real world we would have to deal with overflow */
	static struct tree_node *array_buff[4096];
	if (root != NULL) {
		array_buff[0] = root;
		bst_sequences_aux(array_buff, 1, 1);
	}
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

struct tree_node *deserialize(size_t nodes) {
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
	printf("Enter the number of nodes in the BST, followed by the preorder traversal, followed by the inorder traversal\n");
	printf("> ");

	size_t nodes;
	while (scanf("%zu", &nodes) == 1) {
		struct tree_node *root = deserialize(nodes);
		bst_sequences(root);
		destroy_tree(root);
		printf("> ");
	}

	return 0;
}
