/* Implement an algorithm to print all valid (e.g., properly opened and closed)
 * combinations of n-pairs of parentheses.
 * EXAMPLE
 * Input: 3
 * Output: ((())), (()()), (())(), ()(()), ()()()
 *
 * Source: Cracking the Coding Interview, page 110, question 9.6
 */
#include <stdio.h>

void print_parens_aux(char *buff, size_t buff_i, unsigned open_used, unsigned n, unsigned to_close) {
	if (open_used == n && to_close == 0) {
		buff[buff_i] = '\0';
		printf("%s\n", buff);
		return;
	}

	if (to_close > 0) {
		buff[buff_i] = ')';
		print_parens_aux(buff, buff_i+1, open_used, n, to_close-1);
	}

	if (open_used < n) {
		buff[buff_i] = '(';
		print_parens_aux(buff, buff_i+1, open_used+1, n, to_close+1);
	}
}

void print_parens(unsigned n) {
	/* n opening parenthesis + n closing + null terminator */
	char results[2*n+1];
	print_parens_aux(results, 0, 0, n, 0);
}

int main(void) {
	printf("Enter a number N to see every combinations of N pairs of parentheses\n");
	printf("> ");
	fflush(stdout);

	unsigned n;

	while (scanf("%u", &n) == 1) {
		print_parens(n);
		printf("> ");
		fflush(stdout);
	}

	return 0;
}
