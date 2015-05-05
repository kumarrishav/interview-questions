/* Given a boolean expression consisting of the symbols 0, 1, &, | and ^, and a
 * desired boolean result value `result`, implement a function to count the number
 * of ways of parenthesizing the expression such that it evaluates to `result`.
 *
 * Source: Cracking the Coding Interview, page 110, question 9.11
 */ 
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int is_operator(char c) {
	return c == '^' || c == '|' || c == '&';
}

unsigned valid_evals(const char *expr, int expected);
unsigned as_last(const char *expr, size_t expr_sz, size_t op_i, int expected) {
	char left[op_i+1];
	char right[expr_sz-op_i];

	strncpy(left, expr, op_i);
	left[op_i] = '\0';

	strcpy(right, expr+op_i+1);

	switch (expr[op_i]) {
	case '^':
		if (expected) {
			return valid_evals(left, 1)*valid_evals(right, 0) +
				valid_evals(left, 0)*valid_evals(right, 1);
		} else {
			return valid_evals(left, 1)*valid_evals(right, 1) +
				valid_evals(left, 0)*valid_evals(right, 0);
		}
		break;
	case '|':
		if (expected) {
			return valid_evals(left, 1)*valid_evals(right, 0) +
				valid_evals(left, 0)*valid_evals(right, 1) +
				valid_evals(left, 1)*valid_evals(right, 1);
		} else {
			return valid_evals(left, 0)*valid_evals(right, 0);
		}
		break;
	case '&':
		if (expected) {
			return valid_evals(left, 1)*valid_evals(right, 1);
		} else {
			return valid_evals(left, 0)*valid_evals(right, 1) +
				valid_evals(left, 1)*valid_evals(right, 0) +
				valid_evals(left, 0)*valid_evals(right, 0);
		}
		break;
	default:
		assert(0);
	}
}

unsigned valid_evals(const char *expr, int expected) {
	if (strcmp(expr, "0") == 0 || strcmp(expr, "1") == 0) {
		return expr[0] == expected+'0';
	}

	unsigned res = 0;
	size_t expr_sz = strlen(expr);
	size_t i;
	for (i = 0; i < expr_sz; i++) {
		if (!is_operator(expr[i])) {
			continue;
		}
		res += as_last(expr, expr_sz, i, expected);
	}

	return res;
}

int main(void) {
	printf("Enter a boolean expression with ^, & and |, and the desired result (1 or 0)\n");
	printf("> ");
	fflush(stdout);

	char input[256];
	int expected;

	while (scanf("%s%d", input, &expected) == 2) {
		printf("There are %u ways to get %d.\n", valid_evals(input, expected), expected);
		printf("> ");
		fflush(stdout);
	}

	return 0;
}
