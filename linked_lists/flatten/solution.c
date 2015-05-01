/* You have a set of linked lists vertically connected by child pointers, essentially forming
 * a multi-layer structure of linked lists. For example, here we have 4 linked lists:
 * the list (1, 2, 3, 4, 5); the list (6, 7); the list (8), and the list (9, 10, 11)
 *
 * 1 - 2 - 3 - 4 - 5
 *     |       |   
 *     6 - 7   8
 *         |
 *         9 - 10 - 11
 *
 * Dashes represent the "next" pointer; vertical lines represent child pointers.
 * To be clear, this is the structure of each node:
 *
 * struct list_node {
 *	int data;
 *	struct list_node *next;
 *	struct list_node *child;
 * };
 *
 * Design an algorithm to flatten the structure, such that every element can be reached
 * by following only "next" pointers.
 *
 * Source: Previous interview experience.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

struct list_node {
	int data;
	struct list_node *next;
	struct list_node *child;
};

struct list_node *flatten_aux(struct list_node *node) {
	struct list_node *prev = NULL;

	while (node != NULL) {
		if (node->child != NULL) {
			struct list_node *flattened_last = flatten_aux(node->child);
			flattened_last->next = node->next;
			node->next = node->child;
			node->child = NULL; // Clarify with interviewer if this is desired
			node = flattened_last;
		} 
		prev = node;
		node = node->next;
	}

	return prev;
}

struct list_node *flatten(struct list_node *head) {
	flatten_aux(head);
	return head;
}

// Yes, there are easier ways to read and parse the input, but wouldn't that be boring?
struct token {
	union {
		int id_value;
		int character;
	} val;
	enum {
		CHAR,
		ID
	} type;
} last_token;

void next_token(void) {
	last_token.val.character = getchar();
	last_token.type = CHAR;

	if (isdigit(last_token.val.character)) {
		ungetc(last_token.val.character, stdin);
		scanf("%d", &last_token.val.id_value);
		last_token.type = ID;
	} else {
		if (last_token.val.character == '\r')
			getchar(); // Gracefully handle \r\n
		if (last_token.val.character != '>' &&
		    last_token.val.character != '(' &&
		    last_token.val.character != ')')
			last_token.val.character = EOF;
	}
}

struct list_node *md_list(void) {

	assert(last_token.type == ID);

	struct list_node *n = malloc(sizeof(*n));
	n->data = last_token.val.id_value;
	n->next = n->child = NULL;

	next_token();

	assert(last_token.type == CHAR);

	if (last_token.val.character == EOF ||
	    last_token.val.character == ')') {
		return n;
	}

	assert(last_token.type == CHAR);
	assert(last_token.val.character == '>' ||
	       last_token.val.character == '(');

	if (last_token.val.character == '>') {
		next_token();
		n->next = md_list();
	} else {
		next_token();
		n->child = md_list();

		assert(last_token.type == CHAR);
		assert(last_token.val.character == ')');

		next_token();

		assert(last_token.type == CHAR);

		if (last_token.val.character == '>') {
			next_token();
			n->next = md_list();
		}
	}

	return n;
}

void destroy_md_list(struct list_node *head) {
	if (head == NULL) {
		return;
	}

	destroy_md_list(head->child);
	destroy_md_list(head->next);
	free(head);
}

void print_md_list(struct list_node *head) {

	if (head == NULL) {
		return;
	}

	printf("%d", head->data);
	if (head->child != NULL) {
		putchar('(');
		print_md_list(head->child);
		putchar(')');
	}

	if (head->next != NULL) {
		putchar('>');
		print_md_list(head->next);
	}
}

int main(void) {

	printf("> ");
	fflush(stdout);

	while (1) {
		next_token();

		if (last_token.type == CHAR && last_token.val.character == EOF) {
			break;
		}

		struct list_node *head = md_list();

		printf("Received: ");
		print_md_list(head);
		putchar('\n');

		printf("Flattened: ");
		print_md_list(flatten(head));
		putchar('\n');

		destroy_md_list(head);

		printf("> ");
		fflush(stdout);
	}

	return 0;
}
