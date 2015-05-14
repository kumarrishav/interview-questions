/* Regular expressions are a useful tool to match text against arbitrarily complex patterns.
 * In this exercise, let's consider a simplified version of regular expressions, where
 * a regular expression consists of ordinary characters and one or more of special-meaning
 * characters:
 *
 * '.' - matches any character
 * '*' - matches zero or more occurrences of the previous entity in the pattern
 *
 * Write a function that receives an input string, a simplified regex,
 * and returns whether the string matches the regex.
 *
 * EXAMPLE
 * ~~~
 * Assume match() receives the string followed by the pattern. Then, we have:
 *
 * - match("aa", "a") = false
 * - match("aa", "aa") = true
 * - match("aaa", "aa") = false
 * - match("ab", ".*") = true
 * - match("aab", "c*a*b") = true
 * - match("ccca", "cc*ca") = true
 * ~~~
 *
 * Source: Careercup (Yahoo interview)
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int match_aux(const char *str, size_t str_offset, const char *pattern, size_t patt_offset) {
	if (pattern[patt_offset] == '\0') {
		return str[str_offset] == '\0';
	}

	if (pattern[patt_offset+1] != '*') {
		if (pattern[patt_offset] != '.') {
			return str[str_offset] == pattern[patt_offset] &&
				match_aux(str, str_offset+1, pattern, patt_offset+1);
		} else {
			return str[str_offset] != '\0' &&
				match_aux(str, str_offset+1, pattern, patt_offset+1);
		}
	}

	assert(pattern[patt_offset] != '*');
	assert(pattern[patt_offset+1] == '*');

	int last_res = match_aux(str, str_offset, pattern, patt_offset+2);
	size_t star_match_sz = 1;

	while (!last_res && str[str_offset+star_match_sz-1] != '\0') {
		if (pattern[patt_offset] != '.' &&
		    str[str_offset+star_match_sz-1] != pattern[patt_offset]) {
			break;
		}
		last_res = match_aux(str, str_offset+star_match_sz, pattern, patt_offset+2);
		star_match_sz++;
	}

	return last_res;
}

int match(const char *str, const char *pattern) {
	return match_aux(str, 0, pattern, 0);
}

static char input_str[512];
static char input_pattern[512];

int main(void) {
	printf("Enter a string and a glob pattern to see if the string matches the pattern.\n"
	       "To generate an empty string, type ! (example: ! a* matches the empty string against a*)\n");
	printf("> ");

	while (scanf("%s%s", input_str, input_pattern) == 2) {
		if (!strcmp(input_str, "!")) {
			input_str[0] = '\0';
		}
		if (!strcmp(input_pattern, "!")) {
			input_pattern[0] = '\0';
		}

		printf("match(\"%s\", \"%s\") = %s\n> ", input_str, input_pattern,
		       match(input_str, input_pattern) ? "true" : "false");
	}

	return 0;
}
