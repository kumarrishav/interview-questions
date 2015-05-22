
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/* Expressions parser */
static int expr(const char *expr_str, size_t *cursor);
static int expr_r(const char *expr_str, size_t *cursor, int res_so_far);
static int term(const char *expr_str, size_t *cursor);
static int term_r(const char *expr_str, size_t *cursor, int res_so_far);
static int factor(const char *expr_str, size_t *cursor);

// Top-level entry point
static int expr(const char *expr_str, size_t *cursor) {
	return expr_r(expr_str, cursor, term(expr_str, cursor));
}

static int expr_r(const char *expr_str, size_t *cursor, int res_so_far) {
	if (expr_str[*cursor] == '+' || expr_str[*cursor] == '-') {
		size_t saved_cursor = *cursor;

		(*cursor)++;
		int term_val = term(expr_str, cursor);

		if (expr_str[saved_cursor] == '+') {
			res_so_far += term_val;
		} else {
			res_so_far -= term_val;
		}

		return expr_r(expr_str, cursor, res_so_far);
	} else {
		return res_so_far;
	}
}

static int term(const char *expr_str, size_t *cursor) {
	return term_r(expr_str, cursor, factor(expr_str, cursor));
}

static int term_r(const char *expr_str, size_t *cursor, int res_so_far) {
	if (expr_str[*cursor] == '*' || expr_str[*cursor] == '/') {
		size_t saved_cursor = *cursor;

		(*cursor)++;
		int factor_val = factor(expr_str, cursor);

		if (expr_str[saved_cursor] == '*') {
			res_so_far *= factor_val;
		} else {
			res_so_far /= factor_val;
		}

		return term_r(expr_str, cursor, res_so_far);
	} else {
		return res_so_far;
	}
}

static int factor(const char *expr_str, size_t *cursor) {
	int val;
	if (expr_str[*cursor] == '(') {
		(*cursor)++;
		val = expr(expr_str, cursor);
		assert(expr_str[*cursor] == ')');
		(*cursor)++;
	} else {
		int read;
		sscanf(&expr_str[*cursor], "%d%n", &val, &read);
		*cursor += read;
	}
	return val;
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void init_possibilities(size_t n, int arr[n]) {
	size_t i;
	for (i = 0; i < n; i++) {
		arr[i] = 1;
	}
}

static int next_possibility(size_t n, int arr[n]) {
	ssize_t i = n-1;
	while (i >= 0 && arr[i] == 9) {
		arr[i] = 1;
		i--;
	}

	if (i < 0) {
		return 0;
	}

	arr[i]++;

	return 1;
}

static int eval_expr(const char *expr_str, int vars[], int target) {
	size_t expr_i = 0;
	size_t var_i = 0;

	char *generated = strdup(expr_str);
	assert(generated != NULL);

	while (generated[expr_i] != '=') {
		if (generated[expr_i] == '_') {
			generated[expr_i] = vars[var_i++]+'0';
		}
		expr_i++;
	}

	generated[expr_i] = '\0';
	size_t cursor = 0;
	int res = expr(generated, &cursor);

	free(generated);

	return res == target;
}

static void print_solution(const char *expr_str, int vars[]) {
	size_t expr_i = 0;
	size_t var_i = 0;

	while (expr_str[expr_i] != '=') {
		if (expr_str[expr_i] == '_') {
			putchar(vars[var_i++]+'0');
		} else {
			putchar(expr_str[expr_i]);
		}
		expr_i++;
	}

	printf("%s\n", &expr_str[expr_i]);
}

unsigned magic_numbers(const char *expr_str) {

	int target;

	size_t vars_sz = 0;
	size_t i;
	for (i = 0; expr_str[i] != '='; i++) {
		if (expr_str[i] == '_') {
			vars_sz++;
		}
	}

	sscanf(&expr_str[i+1], "%d", &target);

	if (vars_sz == 0) {
		if (eval_expr(expr_str, NULL, target)) {
			print_solution(expr_str, NULL);
			return 1;
		} else {
			return 0;
		}
	}

	int vars[vars_sz];
	init_possibilities(vars_sz, vars);
	if (eval_expr(expr_str, vars, target)) {
		print_solution(expr_str, vars);
	}

	unsigned matches = 0;
	while (next_possibility(vars_sz, vars)) {
		unsigned matches_incr = eval_expr(expr_str, vars, target);
		if (matches_incr > 0) {
			print_solution(expr_str, vars);
		}
		matches += matches_incr;
	}

	return matches;
}

static char buf[512];

int main(void) {
	printf("Enter an expression pattern to find matches.\n");
	printf("> ");

	while (scanf("%s", buf) == 1) {
		unsigned found = magic_numbers(buf);
		if (found == 0) {
			printf("No matches found.\n");
		} else {
			printf("Total: %u\n", found);
		}
		printf("> ");
	}

	return 0;
}
