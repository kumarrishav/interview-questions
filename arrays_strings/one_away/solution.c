/* There are three types of edits that can be performed on strings: insert a character,
 * remove a character, or replace a character. Given two strings, write a function to check if they
 * are one edit (or zero edits) away.
 *
 * EXAMPLE
 *
 * pale, ple -> true
 * pales, pale -> true
 * pale, bale -> true
 * pale, bake -> false
 *
 * Source: Cracking the Coding Interview, 6th Ed., page 91, exercise 1.5
 */
#include <stdio.h>
#include <string.h>

size_t compare_strings(const char *str1, const char *str2) {
	size_t i;
	for (i = 0; str1[i] != '\0' && str1[i] == str2[i]; i++)
		; /* Intentionally left blank */
	return i;
}

int one_away(const char *str1, const char *str2) {
	const char *largest_str = str1;
	size_t largest_len = strlen(str1);

	const char *smallest_str = str2;
	size_t smallest_len = strlen(str2);

	if (largest_len < smallest_len) {
		size_t tmp = smallest_len;
		smallest_len = largest_len;
		largest_len = tmp;
		largest_str = str2;
		smallest_str = str1;
	}

	if (largest_len > smallest_len+1) {
		return 0;
	}

	size_t i = compare_strings(largest_str, smallest_str);
	if (largest_str[i] == '\0') {
		return 1;
	}

	if (largest_len == smallest_len) {
		return compare_strings(largest_str+i+1, smallest_str+i+1)+i+1 == largest_len;
	} else {
		return compare_strings(largest_str+i+1, smallest_str+i)+i+1 == largest_len;
	}	
}

static char str1[1024];
static char str2[1024];

int main(void) {
	printf("Enter two strings\n");
	printf("> ");

	while (scanf("%s%s", str1, str2) == 2) {
		if (one_away(str1, str2)) {
			printf("Yes.\n");
		} else {
			printf("No.\n");
		}
		printf("> ");
	}

	return 0;
}
