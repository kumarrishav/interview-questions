/* Reverse Polish Notation (RPN) calculators don't have the need for parenthesis.
 * In RPN, the operands of an operator are always specified before the operator, and as such
 * there is only one way to interpret the expression (in other words, it uses postfix operators).
 *
 * For example, this expression:
 *
 * 3 4 + 5 3 - *
 * Is equal to (3+4)*(5-3), and it evaluates to 14.
 *
 * Design and implement a method to evaluate an RPN expression.
 *
 * Source: past interview experience
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define MAX_EXPR_SZ 1024

enum token_type {
	OPERATOR,
	VALUE
};

struct list_node {
	enum token_type type;
	union {
		char op;
		int val;
	} token;
	struct list_node *next;
};

static void eval_expr_aux(struct list_node *head, struct list_node *curr) {
	if (curr->next == head || curr->next->type == OPERATOR) {
		return;
	}

	eval_expr_aux(head, curr->next);
	int res;
	switch (curr->next->next->token.op) {
	case '+':
		res = curr->token.val + curr->next->token.val;
		break;
	case '-':
		res = curr->token.val - curr->next->token.val;
		break;
	case '*':
		res = curr->token.val * curr->next->token.val;
		break;
	case '/':
		res = curr->token.val / curr->next->token.val;
		break;
	default:
		assert(0);
	}
	curr->token.val = res;

	struct list_node *old_next = curr->next;
	curr->next = curr->next->next->next;
	free(old_next->next);
	free(old_next);

	eval_expr_aux(head, curr);
}

int eval_expr(struct list_node *head) {
	eval_expr_aux(head, head->next);
	int val = head->next->token.val;
	free(head->next);
	head->next = head;
	return val;
}

static int read_expr(struct list_node *head) {
	static char line[MAX_EXPR_SZ];

	if (fgets(line, sizeof(line), stdin) == NULL) {
		return 0;
	}

	head->next = head;
	struct list_node *prev = head;
	int val;
	char op;
	size_t offset = 0;

	while (1) {
		int offset_incr = 0;
		struct list_node *new_node = malloc(sizeof(*new_node));
		assert(new_node != NULL);

		if (sscanf(&line[offset], " %d%n", &val, &offset_incr) == 1) {
			new_node->type = VALUE;
			new_node->token.val = val;
		} else if (sscanf(&line[offset], " %c%n", &op, &offset_incr) == 1) {
			assert(op == '+' || op == '-' || op == '*' || op == '/');
			new_node->type = OPERATOR;
			new_node->token.op = op;
		} else {
			free(new_node);
			break;
		}

		prev->next = new_node;
		new_node->next = head;
		prev = new_node;
		offset += offset_incr;
	}

	return 1;
}

#ifdef DEBUG_PARSER
static void print_expr(struct list_node *head) {
	struct list_node *curr;
	for (curr = head->next; curr != head; curr = curr->next) {
		if (curr->type == OPERATOR) {
			putchar(curr->token.op);
			putchar(' ');
		} else {
			printf("%d ", curr->token.val);
		}
	}
	putchar('\n');
}
#endif

int main(void) {
	printf("Enter an RPN expression.\n");
	printf("> ");

	struct list_node head;

	while (read_expr(&head)) {
		#ifdef DEBUG_PARSER
		print_expr(&head);
		#endif
		if (head.next != &head) {
			printf("%d\n", eval_expr(&head));
		}
		printf("> ");
	}

	return 0;
}
