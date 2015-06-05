/* Write a function that counts the maximum nesting level of parenthesis in a string.
 *
 * EXAMPLE
 *
 * For the string "abc(123(xyz))m(((n)))o", the maximum nesting level is 3.
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <assert.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

unsigned max_nest_depth(const char *str) {
	unsigned res = 0;
	unsigned opened = 0;

	size_t i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == '(') {
			opened++;
			res = max(res, opened);
		} else if (str[i] == ')') {
			assert(opened > 0);
			opened--;
		}
	}

	assert(opened == 0);

	return res;
}

static char buf[512];

int main(void) {
	printf("Enter a string with parentheses to count max nesting level.\n");
	printf("> ");

	while (scanf("%s", buf) == 1) {
		printf("Max nesting level = %d\n> ", max_nest_depth(buf));
	}

	return 0;
}
