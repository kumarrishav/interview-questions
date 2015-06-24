/* Write an arithmetic expressions parser.
 * The parser should take into consideration the usual arithemtic operators precedence.
 * Parenthesized expressions are allowed and have the usual effect.
 *
 * EXAMPLE
 *
 * Input: 4*(5+2)-1
 * Output: 27
 *
 */
#include <stdio.h>
#include <assert.h>

static int expr(const char *expr_str, size_t *cursor);
static int expr_r(const char *expr_str, size_t *cursor, int res_so_far);
static int term(const char *expr_str, size_t *cursor);
static int term_r(const char *expr_str, size_t *cursor, int res_so_far);
static int factor(const char *expr_str, size_t *cursor);

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

// Top-level expression evaluator
int eval_expr(const char *expr_str) {
	size_t cursor = 0;
	return expr(expr_str, &cursor);
}

static char buff[512];
int main(void) {
	printf("Enter an arithmetic expression to evaluate it (no spaces).\n");
	printf("> ");

	while (scanf("%s", buff) == 1) {
		printf("%d\n", eval_expr(buff));
		printf("> ");
	}

	return 0;
}
