/* Write a program to sort a stack in ascending order (with biggest items on top).
 * You may use at most one additional stack to hold items, but you may not copy
 * the elements into any other data structure (such as an array). The stack supports
 * the following operations: push, pop, peek, and isEmpty.
 *
 * Source: Cracking the Coding Interview, page 81, question 3.6
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#define STACK_SZ 256

struct stack {
	int values[STACK_SZ];
	size_t top;
};

void init_stack(struct stack *s) {
	s->top = 0;
}

void push(struct stack *s, int val) {
	assert(s->top < STACK_SZ);
	s->values[s->top++] = val;
}

int pop(struct stack *s) {
	assert(s->top > 0);
	return s->values[--s->top];
}

int peek(struct stack *s) {
	assert(s->top > 0);
	return s->values[s->top-1];
}

int is_empty(struct stack *s) {
	return s->top == 0;
}

void print_stack(struct stack *s) {
	printf("%zu elements are in the stack:\n", s->top);

	size_t i;
	for (i = s->top; i > 0; i--) {
		printf("%d\n", s->values[i-1]);
	}
}

void sort_stack(struct stack *stack, struct stack *aux) {
	while (!is_empty(stack)) {
		push(aux, pop(stack));
	}

	while (!is_empty(aux)) {
		int popped = 0;
		int to_insert = pop(aux);
		while (!is_empty(stack) && peek(stack) > to_insert) {
			push(aux, pop(stack));
			popped++;
		}
		push(stack, to_insert);
		while (popped != 0) {
			push(stack, pop(aux));
			popped--;
		}
	}
}


static char input_line[256];
static struct stack main_stack;
static struct stack aux_stack;

int main(void) {
	printf("Available commands:\n"
	       "PUSH v - Pushes the value v.\n"
	       "POP - Pops from the stack.\n"
	       "SORT - Sorts the stack (biggest item on top) using an auxiliary stack.\n"
	       "Q - quit\n");

	int finish = 0;
	init_stack(&main_stack);

	while (!finish) {

		printf("> ");
		fflush(stdout);

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			break;
		}

		int value;
		char op[5];

		if (sscanf(input_line, " PUSH %d", &value) == 1) {
			push(&main_stack, value);
			printf("Pushed %d.\n", value);
		} else {
			sscanf(input_line, " %s", op);
			if (strcmp(op, "POP") == 0) {
				printf("Popped: %d\n", pop(&main_stack));
			} else if (strcmp(op, "SORT") == 0) {
				init_stack(&aux_stack);
				sort_stack(&main_stack, &aux_stack);
				printf("Sorted stack.\n");
				print_stack(&main_stack);
			} else {
				finish = 1;
			}
		}
	}

	return 0;
}
