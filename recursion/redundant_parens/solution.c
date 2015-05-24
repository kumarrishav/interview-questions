/* You are given an arbitrary arithmetic expression, such as:
 *
 * 5+2
 * ((4*8)+3)
 * 4/(3/2)
 *
 * How would you go about implementing an expression evaluator that not only
 * evaluates the result of an arithmetic expression, but also removes all
 * pairs of redundant parentheses?
 *
 * EXAMPLE:
 *
 * Input 1: ((4*8)+3)
 * Input 2: 4*(8+3)
 *
 * Output 1: 4*8+3 = 35
 * Output 2: 4*(8+3) = 44
 *
 * Source: Careercup
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* NOTE: The core implementation is based on the well-known arithmetic expressions parser grammar
 *       outlined in the first chapters of the Dragon Book:
 *
 * expr -> expr + term
 *       | expr - term
 *       | term
 *
 * term -> term * factor
 *       | term / factor
 *       | factor
 *
 * factor -> number
 *         | ( expr )
 *
 * This raw grammar has a problem though: it is left recursive, so we can't simply implement each
 * rule as a function, because the recursion would never stop. The grammar can be transformed into
 * a right recursive grammar by doing some manipulation. Consider a grammar rule that is left
 * recursive:
 *
 * A -> Aa | B
 *
 * Where a and B are sequences of terminals and nonterminals that do not start with A.
 *
 * This rule can be transformed into a right recursive rule by transforming it into:
 *
 * A -> BR
 * R -> aR | e
 *
 * Where 'e' denotes epsilon (the empty string)
 *
 * Doing this in the original expressions grammar yields:
 *
 * expr -> term expr_r
 *
 * expr_r -> + term expr_r
 *         | - term expr_r
 *         | e
 *
 * term -> factor term_r
 *
 * term_r -> * factor term_r
 *         | / factor term_r
 *         | e
 *
 * factor -> number
 *         | ( expr )
 *
 * This is the grammar that the code implements. A set of clever but simple little steps in the
 * parsing of the rule factor -> ( expr ) are all it takes to determine if a given pair of
 * parenthesis is really necessary.
 *
 */

enum {
	PLUS,
	MINUS,
	MUL,
	DIV
};

struct ops_list {
	unsigned char seen[4];
	int res;
};

static int char_to_op(char c) {
	switch (c) {
	case '+':
		return PLUS;
	case '-':
		return MINUS;
	case '*':
		return MUL;
	case '/':
		return DIV;
	default:
		return -1;
	}
}

static int higher_prio(int op1, int op2) {
	return op1 >= MUL && op2 <= MINUS;
}

static void merge_seen(struct ops_list *dest, struct ops_list *other) {
	size_t i;
	for (i = 0; i < sizeof(dest->seen); i++) {
		if (dest->seen[i] || other->seen[i]) {
			dest->seen[i] = 1;
		}
	}
}

static int redundant_parens(const char *expr_str, size_t l, size_t r, struct ops_list *ops) {
	size_t i;
	for (i = 0; i < sizeof(ops->seen) && !ops->seen[i]; i++);

	if (i == sizeof(ops->seen)) {
		return 1;
	}

	int op_l = -1;
	int op_r = char_to_op(expr_str[r+1]);

	if (l > 0) {
		op_l = char_to_op(expr_str[l-1]);
	}

	return !((op_l != -1 && higher_prio(op_l, i)) ||
		 (op_r != -1 && higher_prio(op_r, i)) ||
		 (op_l == MINUS && i <= MINUS) ||
		 (op_l == DIV));
}


static struct ops_list expr(const char *expr_str, size_t *cursor, unsigned char ign[]);
static struct ops_list expr_r(const char *expr_str, size_t *cursor, unsigned char ign[],
			      struct ops_list first_term);
static struct ops_list term(const char *expr_str, size_t *cursor, unsigned char ign[]);
static struct ops_list term_r(const char *expr_str, size_t *cursor, unsigned char ign[],
			      struct ops_list first_term);
static struct ops_list factor(const char *expr_str, size_t *cursor, unsigned char ign[]);


static struct ops_list expr(const char *expr_str, size_t *cursor, unsigned char ign[]) {
	return expr_r(expr_str, cursor, ign, term(expr_str, cursor, ign));
}

static struct ops_list expr_r(const char *expr_str, size_t *cursor, unsigned char ign[],
			      struct ops_list first_term) {
	if (expr_str[*cursor] == '+' || expr_str[*cursor] == '-') {

		size_t saved_cursor = *cursor;
		(*cursor)++;
		struct ops_list second_term = term(expr_str, cursor, ign);
		merge_seen(&first_term, &second_term);

		if (expr_str[saved_cursor] == '+') {
			first_term.res += second_term.res;
			first_term.seen[PLUS] = 1;
		} else {
			first_term.res -= second_term.res;
			first_term.seen[MINUS] = 1;
		}

		return expr_r(expr_str, cursor, ign, first_term);
	} else {
		return first_term;
	}
}

static struct ops_list term(const char *expr_str, size_t *cursor, unsigned char ign[]) {
	return term_r(expr_str, cursor, ign, factor(expr_str, cursor, ign));
}

static struct ops_list term_r(const char *expr_str, size_t *cursor, unsigned char ign[],
			      struct ops_list first_term) {
	if (expr_str[*cursor] == '*' || expr_str[*cursor] == '/') {

		size_t saved_cursor = *cursor;
		(*cursor)++;
		struct ops_list second_term = factor(expr_str, cursor, ign);
		merge_seen(&first_term, &second_term);

		if (expr_str[saved_cursor] == '*') {
			first_term.res *= second_term.res;
			first_term.seen[MUL] = 1;
		} else {
			first_term.res /= second_term.res;
			first_term.seen[DIV] = 1;
		}

		return term_r(expr_str, cursor, ign, first_term);
	} else {
		return first_term;
	}
}

static struct ops_list factor(const char *expr_str, size_t *cursor, unsigned char ign[]) {
	if (expr_str[*cursor] == '(') {
		size_t saved_cursor = *cursor;

		(*cursor)++;
		struct ops_list expr_res = expr(expr_str, cursor, ign);
		assert(expr_str[*cursor] == ')');

		if (redundant_parens(expr_str, saved_cursor, *cursor, &expr_res)) {
			ign[saved_cursor] = ign[*cursor] = 1;
		} else {
			memset(&expr_res.seen, 0, sizeof(expr_res.seen));
		}

		(*cursor)++;

		return expr_res;
	} else {
		struct ops_list ret;

		int read;
		sscanf(&expr_str[*cursor], "%d%n", &ret.res, &read);
		*cursor += read;
		memset(&ret.seen, 0, sizeof(ret.seen));

		return ret;
	}

}

/* Top-level evaluator. Returns a new string which has no redundant parenthesis
 * Returns the result in `out_value`.
 */
char *eval_expr(const char *expr_str, int *out_value) {
	size_t expr_len = strlen(expr_str);
	unsigned char ignore[expr_len];
	memset(ignore, 0, sizeof(ignore));

	size_t cursor = 0;
	struct ops_list res = expr(expr_str, &cursor, ignore);

	*out_value = res.res;

	char *new_expr = malloc(expr_len+1);
	if (new_expr == NULL) {
		return NULL;
	}

	size_t i, j;
	for (i = 0, j = 0; expr_str[i] != '\0'; i++) {
		if (!ignore[i]) {
			new_expr[j++] = expr_str[i];
		}
	}

	new_expr[j] = '\0';

	return new_expr;
}

static char input_buff[512];
int main(void) {
	printf("Enter an expression to remove redundant parentheses and evaluate it.\n");
	printf("> ");

	while (scanf("%s", input_buff) == 1) {
		int result;

		char *canonical = eval_expr(input_buff, &result);

		if (canonical == NULL) {
			fprintf(stderr, "Error evaluating expression - out of memory?\n");
		} else {
			printf("%s = %d\n", canonical, result);
		}

		free(canonical);
		printf("> ");
	}

	return 0;
}
