/* Implement a function to check if a binary tree is balanced. For the purposes of
 * this question, a balanced tree is defined to be a tree such that the heights of the
 * two subtrees of any node never differ by more than one.
 *
 * Source: Cracking the Coding Interview, page 86, question 4.1
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define BALANCED_THRES 1

struct tree_node {
	int value;
	struct tree_node *right;
	struct tree_node *left;
};

struct token {
	enum {
		INTEGER,
		PARENTHESIS,
		EMPTY
	} type;
	union {
		int i;
		char c;
	} val;
};

static struct token last_token;

char gettoken(void) {
	int c = getchar();

	if (c == '(' || c == ')') {
		last_token.type = PARENTHESIS;
		last_token.val.c = c;
	} else if (isspace(c) || c == EOF) {
		last_token.type = EMPTY;
		last_token.val.c = c;
	}
	else {
		ungetc(c, stdin);
		if (scanf("%d", &last_token.val.i) == 1) {
			last_token.type = INTEGER;
			return c;
		} else {
			fprintf(stderr, "Error: invalid token %c\n", c);
			assert(0);
		}
	}

	return c;
}

struct tree_node *parse_node() {

	if (last_token.type == EMPTY ||
	    (last_token.type == PARENTHESIS && last_token.val.c == ')')) {
		return NULL;
	}

	struct tree_node *ret = malloc(sizeof(*ret));
	assert(ret != NULL);
	ret->value = last_token.val.i;

	if (gettoken() != '(')
		fprintf(stderr, "Warning: expected '('\n");

	gettoken();
	ret->left = parse_node();

	if (last_token.type != PARENTHESIS || last_token.val.c != ')')
		fprintf(stderr, "Warning: expected ')'\n");

	if (gettoken() != '(')
		fprintf(stderr, "Warning: expected '(' for second child\n");

	gettoken();
	ret->right = parse_node();

	if (last_token.type != PARENTHESIS || last_token.val.c != ')')
		fprintf(stderr, "Warning: expected ')' to close second child\n");

	gettoken();

	return ret;
}

void print_tree(struct tree_node *root) {
	if (root == NULL)
		return;
	printf("%d(", root->value);
	print_tree(root->left);
	printf(")(");
	print_tree(root->right);
	printf(")");
}

int is_balanced_aux(struct tree_node *node, int *res) {
	if (node == NULL) {
		*res = 1;
		return 0;
	}

	int left_res = is_balanced_aux(node->left, res);
	if (!*res)
		return -1;
	int right_res = is_balanced_aux(node->right, res);
	if (!*res)
		return -1;

	*res = (abs(left_res - right_res) <= BALANCED_THRES);
	return max(left_res, right_res)+1;
}

int is_balanced(struct tree_node *root) {
	int res;
	(void) is_balanced_aux(root, &res);
	return res;
}

int main(void) {
	printf("Enter the tree in the form node(left_child)(right_child)\n");
	printf("Example: 0(1(2()())(3()()))(4()())\n");

	while (1) {
		printf("> ");
		fflush(stdout);
		if (gettoken() == EOF)
			break;
		struct tree_node *root = parse_node();
		printf("  \x1b[32mTree:\x1b[0m ");
		print_tree(root);
		printf("\n");
		printf("  \x1b[32mIs balanced:\x1b[0m %s\n", is_balanced(root) ? "yes" : "no");
	}

	return 0;
}
