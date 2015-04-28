/* Describe how you could use a single array to implement three stacks.
 *
 * Source: Cracking the Coding Interview, page 80, question 3.1
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define ARRAY_SZ 1024

struct stack_node {
	int data;
	struct stack_node *below;
};

struct stacks {
	struct stack_node stacks_array[ARRAY_SZ];
	struct stack_node *top[4];
};

static void push_internal(struct stacks *s, size_t stack_no, struct stack_node *n) {
	n->below = s->top[stack_no];
	s->top[stack_no] = n;
}

static struct stack_node *pop_internal(struct stacks *s, size_t stack_no) {
	struct stack_node *ret = s->top[stack_no];
	assert(ret != NULL);
	s->top[stack_no] = ret->below;
	return ret;
}

struct stacks *create_stacks(void) {
	struct stacks *ret = malloc(sizeof(*ret));

	if (ret == NULL) {
		return NULL;
	}

	size_t i;
	for (i = 0; i < sizeof(ret->top)/sizeof(*ret->top); i++) {
		ret->top[i] = NULL;
	}

	for (i = 0; i < sizeof(ret->stacks_array)/sizeof(*ret->stacks_array); i++) {
		push_internal(ret, 3, &ret->stacks_array[i]);
	}

	return ret;
}

void destroy_stacks(struct stacks *s) {
	free(s);
}

void push(struct stacks *s, size_t stack_no, int value) {
	struct stack_node *new_node = pop_internal(s, 3);
	new_node->data = value;
	push_internal(s, stack_no, new_node);
}

int pop(struct stacks *s, size_t stack_no) {
	struct stack_node *popped = pop_internal(s, stack_no);
	push_internal(s, 3, popped);
	return popped->data;
}

static char input_line[256];
int main(void) {
	printf("Available commands:\n"
	       "I s v - Pushes the value v into stack s. 0 <= s < < 3\n"
	       "P s - Pops from stack s. 0 <= s < 3\n"
	       "Q - quit\n");

	struct stacks *s = create_stacks();
	if (s == NULL) {
		fprintf(stderr, "Couldn't create stacks - out of memory.\n");
		return 1;
	}

	int finish = 0;

	while (!finish) {

		printf("> ");
		fflush(stdout);

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			break;
		}

		size_t stack_no;
		int value;
		char op;

		if (sscanf(input_line, " I %zu %d", &stack_no, &value) == 2) {
			push(s, stack_no, value);
			printf("Pushed %d into stack %zu\n", value, stack_no);
		} else if (sscanf(input_line, " P %zu", &stack_no) == 1) {
			int popped = pop(s, stack_no);
			printf("Popped from %zu: %d\n", stack_no, popped);
		} else if (sscanf(input_line, " %c", &op)) {
			if (op != 'Q') {
				fprintf(stderr, "Invalid input line, ignoring (%s)\n", input_line);
			} else {
				finish = 1;
			}
		}
	}

	destroy_stacks(s);

	return 0;
}
