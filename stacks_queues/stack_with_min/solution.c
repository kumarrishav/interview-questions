/* How would you design a stack which, in addition to push and pop, also has a
 * function min which returns the minimum element? Push, pop and min should
 * all operate in O(1) time.
 *
 * Source: Cracking the Coding Interview, page 80, question 3.2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAX_STACK_SZ 1024

struct stack_with_min {
	int stack[MAX_STACK_SZ];
	size_t top;

	int mins[MAX_STACK_SZ];
	size_t mins_top;
};

struct stack_with_min *new_stack(void) {
	struct stack_with_min *res = malloc(sizeof(*res));

	if (res == NULL) {
		return NULL;
	}

	res->top = res->mins_top = 0;

	return res;
}

void free_stack(struct stack_with_min *s) {
	free(s);
}

void push(struct stack_with_min *s, int element) {
	assert(s->top < MAX_STACK_SZ);

	s->stack[s->top++] = element;

	if (s->mins_top == 0 || element <= s->mins[s->mins_top-1]) {
		s->mins[s->mins_top++] = element;
	}
}

int pop(struct stack_with_min *s) {
	assert(s->top > 0);

	int ret = s->stack[--s->top];

	assert(s->mins_top > 0);
	if (s->top == 0 || ret == s->mins[s->mins_top-1]) {
		s->mins_top--;
	}

	return ret;
}

int min(struct stack_with_min *s) {
	assert(s->mins_top > 0);
	return s->mins[s->mins_top-1];
}

static char input_line[256];
int main(void) {
	printf("Available commands:\n"
	       "PUSH v - Pushes the value v.\n"
	       "POP - Pops from the stack.\n"
	       "MIN - Returns the minimum value currently in the stack.\n"
	       "Q - quit\n");

	struct stack_with_min *s = new_stack();
	if (s == NULL) {
		fprintf(stderr, "Couldn't create stack - out of memory.\n");
		return 1;
	}

	int finish = 0;

	while (!finish) {

		printf("> ");
		fflush(stdout);

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			break;
		}

		int value;
		char op[4];

		if (sscanf(input_line, " PUSH %d", &value) == 1) {
			push(s, value);
			printf("Pushed %d.\n", value);
		} else {
			sscanf(input_line, " %s", op);
			if (strcmp(op, "POP") == 0) {
				printf("Popped: %d\n", pop(s));
			} else if (strcmp(op, "MIN") == 0) {
				printf("Min: %d\n", min(s));
			} else {
				finish = 1;
			}
		}
	}

	free_stack(s);

	return 0;
}

