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
#define STACK_SZ 1024

static int stack[STACK_SZ];
static size_t stack_pointer = 0;

static void push(int n) {
	assert(stack_pointer < STACK_SZ);
	stack[stack_pointer++] = n;
}

static int pop(void) {
	assert(stack_pointer > 0);
	return stack[--stack_pointer];
}

int eval_expr(const char *expr) {
	int offset = 0;

	while (1) {
		int val;
		int offset_incr;
		char op;
		if (sscanf(&expr[offset], " %d%n", &val, &offset_incr) == 1) {
			push(val);
		} else if (sscanf(&expr[offset], " %c%n", &op, &offset_incr) == 1) {
			int right = pop();
			int left = pop();
			switch (op) {
			case '+':
				push(left+right);
				break;
			case '-':
				push(left-right);
				break;
			case '*':
				push(left*right);
				break;
			case '/':
				assert(right != 0);
				push(left/right);
				break;
			default:
				assert(0);
			}
		} else {
			break;
		}

		offset += offset_incr;
	}

	return pop();
}

static char input_buff[STACK_SZ];

int main(void) {
	printf("Enter an RPN expression.\n");
	printf("> ");

	while (fgets(input_buff, sizeof(input_buff), stdin) != NULL) {
		int first_tok;
		sscanf(input_buff, " %n", &first_tok);
		if (input_buff[first_tok] != '\0') {
			printf("%d\n", eval_expr(input_buff));
		}
		printf("> ");
	}

	return 0;
}
