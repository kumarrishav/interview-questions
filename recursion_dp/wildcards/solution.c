/* You are given a string where each character is either a 0, a 1,
 * or a question mark (?). A question mark means that that position can hold
 * a 1 or a 0.
 * Write a function that receives one of these strings and prints every possible
 * string of 0s and 1s according to the wildcards in the input.
 *
 * EXAMPLE
 * Input: 0?1?
 * Output: 0010, 0011, 0110, 0111
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

void wildcards_aux(char *str, size_t str_i) {
	if (str[str_i] == '\0') {
		printf("%s\n", str);
		return;
	}

	if (str[str_i] != '?') {
		wildcards_aux(str, str_i+1);
	} else {
		str[str_i] = '0';
		wildcards_aux(str, str_i);
		str[str_i] = '1';
		wildcards_aux(str, str_i);
		str[str_i] = '?';
	}
}

void wildcards(char *str) {
	wildcards_aux(str, 0);
}

static char input_str[512];
int main(void) {
	printf("Enter a string of 0s, 1s and ? to generate all possible combinations\n");
	printf("> ");

	while (scanf("%s", input_str) == 1) {
		wildcards(input_str);
		printf("> ");
	}

	return 0;
}
