/* You are given an equation in the form of a string. The only arithmetic operators allowed
 * are + and -.
 * Assume that each missing value is an integer n such that 1 <= n <= 9. Missing values are
 * represented by _.
 * Write a function that receives the equation and prints every solution to the equation.
 *
 * EXAMPLE:
 *
 * Input:
 * _+13-_=6
 *
 * Output:
 * 1+13-8=6
 * 2+13-9=6
 *
 * Source: Invented
 */
#include <stdio.h>
#include <assert.h>

static int next_result(char *pattern, size_t patt_i, int new_n, int res_so_far) {
	if (patt_i == 0) {
		return new_n;
	}
	switch (pattern[patt_i-1]) {
	case '+':
		return res_so_far+new_n;
	case '-':
		return res_so_far-new_n;
	default:
		assert(0);
	}
}

static unsigned magic_numbers_aux(char *pattern, size_t patt_i, int res_so_far) {
	if (patt_i > 0 && pattern[patt_i-1] == '=') {
		int target;
		sscanf(&pattern[patt_i], "%d", &target);
		if (res_so_far == target) {
			printf("%s\n", pattern);
			return 1;
		}
		return 0;
	}

	if (pattern[patt_i] != '_') {
		int n;
		int read;
		sscanf(&pattern[patt_i], "%d%n", &n, &read);
		res_so_far = next_result(pattern, patt_i, n, res_so_far);
		return magic_numbers_aux(pattern, patt_i+read+1, res_so_far);
	}

	int i;
	int matches = 0;
	for (i = 1; i < 10; i++) {
		int new_res = next_result(pattern, patt_i, i, res_so_far);
		pattern[patt_i] = '0'+i;
		matches += magic_numbers_aux(pattern, patt_i+2, new_res);
	}

	pattern[patt_i] = '_';

	return matches;
}

unsigned magic_numbers(char *pattern) {
	return magic_numbers_aux(pattern, 0, 0);
}

static char buff[512];
int main(void) {
	printf("Enter an expression pattern to find matches.\n");
	printf("> ");

	while (scanf("%s", buff) == 1) {
		unsigned found = magic_numbers(buff);
		if (found == 0) {
			printf("No matches found.\n");
		} else {
			printf("Total: %u\n", found);
		}
		printf("> ");
	}

	return 0;
}
