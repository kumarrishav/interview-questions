/* Design a system to serialize and deserialize a binary tree
 *
 * Source: Careercup (Uber interview)
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Note: For simplicity this assumes a binary search tree with unique keys */

struct tree_node {
	int value;
	struct tree_node *left;
	struct tree_node *right;
};

static void serialize_inorder(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	serialize_inorder(root->left);
	printf("%d ", root->value);
	serialize_inorder(root->right);
}

static void serialize_preorder(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	printf("%d ", root->value);
	serialize_preorder(root->left);
	serialize_preorder(root->right);
}

void serialize(struct tree_node *root) {
	if (root == NULL) {
		return;
	}

	serialize_preorder(root);
	printf("\n");
	serialize_inorder(root);
	printf("\n");
}

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

struct tree_node *deserialize(void) {
	static int buff[4096];

	size_t i = 0;
	int val;

	while (scanf("%d", &val) == 1) {
		assert(i < sizeof(buff)/sizeof(buff[0]));
		buff[i++] = val;
	}

	return deserialize_internal(&buff[0], &buff[i/2], i/2);
}

int main(void) {
	// Dog fooding at its best

	printf("Enter preorder traversal followed by inorder traversal\n");

	struct tree_node *root = deserialize();
	printf("Built tree:\n");
	serialize(root);

	return 0;
}
