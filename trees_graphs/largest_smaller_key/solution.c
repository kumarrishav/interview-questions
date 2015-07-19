/* Given the root of a binary search tree and a key x, find the largest key in the tree that is
 * smaller than x.
 *
 * EXAMPLE
 *
 * If an in-order traversal of all keys in the tree is [ 2, 3, 4, 7, 17, 19, 21, 35, 89 ] and
 * x is 19, the biggest key that is smaller than x is 17.
 *
 * Source: pramp.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static struct tree_node *tree_max(struct tree_node *root) {
	if (root == NULL)
		return NULL;
	while (root->right != NULL)
		root = root->right;
	return root;
}

struct tree_node *largest_smaller_key(int x, struct tree_node *root) {
	struct tree_node *curr = root;
	struct tree_node *last_r = NULL;
	struct tree_node *ret = NULL;

	while (curr != NULL && x != curr->value) {
		if (x < curr->value) {
			curr = curr->left;
		} else {
			last_r = curr;
			curr = curr->right;
		}
	}

	if (curr == NULL) {
		ret = last_r;
	} else {
		assert(x == curr->value);
		if (curr->left != NULL) {
			ret = tree_max(curr->left);
		} else {
			ret = last_r;
		}
	}

	return ret;		
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
	printf("Then, enter the value x to find the largest key in the BST that is smaller than x\n");
	printf("> ");

	size_t nodes;
	while (scanf("%zu", &nodes) == 1) {
		struct tree_node *root = deserialize(nodes);
		int x;
		scanf("%d", &x);

		struct tree_node *ls_k = largest_smaller_key(x, root);
		if (ls_k == NULL) {
			printf("No such key.\n");
		} else {
			printf("%d\n", ls_k->value);
		}

		destroy_tree(root);
		printf("> ");
	}

	return 0;
}
